#include "xparameters.h"
#include "xgpio.h"

int main() {
    XGpio my_bridge;
    u32 switch_data; // A 32-bit variable to hold the switch states
    
    // 1. Initialize the AXI GPIO hardware block using its dictionary ID
    // (If the compiler complains about this ID later, we will check xparameters.h like last time)
    XGpio_Initialize(&my_bridge, XPAR_AXI_GPIO_0_BASEADDR);
    
    // 2. Set the Data Directions (1 = Input, 0 = Output)
    // Channel 1 is connected to the LEDs. We want them as Outputs (0x0).
    XGpio_SetDataDirection(&my_bridge, 1, 0x0);
    
    // Channel 2 is connected to the Switches. We want them as Inputs (0xF).
    // (0xF in binary is 1111, meaning all 4 pins are configured to listen).
    XGpio_SetDataDirection(&my_bridge, 2, 0xF);
    
    // 3. The Infinite Router Loop
    while(1) {
        // Read the current voltage state of the 4 switches on Channel 2
        switch_data = XGpio_DiscreteRead(&my_bridge, 2);
        
        // Take that exact 4-bit state and write it to the LEDs on Channel 1
        XGpio_DiscreteWrite(&my_bridge, 1, switch_data);
    }
    
    return 0;
}