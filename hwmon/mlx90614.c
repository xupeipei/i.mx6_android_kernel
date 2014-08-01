/*Infra Red thermometer mlx90614*/
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
#include <linux/ioctl.h>

#define MLX90614_DRIVER_NAME "mlx90614"
#define IMX_GPIO_NR(bank, nr)		(((bank) - 1) * 32 + (nr))

#define INFRA_POWER	      IMX_GPIO_NR(6, 16)
#define FLASH_POWER	      IMX_GPIO_NR(3, 27)
#define FLASH_LED	      IMX_GPIO_NR(1, 9)


//MLX90614 constants
#define SA				0x00	// Slave address
#define DEFAULT_SA		0x5A	// Default Slave address
#define RAM_Access		0x00	// RAM access command
#define EEPROM_Access	0x20	// EEPROM access command
#define RAM_Tobj1		0x07	// To1 address in the eeprom

#define LED_CHAR_BASE		'O'
#define LED_IOWR(num, dtype)	_IOWR(LED_CHAR_BASE, num, dtype)
#define LED_TORCH	LED_IOWR(1, unsigned int)
#define LED_FLASH	LED_IOWR(2, unsigned int)
#define LED_CLOSE   LED_IOWR(3, unsigned int)
#define INFRA_OPEN   LED_IOWR(4, unsigned int)
#define INFRA_CLOSE   LED_IOWR(5, unsigned int)

struct mlx90614_info {
	struct mutex mutex;
	struct miscdevice miscdev;
	struct i2c_client *i2c_dev;
};

struct mlx90614_info *infra = NULL;

unsigned char PEC_calculation(unsigned char pec[])
{
	unsigned char 	crc[6];
	unsigned char	BitPosition=47;
	unsigned char	shift;
	unsigned char	i;
	unsigned char	j;
	unsigned char	temp;

	do{
		crc[5]=0;				/* Load CRC value 0x000000000107 */
		crc[4]=0;
		crc[3]=0;
		crc[2]=0;
		crc[1]=0x01;
		crc[0]=0x07;
		BitPosition=47;			/* Set maximum bit position at 47 */
		shift=0;
				
		//Find first 1 in the transmited message
		i=5;					/* Set highest index */
		j=0;
		while((pec[i]&(0x80>>j))==0 && i>0){
			BitPosition--;
			if(j<7){
				j++;
			}
			else{
				j=0x00;
				i--;
			}
		}/*End of while */
		
		shift=BitPosition-8;	/*Get shift value for crc value*/
		
		
		//Shift crc value 
		while(shift){
			for(i=5; i<0xFF; i--){
				if((crc[i-1]&0x80) && (i>0)){
					temp=1;
				}
				else{
					temp=0;
				}
				crc[i]<<=1;
				crc[i]+=temp;
			}/*End of for*/
			shift--;
		}/*End of while*/
		
		
		//Exclusive OR between pec and crc		
		for(i=0; i<=5; i++){
			pec[i] ^=crc[i];
		}/*End of for*/
	}while(BitPosition>8);/*End of do-while*/
	
	return pec[0];
}/*End of PEC_calculation*/


/*
*	function:send cmd
*	return :temperature data
*/
static unsigned int mlx90614_sendcommand(unsigned char cmd)
{
	unsigned int data = 0;
	unsigned char pec, PecReg;
	unsigned char arr[6];			// Buffer for the sent bytes
	int ret;
	struct i2c_msg msgs[2];
	unsigned char buffer[4] = {0};

	msgs[0].len = 1;
	msgs[0].addr = (SA << 1);
	msgs[0].flags = 0;
	msgs[0].buf = (char*)&cmd;
	msgs[1].len = 3;
	msgs[1].addr = (SA << 1);
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = &buffer;

	ret = i2c_transfer(infra->i2c_dev->adapter, msgs, 2);

	//ret = i2c_smbus_read_block_data(infra->i2c_dev, cmd, buffer);
	
	pec = buffer[2];
	
	arr[5] = (SA << 1);		                             //
	arr[4] = cmd;				                         //
	arr[3] = (SA << 1);		                             //Load array arr 
	arr[2] = buffer[0];				                     //
	arr[1] = buffer[1];				                     //
	arr[0] = 0;					                         //
	PecReg = PEC_calculation(arr);                       //Calculate CRC
	
	if (pec != PecReg) {
		printk(KERN_INFO "*****************MLX90614 data error\n");
		return 0;
	}

	printk(KERN_INFO "ret = %d\n", ret);
	printk(KERN_INFO "PecReg = %d\n", PecReg);
	printk(KERN_INFO "pec = %d\n", pec);
	printk(KERN_INFO "low = %d\n", buffer[0]);
	printk(KERN_INFO "high = %d\n", buffer[1]);
	*((unsigned char *)(&data))= buffer[0];	          // 
	*((unsigned char *)(&data)+1)= buffer[1] ;        //
	
	return data;
}

static ssize_t mlx90614_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	struct mlx90614_info *info = container_of(file->private_data, struct mlx90614_info, miscdev);
	unsigned char	command;	  			//Contains the access command
	unsigned int 	data;		  			//Contains data value	
	
	printk(KERN_INFO "*******************mlx90614_read\n");


	gpio_set_value(INFRA_POWER, 0);
	mdelay(10);
	gpio_set_value(INFRA_POWER, 1);

	
	mutex_lock(&info->mutex);
	
	command = RAM_Access | RAM_Tobj1;
	data = mlx90614_sendcommand(command);

	printk(KERN_INFO "*******************data %d\n", data);
	copy_to_user(buf, (char*)&data, sizeof(data));

	mutex_unlock(&info->mutex);
	
	return 0;
}

static int mlx90614_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "**************mlx90614_open\n");

	/*do some init.....*/
	return 0;
}

static int mlx90614_close(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "**************mlx90614_close\n");
	
	/*enter sleep mode*/
	return 0;
}

static long flash_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd)
	{
		case LED_TORCH:
			gpio_set_value(FLASH_POWER, 1);
			gpio_set_value(FLASH_LED, 0);
			break;
		case LED_FLASH:
			gpio_set_value(FLASH_POWER, 1);
			gpio_set_value(FLASH_LED, 1);
			break;
		case LED_CLOSE:
			gpio_set_value(FLASH_POWER, 0);
			gpio_set_value(FLASH_LED, 0);
			break;
		case INFRA_OPEN:
			gpio_set_value(INFRA_POWER, 1);
			/*wake up from sleep mode*/
			mdelay(1000);
			break;
			
		case INFRA_CLOSE:
			gpio_set_value(INFRA_POWER, 0);
			break;
		default:
			//gpio_set_value(FLASH_POWER, 0);		
			//gpio_set_value(FLASH_LED, 0);
			break;
	}
}

static const struct file_operations mlx90614_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.read		= mlx90614_read,
	.open		= mlx90614_open,
	.release	= mlx90614_close,
	.unlocked_ioctl  = flash_ioctl,
};

static int __devinit mlx90614_probe(struct i2c_client *client,
				 const struct i2c_device_id *id)
{
	struct mlx90614_info *info;
	int r = 0;

	printk(KERN_INFO "***********************mlx90614_probe\n");
	/* private data allocation */
	info = kzalloc(sizeof(struct mlx90614_info), GFP_KERNEL);
	if (!info) {
		r = -ENOMEM;
		goto err_info_alloc;
	}
	info->i2c_dev = client;
	infra = info;
	mutex_init(&info->mutex);

	i2c_set_clientdata(client, info);
		
	r = gpio_request(INFRA_POWER, "MLX90614_POWER");
	if (r < 0) {
		printk(KERN_ERR "failed to request GPIO for MLX90614 POWER ! error = %d\n", r);
		goto err_kmalloc;
	}

	r = gpio_direction_output(INFRA_POWER, 0);
	if (r < 0) {
		printk(KERN_INFO "fail to set direction of MLX90614 power GPIO output! error = %d\n", r);
		goto err_kmalloc;
	}

	
	r = gpio_request(FLASH_POWER, "FLASH_POWER");
	if (r < 0) {
		printk(KERN_ERR "failed to request GPIO for FLASH_POWER! error = %d\n", r);
		goto err_kmalloc;
	}

	r = gpio_direction_output(FLASH_POWER, 0);
	if (r < 0) {
		printk(KERN_INFO "fail to set direction of FLASH_POWER GPIO output! error = %d\n", r);
		goto err_kmalloc;
	}
	

	r = gpio_request(FLASH_LED, "FLASH_LED");
	if (r < 0) {
		printk(KERN_ERR "failed to request GPIO for FLASH_LED ! error = %d\n", r);
		goto err_kmalloc;
	}

	r = gpio_direction_output(FLASH_LED, 0);
	if (r < 0) {
		printk(KERN_INFO "fail to set direction of FLASH_LED GPIO output! error = %d\n", r);
		goto err_kmalloc;
	}

	//gpio_set_value(FLASH_POWER, 1);
	info->miscdev.minor = MLX90614_MINOR;
	info->miscdev.name = MLX90614_DRIVER_NAME;
	info->miscdev.fops = &mlx90614_fops;
	info->miscdev.parent = &client->dev;
	r = misc_register(&info->miscdev);
	if (r < 0) {
		printk(KERN_INFO "failed to register mlx90614\n");
		goto err_kmalloc;
	}
	/*enter sleep mode*/
	mdelay(1000);

	printk(KERN_INFO "***************mlx90614 load success\n");
	
	return 0;
err_kmalloc:
	kfree(info);
err_info_alloc:
	return r;
}

static struct i2c_device_id mlx90614_id_table [] ={
	{MLX90614_DRIVER_NAME,0}
};

static __devexit int mlx90614_remove(struct i2c_client *client)
{
	struct mlx90614_info *info = i2c_get_clientdata(client);

	misc_deregister(&info->miscdev);
	kfree(info);
	gpio_free(INFRA_POWER);
	
	infra = NULL;
	return 0;
}

static struct i2c_driver mlx90614_driver = {
	.driver = {
		.name = MLX90614_DRIVER_NAME,
	},
	.probe = mlx90614_probe,
	.id_table = mlx90614_id_table,
	.remove = __devexit_p(mlx90614_remove),
};

static int __init mlx90614_init(void)
{
	int r;

	r = i2c_add_driver(&mlx90614_driver);
	if (r) {
		printk(KERN_INFO "fail to add i2c drive mlx90614\n");
		return r;
	}
	
	return 0;
}

static void __exit mlx90614_exit(void)
{
	i2c_del_driver(&mlx90614_driver);
}

module_init(mlx90614_init);
module_exit(mlx90614_exit);

MODULE_LICENSE("GPL");
