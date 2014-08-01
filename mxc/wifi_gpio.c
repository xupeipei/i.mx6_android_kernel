/*wifi resume */
#include <linux/completion.h>
#include <linux/crc-ccitt.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <asm/gpio.h>
#include <asm/delay.h>
#include <linux/i2c.h>

#define WIFI_DRIVER_NAME "wifi"
#define IMX_GPIO_NR(bank, nr)		(((bank) - 1) * 32 + (nr))

#define WIFI_POWER	      IMX_GPIO_NR(6, 8)

struct miscdevice miscdev;

static int wifi_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "**************wifi_open\n");
	gpio_set_value(WIFI_POWER, 0);

	mdelay(1000);
	gpio_set_value(WIFI_POWER, 1);
	/*do some init.....*/
	return 0;
}

static int wifi_close(struct inode *inode, struct file *file)
{
	//printk(KERN_INFO "**************mlx90614_close\n");
	//gpio_set_value(INFRA_POWER, 0);
	
	return 0;
}

static const struct file_operations wifi_fops = {
	.owner		= THIS_MODULE,
	.open		= wifi_open,
	.release	= wifi_close,
};

static int __init wifi_init(void)
{
	int r = 0;
	
	printk(KERN_INFO "***********************wifi_init\n");
			
	r = gpio_request(WIFI_POWER, "WIFI_POWER");
	if (r < 0) {
		printk(KERN_ERR "failed to request GPIO for WIFI POWER ! error = %d\n", r);
		return 1;
	}
	
	r = gpio_direction_output(WIFI_POWER, 1);
	if (r < 0) {
		printk(KERN_INFO "fail to set direction of WIFI POWER GPIO output! error = %d\n", r);
		return 1;
	}
		
	miscdev.minor = WIFI_MINOR;
	miscdev.name = WIFI_DRIVER_NAME;
	miscdev.fops = &wifi_fops;
	r = misc_register(&miscdev);
	if (r < 0) {
		printk(KERN_INFO "failed to register WIFI\n");
		return 1;
	}
	printk(KERN_INFO "***************WIFI load success\n");
		
	return r;
}

static void __exit wifi_exit(void)
{
	misc_deregister(&miscdev);
}

module_init(wifi_init);
module_exit(wifi_exit);

MODULE_LICENSE("GPL");

