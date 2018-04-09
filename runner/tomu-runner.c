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

#define CAP0A_PORT     GPIOC
#define CAP0A_PIN      GPIO0
#define CAP1A_PORT     GPIOC
#define CAP1A_PIN      GPIO1
#define CAP0B_PORT     GPIOE
#define CAP0B_PIN      GPIO12
#define CAP1B_PORT     GPIOE
#define CAP1B_PIN      GPIO13

// Place this program at offset 0x2000.
// Don't autorun this program at startup, because it is loaded once at the
// factory, and is then never again used.
TOBOOT_CONFIGURATION_AT_OFFSET(TOBOOT_CONFIG_FLAG_AUTORUN_DISABLED, 0x2000);

static void rx_callback(const void *buf, uint16_t len)
{
    const uint8_t *str = buf;
    if (!len)
        return;

    switch (str[0]) {
    case 'r':
        gpio_set(LED_RED_PORT, LED_RED_PIN);
        break;
    case 'R':
        gpio_clear(LED_RED_PORT, LED_RED_PIN);
        break;

    case 'g':
        gpio_set(LED_GREEN_PORT, LED_GREEN_PIN);
        break;
    case 'G':
        gpio_clear(LED_GREEN_PORT, LED_GREEN_PIN);
        break;

    case 'z':
        gpio_set(CAP0A_PORT, CAP0A_PIN);
        break;
    case 'Z':
        gpio_clear(CAP0A_PORT, CAP0A_PIN);
        break;

    case 'x':
        gpio_set(CAP0B_PORT, CAP0B_PIN);
        break;
    case 'X':
        gpio_clear(CAP0B_PORT, CAP0B_PIN);
        break;

    case 'c':
        gpio_set(CAP1A_PORT, CAP1A_PIN);
        break;
    case 'C':
        gpio_clear(CAP1A_PORT, CAP1A_PIN);
        break;

    case 'v':
        gpio_set(CAP1B_PORT, CAP1B_PIN);
        break;
    case 'V':
        gpio_clear(CAP1B_PORT, CAP1B_PIN);
        break;
    }
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
    gpio_set(LED_RED_PORT, LED_RED_PIN);
    gpio_mode_setup(LED_GREEN_PORT, GPIO_MODE_WIRED_AND, LED_GREEN_PIN);
    gpio_set(LED_GREEN_PORT, LED_GREEN_PIN);
    gpio_mode_setup(CAP0A_PORT, GPIO_MODE_WIRED_AND, CAP0A_PIN);
    gpio_mode_setup(CAP1A_PORT, GPIO_MODE_WIRED_AND, CAP1A_PIN);
    gpio_mode_setup(CAP0B_PORT, GPIO_MODE_WIRED_AND, CAP0B_PIN);
    gpio_mode_setup(CAP1B_PORT, GPIO_MODE_WIRED_AND, CAP1B_PIN);

    /* Configure USB Core & stack for USB */
    usb_cdcacm_setup();
    usb_cdcacm_set_rxcallback(rx_callback);

    /* Enable USB IRQs */
	nvic_enable_irq(NVIC_USB_IRQ);

    uint32_t loops = 0;
    while(1) {
        usb_cdcacm_puts("test-in-progress\n\r");
        if (loops++ & 1)
            gpio_toggle(LED_RED_PORT, LED_RED_PIN);
        else
            gpio_toggle(LED_GREEN_PORT, LED_GREEN_PIN);
        for(i = 0; i != 500000; ++i)
			__asm__("nop");
    }
}
