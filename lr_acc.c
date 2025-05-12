/* * Device driver for the VGA video generator
 *
 * A Platform device implemented using the misc subsystem
 *
 * Stephen A. Edwards
 * Columbia University
 *
 * References:
 * Linux source: Documentation/driver-model/platform.txt
 *               drivers/misc/arm-charlcd.c
 * http://www.linuxforu.com/tag/linux-device-drivers/
 * http://free-electrons.com/docs/
 *
 * "make" to build
 * insmod lr_acc.ko
 *
 * Check code style with
 * checkpatch.pl --file --no-tree lr_acc.c
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "lr_acc.h"

#define DRIVER_NAME "lr_acc"

/* Device registers */
#define BG_RED(x) (x)
#define BG_GREEN(x) ((x)+1)
#define BG_BLUE(x) ((x)+2)

#define VGA_BACKGROUND_OFFSET 0

/*
 * Information about our device
 */
struct lr_acc_dev {
	struct resource res; /* Resource: our registers */
	void __iomem *virtbase; /* Where registers can be accessed in memory */
    lr_acc_arg_t data;
} dev;

/* Write background color */
static void write_data(lr_acc_arg_t *data)
{
	// short x = (data->data1 >> 4) & 0x0f | ((data->data2 >> 4) & 0x0f) << 4 | ((data->data3 >> 4) & 0x0f) << 8 | ((data->data4 >> 4) & 0x0f) << 12;
	// short y = (data->data1 & 0x0f) | ((data->data2 & 0x0f) << 4) | ((data->data3 & 0x0f) << 8) | ((data->data4 & 0x0f) << 12);
	if (data->go) {
		iowrite32((u32)1, dev.virtbase + (1 << 9));
	} else {
		iowrite32((u32)data->data.data, dev.virtbase + 1);
	}
	
}

static void read_data(lr_acc_arg_t *data)
{
    int a  = ioread32(dev.virtbase + 0);
    int b  = ioread32(dev.virtbase + 4);
    int c  = ioread32(dev.virtbase + 8);
    // data->address    = (hi << 8) | (lo & 0xFF);
	printf("First: %d\n", a);
	printf("Second: %d\n", b);
	printf("Third: %d\n", c);
}
/*
 * Handle ioctl() calls from userspace:
 * Read or write the segments on single digits.
 * Note extensive error checking of arguments
 */
static long lr_acc_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	lr_acc_arg_t vla;

	switch (cmd) {
		case LR_ACC_WRITE_DATA:
			if (copy_from_user(&vla, (lr_acc_arg_t *) arg,
					sizeof(lr_acc_arg_t)))
				return -EACCES;
			write_data(&vla);
			break;

		case LR_ACC_READ_DATA:
			if (copy_to_user((lr_acc_arg_t *) arg, &vla,
					sizeof(lr_acc_arg_t)))
				return -EACCES;
			read_data(&vla);
			break;

		default:
			return -EINVAL;
	}

	return 0;
}

/* The operations our device knows how to do */
static const struct file_operations lr_acc_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl = lr_acc_ioctl,
};

/* Information about our device for the "misc" framework -- like a char dev */
static struct miscdevice lr_acc_misc_device = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= DRIVER_NAME,
	.fops		= &lr_acc_fops,
};

/*
 * Initialization code: get resources (registers) and display
 * a welcome message
 */
static int __init lr_acc_probe(struct platform_device *pdev)
{
    // lr_acc_color_t beige = { 0xf9, 0xe4, 0xb7 };
    // lr_acc_color_t beige = { 0xff, 0xff, 0xff };
		
	int ret;

	/* Register ourselves as a misc device: creates /dev/lr_acc */
	ret = misc_register(&lr_acc_misc_device);

	/* Get the address of our registers from the device tree */
	ret = of_address_to_resource(pdev->dev.of_node, 0, &dev.res);
	if (ret) {
		ret = -ENOENT;
		goto out_deregister;
	}

	/* Make sure we can use these registers */
	if (request_mem_region(dev.res.start, resource_size(&dev.res),
			       DRIVER_NAME) == NULL) {
		ret = -EBUSY;
		goto out_deregister;
	}

	/* Arrange access to our registers */
	dev.virtbase = of_iomap(pdev->dev.of_node, 0);
	if (dev.virtbase == NULL) {
		ret = -ENOMEM;
		goto out_release_mem_region;
	}
        
	/* Set an initial color */
        // write_background(&beige);

	return 0;

out_release_mem_region:
	release_mem_region(dev.res.start, resource_size(&dev.res));
out_deregister:
	misc_deregister(&lr_acc_misc_device);
	return ret;
}

/* Clean-up code: release resources */
static int lr_acc_remove(struct platform_device *pdev)
{
	iounmap(dev.virtbase);
	release_mem_region(dev.res.start, resource_size(&dev.res));
	misc_deregister(&lr_acc_misc_device);
	return 0;
}

/* Which "compatible" string(s) to search for in the Device Tree */
#ifdef CONFIG_OF
static const struct of_device_id lr_acc_of_match[] = {
	{ .compatible = "csee4840,lr_acc-1.0" },
	{},
};
MODULE_DEVICE_TABLE(of, lr_acc_of_match);
#endif

/* Information for registering ourselves as a "platform" driver */
static struct platform_driver lr_acc_driver = {
	.driver	= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(lr_acc_of_match),
	},
	.remove	= __exit_p(lr_acc_remove),
};

/* Called when the module is loaded: set things up */
static int __init lr_acc_init(void)
{
	pr_info(DRIVER_NAME ": init\n");
	return platform_driver_probe(&lr_acc_driver, lr_acc_probe);
}

/* Calball when the module is unloaded: release resources */
static void __exit lr_acc_exit(void)
{
	platform_driver_unregister(&lr_acc_driver);
	pr_info(DRIVER_NAME ": exit\n");
}

module_init(lr_acc_init);
module_exit(lr_acc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Venkat Suprabath Bitra, Columbia University");
MODULE_DESCRIPTION("LR Accumulator Driver");
