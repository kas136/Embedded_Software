#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");

static struct timer_list my_timer;

#define LED1 4
#define LED2 17
#define LED3 27

int LED[3] = {LED1, LED2, LED3};

static void my_timer_func(unsigned long data)
{
	if(data > 2L)
	{
		data = 0L;
	}

	printk("timer_func start %ld\n", data);
	
	int i;

	for(i = 0; i < 3; i++)
	{
		gpio_set_value(LED[i], 0);
		gpio_free(LED[i]);
	}

	gpio_request_one(LED[data], GPIOF_OUT_INIT_LOW, "LED");

	gpio_set_value(LED[data], 1);

	my_timer.data = data + 1;
	my_timer.expires = jiffies + (1*HZ);
	add_timer(&my_timer);
}

static int __init simple_led_init(void)
{
	printk("start\n");

	init_timer(&my_timer);

	my_timer.function = my_timer_func;
	my_timer.data = 0L;
	my_timer.expires = jiffies + (1*HZ);
	add_timer(&my_timer);

	return 0;
}

static void __exit simple_led_exit(void)
{
	printk("finish\n");

	del_timer(&my_timer);
	
	int i;

	for(i = 0; i < 3; i++)
	{
		gpio_set_value(LED[i], 0);
		gpio_free(LED[i]);
	}
}

module_init(simple_led_init);
module_exit(simple_led_exit);
