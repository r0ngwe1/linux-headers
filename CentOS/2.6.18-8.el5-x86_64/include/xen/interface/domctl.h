/******************************************************************************
 * domctl.h
 * 
 * Domain management operations. For use by node control stack.
 * 
 * Copyright (c) 2002-2003, B Dragovic
 * Copyright (c) 2002-2006, K Fraser
 */

#ifndef __XEN_PUBLIC_DOMCTL_H__
#define __XEN_PUBLIC_DOMCTL_H__

#if !defined(__XEN__) && !defined(__XEN_TOOLS__)
#error "domctl operations are intended for use by node control tools only"
#endif

#include "xen.h"

#define XEN_DOMCTL_INTERFACE_VERSION 0x00000003

struct xenctl_cpumap {
    XEN_GUEST_HANDLE(uint8_t) bitmap;
    uint32_t nr_cpus;
};

/*
 * NB. xen_domctl.domain is an IN/OUT parameter for this operation.
 * If it is specified as zero, an id is auto-allocated and returned.
 */
#define XEN_DOMCTL_createdomain       1
struct xen_domctl_createdomain {
    /* IN parameters */
    uint32_t ssidref;
    xen_domain_handle_t handle;
};
typedef struct xen_domctl_createdomain xen_domctl_createdomain_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_createdomain_t);

#define XEN_DOMCTL_destroydomain      2
#define XEN_DOMCTL_pausedomain        3
#define XEN_DOMCTL_unpausedomain      4

#define XEN_DOMCTL_getdomaininfo      5
struct xen_domctl_getdomaininfo {
    /* OUT variables. */
    domid_t  domain;              /* Also echoed in domctl.domain */
#define DOMFLAGS_DYING     (1<<0) /* Domain is scheduled to die.             */
#define DOMFLAGS_SHUTDOWN  (1<<2) /* The guest OS has shut down.             */
#define DOMFLAGS_PAUSED    (1<<3) /* Currently paused by control software.   */
#define DOMFLAGS_BLOCKED   (1<<4) /* Currently blocked pending an event.     */
#define DOMFLAGS_RUNNING   (1<<5) /* Domain is currently running.            */
#define DOMFLAGS_CPUMASK      255 /* CPU to which this domain is bound.      */
#define DOMFLAGS_CPUSHIFT       8
#define DOMFLAGS_SHUTDOWNMASK 255 /* DOMFLAGS_SHUTDOWN guest-supplied code.  */
#define DOMFLAGS_SHUTDOWNSHIFT 16
    uint32_t flags;
    uint64_t tot_pages;
    uint64_t max_pages;
    uint64_t shared_info_frame;  /* MFN of shared_info struct */
    uint64_t cpu_time;
    uint32_t nr_online_vcpus;     /* Number of VCPUs currently online. */
    uint32_t max_vcpu_id;         /* Maximum VCPUID in use by this domain. */
    uint32_t ssidref;
    xen_domain_handle_t handle;
};
typedef struct xen_domctl_getdomaininfo xen_domctl_getdomaininfo_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_getdomaininfo_t);


#define XEN_DOMCTL_getmemlist         6
struct xen_domctl_getmemlist {
    /* IN variables. */
    /* Max entries to write to output buffer. */
    uint64_t max_pfns;
    /* Start index in guest's page list. */
    uint64_t start_pfn;
    XEN_GUEST_HANDLE(xen_pfn_t) buffer;
    /* OUT variables. */
    uint64_t num_pfns;
};
typedef struct xen_domctl_getmemlist xen_domctl_getmemlist_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_getmemlist_t);


#define XEN_DOMCTL_getpageframeinfo   7

#define XEN_DOMCTL_PFINFO_LTAB_SHIFT 28
#define XEN_DOMCTL_PFINFO_NOTAB   (0x0<<28)
#define XEN_DOMCTL_PFINFO_L1TAB   (0x1<<28)
#define XEN_DOMCTL_PFINFO_L2TAB   (0x2<<28)
#define XEN_DOMCTL_PFINFO_L3TAB   (0x3<<28)
#define XEN_DOMCTL_PFINFO_L4TAB   (0x4<<28)
#define XEN_DOMCTL_PFINFO_LTABTYPE_MASK (0x7<<28)
#define XEN_DOMCTL_PFINFO_LPINTAB (0x1<<31)
#define XEN_DOMCTL_PFINFO_XTAB    (0xf<<28) /* invalid page */
#define XEN_DOMCTL_PFINFO_LTAB_MASK (0xf<<28)

struct xen_domctl_getpageframeinfo {
    /* IN variables. */
    uint64_t gmfn;        /* GMFN to query */
    /* OUT variables. */
    /* Is the page PINNED to a type? */
    uint32_t type;         /* see above type defs */
};
typedef struct xen_domctl_getpageframeinfo xen_domctl_getpageframeinfo_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_getpageframeinfo_t);


#define XEN_DOMCTL_getpageframeinfo2  8
struct xen_domctl_getpageframeinfo2 {
    /* IN variables. */
    uint64_t num;
    /* IN/OUT variables. */
    XEN_GUEST_HANDLE(ulong) array;
};
typedef struct xen_domctl_getpageframeinfo2 xen_domctl_getpageframeinfo2_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_getpageframeinfo2_t);


/*
 * Control shadow pagetables operation
 */
#define XEN_DOMCTL_shadow_op         10

/* Disable shadow mode. */
#define XEN_DOMCTL_SHADOW_OP_OFF         0

/* Enable shadow mode (mode contains ORed XEN_DOMCTL_SHADOW_ENABLE_* flags). */
#define XEN_DOMCTL_SHADOW_OP_ENABLE      32

/* Log-dirty bitmap operations. */
 /* Return the bitmap and clean internal copy for next round. */
#define XEN_DOMCTL_SHADOW_OP_CLEAN       11
 /* Return the bitmap but do not modify internal copy. */
#define XEN_DOMCTL_SHADOW_OP_PEEK        12

/* Memory allocation accessors. */
#define XEN_DOMCTL_SHADOW_OP_GET_ALLOCATION   30
#define XEN_DOMCTL_SHADOW_OP_SET_ALLOCATION   31

/* Legacy enable operations. */
 /* Equiv. to ENABLE with no mode flags. */
#define XEN_DOMCTL_SHADOW_OP_ENABLE_TEST       1
 /* Equiv. to ENABLE with mode flag ENABLE_LOG_DIRTY. */
#define XEN_DOMCTL_SHADOW_OP_ENABLE_LOGDIRTY   2
 /* Equiv. to ENABLE with mode flags ENABLE_REFCOUNT and ENABLE_TRANSLATE. */
#define XEN_DOMCTL_SHADOW_OP_ENABLE_TRANSLATE  3

/* Mode flags for XEN_DOMCTL_SHADOW_OP_ENABLE. */
 /*
  * Shadow pagetables are refcounted: guest does not use explicit mmu
  * operations nor write-protect its pagetables.
  */
#define XEN_DOMCTL_SHADOW_ENABLE_REFCOUNT  (1 << 1)
 /*
  * Log pages in a bitmap as they are dirtied.
  * Used for live relocation to determine which pages must be re-sent.
  */
#define XEN_DOMCTL_SHADOW_ENABLE_LOG_DIRTY (1 << 2)
 /*
  * Automatically translate GPFNs into MFNs.
  */
#define XEN_DOMCTL_SHADOW_ENABLE_TRANSLATE (1 << 3)
 /*
  * Xen does not steal virtual address space from the guest.
  * Requires HVM support.
  */
#define XEN_DOMCTL_SHADOW_ENABLE_EXTERNAL  (1 << 4)

struct xen_domctl_shadow_op_stats {
    uint32_t fault_count;
    uint32_t dirty_count;
};
typedef struct xen_domctl_shadow_op_stats xen_domctl_shadow_op_stats_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_shadow_op_stats_t);

struct xen_domctl_shadow_op {
    /* IN variables. */
    uint32_t       op;       /* XEN_DOMCTL_SHADOW_OP_* */

    /* OP_ENABLE */
    uint32_t       mode;     /* XEN_DOMCTL_SHADOW_ENABLE_* */

    /* OP_GET_ALLOCATION / OP_SET_ALLOCATION */
    uint32_t       mb;       /* Shadow memory allocation in MB */

    /* OP_PEEK / OP_CLEAN */
    XEN_GUEST_HANDLE(ulong) dirty_bitmap;
    uint64_t       pages;    /* Size of buffer. Updated with actual size. */
    struct xen_domctl_shadow_op_stats stats;
};
typedef struct xen_domctl_shadow_op xen_domctl_shadow_op_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_shadow_op_t);


#define XEN_DOMCTL_max_mem           11
struct xen_domctl_max_mem {
    /* IN variables. */
    uint64_t max_memkb;
};
typedef struct xen_domctl_max_mem xen_domctl_max_mem_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_max_mem_t);


#define XEN_DOMCTL_setvcpucontext    12
#define XEN_DOMCTL_getvcpucontext    13
struct xen_domctl_vcpucontext {
    uint32_t              vcpu;                  /* IN */
    XEN_GUEST_HANDLE(vcpu_guest_context_t) ctxt; /* IN/OUT */
};
typedef struct xen_domctl_vcpucontext xen_domctl_vcpucontext_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_vcpucontext_t);


#define XEN_DOMCTL_getvcpuinfo       14
struct xen_domctl_getvcpuinfo {
    /* IN variables. */
    uint32_t vcpu;
    /* OUT variables. */
    uint8_t  online;                  /* currently online (not hotplugged)? */
    uint8_t  blocked;                 /* blocked waiting for an event? */
    uint8_t  running;                 /* currently scheduled on its CPU? */
    uint64_t cpu_time;                /* total cpu time consumed (ns) */
    uint32_t cpu;                     /* current mapping   */
};
typedef struct xen_domctl_getvcpuinfo xen_domctl_getvcpuinfo_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_getvcpuinfo_t);


/* Get/set which physical cpus a vcpu can execute on. */
#define XEN_DOMCTL_setvcpuaffinity    9
#define XEN_DOMCTL_getvcpuaffinity   25
struct xen_domctl_vcpuaffinity {
    uint32_t  vcpu;              /* IN */
    struct xenctl_cpumap cpumap; /* IN/OUT */
};
typedef struct xen_domctl_vcpuaffinity xen_domctl_vcpuaffinity_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_vcpuaffinity_t);


#define XEN_DOMCTL_max_vcpus         15
struct xen_domctl_max_vcpus {
    uint32_t max;           /* maximum number of vcpus */
};
typedef struct xen_domctl_max_vcpus xen_domctl_max_vcpus_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_max_vcpus_t);


#define XEN_DOMCTL_scheduler_op      16
/* Scheduler types. */
#define XEN_SCHEDULER_SEDF     4
#define XEN_SCHEDULER_CREDIT   5
/* Set or get info? */
#define XEN_DOMCTL_SCHEDOP_putinfo 0
#define XEN_DOMCTL_SCHEDOP_getinfo 1
struct xen_domctl_scheduler_op {
    uint32_t sched_id;  /* XEN_SCHEDULER_* */
    uint32_t cmd;       /* XEN_DOMCTL_SCHEDOP_* */
    union {
        struct xen_domctl_sched_sedf {
            uint64_t period;
            uint64_t slice;
            uint64_t latency;
            uint32_t extratime;
            uint32_t weight;
        } sedf;
        struct xen_domctl_sched_credit {
            uint16_t weight;
            uint16_t cap;
        } credit;
    } u;
};
typedef struct xen_domctl_scheduler_op xen_domctl_scheduler_op_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_scheduler_op_t);


#define XEN_DOMCTL_setdomainhandle   17
struct xen_domctl_setdomainhandle {
    xen_domain_handle_t handle;
};
typedef struct xen_domctl_setdomainhandle xen_domctl_setdomainhandle_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_setdomainhandle_t);


#define XEN_DOMCTL_setdebugging      18
struct xen_domctl_setdebugging {
    uint8_t enable;
};
typedef struct xen_domctl_setdebugging xen_domctl_setdebugging_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_setdebugging_t);


#define XEN_DOMCTL_irq_permission    19
struct xen_domctl_irq_permission {
    uint8_t pirq;
    uint8_t allow_access;    /* flag to specify enable/disable of IRQ access */
};
typedef struct xen_domctl_irq_permission xen_domctl_irq_permission_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_irq_permission_t);


#define XEN_DOMCTL_iomem_permission  20
struct xen_domctl_iomem_permission {
    uint64_t first_mfn;       /* first page (physical page number) in range */
    uint64_t nr_mfns;         /* number of pages in range (>0) */
    uint8_t  allow_access;    /* allow (!0) or deny (0) access to range? */
};
typedef struct xen_domctl_iomem_permission xen_domctl_iomem_permission_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_iomem_permission_t);


#define XEN_DOMCTL_ioport_permission 21
struct xen_domctl_ioport_permission {
    uint32_t first_port;              /* first port int range */
    uint32_t nr_ports;                /* size of port range */
    uint8_t  allow_access;            /* allow or deny access to range? */
};
typedef struct xen_domctl_ioport_permission xen_domctl_ioport_permission_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_ioport_permission_t);

#define XEN_DOMCTL_hypercall_init    22
struct xen_domctl_hypercall_init {
    uint64_t  gmfn;            /* GMFN to be initialised */
};
typedef struct xen_domctl_hypercall_init xen_domctl_hypercall_init_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_hypercall_init_t);

#define XEN_DOMCTL_arch_setup        23
#define _XEN_DOMAINSETUP_hvm_guest 0
#define XEN_DOMAINSETUP_hvm_guest  (1UL<<_XEN_DOMAINSETUP_hvm_guest)
#define _XEN_DOMAINSETUP_query 1 /* Get parameters (for save)  */
#define XEN_DOMAINSETUP_query  (1UL<<_XEN_DOMAINSETUP_query)
typedef struct xen_domctl_arch_setup {
    uint64_t flags;      /* XEN_DOMAINSETUP_* */
#ifdef __ia64__
    uint64_t bp;            /* mpaddr of boot param area */
    uint64_t maxmem;        /* Highest memory address for MDT.  */
    uint64_t xsi_va;        /* Xen shared_info area virtual address.  */
    uint32_t hypercall_imm; /* Break imm for Xen hypercalls.  */
#endif
} xen_domctl_arch_setup_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_arch_setup_t);

#define XEN_DOMCTL_settimeoffset     24
struct xen_domctl_settimeoffset {
    int32_t  time_offset_seconds; /* applied to domain wallclock time */
};
typedef struct xen_domctl_settimeoffset xen_domctl_settimeoffset_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_settimeoffset_t);

struct xen_domctl {
    uint32_t cmd;
    uint32_t interface_version; /* XEN_DOMCTL_INTERFACE_VERSION */
    domid_t  domain;
    union {
        struct xen_domctl_createdomain      createdomain;
        struct xen_domctl_getdomaininfo     getdomaininfo;
        struct xen_domctl_getmemlist        getmemlist;
        struct xen_domctl_getpageframeinfo  getpageframeinfo;
        struct xen_domctl_getpageframeinfo2 getpageframeinfo2;
        struct xen_domctl_vcpuaffinity      vcpuaffinity;
        struct xen_domctl_shadow_op         shadow_op;
        struct xen_domctl_max_mem           max_mem;
        struct xen_domctl_vcpucontext       vcpucontext;
        struct xen_domctl_getvcpuinfo       getvcpuinfo;
        struct xen_domctl_max_vcpus         max_vcpus;
        struct xen_domctl_scheduler_op      scheduler_op;
        struct xen_domctl_setdomainhandle   setdomainhandle;
        struct xen_domctl_setdebugging      setdebugging;
        struct xen_domctl_irq_permission    irq_permission;
        struct xen_domctl_iomem_permission  iomem_permission;
        struct xen_domctl_ioport_permission ioport_permission;
        struct xen_domctl_hypercall_init    hypercall_init;
        struct xen_domctl_arch_setup        arch_setup;
        struct xen_domctl_settimeoffset     settimeoffset;
        uint8_t                             pad[128];
    } u;
};
typedef struct xen_domctl xen_domctl_t;
DEFINE_XEN_GUEST_HANDLE(xen_domctl_t);

#endif /* __XEN_PUBLIC_DOMCTL_H__ */

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
