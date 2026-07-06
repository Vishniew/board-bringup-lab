#include "xparameters.h"
#include "xgpio.h"
#include "sleep.h"
#include "xstatus.h"

#define LED_CHANNEL 1

int main(void)
{
    XGpio gpio;
    int status;
    u32 value = 0x01;

    status = XGpio_Initialize(&gpio, XPAR_AXI_GPIO_0_BASEADDR);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // 0 = output, 1 = input
    XGpio_SetDataDirection(&gpio, LED_CHANNEL, 0x00);

    while (1) {
        XGpio_DiscreteWrite(&gpio, LED_CHANNEL, value);

        value <<= 1;
        if (value > 0x80) {
            value = 0x01;
        }

        sleep(1);
    }

    return XST_SUCCESS;
}