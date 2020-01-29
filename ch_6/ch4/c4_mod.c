#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

#define SENSOR1 17
#define LED1 4

#define DEV_NAME "challenge_dev"

static int irq_num;

static struct timer_list my_timer;

static void my_timer_func(unsigned long data)
{
	printk("timer\n");

	if(data > 2L)
	{
     	gpio_set_value(LED1, 0);
		gpio_free(LED1);
    }

	else
	{
    	gpio_set_value(LED1, 1);
		my_timer.data = data + 1;
		my_timer.expires = jiffies + (1*HZ);

		add_timer(&my_timer);
	}
}

static int challenge_open(struct inode *inode, struct file* file)
{
	printk("sensor open\n");
	enable_irq(irq_num);
	return 0;
}

static int challenge_release(struct inode *inode, struct file* file)
{
	printk("senosr close \n");
	disable_irq(irq_num);
	return 0;
}

struct file_operations challenge_fops=
{
	.open = challenge_open,
	.release = challenge_release,
};

static irqreturn_t challenge_isr(int irq, void* dev_id)
{
	printk("detect\n");

	gpio_set_value(LED1, 0);
	gpio_free(LED1);
	gpio_request_one(LED1, GPIOF_OUT_INIT_LOW, "LED1");

	del_timer(&my_timer);
	init_timer(&my_timer);
	my_timer.function = my_timer_func;
	my_timer.data = 1L;
	my_timer.expires = jiffies + (1*HZ);
	add_timer(&my_timer);

	return IRQ_HANDLED;
}

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init challenge_init(void)
{
	int ret;

	printk("Init Module\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &challenge_fops);
	cdev_add(cd_cdev, dev_num, 1);
	
	init_timer(&my_timer);

	gpio_request_one(SENSOR1, GPIOF_IN, "sensor1");
	irq_num = gpio_to_irq(SENSOR1);
	ret = request_irq(irq_num, challenge_isr, IRQF_TRIGGER_FALLING, "sensor_irq", NULL);
	if(ret)
	{
		printk(KERN_ERR "Unable to request IRQ: %d\n", ret);
		free_irq(irq_num, NULL);
	}

	else
	{
		disable_irq(irq_num);
	}
	
	return 0;
}

static void __exit challenge_exit(void)
{
	printk("Exit Module\n");
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);

	free_irq(irq_num, NULL);
	gpio_free(SENSOR1);
	del_timer(&my_timer);
    gpio_set_value(LED1, 0);
	gpio_free(LED1);

}

module_init(challenge_init);
module_exit(challenge_exit);
