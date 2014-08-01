/*
 * Driver for the PN532 NFC chip.
 * tocel 2013 
 */
#include <linux/completion.h>
#include <linux/crc-ccitt.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/nfc/pn532.h>
#include <linux/poll.h>
#include <linux/regulator/consumer.h>
#include <linux/serial_core.h> /* for TCGETS */
#include <linux/slab.h>
#include <asm/gpio.h>
#include <asm/delay.h>
#include <linux/i2c.h>

#define DRIVER_CARD	"PN532 NFC"
#define DRIVER_DESC	"NFC driver for PN532"

#define IMX_GPIO_NR(bank, nr)		(((bank) - 1) * 32 + (nr))

#define GPIO_NFC_WAKEUP	IMX_GPIO_NR(7, 12)
#define GPIO_NFC_IRQ	IMX_GPIO_NR(2, 2)
#define GPIO_NFC_EN     IMX_GPIO_NR(2, 1)

#define PN532_PACKBUFFSIZ 64

char pn532ack[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
char pn532response_firmwarevers[] = {0x00, 0xFF, 0x06, 0xFA, 0xD5, 0x03};

char pn532_packetbuffer[PN532_PACKBUFFSIZ];

unsigned char uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
unsigned char uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

static struct i2c_device_id pn532_id_table[] = {
	{ PN532_DRIVER_NAME, 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, pn532_id_table);

struct pn532_info {
	struct mutex mutex;
	struct miscdevice miscdev;
	struct i2c_client *i2c_dev;
};

struct pn532_info *pn532 = NULL;
/**************************************************************************/
/*! 
    @brief  Prints a hexadecimal value in plain characters

    @param  data      Pointer to the byte data
    @param  numBytes  Data length in bytes
*/
/**************************************************************************/
void PrintHex(const char * data, const unsigned int numBytes)
{
	unsigned int szPos;
	for (szPos=0; szPos < numBytes; szPos++) 
	{		
		printk(KERN_INFO "0x%x ", data[szPos]&0xff);

		if ((numBytes > 1) && (szPos != numBytes - 1))
		{
			printk(KERN_INFO " ");
		}
	}
	printk(KERN_INFO "\n");
}

/**************************************************************************/
/*! 
    @brief  Prints a hexadecimal value in plain characters, along with
            the char equivalents in the following format

            00 00 00 00 00 00  ......

    @param  data      Pointer to the byte data
    @param  numBytes  Data length in bytes
*/
/**************************************************************************/
void PrintHexChar(const char * data, const unsigned int numBytes)
{
	unsigned int szPos;
	
	for (szPos=0; szPos < numBytes; szPos++) 
	{
		printk(KERN_INFO "0x%x", data[szPos]);
		if ((numBytes > 1) && (szPos != numBytes - 1))
		{
			printk(KERN_INFO " ");
		}
	}
	printk(KERN_INFO "  ");
	for (szPos=0; szPos < numBytes; szPos++) 
	{
		if (data[szPos] <= 0x1F)
			printk(KERN_INFO ".");
		else
			printk(KERN_INFO "%d", (char)data[szPos]);
	}
	printk(KERN_INFO "");
}

/**************************************************************************/
/*! 
    @brief  Writes a command to the PN532, automatically inserting the
            preamble and required frame details (checksum, len, etc.)

    @param  cmd       Pointer to the command buffer
    @param  cmdlen    Command length in bytes 
*/
/**************************************************************************/
void wiresendcommand(unsigned char* cmd, unsigned char cmdlen)
 {
	unsigned char checksum;
	unsigned char i;
	int ret;
	char buf[64] = {0};
	  
	cmdlen++;
	  
	mdelay(2);                // or whatever the delay is for waking up the board
	
	checksum = PN532_PREAMBLE + PN532_PREAMBLE + PN532_STARTCODE2;
	buf[0] = PN532_PREAMBLE;
	buf[1] = PN532_PREAMBLE;
	buf[2] = PN532_STARTCODE2;
	buf[3] = cmdlen;
	buf[4] = ~cmdlen + 1;
	buf[5] = PN532_HOSTTOPN532;
	checksum += PN532_HOSTTOPN532;
	for (i = 0; i < cmdlen - 1; i++) {
		buf[6 + i] = cmd[i];
	    checksum += cmd[i];
	}
	buf[5 + cmdlen] = ~checksum;
	buf[6 + cmdlen] = PN532_POSTAMBLE;
	ret = i2c_master_send(pn532->i2c_dev, buf, 7 + cmdlen);
	
	if (ret != (7 + cmdlen)) {
		printk(KERN_INFO "fail to send nfc cmd\n");
	}
} 

/**************************************************************************/
/*! 
    @brief  Checks the IRQ pin to know if the PN532 is ready
	
	@returns 0 if the PN532 is busy, 1 if it is free
*/
/**************************************************************************/
unsigned char wirereadstatus(void) 
{
  	unsigned char ret = gpio_get_value(GPIO_NFC_IRQ);
  
 	if (ret == 1)
    	return PN532_I2C_BUSY;
  	else
    	return PN532_I2C_READY;
}

/**************************************************************************/
/*! 
    @brief  Reads n bytes of data from the PN532 via I2C

    @param  buff      Pointer to the buffer where data will be written
    @param  n         Number of bytes to be read
*/
/**************************************************************************/
void wirereaddata(unsigned char* buff, unsigned char n)
 {
	int ret;
	unsigned char i;
	unsigned char buffer[64] = {0};
	mdelay(2); 
	ret = i2c_master_recv(pn532->i2c_dev, buffer,  n + 1);
	
	if (ret != (n + 1))  {
		printk(KERN_INFO "read data failed!\n");
		
		return ;
	}
	
	for (i = 0; i < n; i++) {
		buff[i] = buffer[i + 1];
	}
}

int readackframe(void)
{
	unsigned char ackbuff[6];
  
	wirereaddata(ackbuff, 6);
    
	return (0 == strncmp((char *)ackbuff, (char *)pn532ack, 6));
}

/**************************************************************************/
/*! 
    @brief  Sends a command and waits a specified period for the ACK

    @param  cmd       Pointer to the command buffer
    @param  cmdlen    The size of the command in bytes 
    @param  timeout   timeout before giving up
    
    @returns  1 if everything is OK, 0 if timeout occured before an
              ACK was recieved
*/
/**************************************************************************/
int sendCommandCheckAck(unsigned char *cmd, unsigned char cmdlen, unsigned short timeout)
 {
	unsigned short timer = 0;           // default timeout of one second
  
	wiresendcommand(cmd, cmdlen);
  
	/*Wait for chip to say its ready!*/
	while (wirereadstatus() != PN532_I2C_READY) {
		if (timeout != 0) {
			timer += 10;
			if (timer > timeout)  
				return 0;
		}
		mdelay(10);
	}
	/*read acknowledgement*/
	if (!readackframe()) {
		return 0;
	}

   return 1; 
} 
 
/**************************************************************************/
/*! 
    @brief  Configures the SAM (Secure Access Module) wake up chip
*/
/**************************************************************************/ 
int SAMConfig(void) 
{
	pn532_packetbuffer[0] = PN532_COMMAND_SAMCONFIGURATION;
	pn532_packetbuffer[1] = 0x01; // normal mode;
	pn532_packetbuffer[2] = 0x14; // timeout 50ms * 20 = 1 second
	pn532_packetbuffer[3] = 0x01; // use IRQ pin!
  
	if (! sendCommandCheckAck(pn532_packetbuffer, 4, 1000))
		return 0;
	// read data packet
	wirereaddata(pn532_packetbuffer, 8);
  
	return  (pn532_packetbuffer[6] == 0x15);
}

/**************************************************************************/
/*! 
    Sets the MxRtyPassiveActivation byte of the RFConfiguration register
    
    @param  maxRetries    0xFF to wait forever, 0x00..0xFE to timeout
                          after mxRetries
    
    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
int setPassiveActivationRetries(unsigned char maxRetries) 
{
	pn532_packetbuffer[0] = PN532_COMMAND_RFCONFIGURATION;
	pn532_packetbuffer[1] = 5;    // Config item 5 (MaxRetries)
	pn532_packetbuffer[2] = 0xFF; // MxRtyATR (default = 0xFF)
	pn532_packetbuffer[3] = 0x01; // MxRtyPSL (default = 0x01)
	pn532_packetbuffer[4] = maxRetries;
 
	if (! sendCommandCheckAck(pn532_packetbuffer, 5, 1000))
		return 0x0;  // no ACK
  
	return 1;
}
 
/**************************************************************************/
/*! 
    @brief  Checks the firmware version of the PN5xx chip

    @returns  The chip's firmware version and ID
*/
/**************************************************************************/
int getFirmwareVersion(void) 
{
	unsigned int response;

	pn532_packetbuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;

	if (! sendCommandCheckAck(pn532_packetbuffer, 1, 1000))
		return 0;

	while (wirereadstatus() != PN532_I2C_READY) {
		udelay(10);
  	}
	// read data packet
	wirereaddata(pn532_packetbuffer, 12);
  
	// check some basic stuff
	if (0 != strncmp((char *)pn532_packetbuffer, (char *)pn532response_firmwarevers, 6)) {
		return 0;
	}
  
	response = pn532_packetbuffer[7];
	response <<= 8;
	response |= pn532_packetbuffer[8];
	response <<= 8;
	response |= pn532_packetbuffer[9];
	response <<= 8;
	response |= pn532_packetbuffer[10];

	return response;
}

/***** ISO14443A Commands ******/

/**************************************************************************/
/*! 
    Waits for an ISO14443A target to enter the field
    
    @param  cardBaudRate  Baud rate of the card
    @param  uid           Pointer to the array that will be populated
                          with the card's UID (up to 7 bytes)
    @param  uidLength     Pointer to the variable that will hold the
                          length of the card's UID.
    
    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
int readPassiveTargetID(unsigned char cardbaudrate, unsigned char * uid, unsigned char * uidLength)
{
	unsigned char sens_res;
	int i = 0;
	pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
	pn532_packetbuffer[1] = 1;  // max 1 cards at once (we can set this to 2 later)
	pn532_packetbuffer[2] = cardbaudrate;
  
	if (! sendCommandCheckAck(pn532_packetbuffer, 3, 1000))
	{
		printk(KERN_INFO "NFC NO CARD FOUND!\n");
		return 0x0;  // no cards read
	}

	while (wirereadstatus() != PN532_I2C_READY) {
		//printk(KERN_INFO "WAIT FOR CARD\n");
		mdelay(10);
		i++;
		if (i == 100)
			break;
  	}
 	//printk(KERN_INFO "readPassiveTargetID\n");
	/*read data packet*/
	wirereaddata(pn532_packetbuffer, 20);
  
  /* ****************************************************************
  	ISO14443A card response should be in the following format:
  
    	byte            Description
    	-------------   ------------------------------------------
    	b0..6           Frame header and preamble
    	b7              Tags Found
    	b8              Tag Number (only one used in this example)
    	b9..10          SENS_RES
    	b11             SEL_RES
    	b12             NFCID Length
    	b13..NFCIDLen   NFCID                                     
    ******************************************************************/
  
	if (pn532_packetbuffer[7] != 1) 
		return 0;
    
	sens_res = pn532_packetbuffer[9];
	sens_res <<= 8;
	sens_res |= pn532_packetbuffer[10];
  
   /* Card appears to be Mifare Classic */
   *uidLength = pn532_packetbuffer[12];

	for (i = 0; i < pn532_packetbuffer[12]; i++) 
	{
		uid[i] = pn532_packetbuffer[13+i];
	}

	return 1;
}

int powerdown_pn532(void)
{
	pn532_packetbuffer[0] = PN532_COMMAND_POWERDOWN;
	pn532_packetbuffer[1] = 0xFB; //Power down
	pn532_packetbuffer[2] = 0x00; //  no irq from wake up
  
	if (! sendCommandCheckAck(pn532_packetbuffer, 3, 1000))
		return 0;

	return 1;
}

static ssize_t pn532_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	struct pn532_info *info = container_of(file->private_data, struct pn532_info, miscdev);
	struct i2c_client *client = info->i2c_dev;
	int r = 0;
	int i;
	int success;
	char *buffer = NULL;
	//printk(KERN_INFO "*******************pn532_read\n");

	mutex_lock(&info->mutex);

	SAMConfig();/*wake up chip*/
	mdelay(10);
	
	success = readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
	
	if (success) {

		buffer = (char *)kmalloc(uidLength + 1, GFP_KERNEL);

		if (!buffer) {
			goto fail_alloc;
		}
		buffer[0] = uidLength;
		for (i = 0; i < uidLength; i++) {
			buffer[i + 1] = uid[i];
		}
		
		r = copy_to_user(buf, buffer, uidLength + 1);
		printk(KERN_INFO "Found a card!\n");
		printk(KERN_INFO "UID Length: %d bytes", uidLength);
		printk(KERN_INFO "UID Value: ");
		for (i = 0; i < uidLength; i++) 
		{
			printk(KERN_INFO "0x%x ", uid[i]); 
		}
		printk(KERN_INFO "\n");
		/* Wait 1 ms before continuing*/
		mdelay(10);
	}else {
		goto end;
	}
	
	powerdown_pn532(); /*power down chip*/
	mutex_unlock(&info->mutex);
	if (buffer)
		kfree(buffer);
		
	return r;
	
end:	
	mutex_unlock(&info->mutex);
	if (buffer)
		kfree(buffer);
	return -EAGAIN;
fail_alloc:
	mutex_unlock(&info->mutex);
	return -ENOSPC;
}

static int pn532_open(struct inode *inode, struct file *file)
{
	struct pn532_info *info = container_of(file->private_data,
					       struct pn532_info, miscdev);
	struct i2c_client *client = info->i2c_dev;
	
	unsigned int version;

	printk(KERN_INFO "*************************pn532_open\n");
	dev_dbg(&client->dev, "%s: info: %p, client %p\n", __func__,
		info, info->i2c_dev);
		
	mutex_lock(&info->mutex);
	/*WAKEUP NFC CONTROLER*/
	gpio_set_value(GPIO_NFC_WAKEUP, 0);
	mdelay(2);
	gpio_set_value(GPIO_NFC_WAKEUP, 1);
	mdelay(10);
	
	/* configure board to read RFID tags*/
	SAMConfig();
	
	mdelay(10);
	/*Set the max number of retry attempts to read from a card*/
	setPassiveActivationRetries(0xFF);
	mdelay(10);
	/*firmware version*/
	version = getFirmwareVersion();
	if (! version) {
		printk(KERN_INFO "Didn't find PN532 board");
	} else {
		/*board info*/
		printk(KERN_INFO "Found chip PN5"); 
		printk(KERN_INFO "0x%x", (version>>24) & 0xFF); 
		printk(KERN_INFO "Firmware ver. "); 
		printk(KERN_INFO "%d", (version>>16) & 0xFF); 
		printk(KERN_INFO "."); 
		printk(KERN_INFO "%d", (version>>8) & 0xFF);
	}
	
	powerdown_pn532();/*power down chip*/
	mutex_unlock(&info->mutex);
	
	return 0;
}

static int pn532_close(struct inode *inode, struct file *file)
{
	struct pn532_info *info = container_of(file->private_data,
					       struct pn532_info, miscdev);
	struct i2c_client *client = info->i2c_dev;

	dev_dbg(&client->dev, "%s: info: %p, client %p\n",
		__func__, info, info->i2c_dev);

	return 0;
}

static const struct file_operations pn532_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.read		= pn532_read,
	.open		= pn532_open,
	.release	= pn532_close,
};

static int __devinit pn532_probe(struct i2c_client *client,
				 const struct i2c_device_id *id)
{
	struct pn532_info *info;
	int r = 0;

	printk(KERN_INFO "***********************pn532_probe\n");
	dev_dbg(&client->dev, "%s\n", __func__);

	/* private data allocation */
	info = kzalloc(sizeof(struct pn532_info), GFP_KERNEL);
	if (!info) {
		dev_err(&client->dev,
			"Cannot allocate memory for pn532_info.\n");
		r = -ENOMEM;
		goto err_info_alloc;
	}
	info->i2c_dev = client;
	pn532 = info;
	mutex_init(&info->mutex);

	i2c_set_clientdata(client, info);

	if (r < 0) {
		dev_err(&client->dev, "Unable to register IRQ handler\n");
		goto err_kmalloc;
	}

	r = gpio_request(GPIO_NFC_EN, "nfc_en");
	if (r < 0) {
		printk(KERN_ERR "failed to request GPIO for en ! error = %d\n", r);
		goto err_kmalloc;
	}

	r = gpio_direction_output(GPIO_NFC_EN, 1);
	if (r < 0) {
		printk("fail to set direction of en GPIO output! error = %d\n", r);
		goto err_kmalloc;
	}
	
	r = gpio_request(GPIO_NFC_IRQ, "nfc_irq");
	if (r < 0) {
		printk(KERN_ERR "failed to request GPIO for RST/WakeUP ! error = %d\n", r);
		goto err_kmalloc;
	}

	r = gpio_direction_input(GPIO_NFC_IRQ);
	if (r < 0) {
		printk("fail to set direction of RST/WakeUP GPIO output! error = %d\n", r);
		goto err_kmalloc;
	}
	
	r = gpio_request(GPIO_NFC_WAKEUP, "nfc_wake");
	if (r < 0) {
		printk(KERN_ERR "failed to request GPIO for RST/WakeUP ! error = %d\n", r);
		goto err_kmalloc;
	}

	r = gpio_direction_output(GPIO_NFC_WAKEUP, 1);
	if (r < 0) {
		printk("fail to set direction of RST/WakeUP GPIO output! error = %d\n", r);
		goto err_kmalloc;
	}
	
	info->miscdev.minor = PN532_MINOR;
	info->miscdev.name = PN532_DRIVER_NAME;
	info->miscdev.fops = &pn532_fops;
	info->miscdev.parent = &client->dev;
	r = misc_register(&info->miscdev);
	if (r < 0) {
		printk(KERN_INFO "failed to register nfc\n");
		dev_err(&client->dev, "Device registration failed\n");
		goto err_kmalloc;
	}

	return 0;
err_kmalloc:
	kfree(info);
err_info_alloc:
	return r;
}

static __devexit int pn532_remove(struct i2c_client *client)
{
	struct pn532_info *info = i2c_get_clientdata(client);

	dev_dbg(&client->dev, "%s\n", __func__);

	misc_deregister(&info->miscdev);
	kfree(info);
	gpio_free(GPIO_NFC_WAKEUP);
	gpio_free(GPIO_NFC_EN);
	gpio_free(GPIO_NFC_IRQ);
	pn532 = NULL;
	return 0;
}

static struct i2c_driver pn532_driver = {
	.driver = {
		.name = PN532_DRIVER_NAME,
	},
	.probe = pn532_probe,
	.id_table = pn532_id_table,
	.remove = __devexit_p(pn532_remove),
};

static int __init pn532_init(void)
{
	int r;

	pr_debug(DRIVER_DESC ": %s\n", __func__);

	r = i2c_add_driver(&pn532_driver);
	if (r) {
		pr_err(PN532_DRIVER_NAME ": driver registration failed\n");
		return r;
	}
	
	return 0;
}

static void __exit pn532_exit(void)
{
	i2c_del_driver(&pn532_driver);
	pr_info(DRIVER_DESC ", Exiting.\n");
}

module_init(pn532_init);
module_exit(pn532_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DRIVER_DESC);
