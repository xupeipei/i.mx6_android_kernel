/*
*file:adc081c021.c
*function:ad driver
*copyright (C) tocel LTD,
*author:only
*/
	
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/semaphore.h> 
#include <linux/delay.h>
	
	
#define DEVICE_NAME "adc081c021"

#define ADC_COVERSION_RESULT 0x00
#define ADC_ALERT_STATUS 0x01
#define ADC_CONFIGURATION 0x02
#define ADC_LOW_LIMIT 0x03
#define ADC_HIGH_LIMIT 0x04
#define ADC_HISTERESIS 0x5
#define ADC_LOWST_CONVERSION 0x06
#define ADC_HIGHST_CONVERSION 0x07

#define I2C_NUM 1

int I2C_MAJOR;
int I2C_MINOR;
char buf1[1];
char buf2[2]; 

struct adc_device {
	struct cdev cdev;
	struct i2c_client *client;
	struct semaphore sem;  
	struct class *class;
};

struct adc_device i2c;

u32 adc081c021_read_adc(void)
{
	struct i2c_msg msgs[2];
	int ret;
	char reg_addr = ADC_COVERSION_RESULT;
	unsigned char buf[2] = {0};
	u32 voltage;
	
	msgs[0].addr	= i2c.client->addr;
	msgs[0].flags	= 0;
	msgs[0].len	= 1;
	msgs[0].buf	= &reg_addr;
	
	msgs[1].addr	= i2c.client->addr;
	msgs[1].flags	= I2C_M_RD;
	msgs[1].len	= 2;
	msgs[1].buf	= buf;

	ret = i2c_transfer(i2c.client->adapter, msgs, 2);

	if (ret != 2) {
		printk(KERN_INFO "adc081c021_read_adc ERR %d\n", ret);
	}
	
	voltage = (((buf[0] & 0x0f) << 4) | ((buf[1] & 0xf0) >> 4)) * 3000 / 256;

	
	return voltage * 2;
}
EXPORT_SYMBOL_GPL(adc081c021_read_adc);

static int adc_open(struct inode *inode, struct file *filp)
{
  //  if(down_interruptible(&(i2c.sem))) {  
       // return -ERESTARTSYS;  
   // }  
	return 0;
}

static int adc_release(struct inode *inode, struct file *filp)
{
	//up(&(i2c.sem));
	return 0;
}

extern void ov5640_xx_read(void);
extern void pmic_read(void);
extern int read_lux(void);
extern unsigned char ft5x0x_read_fw_ver();
extern int fm2018_hw_init(void);


static ssize_t adc_read(struct file *filp, char *buf, unsigned int count, loff_t *ppos)
{
	struct i2c_msg msgs[2];
	int ret;
	char reg_addr = ADC_COVERSION_RESULT;
	unsigned char buf1[2] = {0};
	u32 voltage;

	//pmic_read();
	//ft5x0x_read_fw_ver();
	//mdelay(1000);
	//read_lux();
	//mdelay(1000);
	//ov5640_xx_read();/*test i2c*/
//	fm2018_hw_init();
//	mdelay(1000);
	msgs[0].addr	= i2c.client->addr;
	msgs[0].flags	= 0;
	msgs[0].len	= 1;
	msgs[0].buf	= &reg_addr;
	
	msgs[1].addr	= i2c.client->addr;
	msgs[1].flags	= I2C_M_RD;
	msgs[1].len	= 2;
	msgs[1].buf	= buf1;

	ret = i2c_transfer(i2c.client->adapter, msgs, 2);

	if (ret != 2) {
		printk(KERN_INFO "adc081c021_read_adc ERR %d\n", ret);
	}
	printk(KERN_INFO "buf1[0] =0x%x ,buf1[1] = 0x%x", buf1[0], buf1[1]);
	voltage = (((buf1[0] & 0x0f) << 4) | ((buf1[1] & 0xf0) >> 4)) * 3000000 / 256;

	printk(KERN_INFO "voltage : %d\n", 2 * voltage);
	if (copy_to_user(buf, &voltage, sizeof(voltage))) {
		return -EFAULT;
	}

	return 0;
}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = adc_open,
	.release = adc_release,
	.read = adc_read,
};

static void adc_setup_cdev(void)
{
	int ret;
	dev_t devno = MKDEV(I2C_MAJOR,I2C_MINOR);

	cdev_init(&i2c.cdev,&fops);
	i2c.cdev.owner = THIS_MODULE;
	i2c.cdev.ops = &fops;
	ret = cdev_add(&i2c.cdev,devno,I2C_NUM);

	if (ret) {
		printk(KERN_INFO "fail to add cdev\n");
	}

	//init_MUTEX(&(i2c.sem));
}

static int adc_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;
	
	i2c.client = client;
	dev_t devno;
	printk(KERN_INFO "***********adc_probe\n");

	ret = alloc_chrdev_region(&devno, 0, I2C_NUM, DEVICE_NAME);

	if (ret) {
		printk(KERN_INFO "fail to register cdev\n");
		goto err1;

	}	
	I2C_MAJOR = MAJOR(devno);

	I2C_MINOR = MINOR(devno);

	adc_setup_cdev();
	i2c.class = class_create(THIS_MODULE, DEVICE_NAME);

	if (IS_ERR(i2c.class)) {
		printk(KERN_INFO "fail to create class\n");
		goto err2;
	}

	device_create(i2c.class, NULL, devno, NULL, DEVICE_NAME);
	
	return 0;

err1:
	return ret;
err2:
	unregister_chrdev_region(devno,I2C_NUM);
	return -1;
}
	
static int adc_remove(struct i2c_client *client)
{
	i2c.client = NULL;
	return 0;	
}
	
static const struct i2c_device_id adc_id[] = {
	{DEVICE_NAME, 0 },	
};
	
struct i2c_driver adc_driver = {
	.driver = {
		.name = DEVICE_NAME,
	},
	.probe = adc_probe,
	.remove = adc_remove,
	.id_table = adc_id,
};
	
static int __init adc_init(void)
{
	int ret;
	ret = i2c_add_driver(&adc_driver);
	
	if (ret) {
		printk(KERN_INFO "fail to add i2c driver\n");
		return -1;
	}
	
	return 0;
}
	
static void __exit adc_exit(void)
{	
	i2c_del_driver(&adc_driver);
	dev_t devno = MKDEV(I2C_MAJOR,I2C_MINOR);
	unregister_chrdev_region(devno,I2C_NUM);
	class_destroy(i2c.class);
	device_destroy(i2c.class,devno);
}
	
MODULE_LICENSE("GPL");
MODULE_AUTHOR("only");
module_init(adc_init);
module_exit(adc_exit);
