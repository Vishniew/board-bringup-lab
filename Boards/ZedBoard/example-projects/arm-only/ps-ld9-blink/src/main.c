#include "xparameters.h"
#include "xgpiops.h"
#include "sleep.h"

#define PS_LED_PIN 7   // ZedBoard LD9 is connected to PS MIO7

int main(void)
{
    XGpioPs gpio;
    XGpioPs_Config *config;
    int status;

#ifdef XPAR_XGPIOPS_0_DEVICE_ID
    config = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
#else
    config = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_BASEADDR);
#endif

    if (config == NULL) {
        return -1;
    }

    status = XGpioPs_CfgInitialize(&gpio, config, config->BaseAddr);
    if (status != XST_SUCCESS) {
        return -1;
    }

    XGpioPs_SetDirectionPin(&gpio, PS_LED_PIN, 1);
    XGpioPs_SetOutputEnablePin(&gpio, PS_LED_PIN, 1);

    while (1) {
        XGpioPs_WritePin(&gpio, PS_LED_PIN, 1);
        sleep(1);

        XGpioPs_WritePin(&gpio, PS_LED_PIN, 0);
        sleep(1);
    }

    return 0;
}