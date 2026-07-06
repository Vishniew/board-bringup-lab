#include "xparameters.h"
#include "xgpio.h"
#include "sleep.h"
#include "xil_printf.h"
int main() {
    XGpio led_device;
    xil_printf("Hello from Zybo ARM\r\n");
    // Wire the software handle to the physical AXI GPIO memory address
    XGpio_Initialize(&led_device, XPAR_AXI_GPIO_0_BASEADDR);
    
    // Set the GPIO channel 1 to be an Output (0x0)
    XGpio_SetDataDirection(&led_device, 1, 0x0);
    
    while(1) {
        xil_printf("Blinking...\r\n");
        // Drive the pin High (Turn LED ON)
        XGpio_DiscreteWrite(&led_device, 1, 1);
        usleep(500000); 
        
        // Drive the pin Low (Turn LED OFF)
        XGpio_DiscreteWrite(&led_device, 1, 0);
        usleep(500000); 
    }
    
    return 0;
}