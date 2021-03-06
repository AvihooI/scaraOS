#ifndef __PCI_H__
#define __PCI_H__

#define PCI_NUM_BARS		6

/* PCI CONFIG SPACE */
#define PCI_CONF_ID		0x00 /* device ID */
#define  PCI_CONF_DEVCE		 0x00
#define  PCI_CONF_VENDOR	 0x02
#define PCI_CONF_CMDSTAT	0x04 /* commands & stats */
#define  PCI_CONF_STATUS	 0x04
#define  PCI_CONF_COMMAND	 0x06
#define	PCI_CONF_CLASSREV	0x08 /* class & revision */
#define  PCI_CONF_REV		 0x08
#define  PCI_CONF_PROGIF	 0x09
#define  PCI_CONF_SUBCLASS	 0x0a
#define  PCI_CONF_CLASS		 0x0b
#define PCI_CONF_HWINFO		0x0c /* cache-line size, lat, hdr type, bist */
#define  PCI_CONF_CACHE_LINE	 0x0c
#define  PCI_CONF_LATENCY	 0x0d
#define  PCI_CONF_HDRTYPE	 0x0e
#define  PCI_CONF_BIST		 0x0f

/* type 0 header */
#define PCI_CONF0_BAR0		0x10
#define PCI_CONF0_BAR1		0x14
#define PCI_CONF0_BAR2		0x18
#define PCI_CONF0_BAR3		0x1c
#define PCI_CONF0_BAR4		0x20
#define PCI_CONF0_BAR5		0x24
#define PCI_CONF0_CIS		0x28 /* CardBUS CIS pointer */
#define PCI_CONF0_SUBSYS	0x2c
#define PCI_CONF0_ROM_BAR	0x30
#define PCI_CONF0_CAP_OFS	0x34
#define PCI_CONF0_RSVD1		0x38
#define PCI_CONF0_IRQ		0x3c /* line, pin, min/max lat */
#define PCI_CONF0_RSVD		0x40

#define PCI_NUM_BUSSES		0x100
#define PCI_NUM_DEVS		0x20
#define PCI_NUM_FUNCS		0x08

#define PCI_BUS_MASK		(PCI_NUM_BUSSES-1)
#define PCI_DEV_MASK		(PCI_NUM_DEVS-1)
#define PCI_FUNC_MASK		(PCI_NUM_FUNCS-1)

#define PCI_FUNC_SHIFT		0
#define PCI_DEV_SHIFT		3
#define PCI_BUS_SHIFT		8

#define PCI_BDF(bus, dev, fn)	((bus & PCI_BUS_MASK) << PCI_BUS_SHIFT) | \
				((dev & PCI_DEV_MASK) << PCI_DEV_SHIFT) | \
				((fn & PCI_FUNC_MASK) << PCI_FUNC_SHIFT)

#ifndef __ASM__
struct pci_dom {
	struct list_head d_list;
	struct list_head d_devices;
	uint32_t d_domain;
	const struct pci_dom_ops *d_ops;
};

struct pci_dom_ops {
	uint32_t (*read_conf32)(struct pci_dom *dom, uint32_t addr);
	void (*write_conf32)(struct pci_dom *dom, uint32_t addr, uint32_t w);
	uint16_t (*read_conf16)(struct pci_dom *dom, uint32_t addr);
	void (*write_conf16)(struct pci_dom *dom, uint32_t addr, uint16_t w);
	uint8_t (*read_conf8)(struct pci_dom *dom, uint32_t addr);
	void (*write_conf8)(struct pci_dom *dom, uint32_t addr, uint8_t w);
};

struct pci_dev {
	const struct pci_driver *pci_driver;
	void *pci_driver_priv;
	struct pci_dom *pci_domain;
	struct list_head pci_list;
	uint32_t pci_bdf;
};

struct pci_driver {
	const char *name;
	int (*attach)(struct pci_dev *dev);
	int (*detach)(struct pci_dev *dev);
};

struct pci_cls_tbl {
	const struct pci_driver *cls_driver;
	uint32_t cls_mask;
	uint32_t cls_val;
}__attribute__((packed));
#define pci_cls_driver(driver, mask, val) \
	_section(".rodata.pci_cls") __attribute__((used)) \
	static const struct pci_cls_tbl \
		__pci_cls_ ##driver ## _ ##mask ## _ ##val = { \
		.cls_driver = &driver, \
		.cls_mask = mask, \
		.cls_val = val, \
	};

void pcidev_conf_write32(struct pci_dev *dev, unsigned int addr, uint32_t v);
void pcidev_conf_write16(struct pci_dev *dev, unsigned int addr, uint16_t v);
void pcidev_conf_write8(struct pci_dev *dev, unsigned int addr, uint8_t v);
uint32_t pcidev_conf_read32(struct pci_dev *dev, unsigned int addr);
uint16_t pcidev_conf_read16(struct pci_dev *dev, unsigned int addr);
uint8_t pcidev_conf_read8(struct pci_dev *dev, unsigned int addr);

void __init pci_init(void);

/* pci_arch_init() adds domains */
void __init pci_domain_add(struct pci_dom *dom);

/* Architectures must implement these */
void __init pci_arch_init(void);
const struct pci_driver *pci_arch_scan_cls(uint32_t cls);

#endif

#endif /* __PCI_H__ */
