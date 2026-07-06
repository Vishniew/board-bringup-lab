#include "xparameters.h"
#include "xgpiops.h"
#include "sleep.h"

int main() {
    XGpioPs ps_gpio;
    XGpioPs_Config *config;

    config = XGpioPs_LookupConfig(XPAR_AXI_GPIO_0_BASEADDR);
    
    XGpioPs_CfgInitialize(&ps_gpio, config, config->BaseAddr);

    XGpioPs_SetDirectionPin(&ps_gpio, 7, 1);
    XGpioPs_SetOutputEnablePin(&ps_gpio, 7, 1);

    while(1) {
        XGpioPs_WritePin(&ps_gpio, 7, 1); 
        usleep(500000); 
        
        XGpioPs_WritePin(&ps_gpio, 7, 0); 
        usleep(500000); 
    }
    
    return 0;
}