/* 
 * drivers/input/touchscreen/ft5x0x_ts.c
 *
 * FocalTech ft5x0x TouchScreen driver. 
 *
 * Copyright (c) 2013  tocel tech Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * note: only support mulititouch
 */
#include <linux/i2c.h>
#include <linux/input.h>
#include "ft5x16.h"
#include <linux/earlysuspend.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <asm/gpio.h>
#include <asm/delay.h>
#include <linux/i2c.h>

#define IMX_GPIO_NR(bank, nr)		(((bank) - 1) * 32 + (nr))

#define TOUCH_RST      IMX_GPIO_NR(2, 5)


static struct i2c_client *this_client = NULL;
static struct ft5x0x_ts_platform_data *pdata;
struct input_dev *input_monior = NULL;

#define CONFIG_FT5X0X_MULTITOUCH 1

struct ts_event {
	u16 x[5];
	u16 y[5];
	u16	pressure;
    u8  touch_point;
};

struct ft5x0x_ts_data {
	struct input_dev	*input_dev;
	struct ts_event		event;
	struct work_struct 	pen_event_work;
	struct workqueue_struct *ts_workqueue;
	struct tasklet_struct ts_task;
	struct early_suspend	early_suspend;
	struct spinlock lock;
};

/***********************************************************************************************
Name	:	ft5x0x_i2c_rxdata 

Input	:	*rxdata
                     *length

Output	:	ret

function	:	

***********************************************************************************************/
static int ft5x0x_i2c_rxdata(char *rxdata, int length)
{
	int ret;
	unsigned char addr = 0;
	struct i2c_msg msgs[] = {
		{
			.addr	= this_client->addr,
			.flags	= 0,
			.len	= 1,
			.buf	= &addr,
		},
		{
			.addr	= this_client->addr,
			.flags	= I2C_M_RD,
			.len	= length,
			.buf	= rxdata,
		},
	};

	ret = i2c_transfer(this_client->adapter, msgs, 2);
	if (ret < 0)
		pr_err("msg %s i2c read error: %d\n", __func__, ret);
	
	return ret;
}

static int ft5x0x_i2c_txdata(char *txdata, int length)
{
	int ret;

	struct i2c_msg msg[] = {
		{
			.addr	= this_client->addr,
			.flags	= 0,
			.len	= length,
			.buf	= txdata,
		},
	};

	ret = i2c_transfer(this_client->adapter, msg, 1);
	if (ret < 0)
		pr_err("%s i2c write error: %d\n", __func__, ret);

	return ret;
}
/***********************************************************************************************
Name	:	 ft5x0x_write_reg

Input	:	addr -- address
                     para -- parameter

Output	:	

function	:	write register of ft5x0x

***********************************************************************************************/
static int ft5x0x_write_reg(u8 addr, u8 para)
{
    u8 buf[3];
    int ret = -1;

    buf[0] = addr;
    buf[1] = para;
    ret = ft5x0x_i2c_txdata(buf, 2);
    if (ret < 0) {
        pr_err("write reg failed! %#x ret: %d", buf[0], ret);
        return -1;
    }
    
    return 0;
}


/***********************************************************************************************
Name	:	ft5x0x_read_reg 

Input	:	addr
                     pdata

Output	:	

function	:	read register of ft5x0x

***********************************************************************************************/
static int ft5x0x_read_reg(u8 addr, u8 *pdata)
{
	int ret;
	u8 buf[2] = {0};

	struct i2c_msg msgs[] = {
		{
			.addr	= this_client->addr,
			.flags	= 0,
			.len	= 1,
			.buf	= &addr,
		},
		{
			.addr	= this_client->addr,
			.flags	= I2C_M_RD,
			.len	= 1,
			.buf	= buf,
		},
	};

    msleep(1);
	ret = i2c_transfer(this_client->adapter, msgs, 2);
	if (ret != 2)
		printk(KERN_INFO "msg %s i2c read reg error: %d\n", __func__, ret);

	*pdata = buf[0];
	return ret;
}

unsigned char ft5x0x_read_fw_ver(void)
{
	unsigned char ver;
	
	ft5x0x_read_reg(FT5X0X_REG_FIRMID, &ver);

	printk(KERN_INFO "*******************FT5X16 ID:%d\n", ver);
	return ver;
}
EXPORT_SYMBOL_GPL(ft5x0x_read_fw_ver);

static void ft5x0x_ts_release(void)
{
	struct ft5x0x_ts_data *data = i2c_get_clientdata(this_client);

	//input_report_abs(data->input_dev, ABS_MT_TRACKING_ID, 0);
	input_report_key(data->input_dev, BTN_TOUCH, 0);
	input_report_abs(data->input_dev, ABS_MT_WIDTH_MAJOR, 0);
	input_report_abs(data->input_dev, ABS_MT_TOUCH_MAJOR, 0);
	input_mt_sync(data->input_dev);
	
	input_sync(data->input_dev);
}

static int ft5x0x_read_data(void)
{
	struct ft5x0x_ts_data *data = i2c_get_clientdata(this_client);
	struct ts_event *event = &data->event;
	u8 buf[32] = {0};
	int ret = -1;

	ret = ft5x0x_i2c_rxdata(buf, 31);

    if (ret < 0) {
		printk(KERN_INFO "%s read_data i2c_rxdata failed: %d\n", __func__, ret);
		return ret;
	}

	memset(event, 0, sizeof(struct ts_event));

	event->touch_point = buf[2] & 0x07;

	//printk(KERN_INFO "touch_point = %d\n", event->touch_point);
	
    if (event->touch_point == 0) {
        ft5x0x_ts_release();
        return 1; 
    }

    switch (event->touch_point) {
		case 5:
			event->x[4] = (s16)(buf[0x1b] & 0x0F)<<8 | (s16)buf[0x1c];
			event->y[4] = (s16)(buf[0x1d] & 0x0F)<<8 | (s16)buf[0x1e];
		case 4:
			event->x[3] = (s16)(buf[0x15] & 0x0F)<<8 | (s16)buf[0x16];
			event->y[3] = (s16)(buf[0x17] & 0x0F)<<8 | (s16)buf[0x18];
		case 3:
			event->x[2] = (s16)(buf[0x0f] & 0x0F)<<8 | (s16)buf[0x10];
			event->y[2] = (s16)(buf[0x11] & 0x0F)<<8 | (s16)buf[0x12];
		case 2:
			event->x[1] = (s16)(buf[9] & 0x0F)<<8 | (s16)buf[10];
			event->y[1] = (s16)(buf[11] & 0x0F)<<8 | (s16)buf[12];
		case 1:
			event->x[0] = (s16)(buf[3] & 0x0F)<<8 | (s16)buf[4];
			event->y[0] = (s16)(buf[5] & 0x0F)<<8 | (s16)buf[6];
            break;
		default:
		    return -1;
	}

    event->pressure = 1;

    return 0;
}

static void ft5x0x_report_value(void)
{
	struct ft5x0x_ts_data *data = i2c_get_clientdata(this_client);
	struct ts_event *event = &data->event;
	int i;

	//input_report_abs(data->input_dev, ABS_MT_TRACKING_ID, event->touch_point);
	input_report_key(data->input_dev, BTN_TOUCH, 1);
	for (i = 0; i < event->touch_point; i++) {
		//input_report_abs(data->input_dev, ABS_MT_TRACKING_ID, i);
		input_report_abs(data->input_dev, ABS_MT_TOUCH_MAJOR, event->pressure);
		input_report_abs(data->input_dev, ABS_MT_POSITION_X, event->x[i]);
		input_report_abs(data->input_dev, ABS_MT_POSITION_Y, event->y[i]);
		//printk(KERN_INFO "point[%d] x : %d Y : %d\n", i, event->x[i], event->y[i]);
		input_report_abs(data->input_dev, ABS_MT_WIDTH_MAJOR, 1);

		input_mt_sync(data->input_dev);
	}
	input_sync(data->input_dev);/**/

}	/*end ft5x0x_report_value*/

static void ft5x0x_ts_pen_irq_work(struct work_struct *work)
{
	int ret = -1;
	
	ret = ft5x0x_read_data();
	
	if (!ret) {	
		ft5x0x_report_value();
	}
    enable_irq(this_client->irq);
}

static irqreturn_t ft5x0x_ts_interrupt(int irq, void *dev_id)
{
	struct ft5x0x_ts_data *data = i2c_get_clientdata(this_client);

    disable_irq_nosync(this_client->irq);	

	if (!work_pending(&data->pen_event_work)) {
		queue_work(data->ts_workqueue, &data->pen_event_work);
	}

	return IRQ_HANDLED;
}
#ifdef CONFIG_HAS_EARLYSUSPEND

static void ft5x0x_ts_suspend(struct early_suspend *handler)
{
	printk("ft5x0x_ts_suspend\n");
	disable_irq(this_client->irq);
}

static void ft5x0x_ts_resume(struct early_suspend *handler)
{
	printk("ft5x0x_ts_resume\n");

	enable_irq(this_client->irq);
}
#endif  //CONFIG_HAS_EARLYSUSPEND

static int ft5x0x_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct ft5x0x_ts_data *ft5x0x_ts;
	struct input_dev *input_dev;
	int err = 0;
	int ret;
	unsigned char uc_reg_value; 
	
	printk(KERN_INFO "*******************ft5x0x_ts_probe\n");
	
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		err = -ENODEV;
		goto exit_check_functionality_failed;
	}


	ret = gpio_request(TOUCH_RST, "TOUCH_RST_GPIO");

	if (ret < 0) {
		printk(KERN_INFO "GPIO REQUEST FAIL TOUCH_RST_GPIO % d\n", ret);
		goto exit_check_functionality_failed;
	}
	gpio_direction_output(TOUCH_RST, 1);

	gpio_set_value(TOUCH_RST, 0);
	mdelay(50);
	gpio_set_value(TOUCH_RST, 1);
	
	ft5x0x_ts = (struct ft5x0x_ts_data *)kzalloc(sizeof(*ft5x0x_ts), GFP_KERNEL);
	if (!ft5x0x_ts)	{
		err = -ENOMEM;
		goto exit_alloc_data_failed;
	}

	this_client = client;
	i2c_set_clientdata(client, ft5x0x_ts);

	INIT_WORK(&ft5x0x_ts->pen_event_work, ft5x0x_ts_pen_irq_work);

	ft5x0x_ts->ts_workqueue = create_singlethread_workqueue(dev_name(&client->dev));
	if (!ft5x0x_ts->ts_workqueue) {
		err = -ESRCH;
		goto exit_create_singlethread;
	}
	
	err = request_irq(this_client->irq, ft5x0x_ts_interrupt, IRQF_TRIGGER_FALLING | IRQF_ONESHOT, "ft5x0x_ts", ft5x0x_ts);
	if (err < 0) {
		dev_err(&client->dev, "ft5x0x_probe: request irq failed\n");
		goto exit_irq_request_failed;
	}

	disable_irq(this_client->irq);
	input_dev = input_allocate_device();
	if (!input_dev) {
		err = -ENOMEM;
		dev_err(&client->dev, "failed to allocate input device\n");
		goto exit_input_dev_alloc_failed;
	}
	
	ft5x0x_ts->input_dev = input_dev;
	input_monior = input_dev;/*for keyboard*/

	set_bit(ABS_MT_TOUCH_MAJOR, input_dev->absbit);
	set_bit(ABS_MT_POSITION_X, input_dev->absbit);
	set_bit(ABS_MT_POSITION_Y, input_dev->absbit);
	set_bit(ABS_MT_WIDTH_MAJOR, input_dev->absbit);
	set_bit(BTN_TOUCH, input_dev->keybit);
//	set_bit(ABS_MT_TRACKING_ID, input_dev->absbit);
	
	set_bit(EV_ABS, input_dev->evbit);
	set_bit(EV_KEY, input_dev->evbit);

	input_set_abs_params(input_dev, ABS_MT_POSITION_X, 0, SCREEN_MAX_X, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y, 0, SCREEN_MAX_Y, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0, 1, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_WIDTH_MAJOR, 0, 1, 0, 0);
	
	//input_set_abs_params(input_dev, ABS_MT_TRACKING_ID, 0, 5, 0, 0);

	input_dev->name = "eGalax Touch Screen";		//FT5X0X_NAME;

	err = input_register_device(input_dev);
	if (err) {
		dev_err(&client->dev,
		"ft5x0x_ts_probe: failed to register input device: %s\n",
		dev_name(&client->dev));
		goto exit_input_register_device_failed;
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	printk(KERN_INFO "==register_early_suspend =\n");
	ft5x0x_ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	ft5x0x_ts->early_suspend.suspend = ft5x0x_ts_suspend;
	ft5x0x_ts->early_suspend.resume	= ft5x0x_ts_resume;
	register_early_suspend(&ft5x0x_ts->early_suspend);
#endif

    msleep(50);
    //get some register information
    uc_reg_value = ft5x0x_read_fw_ver();
    printk(KERN_INFO "ft5x16 Firmware version = 0x%x\n", uc_reg_value);

	enable_irq(this_client->irq);

	printk(KERN_INFO "load tp driver succcess!\n");
    return 0;

exit_input_register_device_failed:
	input_free_device(input_dev);
exit_input_dev_alloc_failed:

	free_irq(this_client->irq, ft5x0x_ts);
exit_irq_request_failed:
exit_platform_data_null:
	cancel_work_sync(&ft5x0x_ts->pen_event_work);
	destroy_workqueue(ft5x0x_ts->ts_workqueue);
exit_create_singlethread:
	i2c_set_clientdata(client, NULL);
	kfree(ft5x0x_ts);
exit_alloc_data_failed:
exit_check_functionality_failed:
	return err;
}

static int __devexit ft5x0x_ts_remove(struct i2c_client *client)
{
	struct ft5x0x_ts_data *ft5x0x_ts = i2c_get_clientdata(client);
	unregister_early_suspend(&ft5x0x_ts->early_suspend);
	free_irq(this_client->irq, ft5x0x_ts);
	input_unregister_device(ft5x0x_ts->input_dev);
	kfree(ft5x0x_ts);
	input_monior = NULL;
	cancel_work_sync(&ft5x0x_ts->pen_event_work);
	destroy_workqueue(ft5x0x_ts->ts_workqueue);
	i2c_set_clientdata(client, NULL);
	return 0;
}

static const struct i2c_device_id ft5x0x_ts_id[] = {
	{ FT5X0X_NAME, 0 },
};

MODULE_DEVICE_TABLE(i2c, ft5x0x_ts_id);

static struct i2c_driver ft5x0x_ts_driver = {
	.probe		= ft5x0x_ts_probe,
	.remove		= __devexit_p(ft5x0x_ts_remove),
	.id_table	= ft5x0x_ts_id,
	.driver	= {
		.name	= FT5X0X_NAME,
		.owner	= THIS_MODULE,
	},
};
static int __init ft5x0x_ts_init(void)
{
	int ret;

	//printk(KERN_INFO "***********ft5x0x_ts_init\n");
	ret = i2c_add_driver(&ft5x0x_ts_driver);

	return ret;
}

static void __exit ft5x0x_ts_exit(void)
{
	i2c_del_driver(&ft5x0x_ts_driver);
}

module_init(ft5x0x_ts_init);
module_exit(ft5x0x_ts_exit);

MODULE_AUTHOR("<wenfs@Focaltech-systems.com>");
MODULE_DESCRIPTION("FocalTech ft5x0x TouchScreen driver");
MODULE_LICENSE("GPL");
