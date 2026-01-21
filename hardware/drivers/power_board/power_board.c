#include<linux/module.h>
#include<linux/init.h>
#include<linux/usb.h>
#include<linux/hwmon.h>
#include<linux/errno.h>
#include<linux/mutex.h>

#define FOSDEM_VENDOR_ID 0xF05D
#define FOSDEM_PRODUCT_KRAB 0x4011

static struct usb_device_id power_board_table [] = {
	{ USB_DEVICE(FOSDEM_VENDOR_ID, FOSDEM_PRODUCT_KRAB) }, /* Krab Controller */
	{ } /* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, power_board_table);

#define POWER_BOARD_MINOR_BASE 0

#define VENDOR_REQUEST_TEMPERATURE 0x10
#define VENDOR_REQUEST_FANSPEED 0x11

struct power_board {
	struct usb_device * udev;
	__u8 bulk_in_addr;
	__u8 bulk_out_addr;

	char *read_buffer;
	size_t read_buffer_len;
	char *write_buffer;
	size_t write_buffer_len;

	int protocol;
	int present;
	struct mutex mutex;
	struct kref kref;
};
#define to_pb(d) container_of(d, struct power_board, kref)

static struct usb_driver power_board_driver;

static int vendor_command(struct usb_device *dev, unsigned char request, unsigned char value, unsigned char index, void *buf, int size)
{
	return usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
			request,
			USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_OTHER,
			value,
			index, buf, size,
			USB_CTRL_GET_TIMEOUT);
}

static void power_board_delete(struct kref *kref)
{
	printk("PB DELETE!\n");
	struct power_board *dev = to_pb(kref);
	usb_put_dev(dev->udev);
	kfree(dev);
}

/*********************************
 * File I/O subsystem
 ********************************/

// Open /dev/powerboard0
static int power_board_open(struct inode *inode, struct file *file)
{
	struct power_board *dev;
	struct usb_interface *interface;
	int result;

	interface = usb_find_interface(&power_board_driver, iminor(inode));
	if (!interface) {
		return -ENODEV;
	}

	dev = usb_get_intfdata(interface);
	if (!dev) {
		return -ENODEV;
	}

	kref_get(&dev->kref);
	file->private_data = dev;

	return 0;
}

// Close /dev/powerboard0
static int power_board_release(struct inode *inode, struct file *file)
{
	struct power_board *dev;
	dev = file->private_data;
	if (dev == NULL)
		return -ENODEV;

	kref_put(&dev->kref, power_board_delete);
	return 0;
}

static ssize_t power_board_read(struct file *file, char *buffer, size_t count,
			 loff_t *ppos)
{
	struct power_board *dev = file->private_data;
	int result;

	if(!dev->present)
		return -ENODEV;

	//result = simple_read_from_buffer(buffer, count, ppos, 
	result = 0;
	return result;
}

static void power_board_write_bulk_callback(struct urb *urb)
{
	//struct power_board *dev = urb->context;
	//int status = urb->status;

	usb_free_coherent(urb->dev, urb->transfer_buffer_length,
			urb->transfer_buffer, urb->transfer_dma);
}

static ssize_t power_board_write(struct file *file, const char *user_buffer,
			  size_t count, loff_t *ppos)
{
	struct power_board *dev = file->private_data;
	int result = 0;
	struct urb *urb = NULL;
	char *buf = NULL;

	// Zero byte write is a noop
	if (count == 0)
		return 0;

	if (dev->present == 0) {
		return -ENODEV;
	}

	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) {
		return -ENOMEM;
	}

	buf = usb_alloc_coherent(dev->udev, count, GFP_KERNEL, &urb->transfer_dma);
	if (!buf) {
		result = -ENOMEM;
		goto error;
	}

	if (copy_from_user(buf, user_buffer, count)) {
		result = -EFAULT;
		goto error;
	}

	usb_fill_bulk_urb(urb, dev->udev,
			usb_sndbulkpipe(dev->udev, dev->bulk_out_addr),
			buf, count, power_board_write_bulk_callback, dev);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	result = usb_submit_urb(urb, GFP_KERNEL);
	if(result) {
		dev_err(&dev->udev->dev, "failed submitting URB, error %d", result);
	}

	usb_free_urb(urb);
	return count;

	//result = usb_bulk_msg(dev->udev, usb_sndbulkpipe(dev->udev, dev->bulk_out_addr), dev->bu);
error:
	usb_free_coherent(dev->udev, count, buf, urb->transfer_dma);
	usb_free_urb(urb);
	return result;
}

static const struct file_operations power_board_fops = {
	.owner = THIS_MODULE,
	.read = power_board_read,
	.write = power_board_write,
	.open = power_board_open,
	.release = power_board_release,
};

/*********************************
 * Board API
 ********************************/

long power_board_get_temperature(struct power_board *pb) {
	int retval;
	int temp;
	long celcius = -1;
	uint8_t *buffer = kmalloc(2, GFP_KERNEL);
	if (!buffer)
		return 0;
	mutex_lock(&pb->mutex);
	retval = vendor_command(pb->udev, VENDOR_REQUEST_TEMPERATURE, 0, 0, buffer, 2);
	if (retval < 0) {
		dev_err(&pb->udev->dev, "retval = %d\n", retval);
		goto out;
	}

	temp = buffer[0] << 8 | buffer[1];
	celcius = ((temp >> (16-12)) * 1000) >> (12-8);
out:
	kfree(buffer);
	mutex_unlock(&pb->mutex);
	return celcius;
}

int power_board_get_fanspeed(struct power_board *pb, uint8_t index) {
	int retval;
	int speed = -1;
	printk("FANSPEED %d start\n", index);
	uint8_t *buffer = kmalloc(2, GFP_KERNEL);
	if (!buffer)
		return 0;

	mutex_lock(&pb->mutex);
	retval = vendor_command(pb->udev, VENDOR_REQUEST_TEMPERATURE, 0, 0, buffer, 2);
	retval = vendor_command(pb->udev, VENDOR_REQUEST_FANSPEED, 0, index, buffer, 2);
	if (retval < 0) {
		dev_info(&pb->udev->dev, "retval = %d\n", retval);
		goto out;
	}
	speed = buffer[0] << 8 | buffer[1];
out:
	kfree(buffer);
	mutex_unlock(&pb->mutex);
	return speed;
}

/*********************************
 * HWMON Integration
 ********************************/

static int power_board_hwmon_read(struct device *dev, enum hwmon_sensor_types type,
		u32 attr, int channel, long *val)
{
	struct power_board *data = dev_get_drvdata(dev);

	switch(type) {
	case hwmon_temp:
		switch(attr) {
		case hwmon_temp_input:
			*val = power_board_get_temperature(data);
			return 0;
		}
		break;
	case hwmon_fan:
		switch(attr) {
		case hwmon_fan_input:
			*val = power_board_get_fanspeed(data, channel);
			return 0;
		}
		break;
	default:
		break;
	}
	return -EINVAL;
}

static int power_board_hwmon_write(struct device *dev, enum hwmon_sensor_types type,
		u32 attr, int channel, long val)
{
	return 0;
}

static umode_t power_board_hwmon_is_visible(const void *data, enum hwmon_sensor_types type,
		u32 attr, int channel)
{
	//const struct power_board *config_data = data;

	switch (type) {
	case hwmon_temp:
		switch (attr) {
		case hwmon_temp_input:
			return 0444;
		}
		break;
	case hwmon_fan:
		switch (attr) {
		case hwmon_fan_input:
			return 0444;
		}
		break;
	default:
		break;
	}
	return 0;
}

static const struct hwmon_channel_info * const power_board_hwmon_info[] = {
	HWMON_CHANNEL_INFO(temp, HWMON_T_INPUT),
	HWMON_CHANNEL_INFO(fan, HWMON_F_INPUT),
	NULL
};

static const struct hwmon_ops power_board_hwmon_ops = {
	.is_visible = power_board_hwmon_is_visible,
	.read = power_board_hwmon_read,
	.write = power_board_hwmon_write,
};

static const struct hwmon_chip_info power_board_chip_info = {
	.ops = &power_board_hwmon_ops,
	.info = power_board_hwmon_info,
};

/*********************************
 * SYSFS Integration
 ********************************/

static ssize_t temperature_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct usb_interface *intf = to_usb_interface(dev);
	struct power_board *pb = usb_get_intfdata(intf);

	long celcius = power_board_get_temperature(pb);
	return sprintf(buf, "%ld", celcius);
}
static DEVICE_ATTR_RO(temperature);

static struct attribute *power_board_attrs[] = {
	&dev_attr_temperature.attr,
	NULL,
};
ATTRIBUTE_GROUPS(power_board);

static struct usb_class_driver power_board_class = {
	.name = "powerboard%d",
	.fops = &power_board_fops,
	.minor_base = POWER_BOARD_MINOR_BASE,
};

static int power_board_probe(struct usb_interface *interface,
		const struct usb_device_id *id)
{
	int result;
	int protocol;
	struct power_board *dev;
	struct device *parent;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint_in;
	struct usb_endpoint_descriptor *endpoint_out;
	int in_end_size;
	int out_end_size;

	parent = &interface->dev;
	iface_desc = interface->cur_altsetting;

	if (iface_desc->desc.bInterfaceClass != 0xFF)
		return -ENODEV;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if(!dev)
		return -ENOMEM;

	kref_init(&dev->kref);
	mutex_init(&dev->mutex);

	dev->udev = usb_get_dev(interface_to_usbdev(interface));

	// Get the bulk-in and bulk-out endpoint
	result = usb_find_common_endpoints(iface_desc, &endpoint_in, &endpoint_out, NULL, NULL);
	if (result) {
		dev_err(&interface->dev, "Unable to find the bulk endpoints\n");
		goto error;
	}
	dev->bulk_in_addr = endpoint_in->bEndpointAddress;
	dev->bulk_out_addr = endpoint_out->bEndpointAddress;
	dev->protocol = protocol;
	dev->present = 1;

	dev->read_buffer_len = usb_endpoint_maxp(endpoint_in) * 10;
	dev->write_buffer_len = usb_endpoint_maxp(endpoint_in) * 10;
	dev->read_buffer = kmalloc(dev->read_buffer_len, GFP_KERNEL);
	if (!dev->read_buffer) {
		result = -ENOMEM;
		goto error;
	}
	dev->write_buffer = kmalloc(dev->write_buffer_len, GFP_KERNEL);
	if (!dev->write_buffer) {
		result = -ENOMEM;
		goto error;
	}

	usb_set_intfdata(interface, dev);

	result = usb_register_dev(interface, &power_board_class);
	if (result) {
		dev_err(&interface->dev, "Unable to allocate minor number.\n");
		usb_set_intfdata(interface, NULL);
		goto error;
	}

	// Register with the HWMON subsystem
	devm_hwmon_device_register_with_info(parent, "PowerBoard", dev, &power_board_chip_info, NULL);

	dev_info(&interface->dev, "Loaded FOSDEM power board driver\n");
	return 0;

error:
	kref_put(&dev->kref, power_board_delete);
	return result;

}

static void power_board_disconnect(struct usb_interface *interface)
{
	struct power_board *dev;
	dev = usb_get_intfdata(interface);

	mutex_lock(&dev->mutex);
	dev->present = 0;

	// Release the minor id
	usb_deregister_dev(interface, &power_board_class);

	// Release the device struct
	usb_set_intfdata(interface, NULL);

	// Clean up the rest of the references
	kref_put(&dev->kref, power_board_delete);
	mutex_unlock(&dev->mutex);
}

static struct usb_driver power_board_driver = {
	.name = "power_board",
	.probe = power_board_probe,
	.disconnect = power_board_disconnect,
	.id_table = power_board_table,
	.dev_groups = power_board_groups,
};

module_usb_driver(power_board_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Martijn Braam");
MODULE_DESCRIPTION("Kernel driver for the FOSDEM power board");
MODULE_VERSION("0.1");
