#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
 
#include <linux/fs.h>
#include <linux/cdev.h>
MODULE_LICENSE("GPL");
 
#define DEV_NAME "ch2_mod2_dev"

extern int my_var;

static int ch2_mod2_read(struct file *file, char *buf, size_t len, loff_t *lof)
{
	return 0;
}

static int ch2_mod2_write(struct file *file, const char *buf, size_t len, loff_t *lof)
{
	return 0;
}

static int ch2_mod2_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int ch2_mod2_release(struct inode *inode, struct file *file)
{
	return 0;
}

struct file_operations ch2_mod2_fops =
{
	.read = ch2_mod2_read,
	.write = ch2_mod2_write,
	.open = ch2_mod2_open,
	.release = ch2_mod2_release,
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ch2_mod2_init(void)
{
	printk("Init mod2\n");
	printk("my_var : %d\n", my_var);

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ch2_mod2_fops);
	cdev_add(cd_cdev, dev_num, 1);

	return 0;
}

static void __exit ch2_mod2_exit(void)
{
	printk("Exit mod2\n");
	printk("my_var : %d\n", my_var);

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
}

module_init(ch2_mod2_init);
module_exit(ch2_mod2_exit);

