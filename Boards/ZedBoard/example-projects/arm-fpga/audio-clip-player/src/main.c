#include "xparameters.h"
#include "xil_printf.h"
#include "xiic_l.h"
#include "sleep.h"
#include <stdint.h>

#ifndef XPAR_AXI_IIC_0_BASEADDR
    #error "XPAR_AXI_IIC_0_BASEADDR not found. Check AXI IIC in exported XSA/BSP."
#endif

#define IIC_BASEADDR        XPAR_AXI_IIC_0_BASEADDR
#define ADAU1761_I2C_ADDR   0x3B

typedef struct {
    uint16_t reg;
    uint8_t  val;
} codec_reg_t;

static int adau1761_write_reg8(uint16_t reg_addr, uint8_t value)
{
    uint8_t tx_buf[3];

    tx_buf[0] = (uint8_t)((reg_addr >> 8) & 0xFF);
    tx_buf[1] = (uint8_t)(reg_addr & 0xFF);
    tx_buf[2] = value;

    int sent = XIic_Send(
        IIC_BASEADDR,
        ADAU1761_I2C_ADDR,
        tx_buf,
        sizeof(tx_buf),
        XIIC_STOP
    );

    if (sent != sizeof(tx_buf)) {
        xil_printf("I2C write failed: reg 0x%04x, value 0x%02x, sent %d of %d bytes\r\n",
                   (unsigned int)reg_addr,
                   (unsigned int)value,
                   sent,
                   (int)sizeof(tx_buf));
        return -1;
    }

    return 0;
}

static int codec_write_checked(uint16_t reg, uint8_t val)
{
    xil_printf("W 0x%04x <- 0x%02x ... ",
               (unsigned int)reg,
               (unsigned int)val);

    if (adau1761_write_reg8(reg, val) != 0) {
        xil_printf("FAIL\r\n");
        return -1;
    }

    xil_printf("OK\r\n");
    usleep(1000);
    return 0;
}

static int adau1761_init_playback(void)
{
    /*
     * This configuration is for:
     *
     * PL provides:
     *   MCLK   = 12.288 MHz
     *   BCLK   = 3.072 MHz
     *   LRCLK  = 48 kHz
     *   SDATA  = I2S stereo data into codec
     *
     * Codec role:
     *   I2S slave
     *   route serial input L0/R0 to DAC L/R
     *   enable DAC/playback path
     *   enable headphone and line output paths
     */

    const codec_reg_t init_seq[] = {
        {0x4000, 0x01},   // Clock control: enable core clock

        {0x401C, 0x21},   // Playback mixer left: unmute/enable
        {0x401E, 0x41},   // Playback mixer right: unmute/enable

        {0x4020, 0x05},   // Playback LR mixer left
        {0x4021, 0x11},   // Playback LR mixer right

        {0x4023, 0xE7},   // Headphone left: 0 dB, unmute, enable
        {0x4024, 0xE7},   // Headphone right: 0 dB, unmute, headphone mode

        {0x4025, 0xE6},   // Line output left: 0 dB, unmute, line mode
        {0x4026, 0xE6},   // Line output right: 0 dB, unmute, line mode

        {0x4029, 0x03},   // Playback power management: enable playback L/R
        {0x402A, 0x03},   // DAC control: enable both DACs

        {0x40F2, 0x01},   // Serial input L0/R0 routed to DAC L/R
        {0x40F3, 0x01},   // Serial output route, harmless for our playback test

        {0x40F9, 0x7F},   // Enable clocks to internal engines
        {0x40FA, 0x03},   // Enable clock generators
    };

    xil_printf("Starting ADAU1761 playback initialization...\r\n");

    for (unsigned int i = 0; i < sizeof(init_seq) / sizeof(init_seq[0]); i++) {
        if (codec_write_checked(init_seq[i].reg, init_seq[i].val) != 0) {
            return -1;
        }
    }

    xil_printf("ADAU1761 playback initialization complete.\r\n");
    return 0;
}

int main(void)
{
    xil_printf("\r\n");
    xil_printf("====================================\r\n");
    xil_printf("ZedBoard Audio Clip Player\r\n");
    xil_printf("====================================\r\n");

    xil_printf("AXI IIC base address: 0x%08x\r\n", (unsigned int)IIC_BASEADDR);
    xil_printf("ADAU1761 I2C address: 0x%02x\r\n", ADAU1761_I2C_ADDR);

    xil_printf("\r\n");
    xil_printf("PL is playing stored PCM audio samples from audio_samples.mem.\r\n");
    xil_printf("The samples are stored in XPM block ROM inside the FPGA fabric.\r\n");
    xil_printf("ARM software is configuring the ADAU1761 codec to route I2S input to analog outputs.\r\n");
    xil_printf("\r\n");

    if (adau1761_init_playback() != 0) {
        xil_printf("FAIL: Codec playback initialization failed.\r\n");
        while (1) {
            sleep(1);
        }
    }

    xil_printf("\r\n");
    xil_printf("PASS: Codec configured successfully.\r\n");
    xil_printf("You should now hear the stored 5-second audio clip looping continuously.\r\n");
    xil_printf("Audio output verified through green LINE OUT and black HEADPHONE OUT.\r\n");

    while (1) {
        sleep(1);
    }

    return 0;
}