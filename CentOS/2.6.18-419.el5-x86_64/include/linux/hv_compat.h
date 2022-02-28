#ifndef _HV_COMPAT_H
#define _HV_COMPAT_H

#define CN_KVP_IDX	0x9
#define CN_KVP_VAL	0x1

#ifdef __KERNEL__

#include <linux/rcupdate.h>
#include <linux/version.h>
#include <linux/netdevice.h>
#include <linux/input.h>
#include <linux/inetdevice.h>
#include <linux/libata-compat.h> /* sg_* apis */
#include <asm/byteorder.h>
#include <asm/unaligned.h>
#include <net/arp.h>
#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_dbg.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_driver.h>
#include <scsi/scsi_eh.h>
#include <scsi/scsi_host.h>

#define CN_KVP_IDX	0x9
#define CN_KVP_VAL	0x1

#define IRQ0_VECTOR FIRST_EXTERNAL_VECTOR

static inline struct page *skb_frag_page(const skb_frag_t *frag)
{
	return frag->page;
}

static inline void *sg_virt(struct scatterlist *sg)
{
	return sg->page;
}

static inline void hv_set_buf(struct scatterlist *sg, void *buf,
                                unsigned int length)
{
	sg->page = (struct page *)buf;
	sg->length = length;
}

static inline int dev_set_name(struct device *dev, const char *fmt, int num)
{
	int err;

	sprintf(dev->bus_id, fmt, num);
	err = kobject_set_name(&dev->kobj, fmt, num);
	return err;
}

#define __DELAYED_WORK_INITIALIZER(n, f, d) {				\
        .work.entry  = { &(n.work).entry, &(n.work).entry },			\
        .work.func = (f),						\
        .work.data = (d),						\
        .work.timer = TIMER_INITIALIZER(NULL, 0, 0),			\
        }

#define DECLARE_DELAYED_WORK(n, f, d)                              \
	struct delayed_work n = __DELAYED_WORK_INITIALIZER(n, f, d)

void kernel_power_off(void);
void rhel_5_x_power_off(void);

#define orderly_poweroff(x) rhel_5_x_power_off()

#define BUS_VIRTUAL	0x06

struct hid_device_id {
	__u16 bus;
	__u16 pad1;
	__u32 vendor;
	__u32 product;
	unsigned long  driver_data;
};

#define HID_ANY_ID      (~0)
#define HID_BUS_ANY     0xffff

#define BTN_TRIGGER_HAPPY	0x2c0
#define KEY_CONTEXT_MENU	0x1b6
#define KEY_VIDEOPHONE		0x1a0
#define KEY_GAMES               0x1a1
#define KEY_MEDIA_REPEAT	0x1b7
#define KEY_WORDPROCESSOR	0x1a5
#define KEY_EDITOR		0x1a6
#define KEY_SPREADSHEET		0x1a7
#define KEY_GRAPHICSEDITOR	0x1a8
#define KEY_PRESENTATION	0x1a9
#define KEY_DATABASE            0x1aa
#define KEY_NEWS		0x1ab
#define KEY_VOICEMAIL		0x1ac
#define KEY_ADDRESSBOOK		0x1ad
#define KEY_LOGOFF		0x1b1
#define KEY_SPELLCHECK		0x1b0
#define KEY_IMAGES		0x1ba
#define KEY_MESSENGER		0x1ae
#define KEY_ZOOMIN		0x1a2
#define KEY_ZOOMOUT		0x1a3
#define KEY_ZOOMRESET		0x1a4
#define ABS_MT_POSITION_X	0x35


struct input_keymap_entry {
#define INPUT_KEYMAP_BY_INDEX   (1 << 0)
	__u8  flags;
	__u8  len;
	__u16 index;
	__u32 keycode;
	__u8  scancode[32];
};
#undef CONFIG_HIDRAW

static inline void *input_get_drvdata(struct input_dev *dev)
{
	return dev->dev;

}

static inline void input_set_drvdata(struct input_dev *dev, void *data)
{
	dev->dev = data;
}

static inline int input_scancode_to_scalar(const struct input_keymap_entry *ke,
                             unsigned int *scancode)
{
        switch (ke->len) {
        case 1:
                *scancode = *((u8 *)ke->scancode);
                break;

        case 2:
                *scancode = *((u16 *)ke->scancode);
                break;

        case 4:
                *scancode = *((u32 *)ke->scancode);
                break;

        default:
                return -EINVAL;
        }

        return 0;
}

/*
 * Synchronization events.
 */

#define SYN_REPORT		0
#define SYN_CONFIG		1
#define SYN_MT_REPORT		2
#define SYN_DROPPED		3

#define ABS_MT_TOUCH_MAJOR	0x30    /* Major axis of touching ellipse */
#define ABS_MT_DISTANCE		0x3b    /* Contact hover distance */
#define ABS_MT_TOUCH_MINOR	0x31    /* Minor axis (omit if circular) */
#define ABS_MT_WIDTH_MAJOR	0x32    /* Major axis of approaching ellipse */
#define ABS_MT_WIDTH_MINOR	0x33    /* Minor axis (omit if circular) */
#define ABS_MT_ORIENTATION	0x34    /* Ellipse orientation */
#define ABS_MT_POSITION_Y	0x36    /* Center Y ellipse position */
#define ABS_MT_TOOL_TYPE	0x37    /* Type of touching device */
#define ABS_MT_BLOB_ID		0x38    /* Group a set of packets as a blob */

/* Implementation details, userspace should not care about these */
#define ABS_MT_FIRST		ABS_MT_TOUCH_MAJOR
#define ABS_MT_LAST		ABS_MT_DISTANCE

#define ABS_MAX                 0x3f
#define ABS_CNT                 (ABS_MAX+1)


struct timespec ns_to_timespec(const s64 nsec);

static inline __u16 get_unaligned_le16(const void *p)
{
    return le16_to_cpu(get_unaligned((__le16 *)p));
}

static inline __u32 get_unaligned_le32(const void *p)
{
    return le32_to_cpu(get_unaligned((__le32 *)p));
}

static inline __u64 get_unaligned_le64(const void *p)
{
    return le64_to_cpu(get_unaligned((__le64 *)p));
}

static inline void put_unaligned_le64(__u64 val, const void *p)
{
    put_unaligned(cpu_to_be64(val), (__le64 *)p);
}

#endif
#endif
