#include "xparameters.h"
#include "xgpio.h"
#include "xtmrctr.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xstatus.h"

#define GPIO_BASEADDR       XPAR_AXI_GPIO_0_BASEADDR
#define TIMER_BASEADDR      XPAR_AXI_TIMER_0_BASEADDR
#define TIMER_INTERRUPT_ID  (XPAR_FABRIC_AXI_TIMER_0_INTR + 32)
#define INTC_BASEADDR       XPAR_XSCUGIC_0_BASEADDR

#define LED_CHANNEL         1
#define TIMER_NUMBER        0

/*
 * 100 MHz AXI clock assumed.
 * 20,000,000 counts = 200 ms.
 */
#define TIMER_RESET_VALUE   20000000U

/*
 * 200 ms x 15 = 3 seconds.
 */
#define TICKS_PER_PATTERN   15U

#define PATTERN_TRAIN_FORWARD     0
#define PATTERN_TRAIN_REVERSE     1
#define PATTERN_ALTERNATE_BLINK   2
#define PATTERN_HALF_HALF         3
#define PATTERN_FILL_EMPTY        4

#define TOTAL_PATTERNS            5

static XGpio Gpio;
static XTmrCtr Timer;
static XScuGic Intc;

static volatile u32 current_pattern = PATTERN_TRAIN_FORWARD;
static volatile u32 pattern_step = 0;
static volatile u32 pattern_tick_count = 0;

static u32 GetLedPattern(u32 pattern, u32 step)
{
    u32 phase;

    switch (pattern) {
    case PATTERN_TRAIN_FORWARD:
    {
        /*
         * Train entering tunnel from LD0 side to LD7 side.
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
         * 10101010 -> 01010101
         */
        if ((step % 2) == 0) {
            return 0xAA;
        } else {
            return 0x55;
        }

    case PATTERN_HALF_HALF:
        /*
         * Lower half ON, then upper half ON.
         */
        if ((step % 2) == 0) {
            return 0x0F;
        } else {
            return 0xF0;
        }

    case PATTERN_FILL_EMPTY:
        /*
         * Fill LEDs one by one, then empty.
         */
        phase = step % 16;

        if (phase < 8) {
            return (1U << (phase + 1)) - 1U;
        } else {
            return 0xFFU >> (phase - 7);
        }

    default:
        return 0x00;
    }
}

static void TimerCallback(void *CallBackRef, u8 TimerNumber)
{
    u32 led_value;

    (void)CallBackRef;
    (void)TimerNumber;

    /*
     * Display current pattern step.
     */
    led_value = GetLedPattern(current_pattern, pattern_step);
    XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, led_value);

    pattern_step++;
    pattern_tick_count++;

    /*
     * Change to next pattern every 3 seconds.
     */
    if (pattern_tick_count >= TICKS_PER_PATTERN) {
        pattern_tick_count = 0;
        pattern_step = 0;

        current_pattern++;

        if (current_pattern >= TOTAL_PATTERNS) {
            current_pattern = PATTERN_TRAIN_FORWARD;
        }
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

    XScuGic_SetPriorityTriggerType(&Intc,
                                   TIMER_INTERRUPT_ID,
                                   0xA0,
                                   0x1);

    status = XScuGic_Connect(&Intc,
                             TIMER_INTERRUPT_ID,
                             (Xil_ExceptionHandler)XTmrCtr_InterruptHandler,
                             &Timer);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    XScuGic_Enable(&Intc, TIMER_INTERRUPT_ID);

    Xil_ExceptionInit();

    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
                                 &Intc);

    Xil_ExceptionEnable();

    return XST_SUCCESS;
}

int main(void)
{
    int status;

    status = XGpio_Initialize(&Gpio, GPIO_BASEADDR);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    XGpio_SetDataDirection(&Gpio, LED_CHANNEL, 0x00);

    /*
     * Startup LED test.
     */
    XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, 0xAA);
    for (volatile int i = 0; i < 10000000; i++);

    XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, 0x55);
    for (volatile int i = 0; i < 10000000; i++);

    XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, 0x00);

    status = XTmrCtr_Initialize(&Timer, TIMER_BASEADDR);
    if (status != XST_SUCCESS) {
        XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, 0xE1);
        return XST_FAILURE;
    }

    XTmrCtr_SetHandler(&Timer, TimerCallback, &Timer);

    XTmrCtr_SetOptions(&Timer,
                       TIMER_NUMBER,
                       XTC_INT_MODE_OPTION |
                       XTC_AUTO_RELOAD_OPTION |
                       XTC_DOWN_COUNT_OPTION);

    XTmrCtr_SetResetValue(&Timer, TIMER_NUMBER, TIMER_RESET_VALUE);

    status = SetupInterruptSystem();
    if (status != XST_SUCCESS) {
        XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, 0xE2);
        return XST_FAILURE;
    }

    XTmrCtr_Start(&Timer, TIMER_NUMBER);

    while (1) {
        /*
         * Main loop is empty.
         * Timer interrupt controls LED pattern updates.
         */
    }

    return 0;
}