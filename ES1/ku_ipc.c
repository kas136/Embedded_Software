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
#include "ku_ipc.h"

MODULE_LICENSE("GPL");

#define DEV_NAME "ku_ipc_dev"
#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2
#define IOCTL_NUM3 IOCTL_START_NUM+3
#define IOCTL_NUM4 IOCTL_START_NUM+4

#define KU_IPC_NUM 'z'
#define KUMSGGET _IOWR(KU_IPC_NUM, IOCTL_NUM1, unsigned long *)
#define KUMSGCLOSE _IOWR(KU_IPC_NUM, IOCTL_NUM2, unsigned long *)
#define KUMSGSND _IOWR(KU_IPC_NUM, IOCTL_NUM3, unsigned long *)
#define KUMSGRCV _IOWR(KU_IPC_NUM, IOCTL_NUM4, unsigned long *)

spinlock_t my_lock;

struct msgbuf
{
	long type;
	char text[70];
};

struct msgqueue
{
	struct msgbuf msgb;
	struct list_head qlist;
};

struct linked_list
{
	int key;
	int msqid;
	int link;
	int count;
	int volume;
	struct msgqueue msgq;
	struct list_head list;
};

static dev_t dev_num;
static struct cdev *cd_cdev;

struct linked_list mylist;

int ku_msgget(int key, int msgflg)
{
	int msqid;
	struct linked_list *tmp;
	
	spin_lock(&my_lock);
	
	list_for_each_entry(tmp, &mylist.list, list)
	{
		if(tmp->key == key)
		{

			if(msgflg == KU_IPC_CREAT)
			{
				tmp->link = tmp->link + 1;
				
				spin_unlock(&my_lock);
				
				return tmp->msqid;
			}

			else if(msgflg == KU_IPC_EXCL)
			{
				spin_unlock(&my_lock);
				
				return -1;
			}
		}
	}

	tmp = (struct linked_list*)kmalloc(sizeof(struct linked_list), GFP_KERNEL);
	tmp->key = key;
	tmp->msqid = key;
	tmp->link = 1;
	tmp->count = 0;
	tmp->volume = 0;
	msqid = tmp->msqid;

	INIT_LIST_HEAD(&tmp->msgq.qlist);
	list_add(&tmp->list, &mylist.list);

	spin_unlock(&my_lock);
	
	return msqid;
}

int ku_msgclose(int msqid)
{
	struct linked_list *tmp = 0;
	struct list_head *pos = 0;
	struct list_head *q = 0;

	list_for_each_safe(pos, q, &mylist.list)
	{
		tmp = list_entry(pos, struct linked_list, list);

		if(tmp->msqid == msqid)
		{
			spin_lock(&my_lock);

			tmp->link = tmp->link - 1;

			if(tmp->link == 0)
			{
				list_del(pos);
				kfree(tmp);

				spin_unlock(&my_lock);

				return 0;
			}

			spin_unlock(&my_lock);

			return 0;
		}
	}

	return -1;
}

int ku_msgsnd(int msqid, void *msgp, int msgsz, int msgflg)
{

	struct linked_list *tmp = 0;
	struct list_head *pos = 0;

	list_for_each(pos, &mylist.list)
	{
		tmp = list_entry(pos, struct linked_list, list);

		if(tmp->msqid == msqid)
		{
			spin_lock(&my_lock);

			if(tmp->count >= KUIPC_MAXMSG)
			{
				if(msgflg == KU_IPC_NOWAIT)
				{
					spin_unlock(&my_lock);
					
					return -1;
				}

				else if(msgflg == 0)
				{
					spin_unlock(&my_lock);
					return 1;
				}
			}

			if(tmp->volume + (msgsz + sizeof(long)) > KUIPC_MAXVOL)
			{
				if(msgflg == KU_IPC_NOWAIT)
				{
					spin_unlock(&my_lock);
					return -1;
				}

				else if(msgflg == 0)
				{
					spin_unlock(&my_lock);
					return 1;
				}
			}

			struct msgqueue *qtmp = (struct msgqueue*)kmalloc(sizeof(struct msgqueue), GFP_KERNEL);

			memset(qtmp->msgb.text, '\0', sizeof(qtmp->msgb.text));
		
			struct MSG *msg = (struct MSG *) msgp;

			qtmp->msgb.type = msg->type;
			strcpy(qtmp->msgb.text, msg->text);
			
			int i;
			
			for(i = msgsz; i < 70; i++)
			{
				qtmp->msgb.text[i] = '\0';
			}

			list_add_tail(&qtmp->qlist, &tmp->msgq.qlist);
			tmp->count = tmp->count + 1;
			tmp->volume = tmp->volume + (msgsz + sizeof(msg->type));

			spin_unlock(&my_lock);
			
			return 0;
		}

	}

	return -1;
}

int ku_msgrcv(int msqid, void *msgp, int msgsz, long msgtyp, int msgflg)
{
	int length;
	length = msgsz;
							
	struct linked_list *tmp = 0;
	struct list_head *pos = 0;
	struct msgqueue *qtmp = 0;
	struct list_head *qpos = 0;
	struct list_head *qq = 0;


	list_for_each(pos, &mylist.list)
	{
		tmp = list_entry(pos, struct linked_list, list);

		if(tmp->msqid == msqid)
		{
			spin_lock(&my_lock);

			if(tmp->count == 0)
			{
				if(msgflg == KU_IPC_NOWAIT || msgflg == 3)
				{
					spin_unlock(&my_lock);
					return -1;
				}

				else if(msgflg == 0)
				{
					printk("check\n");
					spin_unlock(&my_lock);
					return 0;
				}
			}

			list_for_each_safe(qpos, qq, &tmp->msgq.qlist)
			{
				qtmp = list_entry(qpos, struct msgqueue, qlist);

				if(qtmp->msgb.type == msgtyp)
				{
					int i;

					for(i = 1; i < 70; i++)
					{
						if(qtmp->msgb.text[i] == '\0')
							break;
					}

					if(i > length)
					{
						if(msgflg == 0)
						{
							spin_unlock(&my_lock);
							return -1;
						}

						else if(msgflg == KU_IPC_NOERROR || msgflg == 3)
						{
							for(i; i < 70; i++)
							{
								qtmp->msgb.text[i] = '\0';
							}
						}
					}

					copy_to_user(msgp, &qtmp->msgb, (msgsz + sizeof(qtmp->msgb.type)));

					tmp->count = tmp->count - 1;
					tmp->volume = tmp->volume - (msgsz + sizeof(qtmp->msgb.type));

					list_del(qpos);
					kfree(qtmp);
					
					spin_unlock(&my_lock);

					return length;
				}
			}
			spin_unlock(&my_lock);
		}
	}

	return -1;
}

static long msg_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case KUMSGGET:
			{
				struct GET{
					int key;
					int msgflg;
				};

				struct GET *get = (struct GET *) arg;

				return ku_msgget(get->key, get->msgflg);
			}

		case KUMSGCLOSE:
			{
				int *msqid = arg;

				return ku_msgclose(*msqid);
			}

		case KUMSGSND:
			{
				struct SND{
					int msqid;
					void *msgp;
					int msgsz;
					int msgflg;
				};

				struct SND *snd = (struct SND *)arg;

				return ku_msgsnd(snd->msqid, snd->msgp, snd->msgsz, snd->msgflg);
			}

		case KUMSGRCV:
			{
				struct RCV{
					int msqid;
					void *msgp;
					int msgsz;
					long msgtyp;
					int msgflg;
				};

				struct RCV *rcv = (struct RCV *)arg;

				return ku_msgrcv(rcv->msqid, rcv->msgp, rcv->msgsz, rcv->msgtyp, rcv->msgflg);
			}

		default:
			return -1;
	}
	return 0;
}

struct file_operations ku_ipc_fops =
{
	.unlocked_ioctl = msg_ioctl
};

static int __init ku_ipc_init(void)
{
	printk("Init mod\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ku_ipc_fops);
	cdev_add(cd_cdev, dev_num, 1);

	INIT_LIST_HEAD(&mylist.list);

	return 0;
}

static void __exit ku_ipc_exit(void)
{
	printk("Exit mod\n");

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
}

module_init(ku_ipc_init);
module_exit(ku_ipc_exit);
