/*
 *  Copyright 2003-2005 Red Hat, Inc.  All rights reserved.
 *  Copyright 2003-2005 Jeff Garzik
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 *  libata documentation is available via 'make {ps|pdf}docs',
 *  as Documentation/DocBook/libata.*
 *
 */

#ifndef __LINUX_LIBATA_H__
#define __LINUX_LIBATA_H__

#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>
#include <asm/scatterlist.h>
#include <asm/io.h>
#include <linux/ata.h>
#include <linux/workqueue.h>
#include <scsi/scsi_host.h>

/*
 * compile-time options: to be removed as soon as all the drivers are
 * converted to the new debugging mechanism
 */
#undef ATA_DEBUG		/* debugging output */
#undef ATA_VERBOSE_DEBUG	/* yet more debugging output */
#undef ATA_IRQ_TRAP		/* define to ack screaming irqs */
#undef ATA_NDEBUG		/* define to disable quick runtime checks */
#undef ATA_ENABLE_PATA		/* define to enable PATA support in some
				 * low-level drivers */


/* note: prints function name for you */
#ifdef ATA_DEBUG
#define DPRINTK(fmt, args...) printk(KERN_ERR "%s: " fmt, __FUNCTION__, ## args)
#ifdef ATA_VERBOSE_DEBUG
#define VPRINTK(fmt, args...) printk(KERN_ERR "%s: " fmt, __FUNCTION__, ## args)
#else
#define VPRINTK(fmt, args...)
#endif	/* ATA_VERBOSE_DEBUG */
#else
#define DPRINTK(fmt, args...)
#define VPRINTK(fmt, args...)
#endif	/* ATA_DEBUG */

#define BPRINTK(fmt, args...) if (ap->flags & ATA_FLAG_DEBUGMSG) printk(KERN_ERR "%s: " fmt, __FUNCTION__, ## args)

/* NEW: debug levels */
#define HAVE_LIBATA_MSG 1

enum {
	ATA_MSG_DRV	= 0x0001,
	ATA_MSG_INFO	= 0x0002,
	ATA_MSG_PROBE	= 0x0004,
	ATA_MSG_WARN	= 0x0008,
	ATA_MSG_MALLOC	= 0x0010,
	ATA_MSG_CTL	= 0x0020,
	ATA_MSG_INTR	= 0x0040,
	ATA_MSG_ERR	= 0x0080,
};

#define ata_msg_drv(p)    ((p)->msg_enable & ATA_MSG_DRV)
#define ata_msg_info(p)   ((p)->msg_enable & ATA_MSG_INFO)
#define ata_msg_probe(p)  ((p)->msg_enable & ATA_MSG_PROBE)
#define ata_msg_warn(p)   ((p)->msg_enable & ATA_MSG_WARN)
#define ata_msg_malloc(p) ((p)->msg_enable & ATA_MSG_MALLOC)
#define ata_msg_ctl(p)    ((p)->msg_enable & ATA_MSG_CTL)
#define ata_msg_intr(p)   ((p)->msg_enable & ATA_MSG_INTR)
#define ata_msg_err(p)    ((p)->msg_enable & ATA_MSG_ERR)

static inline u32 ata_msg_init(int dval, int default_msg_enable_bits)
{
	if (dval < 0 || dval >= (sizeof(u32) * 8))
		return default_msg_enable_bits; /* should be 0x1 - only driver info msgs */
	if (!dval)
		return 0;
	return (1 << dval) - 1;
}

/* defines only for the constants which don't work well as enums */
#define ATA_TAG_POISON		0xfafbfcfdU

/* move to PCI layer? */
static inline struct device *pci_dev_to_dev(struct pci_dev *pdev)
{
	return &pdev->dev;
}

enum {
	/* various global constants */
	LIBATA_MAX_PRD		= ATA_MAX_PRD / 2,
	ATA_MAX_PORTS		= 8,
	ATA_DEF_QUEUE		= 1,
	/* tag ATA_MAX_QUEUE - 1 is reserved for internal commands */
	ATA_MAX_QUEUE		= 32,
	ATA_TAG_INTERNAL	= ATA_MAX_QUEUE - 1,
	ATA_MAX_SECTORS		= 200,	/* FIXME */
	ATA_MAX_SECTORS_LBA48	= 65535,
	ATA_MAX_BUS		= 2,
	ATA_DEF_BUSY_WAIT	= 10000,
	ATA_SHORT_PAUSE		= (HZ >> 6) + 1,

	ATA_SHT_EMULATED	= 1,
	ATA_SHT_CMD_PER_LUN	= 1,
	ATA_SHT_THIS_ID		= -1,
	ATA_SHT_USE_CLUSTERING	= 1,

	/* struct ata_device stuff */
	ATA_DFLAG_LBA		= (1 << 0), /* device supports LBA */
	ATA_DFLAG_LBA48		= (1 << 1), /* device supports LBA48 */
	ATA_DFLAG_CDB_INTR	= (1 << 2), /* device asserts INTRQ when ready for CDB */
	ATA_DFLAG_NCQ		= (1 << 3), /* device supports NCQ */
	ATA_DFLAG_CFG_MASK	= (1 << 8) - 1,

	ATA_DFLAG_PIO		= (1 << 8), /* device currently in PIO mode */
	ATA_DFLAG_SUSPENDED	= (1 << 9), /* device suspended */
	ATA_DFLAG_INIT_MASK	= (1 << 16) - 1,

	ATA_DFLAG_DETACH	= (1 << 16),
	ATA_DFLAG_DETACHED	= (1 << 17),

	ATA_DEV_UNKNOWN		= 0,	/* unknown device */
	ATA_DEV_ATA		= 1,	/* ATA device */
	ATA_DEV_ATA_UNSUP	= 2,	/* ATA device (unsupported) */
	ATA_DEV_ATAPI		= 3,	/* ATAPI device */
	ATA_DEV_ATAPI_UNSUP	= 4,	/* ATAPI device (unsupported) */
	ATA_DEV_NONE		= 5,	/* no device */

	/* struct ata_port flags */
	ATA_FLAG_SLAVE_POSS	= (1 << 0), /* host supports slave dev */
					    /* (doesn't imply presence) */
	ATA_FLAG_SATA		= (1 << 1),
	ATA_FLAG_NO_LEGACY	= (1 << 2), /* no legacy mode check */
	ATA_FLAG_MMIO		= (1 << 3), /* use MMIO, not PIO */
	ATA_FLAG_SRST		= (1 << 4), /* (obsolete) use ATA SRST, not E.D.D. */
	ATA_FLAG_SATA_RESET	= (1 << 5), /* (obsolete) use COMRESET */
	ATA_FLAG_NO_ATAPI	= (1 << 6), /* No ATAPI support */
	ATA_FLAG_PIO_DMA	= (1 << 7), /* PIO cmds via DMA */
	ATA_FLAG_PIO_LBA48	= (1 << 8), /* Host DMA engine is LBA28 only */
	ATA_FLAG_PIO_POLLING	= (1 << 9), /* use polling PIO if LLD
					     * doesn't handle PIO interrupts */
	ATA_FLAG_NCQ		= (1 << 10), /* host supports NCQ */
	ATA_FLAG_HRST_TO_RESUME	= (1 << 11), /* hardreset to resume phy */
	ATA_FLAG_SKIP_D2H_BSY	= (1 << 12), /* can't wait for the first D2H
					      * Register FIS clearing BSY */
	ATA_FLAG_DEBUGMSG	= (1 << 13),

	/* The following flag belongs to ap->pflags but is kept in
	 * ap->flags because it's referenced in many LLDs and will be
	 * removed in not-too-distant future.
	 */
	ATA_FLAG_DISABLED	= (1 << 23), /* port is disabled, ignore it */

	/* bits 24:31 of ap->flags are reserved for LLD specific flags */

	/* struct ata_port pflags */
	ATA_PFLAG_EH_PENDING	= (1 << 0), /* EH pending */
	ATA_PFLAG_EH_IN_PROGRESS = (1 << 1), /* EH in progress */
	ATA_PFLAG_FROZEN	= (1 << 2), /* port is frozen */
	ATA_PFLAG_RECOVERED	= (1 << 3), /* recovery action performed */
	ATA_PFLAG_LOADING	= (1 << 4), /* boot/loading probe */
	ATA_PFLAG_UNLOADING	= (1 << 5), /* module is unloading */
	ATA_PFLAG_SCSI_HOTPLUG	= (1 << 6), /* SCSI hotplug scheduled */

	ATA_PFLAG_FLUSH_PORT_TASK = (1 << 16), /* flush port task */
	ATA_PFLAG_SUSPENDED	= (1 << 17), /* port is suspended (power) */
	ATA_PFLAG_PM_PENDING	= (1 << 18), /* PM operation pending */

	/* struct ata_queued_cmd flags */
	ATA_QCFLAG_ACTIVE	= (1 << 0), /* cmd not yet ack'd to scsi lyer */
	ATA_QCFLAG_SG		= (1 << 1), /* have s/g table? */
	ATA_QCFLAG_SINGLE	= (1 << 2), /* no s/g, just a single buffer */
	ATA_QCFLAG_DMAMAP	= ATA_QCFLAG_SG | ATA_QCFLAG_SINGLE,
	ATA_QCFLAG_IO		= (1 << 3), /* standard IO command */
	ATA_QCFLAG_RESULT_TF	= (1 << 4), /* result TF requested */

	ATA_QCFLAG_FAILED	= (1 << 16), /* cmd failed and is owned by EH */
	ATA_QCFLAG_SENSE_VALID	= (1 << 17), /* sense data valid */
	ATA_QCFLAG_EH_SCHEDULED = (1 << 18), /* EH scheduled (obsolete) */

	/* host set flags */
	ATA_HOST_SIMPLEX	= (1 << 0),	/* Host is simplex, one DMA channel per host_set only */
	
	/* various lengths of time */
	ATA_TMOUT_BOOT		= 30 * HZ,	/* heuristic */
	ATA_TMOUT_BOOT_QUICK	= 7 * HZ,	/* heuristic */
	ATA_TMOUT_INTERNAL	= 30 * HZ,
	ATA_TMOUT_INTERNAL_QUICK = 5 * HZ,

	/* ATA bus states */
	BUS_UNKNOWN		= 0,
	BUS_DMA			= 1,
	BUS_IDLE		= 2,
	BUS_NOINTR		= 3,
	BUS_NODATA		= 4,
	BUS_TIMER		= 5,
	BUS_PIO			= 6,
	BUS_EDD			= 7,
	BUS_IDENTIFY		= 8,
	BUS_PACKET		= 9,

	/* SATA port states */
	PORT_UNKNOWN		= 0,
	PORT_ENABLED		= 1,
	PORT_DISABLED		= 2,

	/* encoding various smaller bitmaps into a single
	 * unsigned int bitmap
	 */
	ATA_BITS_PIO		= 5,
	ATA_BITS_MWDMA		= 3,
	ATA_BITS_UDMA		= 8,

	ATA_SHIFT_PIO		= 0,
	ATA_SHIFT_MWDMA		= ATA_SHIFT_PIO + ATA_BITS_PIO,
	ATA_SHIFT_UDMA		= ATA_SHIFT_MWDMA + ATA_BITS_MWDMA,

	ATA_MASK_PIO		= ((1 << ATA_BITS_PIO) - 1) << ATA_SHIFT_PIO,
	ATA_MASK_MWDMA		= ((1 << ATA_BITS_MWDMA) - 1) << ATA_SHIFT_MWDMA,
	ATA_MASK_UDMA		= ((1 << ATA_BITS_UDMA) - 1) << ATA_SHIFT_UDMA,

	/* size of buffer to pad xfers ending on unaligned boundaries */
	ATA_DMA_PAD_SZ		= 4,
	ATA_DMA_PAD_BUF_SZ	= ATA_DMA_PAD_SZ * ATA_MAX_QUEUE,

	/* masks for port functions */
	ATA_PORT_PRIMARY	= (1 << 0),
	ATA_PORT_SECONDARY	= (1 << 1),

	/* ering size */
	ATA_ERING_SIZE		= 32,

	/* desc_len for ata_eh_info and context */
	ATA_EH_DESC_LEN		= 80,

	/* reset / recovery action types */
	ATA_EH_REVALIDATE	= (1 << 0),
	ATA_EH_SOFTRESET	= (1 << 1),
	ATA_EH_HARDRESET	= (1 << 2),
	ATA_EH_SUSPEND		= (1 << 3),
	ATA_EH_RESUME		= (1 << 4),
	ATA_EH_PM_FREEZE	= (1 << 5),

	ATA_EH_RESET_MASK	= ATA_EH_SOFTRESET | ATA_EH_HARDRESET,
	ATA_EH_PERDEV_MASK	= ATA_EH_REVALIDATE | ATA_EH_SUSPEND |
				  ATA_EH_RESUME | ATA_EH_PM_FREEZE,

	/* ata_eh_info->flags */
	ATA_EHI_HOTPLUGGED	= (1 << 0),  /* could have been hotplugged */
	ATA_EHI_RESUME_LINK	= (1 << 1),  /* resume link (reset modifier) */
	ATA_EHI_NO_AUTOPSY	= (1 << 2),  /* no autopsy */
	ATA_EHI_QUIET		= (1 << 3),  /* be quiet */

	ATA_EHI_DID_RESET	= (1 << 16), /* already reset this port */

	ATA_EHI_RESET_MODIFIER_MASK = ATA_EHI_RESUME_LINK,

	/* max repeat if error condition is still set after ->error_handler */
	ATA_EH_MAX_REPEAT	= 5,

	/* how hard are we gonna try to probe/recover devices */
	ATA_PROBE_MAX_TRIES	= 3,
	ATA_EH_RESET_TRIES	= 3,
	ATA_EH_DEV_TRIES	= 3,

	/* Drive spinup time (time from power-on to the first D2H FIS)
	 * in msecs - 8s currently.  Failing to get ready in this time
	 * isn't critical.  It will result in reset failure for
	 * controllers which can't wait for the first D2H FIS.  libata
	 * will retry, so it just has to be long enough to spin up
	 * most devices.
	 */
	ATA_SPINUP_WAIT		= 8000,
};

enum hsm_task_states {
	HSM_ST_UNKNOWN,		/* state unknown */
	HSM_ST_IDLE,		/* no command on going */
	HSM_ST,			/* (waiting the device to) transfer data */
	HSM_ST_LAST,		/* (waiting the device to) complete command */
	HSM_ST_ERR,		/* error */
	HSM_ST_FIRST,		/* (waiting the device to)
				   write CDB or first data block */
};

enum ata_completion_errors {
	AC_ERR_DEV		= (1 << 0), /* device reported error */
	AC_ERR_HSM		= (1 << 1), /* host state machine violation */
	AC_ERR_TIMEOUT		= (1 << 2), /* timeout */
	AC_ERR_MEDIA		= (1 << 3), /* media error */
	AC_ERR_ATA_BUS		= (1 << 4), /* ATA bus error */
	AC_ERR_HOST_BUS		= (1 << 5), /* host bus error */
	AC_ERR_SYSTEM		= (1 << 6), /* system error */
	AC_ERR_INVALID		= (1 << 7), /* invalid argument */
	AC_ERR_OTHER		= (1 << 8), /* unknown */
};

/* forward declarations */
struct scsi_device;
struct ata_port_operations;
struct ata_port;
struct ata_queued_cmd;

/* typedefs */
typedef void (*ata_qc_cb_t) (struct ata_queued_cmd *qc);
typedef int (*ata_prereset_fn_t)(struct ata_port *ap);
typedef int (*ata_reset_fn_t)(struct ata_port *ap, unsigned int *classes);
typedef void (*ata_postreset_fn_t)(struct ata_port *ap, unsigned int *classes);

struct ata_ioports {
	unsigned long		cmd_addr;
	unsigned long		data_addr;
	unsigned long		error_addr;
	unsigned long		feature_addr;
	unsigned long		nsect_addr;
	unsigned long		lbal_addr;
	unsigned long		lbam_addr;
	unsigned long		lbah_addr;
	unsigned long		device_addr;
	unsigned long		status_addr;
	unsigned long		command_addr;
	unsigned long		altstatus_addr;
	unsigned long		ctl_addr;
	unsigned long		bmdma_addr;
	unsigned long		scr_addr;
};

struct ata_probe_ent {
	struct list_head	node;
	struct device 		*dev;
	const struct ata_port_operations *port_ops;
	struct scsi_host_template *sht;
	struct ata_ioports	port[ATA_MAX_PORTS];
	unsigned int		n_ports;
	unsigned int		hard_port_no;
	unsigned int		pio_mask;
	unsigned int		mwdma_mask;
	unsigned int		udma_mask;
	unsigned int		legacy_mode;
	unsigned long		irq;
	unsigned int		irq_flags;
	unsigned long		host_flags;
	unsigned long		port_flags[ATA_MAX_PORTS];	/* pata fix */
	unsigned long		host_set_flags;
	void __iomem		*mmio_base;
	void			*private_data;
};

struct ata_host_set {
	spinlock_t		lock;
	struct device 		*dev;
	unsigned long		irq;
	void __iomem		*mmio_base;
	unsigned int		n_ports;
	void			*private_data;
	const struct ata_port_operations *ops;
	unsigned long		flags;
	int			simplex_claimed;	/* Keep seperate in case we
							   ever need to do this locked */
	struct ata_host_set	*next;		/* for legacy mode */
	struct ata_port		*ports[0];
};

struct ata_queued_cmd {
	struct ata_port		*ap;
	struct ata_device	*dev;

	struct scsi_cmnd	*scsicmd;
	void			(*scsidone)(struct scsi_cmnd *);

	struct ata_taskfile	tf;
	u8			cdb[ATAPI_CDB_LEN];

	unsigned long		flags;		/* ATA_QCFLAG_xxx */
	unsigned int		tag;
	unsigned int		n_elem;
	unsigned int		orig_n_elem;

	int			dma_dir;

	unsigned int		pad_len;

	unsigned int		nsect;
	unsigned int		cursect;

	unsigned int		nbytes;
	unsigned int		curbytes;

	unsigned int		cursg;
	unsigned int		cursg_ofs;

	struct scatterlist	sgent;
	struct scatterlist	pad_sgent;
	void			*buf_virt;

	/* DO NOT iterate over __sg manually, use ata_for_each_sg() */
	struct scatterlist	*__sg;

	unsigned int		err_mask;
	struct ata_taskfile	result_tf;
	ata_qc_cb_t		complete_fn;

	void			*private_data;
};

struct ata_host_stats {
	unsigned long		unhandled_irq;
	unsigned long		idle_irq;
	unsigned long		rw_reqbuf;
};

struct ata_ering_entry {
	int			is_io;
	unsigned int		err_mask;
	u64			timestamp;
};

struct ata_ering {
	int			cursor;
	struct ata_ering_entry	ring[ATA_ERING_SIZE];
};

struct ata_device {
	struct ata_port		*ap;
	unsigned int		devno;		/* 0 or 1 */
	unsigned long		flags;		/* ATA_DFLAG_xxx */
	struct scsi_device	*sdev;		/* attached SCSI device */
	/* n_sector is used as CLEAR_OFFSET, read comment above CLEAR_OFFSET */
	u64			n_sectors;	/* size of device, if ATA */
	unsigned int		class;		/* ATA_DEV_xxx */
	u16			id[ATA_ID_WORDS]; /* IDENTIFY xxx DEVICE data */
	u8			pio_mode;
	u8			dma_mode;
	u8			xfer_mode;
	unsigned int		xfer_shift;	/* ATA_SHIFT_xxx */

	unsigned int		multi_count;	/* sectors count for
						   READ/WRITE MULTIPLE */
	unsigned int		max_sectors;	/* per-device max sectors */
	unsigned int		cdb_len;

	/* per-dev xfer mask */
	unsigned int		pio_mask;
	unsigned int		mwdma_mask;
	unsigned int		udma_mask;

	/* for CHS addressing */
	u16			cylinders;	/* Number of cylinders */
	u16			heads;		/* Number of heads */
	u16			sectors;	/* Number of sectors per track */

	/* error history */
	struct ata_ering	ering;
};

/* Offset into struct ata_device.  Fields above it are maintained
 * acress device init.  Fields below are zeroed.
 */
#define ATA_DEVICE_CLEAR_OFFSET		offsetof(struct ata_device, n_sectors)

struct ata_eh_info {
	struct ata_device	*dev;		/* offending device */
	u32			serror;		/* SError from LLDD */
	unsigned int		err_mask;	/* port-wide err_mask */
	unsigned int		action;		/* ATA_EH_* action mask */
	unsigned int		dev_action[ATA_MAX_DEVICES]; /* dev EH action */
	unsigned int		flags;		/* ATA_EHI_* flags */

	unsigned long		hotplug_timestamp;
	unsigned int		probe_mask;

	char			desc[ATA_EH_DESC_LEN];
	int			desc_len;
};

struct ata_eh_context {
	struct ata_eh_info	i;
	int			tries[ATA_MAX_DEVICES];
	unsigned int		classes[ATA_MAX_DEVICES];
	unsigned int		did_probe_mask;
};

struct ata_port {
	struct Scsi_Host	*host;	/* our co-allocated scsi host */
	const struct ata_port_operations *ops;
	spinlock_t		*lock;
	unsigned long		flags;	/* ATA_FLAG_xxx */
	unsigned int		pflags; /* ATA_PFLAG_xxx */
	unsigned int		id;	/* unique id req'd by scsi midlyr */
	unsigned int		port_no; /* unique port #; from zero */
	unsigned int		hard_port_no;	/* hardware port #; from zero */

	struct ata_prd		*prd;	 /* our SG list */
	dma_addr_t		prd_dma; /* and its DMA mapping */

	void			*pad;	/* array of DMA pad buffers */
	dma_addr_t		pad_dma;

	struct ata_ioports	ioaddr;	/* ATA cmd/ctl/dma register blocks */

	u8			ctl;	/* cache of ATA control register */
	u8			last_ctl;	/* Cache last written value */
	unsigned int		pio_mask;
	unsigned int		mwdma_mask;
	unsigned int		udma_mask;
	unsigned int		cbl;	/* cable type; ATA_CBL_xxx */
	unsigned int		hw_sata_spd_limit;
	unsigned int		sata_spd_limit;	/* SATA PHY speed limit */

	/* record runtime error info, protected by host_set lock */
	struct ata_eh_info	eh_info;
	/* EH context owned by EH */
	struct ata_eh_context	eh_context;

	struct ata_device	device[ATA_MAX_DEVICES];

	struct ata_queued_cmd	qcmd[ATA_MAX_QUEUE];
	unsigned long		qc_allocated;
	unsigned int		qc_active;

	unsigned int		active_tag;
	u32			sactive;

	struct ata_host_stats	stats;
	struct ata_host_set	*host_set;
	struct device 		*dev;

	struct work_struct	port_task;
	struct work_struct	hotplug_task;
	struct work_struct	scsi_rescan_task;

	unsigned int		hsm_task_state;

	u32			msg_enable;
	struct list_head	eh_done_q;
	wait_queue_head_t	eh_wait_q;

	pm_message_t		pm_mesg;
	int			*pm_result;

	void			*private_data;

	u8			sector_buf[ATA_SECT_SIZE]; /* owned by EH */
};

struct ata_port_operations {
	void (*port_disable) (struct ata_port *);

	void (*dev_config) (struct ata_port *, struct ata_device *);

	void (*set_piomode) (struct ata_port *, struct ata_device *);
	void (*set_dmamode) (struct ata_port *, struct ata_device *);
	unsigned long (*mode_filter) (const struct ata_port *, struct ata_device *, unsigned long);

	void (*tf_load) (struct ata_port *ap, const struct ata_taskfile *tf);
	void (*tf_read) (struct ata_port *ap, struct ata_taskfile *tf);

	void (*exec_command)(struct ata_port *ap, const struct ata_taskfile *tf);
	u8   (*check_status)(struct ata_port *ap);
	u8   (*check_altstatus)(struct ata_port *ap);
	void (*dev_select)(struct ata_port *ap, unsigned int device);

	void (*phy_reset) (struct ata_port *ap); /* obsolete */
	void (*set_mode) (struct ata_port *ap);

	void (*post_set_mode) (struct ata_port *ap);

	int (*check_atapi_dma) (struct ata_queued_cmd *qc);

	void (*bmdma_setup) (struct ata_queued_cmd *qc);
	void (*bmdma_start) (struct ata_queued_cmd *qc);

	void (*data_xfer) (struct ata_device *, unsigned char *, unsigned int, int);

	void (*qc_prep) (struct ata_queued_cmd *qc);
	unsigned int (*qc_issue) (struct ata_queued_cmd *qc);

	/* Error handlers.  ->error_handler overrides ->eng_timeout and
	 * indicates that new-style EH is in place.
	 */
	void (*eng_timeout) (struct ata_port *ap); /* obsolete */

	void (*freeze) (struct ata_port *ap);
	void (*thaw) (struct ata_port *ap);
	void (*error_handler) (struct ata_port *ap);
	void (*post_internal_cmd) (struct ata_queued_cmd *qc);

	irqreturn_t (*irq_handler)(int, void *, struct pt_regs *);
	void (*irq_clear) (struct ata_port *);

	u32 (*scr_read) (struct ata_port *ap, unsigned int sc_reg);
	void (*scr_write) (struct ata_port *ap, unsigned int sc_reg,
			   u32 val);

	int (*port_suspend) (struct ata_port *ap, pm_message_t mesg);
	int (*port_resume) (struct ata_port *ap);

	int (*port_start) (struct ata_port *ap);
	void (*port_stop) (struct ata_port *ap);

	void (*host_stop) (struct ata_host_set *host_set);

	void (*bmdma_stop) (struct ata_queued_cmd *qc);
	u8   (*bmdma_status) (struct ata_port *ap);
};

struct ata_port_info {
	struct scsi_host_template	*sht;
	unsigned long		host_flags;
	unsigned long		pio_mask;
	unsigned long		mwdma_mask;
	unsigned long		udma_mask;
	const struct ata_port_operations *port_ops;
	void 			*private_data;
};

struct ata_timing {
	unsigned short mode;		/* ATA mode */
	unsigned short setup;		/* t1 */
	unsigned short act8b;		/* t2 for 8-bit I/O */
	unsigned short rec8b;		/* t2i for 8-bit I/O */
	unsigned short cyc8b;		/* t0 for 8-bit I/O */
	unsigned short active;		/* t2 or tD */
	unsigned short recover;		/* t2i or tK */
	unsigned short cycle;		/* t0 */
	unsigned short udma;		/* t2CYCTYP/2 */
};

#define FIT(v,vmin,vmax)	max_t(short,min_t(short,v,vmax),vmin)

extern const unsigned long sata_deb_timing_normal[];
extern const unsigned long sata_deb_timing_hotplug[];
extern const unsigned long sata_deb_timing_long[];

static inline const unsigned long *
sata_ehc_deb_timing(struct ata_eh_context *ehc)
{
	if (ehc->i.flags & ATA_EHI_HOTPLUGGED)
		return sata_deb_timing_hotplug;
	else
		return sata_deb_timing_normal;
}

extern void ata_port_probe(struct ata_port *);
extern void __sata_phy_reset(struct ata_port *ap);
extern void sata_phy_reset(struct ata_port *ap);
extern void ata_bus_reset(struct ata_port *ap);
extern int sata_set_spd(struct ata_port *ap);
extern int sata_phy_debounce(struct ata_port *ap, const unsigned long *param);
extern int sata_phy_resume(struct ata_port *ap, const unsigned long *param);
extern int ata_std_prereset(struct ata_port *ap);
extern int ata_std_softreset(struct ata_port *ap, unsigned int *classes);
extern int sata_std_hardreset(struct ata_port *ap, unsigned int *class);
extern void ata_std_postreset(struct ata_port *ap, unsigned int *classes);
extern int ata_dev_revalidate(struct ata_device *dev, int post_reset);
extern void ata_port_disable(struct ata_port *);
extern void ata_std_ports(struct ata_ioports *ioaddr);
#ifdef CONFIG_PCI
extern int ata_pci_init_one (struct pci_dev *pdev, struct ata_port_info **port_info,
			     unsigned int n_ports);
extern void ata_pci_remove_one (struct pci_dev *pdev);
extern void ata_pci_device_do_suspend(struct pci_dev *pdev, pm_message_t state);
extern void ata_pci_device_do_resume(struct pci_dev *pdev);
extern int ata_pci_device_suspend(struct pci_dev *pdev, pm_message_t state);
extern int ata_pci_device_resume(struct pci_dev *pdev);
extern int ata_pci_clear_simplex(struct pci_dev *pdev);
#endif /* CONFIG_PCI */
extern int ata_device_add(const struct ata_probe_ent *ent);
extern void ata_port_detach(struct ata_port *ap);
extern void ata_host_set_init(struct ata_host_set *, struct device *,
			      unsigned long, const struct ata_port_operations *);
extern void ata_host_set_remove(struct ata_host_set *host_set);
extern int ata_scsi_detect(struct scsi_host_template *sht);
extern int ata_scsi_ioctl(struct scsi_device *dev, int cmd, void __user *arg);
extern int ata_scsi_queuecmd(struct scsi_cmnd *cmd, void (*done)(struct scsi_cmnd *));
extern int ata_scsi_release(struct Scsi_Host *host);
extern void ata_sas_port_destroy(struct ata_port *);
extern struct ata_port *ata_sas_port_alloc(struct ata_host_set *,
					   struct ata_port_info *, struct Scsi_Host *);
extern int ata_sas_port_init(struct ata_port *);
extern int ata_sas_port_start(struct ata_port *ap);
extern void ata_sas_port_stop(struct ata_port *ap);
extern int ata_sas_slave_configure(struct scsi_device *, struct ata_port *);
extern int ata_sas_queuecmd(struct scsi_cmnd *cmd, void (*done)(struct scsi_cmnd *),
			    struct ata_port *ap);
extern unsigned int ata_host_intr(struct ata_port *ap, struct ata_queued_cmd *qc);
extern int sata_scr_valid(struct ata_port *ap);
extern int sata_scr_read(struct ata_port *ap, int reg, u32 *val);
extern int sata_scr_write(struct ata_port *ap, int reg, u32 val);
extern int sata_scr_write_flush(struct ata_port *ap, int reg, u32 val);
extern int ata_port_online(struct ata_port *ap);
extern int ata_port_offline(struct ata_port *ap);
extern int ata_scsi_device_resume(struct scsi_device *);
extern int ata_scsi_device_suspend(struct scsi_device *, pm_message_t state);
extern int ata_host_set_suspend(struct ata_host_set *host_set,
				pm_message_t mesg);
extern void ata_host_set_resume(struct ata_host_set *host_set);
extern int ata_ratelimit(void);
extern unsigned int ata_busy_sleep(struct ata_port *ap,
				   unsigned long timeout_pat,
				   unsigned long timeout);
extern void ata_port_queue_task(struct ata_port *ap, void (*fn)(void *),
				void *data, unsigned long delay);
extern u32 ata_wait_register(void __iomem *reg, u32 mask, u32 val,
			     unsigned long interval_msec,
			     unsigned long timeout_msec);

/*
 * Default driver ops implementations
 */
extern void ata_tf_load(struct ata_port *ap, const struct ata_taskfile *tf);
extern void ata_tf_read(struct ata_port *ap, struct ata_taskfile *tf);
extern void ata_tf_to_fis(const struct ata_taskfile *tf, u8 *fis, u8 pmp);
extern void ata_tf_from_fis(const u8 *fis, struct ata_taskfile *tf);
extern void ata_noop_dev_select (struct ata_port *ap, unsigned int device);
extern void ata_std_dev_select (struct ata_port *ap, unsigned int device);
extern u8 ata_check_status(struct ata_port *ap);
extern u8 ata_altstatus(struct ata_port *ap);
extern void ata_exec_command(struct ata_port *ap, const struct ata_taskfile *tf);
extern int ata_port_start (struct ata_port *ap);
extern void ata_port_stop (struct ata_port *ap);
extern void ata_host_stop (struct ata_host_set *host_set);
extern irqreturn_t ata_interrupt (int irq, void *dev_instance, struct pt_regs *regs);
extern void ata_mmio_data_xfer(struct ata_device *adev, unsigned char *buf,
			       unsigned int buflen, int write_data);
extern void ata_pio_data_xfer(struct ata_device *adev, unsigned char *buf,
			      unsigned int buflen, int write_data);
extern void ata_pio_data_xfer_noirq(struct ata_device *adev, unsigned char *buf,
			      unsigned int buflen, int write_data);
extern void ata_qc_prep(struct ata_queued_cmd *qc);
extern void ata_noop_qc_prep(struct ata_queued_cmd *qc);
extern unsigned int ata_qc_issue_prot(struct ata_queued_cmd *qc);
extern void ata_sg_init_one(struct ata_queued_cmd *qc, void *buf,
		unsigned int buflen);
extern void ata_sg_init(struct ata_queued_cmd *qc, struct scatterlist *sg,
		 unsigned int n_elem);
extern unsigned int ata_dev_classify(const struct ata_taskfile *tf);
extern void ata_id_string(const u16 *id, unsigned char *s,
			  unsigned int ofs, unsigned int len);
extern void ata_id_c_string(const u16 *id, unsigned char *s,
			    unsigned int ofs, unsigned int len);
extern void ata_bmdma_setup (struct ata_queued_cmd *qc);
extern void ata_bmdma_start (struct ata_queued_cmd *qc);
extern void ata_bmdma_stop(struct ata_queued_cmd *qc);
extern u8   ata_bmdma_status(struct ata_port *ap);
extern void ata_bmdma_irq_clear(struct ata_port *ap);
extern void ata_bmdma_freeze(struct ata_port *ap);
extern void ata_bmdma_thaw(struct ata_port *ap);
extern void ata_bmdma_drive_eh(struct ata_port *ap, ata_prereset_fn_t prereset,
			       ata_reset_fn_t softreset,
			       ata_reset_fn_t hardreset,
			       ata_postreset_fn_t postreset);
extern void ata_bmdma_error_handler(struct ata_port *ap);
extern void ata_bmdma_post_internal_cmd(struct ata_queued_cmd *qc);
extern int ata_hsm_move(struct ata_port *ap, struct ata_queued_cmd *qc,
			u8 status, int in_wq);
extern void ata_qc_complete(struct ata_queued_cmd *qc);
extern int ata_qc_complete_multiple(struct ata_port *ap, u32 qc_active,
				    void (*finish_qc)(struct ata_queued_cmd *));
extern void ata_scsi_simulate(struct ata_device *dev, struct scsi_cmnd *cmd,
			      void (*done)(struct scsi_cmnd *));
extern int ata_std_bios_param(struct scsi_device *sdev,
			      struct block_device *bdev,
			      sector_t capacity, int geom[]);
extern int ata_scsi_slave_config(struct scsi_device *sdev);
extern void ata_scsi_slave_destroy(struct scsi_device *sdev);
extern int ata_scsi_change_queue_depth(struct scsi_device *sdev,
				       int queue_depth);
extern struct ata_device *ata_dev_pair(struct ata_device *adev);

/*
 * Timing helpers
 */

extern unsigned int ata_pio_need_iordy(const struct ata_device *);
extern int ata_timing_compute(struct ata_device *, unsigned short,
			      struct ata_timing *, int, int);
extern void ata_timing_merge(const struct ata_timing *,
			     const struct ata_timing *, struct ata_timing *,
			     unsigned int);

enum {
	ATA_TIMING_SETUP	= (1 << 0),
	ATA_TIMING_ACT8B	= (1 << 1),
	ATA_TIMING_REC8B	= (1 << 2),
	ATA_TIMING_CYC8B	= (1 << 3),
	ATA_TIMING_8BIT		= ATA_TIMING_ACT8B | ATA_TIMING_REC8B |
				  ATA_TIMING_CYC8B,
	ATA_TIMING_ACTIVE	= (1 << 4),
	ATA_TIMING_RECOVER	= (1 << 5),
	ATA_TIMING_CYCLE	= (1 << 6),
	ATA_TIMING_UDMA		= (1 << 7),
	ATA_TIMING_ALL		= ATA_TIMING_SETUP | ATA_TIMING_ACT8B |
				  ATA_TIMING_REC8B | ATA_TIMING_CYC8B |
				  ATA_TIMING_ACTIVE | ATA_TIMING_RECOVER |
				  ATA_TIMING_CYCLE | ATA_TIMING_UDMA,
};


#ifdef CONFIG_PCI
struct pci_bits {
	unsigned int		reg;	/* PCI config register to read */
	unsigned int		width;	/* 1 (8 bit), 2 (16 bit), 4 (32 bit) */
	unsigned long		mask;
	unsigned long		val;
};

extern void ata_pci_host_stop (struct ata_host_set *host_set);
extern struct ata_probe_ent *
ata_pci_init_native_mode(struct pci_dev *pdev, struct ata_port_info **port, int portmask);
extern int pci_test_config_bits(struct pci_dev *pdev, const struct pci_bits *bits);
extern unsigned long ata_pci_default_filter(const struct ata_port *, struct ata_device *, unsigned long);
#endif /* CONFIG_PCI */

/*
 * EH
 */
extern void ata_eng_timeout(struct ata_port *ap);

extern void ata_port_schedule_eh(struct ata_port *ap);
extern int ata_port_abort(struct ata_port *ap);
extern int ata_port_freeze(struct ata_port *ap);

extern void ata_eh_freeze_port(struct ata_port *ap);
extern void ata_eh_thaw_port(struct ata_port *ap);

extern void ata_eh_qc_complete(struct ata_queued_cmd *qc);
extern void ata_eh_qc_retry(struct ata_queued_cmd *qc);

extern void ata_do_eh(struct ata_port *ap, ata_prereset_fn_t prereset,
		      ata_reset_fn_t softreset, ata_reset_fn_t hardreset,
		      ata_postreset_fn_t postreset);

/*
 * printk helpers
 */
#define ata_port_printk(ap, lv, fmt, args...) \
	printk(lv"ata%u: "fmt, (ap)->id , ##args)

#define ata_dev_printk(dev, lv, fmt, args...) \
	printk(lv"ata%u.%02u: "fmt, (dev)->ap->id, (dev)->devno , ##args)

/*
 * ata_eh_info helpers
 */
#define ata_ehi_push_desc(ehi, fmt, args...) do { \
	(ehi)->desc_len += scnprintf((ehi)->desc + (ehi)->desc_len, \
				     ATA_EH_DESC_LEN - (ehi)->desc_len, \
				     fmt , ##args); \
} while (0)

#define ata_ehi_clear_desc(ehi) do { \
	(ehi)->desc[0] = '\0'; \
	(ehi)->desc_len = 0; \
} while (0)

static inline void __ata_ehi_hotplugged(struct ata_eh_info *ehi)
{
	if (ehi->flags & ATA_EHI_HOTPLUGGED)
		return;

	ehi->flags |= ATA_EHI_HOTPLUGGED | ATA_EHI_RESUME_LINK;
	ehi->hotplug_timestamp = jiffies;

	ehi->action |= ATA_EH_SOFTRESET;
	ehi->probe_mask |= (1 << ATA_MAX_DEVICES) - 1;
}

static inline void ata_ehi_hotplugged(struct ata_eh_info *ehi)
{
	__ata_ehi_hotplugged(ehi);
	ehi->err_mask |= AC_ERR_ATA_BUS;
}

/*
 * qc helpers
 */
static inline int
ata_sg_is_last(struct scatterlist *sg, struct ata_queued_cmd *qc)
{
	if (sg == &qc->pad_sgent)
		return 1;
	if (qc->pad_len)
		return 0;
	if (((sg - qc->__sg) + 1) == qc->n_elem)
		return 1;
	return 0;
}

static inline struct scatterlist *
ata_qc_first_sg(struct ata_queued_cmd *qc)
{
	if (qc->n_elem)
		return qc->__sg;
	if (qc->pad_len)
		return &qc->pad_sgent;
	return NULL;
}

static inline struct scatterlist *
ata_qc_next_sg(struct scatterlist *sg, struct ata_queued_cmd *qc)
{
	if (sg == &qc->pad_sgent)
		return NULL;
	if (++sg - qc->__sg < qc->n_elem)
		return sg;
	if (qc->pad_len)
		return &qc->pad_sgent;
	return NULL;
}

#define ata_for_each_sg(sg, qc) \
	for (sg = ata_qc_first_sg(qc); sg; sg = ata_qc_next_sg(sg, qc))

static inline unsigned int ata_tag_valid(unsigned int tag)
{
	return (tag < ATA_MAX_QUEUE) ? 1 : 0;
}

static inline unsigned int ata_tag_internal(unsigned int tag)
{
	return tag == ATA_MAX_QUEUE - 1;
}

/*
 * device helpers
 */
static inline unsigned int ata_class_enabled(unsigned int class)
{
	return class == ATA_DEV_ATA || class == ATA_DEV_ATAPI;
}

static inline unsigned int ata_class_disabled(unsigned int class)
{
	return class == ATA_DEV_ATA_UNSUP || class == ATA_DEV_ATAPI_UNSUP;
}

static inline unsigned int ata_class_absent(unsigned int class)
{
	return !ata_class_enabled(class) && !ata_class_disabled(class);
}

static inline unsigned int ata_dev_enabled(const struct ata_device *dev)
{
	return ata_class_enabled(dev->class);
}

static inline unsigned int ata_dev_disabled(const struct ata_device *dev)
{
	return ata_class_disabled(dev->class);
}

static inline unsigned int ata_dev_absent(const struct ata_device *dev)
{
	return ata_class_absent(dev->class);
}

static inline unsigned int ata_dev_ready(const struct ata_device *dev)
{
	return ata_dev_enabled(dev) && !(dev->flags & ATA_DFLAG_SUSPENDED);
}

/*
 * port helpers
 */
static inline int ata_port_max_devices(const struct ata_port *ap)
{
	if (ap->flags & ATA_FLAG_SLAVE_POSS)
		return 2;
	return 1;
}


static inline u8 ata_chk_status(struct ata_port *ap)
{
	return ap->ops->check_status(ap);
}


/**
 *	ata_pause - Flush writes and pause 400 nanoseconds.
 *	@ap: Port to wait for.
 *
 *	LOCKING:
 *	Inherited from caller.
 */

static inline void ata_pause(struct ata_port *ap)
{
	ata_altstatus(ap);
	ndelay(400);
}


/**
 *	ata_busy_wait - Wait for a port status register
 *	@ap: Port to wait for.
 *
 *	Waits up to max*10 microseconds for the selected bits in the port's
 *	status register to be cleared.
 *	Returns final value of status register.
 *
 *	LOCKING:
 *	Inherited from caller.
 */

static inline u8 ata_busy_wait(struct ata_port *ap, unsigned int bits,
			       unsigned int max)
{
	u8 status;

	do {
		udelay(10);
		status = ata_chk_status(ap);
		max--;
	} while ((status & bits) && (max > 0));

	return status;
}


/**
 *	ata_wait_idle - Wait for a port to be idle.
 *	@ap: Port to wait for.
 *
 *	Waits up to 10ms for port's BUSY and DRQ signals to clear.
 *	Returns final value of status register.
 *
 *	LOCKING:
 *	Inherited from caller.
 */

static inline u8 ata_wait_idle(struct ata_port *ap)
{
	u8 status = ata_busy_wait(ap, ATA_BUSY | ATA_DRQ, 1000);

	if (status & (ATA_BUSY | ATA_DRQ)) {
		unsigned long l = ap->ioaddr.status_addr;
		if (ata_msg_warn(ap))
			printk(KERN_WARNING "ATA: abnormal status 0x%X on port 0x%lX\n",
				status, l);
	}

	return status;
}

static inline void ata_qc_set_polling(struct ata_queued_cmd *qc)
{
	qc->tf.ctl |= ATA_NIEN;
}

static inline struct ata_queued_cmd *__ata_qc_from_tag(struct ata_port *ap,
						       unsigned int tag)
{
	if (likely(ata_tag_valid(tag)))
		return &ap->qcmd[tag];
	return NULL;
}

static inline struct ata_queued_cmd *ata_qc_from_tag(struct ata_port *ap,
						     unsigned int tag)
{
	struct ata_queued_cmd *qc = __ata_qc_from_tag(ap, tag);

	if (unlikely(!qc) || !ap->ops->error_handler)
		return qc;

	if ((qc->flags & (ATA_QCFLAG_ACTIVE |
			  ATA_QCFLAG_FAILED)) == ATA_QCFLAG_ACTIVE)
		return qc;

	return NULL;
}

static inline void ata_tf_init(struct ata_device *dev, struct ata_taskfile *tf)
{
	memset(tf, 0, sizeof(*tf));

	tf->ctl = dev->ap->ctl;
	if (dev->devno == 0)
		tf->device = ATA_DEVICE_OBS;
	else
		tf->device = ATA_DEVICE_OBS | ATA_DEV1;
}

static inline void ata_qc_reinit(struct ata_queued_cmd *qc)
{
	qc->__sg = NULL;
	qc->flags = 0;
	qc->cursect = qc->cursg = qc->cursg_ofs = 0;
	qc->nsect = 0;
	qc->nbytes = qc->curbytes = 0;
	qc->err_mask = 0;

	ata_tf_init(qc->dev, &qc->tf);

	/* init result_tf such that it indicates normal completion */
	qc->result_tf.command = ATA_DRDY;
	qc->result_tf.feature = 0;
}

/**
 *	ata_irq_on - Enable interrupts on a port.
 *	@ap: Port on which interrupts are enabled.
 *
 *	Enable interrupts on a legacy IDE device using MMIO or PIO,
 *	wait for idle, clear any pending interrupts.
 *
 *	LOCKING:
 *	Inherited from caller.
 */

static inline u8 ata_irq_on(struct ata_port *ap)
{
	struct ata_ioports *ioaddr = &ap->ioaddr;
	u8 tmp;

	ap->ctl &= ~ATA_NIEN;
	ap->last_ctl = ap->ctl;

	if (ap->flags & ATA_FLAG_MMIO)
		writeb(ap->ctl, (void __iomem *) ioaddr->ctl_addr);
	else
		outb(ap->ctl, ioaddr->ctl_addr);
	tmp = ata_wait_idle(ap);

	ap->ops->irq_clear(ap);

	return tmp;
}


/**
 *	ata_irq_ack - Acknowledge a device interrupt.
 *	@ap: Port on which interrupts are enabled.
 *
 *	Wait up to 10 ms for legacy IDE device to become idle (BUSY
 *	or BUSY+DRQ clear).  Obtain dma status and port status from
 *	device.  Clear the interrupt.  Return port status.
 *
 *	LOCKING:
 */

static inline u8 ata_irq_ack(struct ata_port *ap, unsigned int chk_drq)
{
	unsigned int bits = chk_drq ? ATA_BUSY | ATA_DRQ : ATA_BUSY;
	u8 host_stat, post_stat, status;

	status = ata_busy_wait(ap, bits, 1000);
	if (status & bits)
		if (ata_msg_err(ap))
			printk(KERN_ERR "abnormal status 0x%X\n", status);

	/* get controller status; clear intr, err bits */
	if (ap->flags & ATA_FLAG_MMIO) {
		void __iomem *mmio = (void __iomem *) ap->ioaddr.bmdma_addr;
		host_stat = readb(mmio + ATA_DMA_STATUS);
		writeb(host_stat | ATA_DMA_INTR | ATA_DMA_ERR,
		       mmio + ATA_DMA_STATUS);

		post_stat = readb(mmio + ATA_DMA_STATUS);
	} else {
		host_stat = inb(ap->ioaddr.bmdma_addr + ATA_DMA_STATUS);
		outb(host_stat | ATA_DMA_INTR | ATA_DMA_ERR,
		     ap->ioaddr.bmdma_addr + ATA_DMA_STATUS);

		post_stat = inb(ap->ioaddr.bmdma_addr + ATA_DMA_STATUS);
	}

	if (ata_msg_intr(ap))
		printk(KERN_INFO "%s: irq ack: host_stat 0x%X, new host_stat 0x%X, drv_stat 0x%X\n",
			__FUNCTION__,
			host_stat, post_stat, status);

	return status;
}

static inline int ata_try_flush_cache(const struct ata_device *dev)
{
	return ata_id_wcache_enabled(dev->id) ||
	       ata_id_has_flush(dev->id) ||
	       ata_id_has_flush_ext(dev->id);
}

static inline unsigned int ac_err_mask(u8 status)
{
	if (status & (ATA_BUSY | ATA_DRQ))
		return AC_ERR_HSM;
	if (status & (ATA_ERR | ATA_DF))
		return AC_ERR_DEV;
	return 0;
}

static inline unsigned int __ac_err_mask(u8 status)
{
	unsigned int mask = ac_err_mask(status);
	if (mask == 0)
		return AC_ERR_OTHER;
	return mask;
}

static inline int ata_pad_alloc(struct ata_port *ap, struct device *dev)
{
	ap->pad_dma = 0;
	ap->pad = dma_alloc_coherent(dev, ATA_DMA_PAD_BUF_SZ,
				     &ap->pad_dma, GFP_KERNEL);
	return (ap->pad == NULL) ? -ENOMEM : 0;
}

static inline void ata_pad_free(struct ata_port *ap, struct device *dev)
{
	dma_free_coherent(dev, ATA_DMA_PAD_BUF_SZ, ap->pad, ap->pad_dma);
}

static inline struct ata_port *ata_shost_to_port(struct Scsi_Host *host)
{
	return (struct ata_port *) &host->hostdata[0];
}

#endif /* __LINUX_LIBATA_H__ */
