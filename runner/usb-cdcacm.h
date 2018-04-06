#ifndef _USB_CDCACM_H
#define _USB_CDCACM_H

void usb_cdcacm_setup(void);
void usb_cdcacm_set_rxcallback(void (*cb)(const void *data, uint16_t len));
void usb_cdcacm_puts(const char *s);

#endif /* _USB_CDCACM_H */