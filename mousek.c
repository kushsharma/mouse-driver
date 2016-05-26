/*
 *  mousek.c, A linux kernel module to control mouse pointer using keyboard
 *	
 *	Copyright (c) 2016 Kush Kumar Sharma *  
 *
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

/* usb stuff */
#include <linux/input.h>
#include <linux/usb.h>

/* miscdevice stuff */
#include <linux/poll.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

/*
 * Version Information
 */
#define DRIVER_VERSION "v0.1"
#define DRIVER_AUTHOR "Kush Kumar Sharma"
#define DRIVER_DESC "Mouse driver"
#define DRIVER_LICENSE "GPL"

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);

#define DEVICE_NAME "mousek"
#define MAX_SCREEN 100
#define MIN_SCREEN 0

int Major;

/*
 * We need a local data structure, as it must be allocated for each new
 * mouse device plugged in the USB bus
 */

struct mousek_device {
    signed char data[4];     /* use a 4-byte protocol */
    struct urb urb;          /* USB Request block, to get USB data*/
    struct input_dev *idev;   /* input device, to push out input  data */
    int x, y;                /* keep track of the position of this device */
};

static struct mousek_device *mouse;

/*
 * Handler for data sent in by the device. The function is called by
 * the USB kernel subsystem whenever the device spits out new data
 
static void mousek_irq(struct urb *urb)
{ //useless right now

    struct mousek_device *mouse = urb->context;
    signed char *data = mouse->data;
    struct input_dev *idev = &mouse->idev;
	
    if (urb->status != USB_ST_NOERROR) return;

    // ignore data[0] which reports mouse buttons 
    mouse->x += data[1];
    mouse->y += data[2];
    printk(KERN_DEBUG "mousek irq, %5i %5i\n", mouse->x, mouse->y);

    input_report_rel(mouse->idev, REL_X,     mouse->x);
	input_report_rel(mouse->idev, REL_Y,     mouse->y);
	input_report_rel(mouse->idev, REL_WHEEL, data[3]);

	input_sync(idev);
}
*/

/*
static irqreturn_t mousek_interrupt(int irq, void *dummy)
{
	input_report_key(mouse->idev, BTN_0, inb(BUTTON_PORT) & 1);
	input_sync(mouse->idev);
	return IRQ_HANDLED;
}
*/


/*
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
int mousek_open(struct inode *inode, struct file *filp)
{
    

    //filp->private_data = mouse;

    /* announce yourself */
    printk(KERN_INFO "mousek: faking an USB mouse via the misc device\n");
    
	//try_module_get(THIS_MODULE);
	
	return 0; /* Ok */
}    

/* close releases the device, like mousek_disconnect */
int mousek_release(struct inode *inode, struct file *filp)
{
    //struct mousek_device *mousek = filp->private_data;
    //input_unregister_device(&mouse->idev);
    //kfree(mousek);
	//usb_kill_urb(mouse->urb);

    printk(KERN_INFO "mousek: closing misc device\n");
    
	
	/*
         * Decrement the usage count, or else once you opened the file, you'll
         * never get get rid of the module.
         */
    //module_put(THIS_MODULE);
	
	return 0;
}

/* poll reports the device as writeable */
ssize_t mousek_read(struct file *filp,   /* see include/linux/fs.h   */
                           char *buffer,        /* buffer to fill with data */
                           size_t length,       /* length of the buffer     */
                           loff_t * offset)
{
    printk(KERN_INFO "mousek: Restricted. That is okay but do not repeat this mistake.\n");
    return 0;
}

/* write accepts data and converts it to mouse movement
 * Called when a process writes to dev file: echo "hi" > /dev/hello
 */
ssize_t mousek_write(struct file *filp, const char *buf, size_t count,
		    loff_t *offp)
{
    //struct mousek_device *mouse = filp->private_data;
    static char localbuf[16];
    //struct urb urb;
    int i;

    /* accept 16 bytes at a time, at most */
    if (count >16) count=16;
    copy_from_user(localbuf, buf, count);

    /* prepare the urb for mousek_irq() */
    //urb.status = USB_ST_NOERROR;
    //urb.context = mousek;
	struct input_dev *dev = mouse->idev;

    /* scan written data */
    for (i=0; i<count; i++) {
	mouse->data[1] = mouse->data[2] = mouse->data[3] = 0;
	switch (localbuf[i]) {
	    case 'u': case 'U': /* up */
		mouse->data[2] = -50;
		break;
	    case 'd': case 'D': /* down */
		mouse->data[2] = 50;
		break;
	    case 'l': case 'L': /* left */
		mouse->data[1] = -50;
		break;
	    case 'r': case 'R': /* right */
		mouse->data[1] = 50;
		break;
		case 'w': case 'W': /* right */
		mouse->data[3] = 10;
		break;
	    default:
		continue;
	}
	//input_report_key(dev, BTN_RIGHT, 1);
	//input_report_key(dev, BTN_RIGHT, 0);

	//input_report_key(dev, KEY_UP, 1); /* keypress */
	//input_report_key(dev, KEY_UP, 0); /* release */
	
	input_report_abs(dev, ABS_X, 2+i);
	input_report_abs(dev, ABS_Y, 90+i);

	
	//input_report_rel(dev, REL_X, mouse->data[1]);
	//input_report_rel(dev, REL_Y, mouse->data[2]);
	
	//input_report_rel(dev, REL_WHEEL, mouse->data[3]);
	
	
	
	//printk(KERN_ALERT "mousek: Control actions of %s received %d %d",localbuf, mouse->data[1], mouse->data[2]);
	input_sync(dev);

	/* if we are here, a valid key is there, fix the urb */
	//mousek_irq(&urb);
    }
	input_report_key(dev, BTN_RIGHT, 1);
	input_report_key(dev, BTN_RIGHT, 0);
	input_sync(dev);

	//input_report_rel(mouse->idev, REL_WHEEL, data[3]);
	
	
    return count;
}


struct file_operations mousek_fops = {
	write:    mousek_write,
	read:     mousek_read,
	open:     mousek_open,
	release:  mousek_release,
};

/*
 * Functions called at module load and unload time: only register and
 * unregister the USB callbacks and the misc entry point
 */
int init_module(void)
{
    int retval;
	
	Major = register_chrdev(0, DEVICE_NAME, &mousek_fops);
	if (Major < 0) {
	  printk(KERN_ALERT "Registering char device failed with %d\n", Major);
	  return Major;
	}
	
	//if (request_irq(BUTTON_IRQ, mousek_interrupt, 0, DEVICE_NAME, NULL)) {
	//	printk(KERN_ERR "mousek: Can't allocate irq \n");
    //    return -EBUSY;
    //}
	
	struct input_dev *input_dev;

    /* allocate and zero a new data structure for the new device */
    mouse = kmalloc(sizeof(struct mousek_device), GFP_KERNEL);
    if (!mouse) return -ENOMEM; /* failure */
    memset(mouse, 0, sizeof(*mouse));

	input_dev = input_allocate_device();
	if (!input_dev) {
		printk(KERN_ERR "mousek.c: Not enough memory\n");
		retval = -ENOMEM;
		//goto err_free_irq;
	}
	//updating struct
	mouse->idev = input_dev;
    
	/* tell the features of this input device: fake only keys */
    //mouse->idev.evbit[0] = BIT(EV_KEY);
    /* and tell which keys: only the arrows */
    //set_bit(103, mouse->idev.keybit); /* Up    */
    //set_bit(105, mouse->idev.keybit); /* Left  */
    //set_bit(106, mouse->idev.keybit); /* Right */
    //set_bit(108, mouse->idev.keybit); /* Down  */
	
	//input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL) | BIT_MASK(EV_ABS);
	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
    set_bit(103, input_dev->keybit); /* Up    */

	input_dev->keybit[BIT_WORD(BTN_MOUSE)] = BIT_MASK(BTN_LEFT) | BIT_MASK(BTN_RIGHT) | BIT_MASK(BTN_MIDDLE);
	//input_dev->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y) | BIT_MASK(REL_WHEEL);
	input_dev->absbit[0] = BIT_MASK(ABS_X) | BIT_MASK(ABS_Y);
	
	input_set_abs_params(input_dev, ABS_X, MIN_SCREEN, MAX_SCREEN, 0, 0);
	input_set_abs_params(input_dev, ABS_Y, MIN_SCREEN, MAX_SCREEN, 0, 0);

	input_dev->name = DEVICE_NAME;	
	input_set_drvdata(input_dev, mouse);
	
	retval = input_register_device(input_dev);
	if (retval) {
		printk(KERN_ERR "mousek: Failed to register device\n");
		goto err_free_dev;
	}

	
	printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
	printk(KERN_INFO "the driver, create a dev file with\n");   
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
	printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
	printk(KERN_INFO "the device file.\n");
	printk(KERN_INFO "Remove the device file and module when done.\n");
	
	
return 0;

err_free_dev:
	input_free_device(mouse->idev);
	kfree(mouse);
//err_free_irq:
//	free_irq(BUTTON_IRQ, button_interrupt);
return retval;
}

void cleanup_module(void)
{
    /*
    * Unregister the device
    */
	if(!mouse) return;
	
	input_unregister_device(mouse->idev);
	kfree(mouse);	
	unregister_chrdev(Major, DEVICE_NAME);
    
	printk(KERN_ALERT "Uninstalled. Delete device from dev.");
	
	//ret = free_irq(BUTTON_IRQ, mousek_interrupt);
	//if (ret < 0)
	//	printk(KERN_ALERT "Error in freeing irq: %d\n", ret);

}
