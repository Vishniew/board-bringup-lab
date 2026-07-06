#include "xparameters.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xstatus.h"
#include "sleep.h"

/*
 * From your xparameters.h:
 * XPAR_AXI_GPIO_0_BASEADDR      = 0x41200000
 * XPAR_FABRIC_AXI_GPIO_0_INTR   = 29
 * XPAR_XSCUGIC_0_BASEADDR       = 0xf8f01000
 */
#define GPIO_BASEADDR      XPAR_AXI_GPIO_0_BASEADDR
#define GPIO_INTERRUPT_ID  (XPAR_FABRIC_AXI_GPIO_0_INTR + 32)
#define INTC_BASEADDR      XPAR_XSCUGIC_0_BASEADDR

#define BUTTON_CHANNEL     1   // GPIO  -> btns_5bits
#define LED_CHANNEL        2   // GPIO2 -> leds_8bits

#define BUTTON_MASK        0x1F
#define LED_MASK           0xFF

/*
 * Button bit masks.
 * Exact physical order may differ on board, but all 5 buttons will work.
 */
#define BUTTON_0           0x01
#define BUTTON_1           0x02
#define BUTTON_2           0x04
#define BUTTON_3           0x08
#define BUTTON_4           0x10

/*
 * Pattern IDs
 */
#define PATTERN_TRAIN_FORWARD     1
#define PATTERN_TRAIN_REVERSE     2
#define PATTERN_ALTERNATE_BLINK   3
#define PATTERN_HALF_HALF         4
#define PATTERN_FILL_EMPTY        5

static XGpio Gpio;
static XScuGic Intc;

/*
 * This variable is changed inside ISR and used inside main loop.
 * So it must be volatile.
 */
static volatile u32 selected_pattern = PATTERN_TRAIN_FORWARD;

static void GpioInterruptHandler(void *CallbackRef)
{
    XGpio *GpioPtr = (XGpio *)CallbackRef;

    u32 interrupt_status;
    u32 button_value;

    interrupt_status = XGpio_InterruptGetStatus(GpioPtr);

    if ((interrupt_status & XGPIO_IR_CH1_MASK) == 0) {
        return;
    }

    /*
     * Clear AXI GPIO interrupt.
     */
    XGpio_InterruptClear(GpioPtr, XGPIO_IR_CH1_MASK);

    /*
     * Read 5 buttons.
     */
    button_value = XGpio_DiscreteRead(GpioPtr, BUTTON_CHANNEL) & BUTTON_MASK;

    /*
     * Select pattern based on pressed button.
     * If multiple buttons are pressed, lower button gets priority.
     */
    if (button_value & BUTTON_0) {
        selected_pattern = PATTERN_TRAIN_FORWARD;
    }
    else if (button_value & BUTTON_1) {
        selected_pattern = PATTERN_TRAIN_REVERSE;
    }
    else if (button_value & BUTTON_2) {
        selected_pattern = PATTERN_ALTERNATE_BLINK;
    }
    else if (button_value & BUTTON_3) {
        selected_pattern = PATTERN_HALF_HALF;
    }
    else if (button_value & BUTTON_4) {
        selected_pattern = PATTERN_FILL_EMPTY;
    }
}

static int SetupInterruptSystem(void)
{
    int status;
    XScuGic_Config *IntcConfig;

    IntcConfig = XScuGic_LookupConfig(INTC_BASEADDR);
    if (IntcConfig == NULL) {
        return XST_FAILURE;
    }

    status = XScuGic_CfgInitialize(&Intc,
                                   IntcConfig,
                                   IntcConfig->CpuBaseAddress);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * 0xA0 = priority
     * 0x1  = active-high level-sensitive interrupt
     */
    XScuGic_SetPriorityTriggerType(&Intc,
                                   GPIO_INTERRUPT_ID,
                                   0xA0,
                                   0x1);

    status = XScuGic_Connect(&Intc,
                             GPIO_INTERRUPT_ID,
                             (Xil_ExceptionHandler)GpioInterruptHandler,
                             &Gpio);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    XScuGic_Enable(&Intc, GPIO_INTERRUPT_ID);

    Xil_ExceptionInit();

    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
                                 &Intc);

    Xil_ExceptionEnable();

    return XST_SUCCESS;
}

static u32 GetLedValue(u32 pattern, u32 step)
{
    u32 phase;

    switch (pattern) {
    case PATTERN_TRAIN_FORWARD:
    {
        /*
        * Train entering tunnel from LD0 side to LD7 side.
        *
        * Train is 4 LEDs long.
        * It moves forward, then slowly disappears into the tunnel.
        *
        * 00001111
        * 00011110
        * 00111100
        * 01111000
        * 11110000
        * 11100000
        * 11000000
        * 10000000
        * 00000000
        */
        static const u32 train_forward[] = {
            0x0F,
            0x1E,
            0x3C,
            0x78,
            0xF0,
            0xE0,
            0xC0,
            0x80,
            0x00
        };

        return train_forward[step % 9];
    }

    case PATTERN_TRAIN_REVERSE:
    {
        /*
        * Train entering tunnel from LD7 side to LD0 side.
        *
        * 11110000
        * 01111000
        * 00111100
        * 00011110
        * 00001111
        * 00000111
        * 00000011
        * 00000001
        * 00000000
        */
        static const u32 train_reverse[] = {
            0xF0,
            0x78,
            0x3C,
            0x1E,
            0x0F,
            0x07,
            0x03,
            0x01,
            0x00
        };

        return train_reverse[step % 9];
    }

    case PATTERN_ALTERNATE_BLINK:
        /*
         * 10101010 -> 01010101 -> repeat
         */
        if (step % 2 == 0) {
            return 0xAA;
        } else {
            return 0x55;
        }

    case PATTERN_HALF_HALF:
        /*
         * Lower 4 LEDs ON, then upper 4 LEDs ON
         */
        if (step % 2 == 0) {
            return 0x0F;
        } else {
            return 0xF0;
        }

    case PATTERN_FILL_EMPTY:
        /*
         * Fill LEDs one by one:
         * 00000001
         * 00000011
         * 00000111
         * ...
         * 11111111
         * then empty slowly.
         */
        phase = step % 16;

        if (phase < 8) {
            return (1U << (phase + 1)) - 1;
        } else {
            return 0xFFU >> (phase - 7);
        }

    default:
        return 0x00;
    }
}

int main(void)
{
    int status;

    u32 current_pattern;
    u32 previous_pattern;
    u32 step;
    u32 led_value;

    status = XGpio_Initialize(&Gpio, GPIO_BASEADDR);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Channel 1: buttons input
     * Channel 2: LEDs output
     */
    XGpio_SetDataDirection(&Gpio, BUTTON_CHANNEL, BUTTON_MASK);
    XGpio_SetDataDirection(&Gpio, LED_CHANNEL, 0x00);

    /*
     * Startup LED test.
     */
    XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, 0xAA);
    usleep(300000);

    XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, 0x55);
    usleep(300000);

    XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, 0x01);

    /*
     * Clear old pending interrupt.
     */
    XGpio_InterruptClear(&Gpio, XGPIO_IR_CH1_MASK);

    status = SetupInterruptSystem();
    if (status != XST_SUCCESS) {
        XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, 0xE1);
        return XST_FAILURE;
    }

    /*
     * Enable AXI GPIO Channel 1 interrupt.
     */
    XGpio_InterruptEnable(&Gpio, XGPIO_IR_CH1_MASK);

    /*
     * Enable global interrupt inside AXI GPIO.
     */
    XGpio_InterruptGlobalEnable(&Gpio);

    previous_pattern = selected_pattern;
    step = 0;

    while (1) {
        current_pattern = selected_pattern;

        /*
         * If user selects a new pattern, restart that pattern from step 0.
         */
        if (current_pattern != previous_pattern) {
            step = 0;
            previous_pattern = current_pattern;
        }

        led_value = GetLedValue(current_pattern, step);
        XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, led_value & LED_MASK);

        step++;

        /*
         * Pattern speed.
         * Reduce this value for faster animation.
         * Increase this value for slower animation.
         */
        usleep(200000);
    }

    return 0;
}