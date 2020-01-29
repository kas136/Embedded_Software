#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/cdev.h>
MODULE_LICENSE("GPL");

#define DEV_NAME "ch2_mod1_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1

#define CH2_IOCTL_NUM 'z'
#define CH2_IOCTL1 _IOWR(CH2_IOCTL_NUM, IOCTL_NUM1, unsigned long *)

int my_var;
EXPORT_SYMBOL(my_var);

static long ch2_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case CH2_IOCTL1:
			my_var++;
			printk("my_var : %d\n", my_var);
			break;
	}

	return 0;
}

static int ch2_ioctl_open(struct inode *inode, struct file *file)
{
	printk("ch2_ioctl open\n");
	return 0;
}

static int ch2_ioctl_release(struct inode *inode, struct file *file)
{
	printk("ch2_ioctl release\n");
	return 0;
}

struct file_operations ch2_ioctl_fops = 
{
	.unlocked_ioctl = ch2_ioctl,
	.open = ch2_ioctl_open,
	.release = ch2_ioctl_release,
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ch2_mod1_init(void)
{
	printk("Init mod1\n");
	my_var = 0;

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
    	cd_cdev = cdev_alloc();
    	cdev_init(cd_cdev, &ch2_ioctl_fops);
    	cdev_add(cd_cdev, dev_num, 1);
	
	return 0;
 }
 
 static void __exit ch2_mod1_exit(void)
 {
	printk("Exit mod1\n");
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
 }
 
 module_init(ch2_mod1_init);
 module_exit(ch2_mod1_exit);

