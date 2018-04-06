#include <libopencm3/cm3/nvic.h>
#include <libopencm3/efm32/cmu.h>
#include <libopencm3/efm32/gpio.h>
#include <libopencm3/efm32/wdog.h>

#include "toboot.h"
#include "usb-cdcacm.h"

#define LED_GREEN_PORT GPIOA
#define LED_GREEN_PIN  GPIO0
#define LED_RED_PORT   GPIOB
#define LED_RED_PIN    GPIO7

// Place this program at offset 0x2000.
// Don't autorun this program at startup, because it is loaded once at the
// factory, and is then never again used.
TOBOOT_CONFIGURATION_AT_OFFSET(TOBOOT_CONFIG_FLAG_AUTORUN_DISABLED, 0x2000);

static void rx_callback(const void *buf, uint16_t len)
{
    (void)len;
    usb_cdcacm_puts(buf);
}

int main(void)
{
    int i;

    /* Disable the watchdog that the bootloader started. */
    WDOG_CTRL = 0;

    /* GPIO peripheral clock is necessary for us to set up the GPIO pins as outputs */
    cmu_periph_clock_enable(CMU_GPIO);

    /* Set up both LEDs as outputs */
    gpio_mode_setup(LED_RED_PORT, GPIO_MODE_WIRED_AND, LED_RED_PIN);
    gpio_mode_setup(LED_GREEN_PORT, GPIO_MODE_WIRED_AND, LED_GREEN_PIN);

    /* Configure USB Core & stack for USB */
    usb_cdcacm_setup();
    usb_cdcacm_set_rxcallback(rx_callback);

    /* Enable USB IRQs */
	nvic_enable_irq(NVIC_USB_IRQ);

    while(1) {
        usb_cdcacm_puts("toggling LED\n\r");
        gpio_toggle(LED_RED_PORT, LED_RED_PIN);
        for(i = 0; i != 500000; ++i)
			__asm__("nop");
    }
}
