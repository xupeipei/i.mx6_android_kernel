/*
 * Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mxcfb.h>
#include <linux/fsl_devices.h>
#include "mxc_dispdrv.h"
#include <linux/delay.h>
#include <linux/spi/spi.h>/*spi*/
#include <linux/gpio.h>
#include <linux/earlysuspend.h>


#define IMX_GPIO_NR(bank, nr)		(((bank) - 1) * 32 + (nr))

#define LCD_CMD	IMX_GPIO_NR(2, 0)
#define LCD_RST IMX_GPIO_NR(4, 20)
#define LCD_EN  IMX_GPIO_NR(6,11)
//#define KEYBOARD_EN IMX_GPIO_NR(6,10)

struct spi_device *gl_spi = NULL;


struct mxc_lcdif_data {
	struct platform_device *pdev;
	struct mxc_dispdrv_handle *disp_lcdif;
	struct early_suspend	early_suspend; 
};

#define DISPDRV_LCD	"lcd"

static struct fb_videomode lcdif_modedb[] = {
	{
	/* 320*480 @ 60Hz , pixel clk @ 20MHz */
	"HVGA", 60, 320, 480, 80000, 3, 3, 2, 2, 2, 2,
	FB_SYNC_CLK_LAT_FALL,
	FB_VMODE_NONINTERLACED,
	0,},
};
static int lcdif_modedb_sz = ARRAY_SIZE(lcdif_modedb);

static int lcdif_init(struct mxc_dispdrv_handle *disp,
	struct mxc_dispdrv_setting *setting)
{
	int ret, i;
	struct mxc_lcdif_data *lcdif = mxc_dispdrv_getdata(disp);
	struct fsl_mxc_lcd_platform_data *plat_data
			= lcdif->pdev->dev.platform_data;
	struct fb_videomode *modedb = lcdif_modedb;
	int modedb_sz = lcdif_modedb_sz;

	/* use platform defined ipu/di */
	setting->dev_id = plat_data->ipu_id;
	setting->disp_id = plat_data->disp_id;

	ret = fb_find_mode(&setting->fbi->var, setting->fbi, setting->dft_mode_str,
				modedb, modedb_sz, NULL, setting->default_bpp);
	if (!ret) {
		fb_videomode_to_var(&setting->fbi->var, &modedb[0]);
		setting->if_fmt = plat_data->default_ifmt;
	}

	INIT_LIST_HEAD(&setting->fbi->modelist);
	for (i = 0; i < modedb_sz; i++) {
		struct fb_videomode m;
		fb_var_to_videomode(&m, &setting->fbi->var);
		if (fb_mode_is_equal(&m, &modedb[i])) {
			fb_add_videomode(&modedb[i],
					&setting->fbi->modelist);
			break;
		}
	}

	return ret;
}

void lcdif_deinit(struct mxc_dispdrv_handle *disp)
{
	/*TODO*/
}

static struct mxc_dispdrv_driver lcdif_drv = {
	.name 	= DISPDRV_LCD,
	.init 	= lcdif_init,
	.deinit	= lcdif_deinit,
};

/*write cmd to lcd control*/
static int lcd_write_cmd(struct spi_device *spi, char cmd)
{
	struct spi_transfer t;
	struct spi_message m;
	int ret;
	char buffer[2] = {0};

	gpio_set_value(LCD_CMD, 0);
	memset(&t, 0, sizeof(t));
	spi_message_init(&m);
	
	t.bits_per_word = 8;
	t.len = 1;             /*write one byte cmd*/
	t.cs_change =0;
	buffer[0] = cmd;
	t.tx_buf = buffer;    

	spi_message_add_tail(&t, &m);

	ret = spi_sync(spi, &m);
	
	if (ret < 0)
		printk(KERN_INFO "fail to write lcd cmd\n");
	return ret;
}

/*write data to lcd control*/
static int lcd_write_data(struct spi_device *spi, char data)
{
	struct spi_transfer t;
	struct spi_message m;
	int ret;
	char buffer[2] = {0};
	
	gpio_set_value(LCD_CMD, 1);
	memset(&t, 0, sizeof(t));
	spi_message_init(&m);
	
	t.bits_per_word = 8;
	t.len = 1;             /*write one byte data*/
	t.cs_change =0;
	buffer[0] = data;
	t.tx_buf = buffer;    

	spi_message_add_tail(&t, &m);

	ret = spi_sync(spi, &m);

	
	if (ret < 0)
		printk(KERN_INFO "fail to write lcd data\n");
	return ret;
}

/*spi probe init lcd*/
static int spi_lcd_probe(struct spi_device *spi)
{
	int r;
	printk(KERN_INFO "**********************************spi_lcd_probe\n");
	gl_spi = spi;
	
	r = gpio_request(LCD_RST, "LCD_RST_GPIO");

	if (r < 0) {
		printk(KERN_INFO "failed to request GPIO for LCD_RST_GPIO ! error = %d\n", r);
		return -1;
	}
	gpio_direction_output(LCD_RST, 0);
	r = gpio_request(LCD_CMD, "LCD_CMD_GPIO");

	if (r < 0) {
		printk(KERN_INFO "failed to request GPIO for LCD_CMD_GPIO ! error = %d\n", r);
		return -1;
	}
	gpio_direction_output(LCD_CMD, 0);
	
	gpio_set_value(LCD_RST, 1);
   	msleep(1);
   	gpio_set_value(LCD_RST, 0);
   	msleep(10);
   	gpio_set_value(LCD_RST, 1);
  	msleep(150);

#if 1
	lcd_write_cmd(spi, 0xE0); 
	lcd_write_data(spi, 0x00); 
	lcd_write_data(spi, 0x14); 
	lcd_write_data(spi, 0x1b); 
	lcd_write_data(spi, 0x02); 
	lcd_write_data(spi, 0x10); 
	lcd_write_data(spi, 0x06); 
	lcd_write_data(spi, 0x3e); 
	lcd_write_data(spi, 0x56); 
	lcd_write_data(spi, 0x53); 
	lcd_write_data(spi, 0x05); 
	lcd_write_data(spi, 0x0D); 
	lcd_write_data(spi, 0x0B); 
	lcd_write_data(spi, 0x33); 
	lcd_write_data(spi, 0x3a); 
	lcd_write_data(spi, 0x0F);  
	 
	lcd_write_cmd(spi, 0xE1); 
	lcd_write_data(spi, 0x00); 
	lcd_write_data(spi, 0x18); 
	lcd_write_data(spi, 0x1c); 
	lcd_write_data(spi, 0x04); 
	lcd_write_data(spi, 0x11); 
	lcd_write_data(spi, 0x07); 
	lcd_write_data(spi, 0x3f); 
	lcd_write_data(spi, 0x45); 
	lcd_write_data(spi, 0x53); 
	lcd_write_data(spi, 0x07); 
	lcd_write_data(spi, 0x0f); 
	lcd_write_data(spi, 0x0b); 
	lcd_write_data(spi, 0x34); 
	lcd_write_data(spi, 0x37); 
	lcd_write_data(spi, 0x0F); 
	
	lcd_write_cmd(spi, 0xC0); 
	lcd_write_data(spi, 0x10); 
	lcd_write_data(spi, 0x10); 
	 
	lcd_write_cmd(spi, 0xC1); 
	lcd_write_data(spi, 0x44); 
	
	lcd_write_cmd(spi, 0xC5); 
	lcd_write_data(spi, 0x00); 
	lcd_write_data(spi, 0x2A); 
	lcd_write_data(spi, 0x80); 
	
	lcd_write_cmd(spi, 0x36); 
	lcd_write_data(spi, 0x48); 
	
	lcd_write_cmd(spi, 0x3A); //Interface Mode Control
	lcd_write_data(spi, 0x66);
	
	lcd_write_cmd(spi, 0x21); 
	
	lcd_write_cmd(spi, 0XB0);  //Interface Mode Control	
	lcd_write_data(spi, 0x00); 
	
	lcd_write_cmd(spi, 0xB1);   //Frame rate 70HZ  
	lcd_write_data(spi, 0xB0); 
	lcd_write_data(spi, 0x11); 
	
	lcd_write_cmd(spi, 0xB4); 
	lcd_write_data(spi, 0x02); 
	  
	lcd_write_cmd(spi, 0xB6);
	lcd_write_data(spi, 0x02);
	lcd_write_data(spi, 0x02); //
	lcd_write_data(spi, 0x3B);
	
	lcd_write_cmd(spi, 0xB7); 
	lcd_write_data(spi, 0xC6); 
	
	lcd_write_cmd(spi, 0xE9); 
	lcd_write_data(spi, 0x00);
	 
	lcd_write_cmd(spi, 0XF7);	
	lcd_write_data(spi, 0xA9); 
	lcd_write_data(spi, 0x51); 
	lcd_write_data(spi, 0x2C); 
	lcd_write_data(spi, 0x82);
	
	/**********set rgb interface mode******************/
	lcd_write_cmd(spi, 0xB0);
	lcd_write_data(spi, 0x02); 
	
	lcd_write_cmd(spi, 0xB6); 
	lcd_write_data(spi, 0x30); //set rgb
	lcd_write_data(spi, 0x02); //GS,SS
	lcd_write_data(spi, 0x3B);
	 /**************************************************/ 
	lcd_write_cmd(spi, 0x2A); //Frame rate control
	lcd_write_data(spi, 0x00);
	lcd_write_data(spi, 0x00);
	lcd_write_data(spi, 0x01);
	lcd_write_data(spi, 0x3F);
	
	lcd_write_cmd(spi, 0x2B); //Display function control
	lcd_write_data(spi, 0x00);
	lcd_write_data(spi, 0x00);
	lcd_write_data(spi, 0x01);
	lcd_write_data(spi, 0xDF);
	
	lcd_write_cmd(spi, 0x21);
	
	lcd_write_cmd(spi, 0x11);
	mdelay(120);
	lcd_write_cmd(spi, 0x29); //display on
	lcd_write_cmd(spi, 0x2c); 
#endif
	printk(KERN_INFO "load spi lcd driver success!\n");
	return 0;
}

static int __devexit lcd_spi_remove(struct spi_device *spi)
{
	return 0;
}

static int lcd_suspend(struct spi_device *spi, pm_message_t mesg)
{
	//gpio_set_value(LCD_EN, 0);
	return 0;
}

static int  lcd_resume(struct spi_device *spi)
{
	//gpio_set_value(LCD_EN, 1);
	return 0;
}

/*spi id table*/
static const struct spi_device_id lcd_ids[] = 
{
	{"lcd_spi",0},
};

static struct spi_driver lcd_spi_driver = {
	.driver = {
		   .name = "lcd_spi",
		   .bus = &spi_bus_type,
		   .owner = THIS_MODULE,
		   },
	.id_table	= lcd_ids,
	.probe	= spi_lcd_probe,
	.resume = lcd_resume,
	.suspend = lcd_suspend,
	.remove = __devexit_p(lcd_spi_remove),
};

static int __init mxc_spi_init(void)
{
	return spi_register_driver(&lcd_spi_driver);
}

static void __exit mxc_spi_exit(void)
{
	spi_unregister_driver(&lcd_spi_driver);
}

module_init(mxc_spi_init);
module_exit(mxc_spi_exit);

#if CONFIG_HAS_EARLYSUSPEND
static void mxc_suspend(struct early_suspend *handler)
{
	lcd_write_cmd(gl_spi, 0x28);//display off
    mdelay(10);
    lcd_write_cmd(gl_spi, 0x10);
    mdelay(120);
	gpio_set_value(LCD_EN, 0);/*close lcd backlight*/
	//gpio_set_value(KEYBOARD_EN, 0);/*close keyboard backlight*/
}

static void mxc_resume(struct early_suspend *handler)
{
	gpio_set_value(LCD_EN, 1);
	//gpio_set_value(KEYBOARD_EN, 1);
    mdelay(10);
    lcd_write_cmd(gl_spi, 0x11);// Exit Sleep/ Standby mode
    mdelay(120);
    lcd_write_cmd(gl_spi, 0x29);
}
#endif
static int mxc_lcdif_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct mxc_lcdif_data *lcdif;

	lcdif = kzalloc(sizeof(struct mxc_lcdif_data), GFP_KERNEL);
	if (!lcdif) {
		ret = -ENOMEM;
		goto alloc_failed;
	}

	lcdif->pdev = pdev;
	lcdif->disp_lcdif = mxc_dispdrv_register(&lcdif_drv);
	mxc_dispdrv_setdata(lcdif->disp_lcdif, lcdif);

	dev_set_drvdata(&pdev->dev, lcdif);

	ret = gpio_request(LCD_EN, "LCD_GPIO");
	if (ret < 0) {
		printk(KERN_INFO "failed to request GPIO for LCD_EN ! error = %d\n", ret);
		return -1;
	}
	gpio_direction_output(LCD_EN, 1);
#ifdef CONFIG_HAS_EARLYSUSPEND
	printk(KERN_INFO "lcd register_early_suspend \n");
	lcdif->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	lcdif->early_suspend.suspend = mxc_suspend;
	lcdif->early_suspend.resume	= mxc_resume;
	register_early_suspend(&lcdif->early_suspend);
#endif

alloc_failed:
	return ret;
}

static int mxc_lcdif_remove(struct platform_device *pdev)
{
	struct mxc_lcdif_data *lcdif = dev_get_drvdata(&pdev->dev);

	mxc_dispdrv_puthandle(lcdif->disp_lcdif);
	mxc_dispdrv_unregister(lcdif->disp_lcdif);
	kfree(lcdif);
	return 0;
}

static struct platform_driver mxc_lcdif_driver = {
	.driver = {
		   .name = "mxc_lcdif",
		   },
	.probe = mxc_lcdif_probe,
	.remove = mxc_lcdif_remove,
};

static int __init mxc_lcdif_init(void)
{
	int ret;

	ret = platform_driver_register(&mxc_lcdif_driver);
	
	return ret;
}

static void __exit mxc_lcdif_exit(void)
{
	platform_driver_unregister(&mxc_lcdif_driver);
}

module_init(mxc_lcdif_init);
module_exit(mxc_lcdif_exit);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("i.MX ipuv3 LCD extern port driver");
MODULE_LICENSE("GPL");
