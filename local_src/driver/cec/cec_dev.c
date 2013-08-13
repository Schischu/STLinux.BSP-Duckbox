#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/poll.h>
#include <linux/version.h>

#include <linux/device.h> /* class_creatre */ 
#include <linux/cdev.h> /* cdev_init */

#include "cec_opcodes.h"
#include "cec_worker.h"
#include "cec_dev.h"

typedef struct
{
	struct file* 	fp;
	struct semaphore sem;

} tCECOpen;

static tCECOpen vOpen;
static wait_queue_head_t wq;

#define OUTBUFFERSIZE 4096
static unsigned int outputBufferStart;
static unsigned int outputBufferEnd;
static unsigned char outputBuffer[OUTBUFFERSIZE];

////////////////////////////////////////////////////////////////////////////////

static unsigned int GetMessageBufferSize (void)
{
	unsigned int availableBytes = 0;
	if (outputBufferStart <= outputBufferEnd)
		availableBytes = (outputBufferEnd - outputBufferStart);
	else // WRAP AROUND
		availableBytes = ((OUTBUFFERSIZE - outputBufferStart) + outputBufferEnd);

	printk("[CEC] GetMessageBufferSize <- START=%d END=%d AVAILABLE=%d\n", outputBufferStart, outputBufferEnd, availableBytes);
	return availableBytes;
}

static void GetMessageFromBuffer (unsigned char * msg, unsigned int len)
{
	printk("[CEC] GetMessageFromBuffer -> START=%d END=%d LEN=%d\n", outputBufferStart, outputBufferEnd, len);
	if (outputBufferStart <= outputBufferEnd)
	{
		memcpy(msg, outputBuffer + outputBufferStart, len);
		outputBufferStart += len;
	}
	else // WRAP AROUND
	{
		unsigned int dataAtTheEnd = OUTBUFFERSIZE - outputBufferStart;
		memcpy(msg, outputBuffer + outputBufferStart, dataAtTheEnd);
		memcpy(msg + dataAtTheEnd, outputBuffer, len - dataAtTheEnd);
		outputBufferStart += len - dataAtTheEnd;
	}

	printk("[CEC] GetMessageFromBuffer <- START=%d END=%d\n", outputBufferStart, outputBufferEnd);
}

inline void AddMessageBuffer (tCECMessage msg)
{
	unsigned int spaceAtTheEnd = OUTBUFFERSIZE - outputBufferEnd;
	unsigned int messageSize = msg.length + 2;

	// CAN WE WRITE THE WHOLE MESSAGE IN ONE PIECE TO THE BUFFER
	if (spaceAtTheEnd >= messageSize)
	{
		memcpy(outputBuffer + outputBufferEnd, &msg, messageSize);
		outputBufferEnd += messageSize;
	}
	else
	{
		memcpy(outputBuffer + outputBufferEnd, &msg, spaceAtTheEnd);
		memcpy(outputBuffer, &msg + spaceAtTheEnd, messageSize - spaceAtTheEnd);
		outputBufferEnd = messageSize - spaceAtTheEnd;
	}
	
	printk("[CEC] AddMessageBuffer START=%d END=%d\n", outputBufferStart, outputBufferEnd);
}

void AddMessageToBuffer (unsigned char *rawmsg, unsigned int len)
{
	// WE CONVERT THE RAW MESSAGE TO MESSAGE
	// BIT WE WILL SAVE IT AS UNSIGNED CHAR TO EASE READING
	tCECMessage message;
	message.address = rawmsg[0];
	message.length = len - 1;
	memcpy(message.data, rawmsg + 1, message.length);

	AddMessageBuffer(message);
	wake_up_interruptible(&wq);
}

////////////////////////////////////////////////////////////////////////////////

static ssize_t CECdev_write(struct file *filp, const char *buff, size_t len, loff_t *off)
{
	unsigned char* kernel_buf = kmalloc(len, GFP_KERNEL);

	printk("[CEC] %s > (len %d, offs %lld)\n", __func__, len, *off);
	if (kernel_buf == NULL)
	{
		printk("[CEC] %s return no mem<\n", __func__);
		return -ENOMEM;
	}

	copy_from_user(kernel_buf, buff, len);
	if (len >= 2)
	{
		tCECMessage *message = (tCECMessage*) kernel_buf;
		if (message->length >= (len - 2))
		{
			unsigned char sendBuf[message->length + 1];
			sendBuf[0] = message->address;
			memcpy(sendBuf+1, message->data, message->length);
			sendMessage(sizeof(sendBuf), sendBuf);
		}
	}
	
	kfree(kernel_buf);
	
	printk("[CEC] %s <\n", __func__);
	return len;
}

static ssize_t CECdev_read(struct file *filp, char __user *buff, size_t len, loff_t *off)
{
	unsigned int availableBytes = GetMessageBufferSize();

	printk("[CEC] %s > (len %d, offs %lld)\n", __func__, len, *off);
	if (vOpen.fp != filp)
	{
		printk("[CEC] %s return eusers<\n", __func__);
		return -EUSERS;
	}

	// BLOCK TILL WE HAVE SOMETHING TO READ
	/*while(availableBytes == 0)
	{
		if (wait_event_interruptible(wq, (outputBufferStart != outputBufferEnd)))
		return -ERESTARTSYS;
	}*/

	// GET SEMAPHORE FOR EXCLUSIVE WRITE LOCK
	if (down_interruptible(&vOpen.sem))
	{
		printk("[CEC] %s return erestartsys<\n", __func__);
		return -ERESTARTSYS;
	}

	// LEN DEFINES HOW MUCH THE READER IS WILLING TO RECEIVE
	if (len > availableBytes)
		len = availableBytes;

	if (len > 0)
	{
		unsigned char msg[len];
		GetMessageFromBuffer(msg, len);
		copy_to_user(buff, msg, len);
	}
	// RELEASE THE EXCLUSIVE WRITE LOCK SEMAPHORE
	up (&vOpen.sem);
	printk("[CEC] %s < (len %d)\n", __func__, len);
	return len;
}

static unsigned int CECdev_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;

	poll_wait(filp, &wq, wait);
	if(outputBufferStart != outputBufferEnd)
	{
		mask = POLLIN | POLLRDNORM;
	}

	return mask;
}

static int CECdev_open(struct inode *inode, struct file *filp)
{
	printk("[CEC] %s >\n", __func__);

	if (vOpen.fp != NULL)
	{
		printk("[CEC] %s eusers <\n", __func__);
			return -EUSERS;
	}
	vOpen.fp = filp;

	printk("[CEC] %s <\n", __func__);
	return 0;
}

static int CECdev_close(struct inode *inode, struct file *filp)
{
	printk("[CEC] %s >\n", __func__);

	if (vOpen.fp == filp)
	{
		vOpen.fp = NULL;
	}

	printk("[CEC] %s <\n", __func__);
	return 0;
}

static int CECdev_ioctl(struct inode *Inode, struct file *File, unsigned int cmd, unsigned long arg)
{
	printk("[CEC] %s > 0x%.8x\n", __func__, cmd);

	switch(cmd) {
	case CEC_GET_ADDRESS:
	{
		tCECAddressinfo *addr_info = (tCECAddressinfo*) arg;
		unsigned short phys_addr = getPhysicalAddress();
		unsigned char logical_device_type = getLogicalDeviceType();
		unsigned char device_type = getDeviceType();
		
		addr_info->physical[0] = (phys_addr>>8) & 0xFF;
		addr_info->physical[1] = (phys_addr)    & 0xFF;
		addr_info->logical = logical_device_type;
		addr_info->type = device_type;
		break;
	}
	case CEC_FLUSH:
		outputBufferStart = outputBufferEnd = 0;
		break;
	default:
		printk("[CEC]: unknown IOCTL 0x%x\n", cmd);
		break;
	}
	printk("[CEC] %s <\n", __func__);
	return 0;
}

static struct file_operations cec_fops =
{
	.owner = THIS_MODULE,
	.ioctl = CECdev_ioctl,
	.write = CECdev_write,
	.read  = CECdev_read,
	.poll  = CECdev_poll,
	.open  = CECdev_open,
	.release  = CECdev_close
};

#define DEVICE_NAME "CEC" 

static        dev_t  cec_dev_num; 
static struct cdev   cec_cdev; 
static struct class *cec_class = 0;
int init_dev(void)
{
	int result;
	outputBufferStart = outputBufferEnd = 0;

	//if (register_chrdev(CEC_MAJOR,"CEC",&cec_fops))
	//	printk("[CEC] unable to get major %d for CEC\n", CEC_MAJOR);

	cec_dev_num = MKDEV(CEC_MAJOR, 0);
	result = register_chrdev_region(cec_dev_num, 1, DEVICE_NAME);
	if (result < 0) {
		printk( KERN_ALERT "CEC cannot register device (%d)\n", result);
		return result;
	}

	cdev_init(&cec_cdev, &cec_fops);
	cec_cdev.owner = THIS_MODULE;
	cec_cdev.ops   = &cec_fops;
	if (cdev_add(&cec_cdev, cec_dev_num, 1) < 0)
	{
		printk("CEC couldn't register '%s' driver\n", DEVICE_NAME);
		return -1;
	}

	cec_class = class_create(THIS_MODULE, DEVICE_NAME);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
	device_create(cec_class, NULL, MKDEV(CEC_MAJOR, 0), NULL, "hdmi_cec", 0);
#else
	class_device_create(cec_class, NULL, MKDEV(CEC_MAJOR, 0), NULL, "hdmi_cec", 0);
#endif

	vOpen.fp = NULL;
	sema_init(&vOpen.sem, 1);

	init_waitqueue_head(&wq);

	return 0;
}

int cleanup_dev(void)
{
	cdev_del(&cec_cdev);
	unregister_chrdev_region(cec_dev_num, 1);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
	device_destroy(cec_class, MKDEV(CEC_MAJOR, 0));
#else
	class_device_destroy(cec_class, MKDEV(CEC_MAJOR, 0));
#endif
	class_destroy(cec_class);
}


