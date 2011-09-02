#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/input.h>
#include <linux/platform_device.h>

MODULE_DESCRIPTION("Wacom Android Driver");
MODULE_LICENSE("GPL");

struct input_dev *wacom_input_dev;        /* Representation of an input device */
static struct platform_device *wacom_dev; /* Device structure */

/* Sysfs method to input events into driver */
static ssize_t write_wacom(struct device *dev, struct device_attribute *attr,
      const char *buffer, size_t count)
{
   int tooltype, proximity, buttons, x, y, pressure, touch, side;
   sscanf(buffer, "%d%d%d%d%d%d%d%d", &tooltype, &proximity, &buttons, &x, &y,
                                      &pressure, &touch, &side);

   //printk("X: %d\tY: %d\n", x, y);
   //printk("%d %d %d %d %d %d %d %d\n", tooltype, proximity, buttons, x, y,
   //                                  pressure, touch, side);
   
   // Get tool type and report in/out proximity
   /*if (tooltype == 1) {
      input_report_key(wacom_input_dev, BTN_TOOL_PEN, proximity);
   } else if (tooltype == 4) {
      input_report_key(wacom_input_dev, BTN_TOOL_RUBBER, proximity);
   }*/

   input_report_key(wacom_input_dev, BTN_TOOL_PEN, tooltype == 1);
   input_report_key(wacom_input_dev, BTN_TOOL_RUBBER, tooltype == 4);
   
   // Don't currently do anything with 'buttons' - redundant data
   input_report_abs(wacom_input_dev, ABS_X, x);
   input_report_abs(wacom_input_dev, ABS_Y, y);
   input_report_abs(wacom_input_dev, ABS_PRESSURE, pressure);
   input_report_key(wacom_input_dev, BTN_TOUCH, touch);
   input_report_key(wacom_input_dev, BTN_STYLUS, side);

   input_sync(wacom_input_dev);

   return count;
}

/* Attach the sysfs write method */
DEVICE_ATTR(input, 0644, NULL, write_wacom);

/* Attribute Descriptor */
static struct attribute *wacom_attrs[] = {
   &dev_attr_input.attr,
   NULL
};

/* Attribute group */
static struct attribute_group wacom_attr_group = {
   .attrs = wacom_attrs,
};

/* Driver initialization */
int __init wacom_init(void)
{
   /* Register a platform device */
   wacom_dev = platform_device_register_simple("wac_android", -1, NULL, 0);
   if (IS_ERR(wacom_dev)) {
      printk ("wacom_init: error\n");
      return PTR_ERR(wacom_dev);
   }

   /* Create a sysfs node to read wacom values */
   sysfs_create_group(&wacom_dev->dev.kobj, &wacom_attr_group);

   /* Allocate an input device data structure */
   wacom_input_dev = input_allocate_device();
   if (!wacom_input_dev) {
      printk("Bad input_allocate_device()\n");
      return -ENOMEM;
   }

   wacom_input_dev->name = "Wacom Tablet";

   /* Announce the Wacom devices capabilities */
   set_bit(EV_ABS, wacom_input_dev->evbit);
   set_bit(EV_KEY, wacom_input_dev->evbit);

   set_bit(BTN_TOOL_PEN, wacom_input_dev->keybit);
   set_bit(BTN_TOOL_RUBBER, wacom_input_dev->keybit);
   set_bit(BTN_TOUCH, wacom_input_dev->keybit);
   set_bit(BTN_STYLUS, wacom_input_dev->keybit);

   set_bit(ABS_X, wacom_input_dev->absbit);
   set_bit(ABS_Y, wacom_input_dev->absbit);
   set_bit(ABS_PRESSURE, wacom_input_dev->absbit);

   // (device, param, min, max, fuzz, flat)
   // NOTE: wacom usb driver has fuzz = 4 for abs x/y

   // The following 2 input params are for the x61 tablet digitizer
   //input_set_abs_params(wacom_input_dev, ABS_X, 0, 21136, 0, 0);
   //input_set_abs_params(wacom_input_dev, ABS_Y, 0, 15900, 0, 0);

   // The following 2 input params are for the 12" digitizer
   input_set_abs_params(wacom_input_dev, ABS_X, 0, 24780, 0, 0);
   input_set_abs_params(wacom_input_dev, ABS_Y, 0, 18630, 0, 0);

   input_set_abs_params(wacom_input_dev, ABS_PRESSURE, 0, 255, 0, 0);
   
   /* Register with the input subsystem */
   input_register_device(wacom_input_dev);

   printk("Wacom Android Driver Initialized.\n");
   return 0;
}

/* Driver Exit */
void __exit wacom_exit(void)
{
   /* Unregister from the input subsystem */
   input_unregister_device(wacom_input_dev);

   /* Cleanup sysfs node */
   sysfs_remove_group(&wacom_dev->dev.kobj, &wacom_attr_group);

   /* Unregister driver */
   platform_device_unregister(wacom_dev);

   return;
}

module_init(wacom_init);
module_exit(wacom_exit);
