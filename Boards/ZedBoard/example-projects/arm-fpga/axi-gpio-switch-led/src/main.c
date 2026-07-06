#include "xparameters.h"
#include "xgpio.h"
#include "xstatus.h"
#include "sleep.h"

#define SWITCH_CHANNEL  1   // GPIO  -> sws_8bits
#define LED_CHANNEL     2   // GPIO2 -> leds_8bits

int main(void)
{
    XGpio gpio;
    int status;
    u32 switch_value;

    status = XGpio_Initialize(&gpio, XPAR_AXI_GPIO_0_BASEADDR);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * AXI GPIO direction:
     * 0 = output
     * 1 = input
     */
    XGpio_SetDataDirection(&gpio, SWITCH_CHANNEL, 0xFF);  // switches as input
    XGpio_SetDataDirection(&gpio, LED_CHANNEL,    0x00);  // LEDs as output

    while (1) {
        switch_value = XGpio_DiscreteRead(&gpio, SWITCH_CHANNEL);
        switch_value = switch_value & 0xFF;

        XGpio_DiscreteWrite(&gpio, LED_CHANNEL, switch_value);

        usleep(10000);
    }

    return XST_SUCCESS;
}