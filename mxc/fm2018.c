/*
*file:fm2018.c
*function:fm2018 driver
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
#include <linux/delay.h>
#include <linux/semaphore.h> 
#include <linux/gpio.h>
#include "fm2018.h"

#define IMX_GPIO_NR(bank, nr)		(((bank) - 1) * 32 + (nr))

#define FM_PWD	IMX_GPIO_NR(1, 2)
#define FM_RST IMX_GPIO_NR(6, 14)
#define FM_BYPASS IMX_GPIO_NR(6, 15)
	
#define DEVICE_NAME "fm2018"

#define BYPASS  0
#define HANDSET 1
#define HANDFREE 2
#define INIT     3
#define TEST_MAIN_MIC 4
#define TEST_SUB_MIC 5

struct i2c_client *fm2018_client;

static int fm2018init()
{
	struct i2c_msg msgs;
	int ret;
	/*SET RST PWD BYPASS LOW*/
	gpio_request(FM_RST, "FM_RST_GPIO");
	gpio_direction_output(FM_RST, 0);
	gpio_request(FM_PWD, "FM_PWD_GPIO");
	gpio_direction_output(FM_PWD, 0);
	gpio_request(FM_BYPASS, "FM_BYPASS_GPIO");
	gpio_direction_output(FM_BYPASS, 0);

	/*set pwd high*/
	gpio_set_value(FM_PWD, 1);
	/*wait 5ms*/
	mdelay(5);
	/*set rst high*/
	gpio_set_value(FM_RST, 1);
	/*wait 15ms*/
	mdelay(15);

	/*down init para*/
	msgs.addr = fm2018_client->addr;
	msgs.flags = 0;
	msgs.len = TWOMICNR_INIT_PARA_LENGTH;
	msgs.buf = parameter_data_Init;

	ret = i2c_transfer(fm2018_client->adapter, &msgs, 1);
	
	if (ret != 1) {
		printk(KERN_INFO "fail to set fm2018 init parameter %d\n", ret);
		return -1;
	}
	/*wait 108 ms*/
	mdelay(115);
	/*enter sleep mode*/
	gpio_set_value(FM_PWD, 0);	

	mdelay(108);
	printk(KERN_INFO "fm2018 init ok\n");
	return 0;
}

static int fm2018reset()
{
	/*Set BYPASS low */
  	gpio_set_value(FM_BYPASS, 0);

  	/*Set PWD high */
  	gpio_set_value(FM_PWD, 1);

  	/* Set RST low */
  	gpio_set_value(FM_RST, 0);
 	/* Wait 1ms */
  	mdelay(15);

  	/* Set RST high */
 	gpio_set_value(FM_RST, 1);

  	/* Wait 15ms */
  	mdelay(15);

  	return 0;
}

static int fm2018_set_mode(int mode)
{
	struct i2c_msg msgs;
	int ret;
	switch (mode) {
		case BYPASS:
		printk(KERN_INFO "fm2018_set_mode*****************************\n");
			msgs.len = TWOMICNR_BYPASS_PARA_LENGTH;
			msgs.buf = parameter_data_bypass_mode;
			break;
		case INIT:
			msgs.len = TWOMICNR_INIT_PARA_LENGTH;
			msgs.buf = parameter_data_Init;
			break;
		case HANDSET:
			msgs.len = TWOMICNR_HANDSET_PARA_LENGTH;
			msgs.buf = parameter_data_handset_mode;
			break;
		case HANDFREE:
			msgs.len = TWOMICNR_HANDFREE_PARA_LENGTH;
			msgs.buf = parameter_data_handfree_mode;
			break;
		case TEST_MAIN_MIC:
			msgs.len = TWOMICNR_TEST_MAIN_PARA_LENGTH;
			msgs.buf = parameter_data_TEST_MAIN_MIC;
			break;
		case TEST_SUB_MIC:
			msgs.len = TWOMICNR_TEST_SUB_PARA_LENGTH;
			msgs.buf = parameter_data_TEST_SUB_MIC;
			break;
		default:
			break;
	}
	
	msgs.addr = fm2018_client->addr;
	msgs.flags = 0;
	ret = i2c_transfer(fm2018_client->adapter, &msgs, 1);

	if (ret != 1) {
		printk(KERN_INFO "fail to set fm2018 mode para %d\n", ret);
		return -1;
	}

	mdelay(108);
	gpio_set_value(FM_PWD, 0);	
	gpio_free(FM_PWD);
	printk(KERN_INFO "fm2018 set mode ok\n");
	
	return 0;
}

int fm2018_hw_init(void)
{	
	/*init fm2018 gpio status*/
	if (fm2018init() <  0) {
		printk(KERN_INFO "fail to init fm2018\n");
		return -1;
	}
	/*reset chip*/
	fm2018reset();

	/*set chip to mode*/
	if (fm2018_set_mode(HANDFREE) < 0) {
		printk(KERN_INFO "fail to set fm2018 mode\n");
		return -1;
	}

	//mdelay(100);
	//gpio_set_value(FM_BYPASS, 1);
	
	return 0;
}
EXPORT_SYMBOL_GPL(fm2018_hw_init);

static int fm2018_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;
	printk(KERN_INFO "***********fm2018_probe\n");
	fm2018_client = client;
	/*init fm2018*/
	if ((ret = fm2018_hw_init()) < 0) {
		return ret;	
	}
	
	return 0;
}
	
static int fm2018_remove(struct i2c_client *client)
{
	return 0;	
}
	
static const struct i2c_device_id fm2018_id[] = {
	{DEVICE_NAME, 0 },	
};
	
struct i2c_driver fm2018_driver = {
	.driver = {
		.name = DEVICE_NAME,
	},
	.probe = fm2018_probe,
	.remove = fm2018_remove,
	.id_table = fm2018_id,
};
	
static int __init fm2018_init(void)
{
	int ret;
	ret = i2c_add_driver(&fm2018_driver);
	
	if (ret) {
		printk(KERN_INFO "fail to add i2c driver\n");
		return -1;
	}
	
	return 0;	
}
	
static void __exit fm2018_exit(void)
{	
	i2c_del_driver(&fm2018_driver);
}
	
MODULE_LICENSE("GPL");
MODULE_AUTHOR("only");
module_init(fm2018_init);
module_exit(fm2018_exit);
