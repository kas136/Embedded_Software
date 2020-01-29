#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

#define PIN1 6
#define PIN2 13
#define PIN3 19
#define PIN4 26

#define STEPS 8

int steps[STEPS][4] = {
	{1, 0, 0, 0},
	{1, 1, 0, 0},
	{0, 1, 0, 0},
	{0, 1, 1, 0},
	{0, 0, 1, 0},
	{0, 0, 1, 1},
	{0, 0, 0, 1},
	{1, 0, 0, 1},
};

void setStep(int p1, int p2, int p3, int p4)
{
	gpio_set_value(PIN1, p1);
	gpio_set_value(PIN2, p2);
	gpio_set_value(PIN3, p3);
	gpio_set_value(PIN4, p4);
}

void moveDegree(int degree, int delay, int direction)
{
	int i, j;

	for(i = 0; i < degree * 64 / 45; i++)
	{
		if(direction == 0)
		{
			for(j = 0; j < STEPS; j++)
			{
				setStep(steps[j][0], steps[j][1], steps[j][2], steps[j][3]);
				udelay(delay);
			}
		}

		else if(direction == 1)
		{
			for(j = 7; j >= 0; j--)
			{
				setStep(steps[j][0], steps[j][1], steps[j][2], steps[j][3]);
				udelay(delay);
			}
		}
	}
}

static int __init simple_motor_init(void)
{
	gpio_request_one(PIN1, GPIOF_OUT_INIT_LOW, "p1");
	gpio_request_one(PIN2, GPIOF_OUT_INIT_LOW, "p2");
	gpio_request_one(PIN3, GPIOF_OUT_INIT_LOW, "p3");
	gpio_request_one(PIN4, GPIOF_OUT_INIT_LOW, "p4");

	moveDegree(360, 1200, 0);
	udelay(1000);
	moveDegree(360, 1200, 1);
	udelay(1000);
	moveDegree(900, 1200, 0);


	return 0;
}

static void __exit simple_motor_exit(void)
{
	gpio_free(PIN1);
	gpio_free(PIN2);
	gpio_free(PIN3);
	gpio_free(PIN4);
}

module_init(simple_motor_init);
module_exit(simple_motor_exit);

