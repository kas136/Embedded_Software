#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/rculist.h>
#include <linux/wait.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include "ku_pir.h"

MODULE_LICENSE("GPL");

#define DEV_NAME "ku_pir_dev"
#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2
#define IOCTL_NUM3 IOCTL_START_NUM+3
#define IOCTL_NUM4 IOCTL_START_NUM+4
#define IOCTL_NUM5 IOCTL_START_NUM+5

#define KU_PIR_NUM 'z'
#define KUPIROPEN _IOWR(KU_PIR_NUM, IOCTL_NUM1, unsigned long *)
#define KUPIRCLOSE _IOWR(KU_PIR_NUM, IOCTL_NUM2, unsigned long *)
#define KUPIRREAD _IOWR(KU_PIR_NUM, IOCTL_NUM3, unsigned long *)
#define KUPIRFLUSH _IOWR(KU_PIR_NUM, IOCTL_NUM4, unsigned long *)
#define KUPIRINSERTDATA _IOWR(KU_PIR_NUM, IOCTL_NUM5, unsigned long *)

struct pirqueue
{
	struct list_head qlist;
	struct ku_pir_data pd;
};

struct linked_list
{
	struct list_head list;
	struct pirqueue pq;
	int cpid;
	int count;
};

static int irq_num;
static dev_t dev_num;
static struct cdev *cd_cdev;

struct linked_list mylist;

wait_queue_head_t my_wq;

int ku_pir_open(int fd)
{
	struct linked_list *tmp = 0;
	struct pirqueue *tmp2 = 0;

	list_for_each_entry_rcu(tmp, &mylist.list, list)
	{
		if(tmp->cpid == current->pid)
		{
			return fd;
		}
	}

	tmp = kmalloc(sizeof(struct linked_list), GFP_KERNEL);
	INIT_LIST_HEAD(&tmp->pq.qlist);
	
	tmp->cpid = current->pid;
	tmp->count = 0;

	list_add_tail_rcu(&tmp->list, &mylist.list);

	return fd;
}

int ku_pir_close(int fd)
{
	struct linked_list *tmp = 0;
	struct list_head *pos = 0;
	struct list_head *q = 0;

	list_for_each_safe(pos, q, &mylist.list)
	{
		tmp = list_entry(pos, struct linked_list, list);

		if(tmp->cpid == current->pid)
		{
			list_del_rcu(pos);
			kfree(tmp);

			return 0;
		}
	}

	return -1;
}

void ku_pir_read(int fd, struct ku_pir_data *data)
{
	struct linked_list *tmp = 0;
	struct pirqueue *qtmp = 0;
	struct list_head *pos = 0;
	struct list_head *q = 0;

	list_for_each_entry_rcu(tmp, &mylist.list, list)
	{
		if(tmp->cpid == current->pid)
		{
			wait_event(my_wq, tmp->count > 0);
			tmp->count--;

			list_for_each_safe(pos, q, &tmp->pq.qlist)
			{
				qtmp = list_entry(pos, struct pirqueue, qlist);
				copy_to_user(data, &qtmp->pd, sizeof(struct ku_pir_data));
				list_del_rcu(pos);
				kfree(qtmp);

				return;
			}
		}
	}
}

void ku_pir_flush(int fd)
{
	struct linked_list *tmp = 0;
	struct pirqueue *qtmp = 0;
	struct list_head *pos = 0;
	struct list_head *q = 0;

	list_for_each_entry_rcu(tmp, &mylist.list, list)
	{
		if(tmp->cpid == current->pid)
		{
			list_for_each_safe(pos, q, &tmp->pq.qlist)
			{
				qtmp = list_entry(pos, struct pirqueue, qlist);
				list_del_rcu(pos);
			}

			tmp->count = 0;
			kfree(qtmp);
			return;
		}
	}
}

int ku_pir_insertData(long unsigned int ts, char rf_flag)
{
	struct linked_list *tmp = 0;
	struct pirqueue *tmp2 = 0;
	struct list_head *pos = 0;
	struct list_head *q = 0;

	int check = 0;

	list_for_each_entry_rcu(tmp, &mylist.list, list)
	{
		struct pirqueue *ptmp = kmalloc(sizeof(struct pirqueue), GFP_KERNEL);
		ptmp->pd.timestamp = ts;
		ptmp->pd.rf_flag = rf_flag;

		if(tmp->count == KUPIR_MAX_MSG)
		{
			list_for_each_safe(pos, q, &tmp->pq.qlist)
			{
				list_del_rcu(pos);
				break;
			}

			tmp->count--;
		}

		list_add_tail_rcu(&ptmp->qlist, &tmp->pq.qlist);
		check++;
		tmp->count++;
		wake_up(&my_wq);
	}

	if(check > 0)
		return 0;

	else
		return -1;
}

static irqreturn_t sensor_irq(int irq, void* dev_id)
{
	int flag;
	flag = gpio_get_value(KUPIR_SENSOR);

	if(flag == 0)
	{
		printk("falling\n");
		ku_pir_insertData(jiffies, '0');
	}

	else if(flag == 1)
	{
		printk("rising\n");
		ku_pir_insertData(jiffies, '1');
	}

	return IRQ_HANDLED;
}

static long ku_pir_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case KUPIROPEN:
			{
				int fd;

				fd = (int) arg;
				
				return ku_pir_open(fd);
			}

		case KUPIRCLOSE:
			{
				int fd;
				
				fd = (int) arg;
				
				return ku_pir_close(fd);
			}

		case KUPIRREAD:
			{
				struct READ {
					int fd;
					struct ku_pir_data *d;
				};

				struct READ *read = (struct READ *) arg;

				ku_pir_read(read->fd, read->d);

				break;
			}

		case KUPIRFLUSH:
			{
				int fd;
				fd = (int) arg;

				ku_pir_flush(fd);

				break;
			}

		case KUPIRINSERTDATA:
			{
				struct INSERT {
					long unsigned int ts;
					char rf_flag;
				};

				struct INSERT *insert = (struct INSERT *) arg;

				return ku_pir_insertData(insert->ts, insert->rf_flag);
			}


		default:
			return -1;
	}

	return 0;
}

struct file_operations ku_pir_fops = 
{
	.unlocked_ioctl = ku_pir_ioctl,
};

static int __init ku_pir_init(void)
{
	printk("Init mod\n");

	int ret;

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ku_pir_fops);
	cdev_add(cd_cdev, dev_num, 1);

	INIT_LIST_HEAD(&mylist.list);
	init_waitqueue_head(&my_wq);

	gpio_request_one(KUPIR_SENSOR, GPIOF_IN, "ku_sensor");
	irq_num = gpio_to_irq(KUPIR_SENSOR);
	ret = request_irq(irq_num, sensor_irq, IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING, "sensor_irq", NULL);

	return 0;
}

static void __exit ku_pir_exit(void)
{
	printk("Exit mod\n");

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
	
	disable_irq(irq_num);
	free_irq(irq_num, NULL);
	gpio_free(KUPIR_SENSOR);
}

module_init(ku_pir_init);
module_exit(ku_pir_exit);
