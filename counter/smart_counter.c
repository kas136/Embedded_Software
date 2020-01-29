#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/kthread.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

#define SENSOR1 17

#define ultraPIN1 19
#define ultraPIN2 26

#define ultraPIN3 22
#define ultraPIN4 23


int person_check;
int ultra1_check;
int ultra2_check;
int pir_check=0;

unsigned long ultra1time;
unsigned long pirtime;
unsigned long ultra2time;


int pircount=0;
int ultra1count = 0;
int ultra2count = 0;
int ret;
int ultranum =0;
struct task_struct *ultra1_task = NULL;
struct task_struct *ultra2_task = NULL;
struct task_struct *pir_task = NULL;

#define DEV_NAME "simple_sensor_dev"

static int irq_num;
static int simple_sensor_open(struct inode *inode, struct file* file)
{
  printk("simple sensor open\n");
  enable_irq(irq_num);
  return 0;
}
static int simple_sensor_release(struct inode *inode, struct file* file)
{
  printk("simple sensor close\n");
  disable_irq(irq_num);
  return 0;
}
struct file_operations simple_sensor_fops=
{
  .open = simple_sensor_open,
  .release = simple_sensor_release,
};
static void ultra1_start(void)
{

    ultra1time = jiffies;
    struct timeval start_time;
    struct timeval end_time;
    
    unsigned long s_time;
    unsigned long e_time;
    unsigned long distance;
    
    while(!kthread_should_stop()) {
        gpio_direction_output(ultraPIN1, 0);
        gpio_direction_input(ultraPIN2);
        
        gpio_set_value(ultraPIN1, 0);
        mdelay(50);
        
        gpio_set_value(ultraPIN1, 1);
        udelay(10);
        
        gpio_set_value(ultraPIN1, 0);
        
        while(gpio_get_value(ultraPIN2) == 0);
        do_gettimeofday(&start_time);
        
        while(gpio_get_value(ultraPIN2) == 1);
        do_gettimeofday(&end_time);
        
        s_time = (unsigned long)start_time.tv_sec*1000000 + (unsigned long)start_time.tv_usec;
        e_time = (unsigned long)end_time.tv_sec*1000000 + (unsigned long)end_time.tv_usec;
        
        distance = (e_time - s_time) / 58;
        
        if(distance <= 20)
        {
            ultra1count++;
            ultra1time = jiffies;
            printk("ult1 : %d\n", ultra1count);
            msleep(1000);
        }
        else if(distance > 100)
        {

            unsigned long a = jiffies- ultra1time;
            if(a >600)
            {
                ultra1count =0;
            }
        }

        msleep(500);
        
    }
    
}
static void ultra2_start(void)
{
    ultra2_check=0;
    ultra2time = jiffies;
    struct timeval start_time;
    struct timeval end_time;
    
    unsigned long s_time;
    unsigned long e_time;
    unsigned long distance;
    
    while(!kthread_should_stop()) {
        gpio_direction_output(ultraPIN3, 0);
        gpio_direction_input(ultraPIN4);
        
        gpio_set_value(ultraPIN3, 0);
        mdelay(50);
        
        gpio_set_value(ultraPIN3, 1);
        udelay(10);
        
        gpio_set_value(ultraPIN3, 0);
        
        while(gpio_get_value(ultraPIN4) == 0);
        do_gettimeofday(&start_time);
        
        while(gpio_get_value(ultraPIN4) == 1);
        do_gettimeofday(&end_time);
        
        s_time = (unsigned long)start_time.tv_sec*1000000 + (unsigned long)start_time.tv_usec;
        e_time = (unsigned long)end_time.tv_sec*1000000 + (unsigned long)end_time.tv_usec;
        
        distance = (e_time - s_time) / 58;
        
        if(distance <= 20)
        {
            ultra2count++;
            printk("ult2 : %d\n", ultra2count);
            ultra2time = jiffies;
            msleep(1000);
        }
        else if(distance > 100)
        {

            unsigned long a = jiffies- ultra2time;
            if(a >600)
            {
                ultra2count =0;
            }
        }
        msleep(500);
        
    }
    
}

static irqreturn_t simple_sensor_isr(int irq, void* dev_id)
{
    printk("pir detect\n");
    pirtime= jiffies;
    
    pircount += ultra2count;
    ultra2count = 0;
    printk("minus start : %d/n", ultra1count);
    pircount -= ultra1count;
    printk("minus end : %d/n", ultra1count);
    ultra1count = 0;
   
    if(pircount <0)
    {
        pircount = 0;
    }
    printk("total count : %d\n", pircount);
  return IRQ_HANDLED;

}
static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init simple_sensor_init(void)
{
    
  int ret;
    gpio_request(ultraPIN1, "ultraPIN1");
    gpio_request(ultraPIN2, "ultraPIN2");
    gpio_request(ultraPIN3, "ultraPIN3");
    gpio_request(ultraPIN4, "ultraPIN4");
    
  printk("init module\n");

  alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
  cd_cdev = cdev_alloc();
  cdev_init(cd_cdev, &simple_sensor_fops);
  cdev_add(cd_cdev, dev_num, 1);

  gpio_request_one(SENSOR1, GPIOF_IN, "sensor1");
  irq_num = gpio_to_irq(SENSOR1);
  ret = request_irq(irq_num, simple_sensor_isr, IRQF_TRIGGER_RISING, "sensor_irq", NULL);
    pirtime = jiffies;
  if(ret)
  {
    printk(KERN_ERR "unable to request IRQ: %d\n", ret);
      free_irq(irq_num, NULL);
  }
    ultra1_task = kthread_run(ultra1_start, NULL, "ultra1_start");
    ultra2_task = kthread_run(ultra2_start, NULL, "ultra2_start");
  return 0;
}
static void __exit simple_sensor_exit(void)
{
    gpio_free(ultraPIN1);
    gpio_free(ultraPIN2);
    gpio_free(ultraPIN3);
    gpio_free(ultraPIN4);
    
  printk("exit module\n");
  cdev_del(cd_cdev);
  unregister_chrdev_region(dev_num,1);

  free_irq(irq_num, NULL);
  gpio_free(SENSOR1);
    kthread_stop(ultra1_task);
    kthread_stop(ultra2_task);
    
}
module_init(simple_sensor_init);
module_exit(simple_sensor_exit);
