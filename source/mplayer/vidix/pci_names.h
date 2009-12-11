/*
 * File: vidix/pci_names.h
 * This file was generated automatically. Don't modify it.
*/
#ifndef MPLAYER_PCI_NAMES_H
#define MPLAYER_PCI_NAMES_H


struct device_id_s
{
	unsigned short	id;
	const char *	name;
};

struct vendor_id_s
{
	unsigned short	id;
	const char *	name;
	const struct device_id_s *	dev_list;
};
const char *pci_vendor_name(unsigned short id);
const char *pci_device_name(unsigned short vendor_id, unsigned short device_id);


#endif /* MPLAYER_PCI_NAMES_H */
