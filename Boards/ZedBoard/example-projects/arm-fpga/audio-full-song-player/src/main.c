#include "xparameters.h"
#include "xil_printf.h"
#include "xiic_l.h"
#include "xil_io.h"
#include "ff.h"
#include "sleep.h"

#include <stdint.h>
#include <string.h>

/* ---------------- Codec / IIC configuration ---------------- */

#if defined(XPAR_AXI_IIC_0_BASEADDR)
#define IIC_BASEADDR XPAR_AXI_IIC_0_BASEADDR
#elif defined(XPAR_IIC_0_BASEADDR)
#define IIC_BASEADDR XPAR_IIC_0_BASEADDR
#else
#error "AXI IIC base address macro not found. Check exported hardware/platform."
#endif

#define ADAU1761_I2C_ADDR 0x3B

/* ---------------- AXI GPIO sample-streaming register ---------------- */

#if defined(XPAR_AXI_GPIO_AUDIO_SAMPLE_BASEADDR)
#define AUDIO_GPIO_BASEADDR XPAR_AXI_GPIO_AUDIO_SAMPLE_BASEADDR
#elif defined(XPAR_XGPIO_0_BASEADDR)
#define AUDIO_GPIO_BASEADDR XPAR_XGPIO_0_BASEADDR
#else
#error "AXI GPIO audio sample base address macro not found."
#endif

#define GPIO_CH1_DATA_OFFSET 0x00U
#define GPIO_CH1_TRI_OFFSET  0x04U
#define GPIO_CH2_DATA_OFFSET 0x08U
#define GPIO_CH2_TRI_OFFSET  0x0CU

/* ---------------- WAV / SD configuration ---------------- */

#define WAV_FILE_NAME       "0:/SONG.WAV"
#define EXPECTED_RATE_HZ    12000U
#define EXPECTED_CHANNELS   1U
#define EXPECTED_BITS       16U

#define AUDIO_BUFFER_SIZE   8192U

static FATFS fatfs;
static FIL wav_file;

typedef struct {
    uint16_t reg;
    uint8_t  val;
} codec_reg_t;

static uint16_t read_le16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t read_le32(const uint8_t *p)
{
    return (uint32_t)p[0]
        | ((uint32_t)p[1] << 8)
        | ((uint32_t)p[2] << 16)
        | ((uint32_t)p[3] << 24);
}

/* ---------------- ADAU1761 codec functions ---------------- */

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
     * Proven working ADAU1761 playback sequence.
     * This routes I2S input from PL to DAC, headphone, and line outputs.
     */
    const codec_reg_t init_seq[] = {
        {0x4000, 0x01},   /* Clock control */

        {0x401C, 0x21},   /* Playback mixer left */
        {0x401E, 0x41},   /* Playback mixer right */

        {0x4020, 0x05},   /* Playback LR mixer left */
        {0x4021, 0x11},   /* Playback LR mixer right */

        {0x4023, 0xE7},   /* Headphone left output */
        {0x4024, 0xE7},   /* Headphone right output */

        {0x4025, 0xE6},   /* Line output left */
        {0x4026, 0xE6},   /* Line output right */

        {0x4029, 0x03},   /* Playback power management */
        {0x402A, 0x03},   /* DAC control */

        {0x40F2, 0x01},   /* Route serial input L0/R0 to DAC L/R */
        {0x40F3, 0x01},   /* Serial output route */

        {0x40F9, 0x7F},   /* Enable clocks to internal engines */
        {0x40FA, 0x03},   /* Enable clock generators */
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

/* ---------------- AXI GPIO audio streaming functions ---------------- */

static void audio_gpio_init(void)
{
    /*
     * AXI GPIO channel 1:
     *   16-bit sample_data output
     *
     * AXI GPIO channel 2:
     *   1-bit sample_toggle output
     */

    Xil_Out32(AUDIO_GPIO_BASEADDR + GPIO_CH1_TRI_OFFSET, 0x00000000U);
    Xil_Out32(AUDIO_GPIO_BASEADDR + GPIO_CH2_TRI_OFFSET, 0x00000000U);

    Xil_Out32(AUDIO_GPIO_BASEADDR + GPIO_CH1_DATA_OFFSET, 0x00000000U);
    Xil_Out32(AUDIO_GPIO_BASEADDR + GPIO_CH2_DATA_OFFSET, 0x00000000U);
}

static void audio_write_sample(int16_t sample)
{
    static uint32_t toggle = 0U;

    /*
     * Important order:
     * 1. Write sample data first.
     * 2. Toggle channel 2 to tell PL a new sample is available.
     */

    Xil_Out32(AUDIO_GPIO_BASEADDR + GPIO_CH1_DATA_OFFSET,
              (uint32_t)((uint16_t)sample));

    toggle ^= 1U;

    Xil_Out32(AUDIO_GPIO_BASEADDR + GPIO_CH2_DATA_OFFSET, toggle);
}

/* ---------------- WAV parser ---------------- */

static int find_wav_data_chunk(uint32_t *data_offset,
                               uint32_t *data_size,
                               uint32_t *sample_rate,
                               uint16_t *channels,
                               uint16_t *bits_per_sample)
{
    FRESULT result;
    UINT bytes_read;
    uint8_t buf[16];

    uint16_t audio_format = 0;

    result = f_read(&wav_file, buf, 12, &bytes_read);
    if (result != FR_OK || bytes_read != 12) {
        xil_printf("FAIL: Could not read RIFF header.\r\n");
        return -1;
    }

    if (memcmp(&buf[0], "RIFF", 4) != 0 || memcmp(&buf[8], "WAVE", 4) != 0) {
        xil_printf("FAIL: Not a valid RIFF/WAVE file.\r\n");
        return -1;
    }

    while (f_tell(&wav_file) < f_size(&wav_file)) {
        uint8_t chunk_header[8];
        uint32_t chunk_size;
        uint32_t chunk_start;

        result = f_read(&wav_file, chunk_header, 8, &bytes_read);
        if (result != FR_OK || bytes_read != 8) {
            xil_printf("FAIL: Could not read WAV chunk header.\r\n");
            return -1;
        }

        chunk_size = read_le32(&chunk_header[4]);
        chunk_start = (uint32_t)f_tell(&wav_file);

        if (memcmp(&chunk_header[0], "fmt ", 4) == 0) {
            uint8_t fmt_buf[16];

            result = f_read(&wav_file, fmt_buf, 16, &bytes_read);
            if (result != FR_OK || bytes_read != 16) {
                xil_printf("FAIL: Could not read fmt chunk.\r\n");
                return -1;
            }

            audio_format     = read_le16(&fmt_buf[0]);
            *channels        = read_le16(&fmt_buf[2]);
            *sample_rate     = read_le32(&fmt_buf[4]);
            *bits_per_sample = read_le16(&fmt_buf[14]);

            f_lseek(&wav_file, chunk_start + chunk_size);
        }
        else if (memcmp(&chunk_header[0], "data", 4) == 0) {
            *data_offset = (uint32_t)f_tell(&wav_file);
            *data_size   = chunk_size;
            break;
        }
        else {
            f_lseek(&wav_file, chunk_start + chunk_size);
        }

        if (chunk_size & 1U) {
            f_lseek(&wav_file, f_tell(&wav_file) + 1U);
        }
    }

    if (audio_format != 1U) {
        xil_printf("FAIL: WAV is not PCM format. audio_format=%u\r\n",
                   (unsigned int)audio_format);
        return -1;
    }

    if (*channels != EXPECTED_CHANNELS ||
        *sample_rate != EXPECTED_RATE_HZ ||
        *bits_per_sample != EXPECTED_BITS ||
        *data_size == 0U) {

        xil_printf("FAIL: Unsupported WAV format.\r\n");
        xil_printf("Expected: mono, 12000 Hz, 16-bit PCM.\r\n");
        xil_printf("Actual channels        : %u\r\n", (unsigned int)*channels);
        xil_printf("Actual sample rate     : %u\r\n", (unsigned int)*sample_rate);
        xil_printf("Actual bits per sample : %u\r\n", (unsigned int)*bits_per_sample);
        xil_printf("Actual data size       : %u\r\n", (unsigned int)*data_size);
        return -1;
    }

    return 0;
}

/* ---------------- Full-song playback ---------------- */

static void play_wav_stream(uint32_t data_offset, uint32_t data_size)
{
    static uint8_t audio_buffer[AUDIO_BUFFER_SIZE];

    FRESULT result;
    UINT bytes_read;

    uint32_t remaining = data_size;
    uint32_t total_samples = data_size / 2U;
    uint32_t played_samples = 0U;

    /*
     * 12 kHz sample rate:
     * 1 / 12000 = 83.33 microseconds.
     *
     * This is a simple polling-based timing method.
     * It is good enough for this first full-song playback test.
     */
    const unsigned int sample_delay_us = 83U;

    xil_printf("Seeking to WAV data offset...\r\n");

    result = f_lseek(&wav_file, data_offset);
    if (result != FR_OK) {
        xil_printf("FAIL: f_lseek to data failed. Error code = %d\r\n", result);
        return;
    }

    xil_printf("Starting full song playback...\r\n");
    xil_printf("Total samples: %u\r\n", (unsigned int)total_samples);

    while (remaining > 0U) {
        UINT to_read;

        if (remaining > AUDIO_BUFFER_SIZE) {
            to_read = AUDIO_BUFFER_SIZE;
        } else {
            to_read = (UINT)remaining;
        }

        /*
         * Keep reads aligned to complete 16-bit samples.
         */
        if (to_read & 1U) {
            to_read--;
        }

        result = f_read(&wav_file, audio_buffer, to_read, &bytes_read);
        if (result != FR_OK) {
            xil_printf("FAIL: f_read failed. Error code = %d\r\n", result);
            break;
        }

        if (bytes_read == 0U) {
            break;
        }

        for (UINT i = 0U; i + 1U < bytes_read; i += 2U) {
            int16_t sample;

            sample = (int16_t)read_le16(&audio_buffer[i]);

            /*
            * Reduce volume to avoid clipping/distortion.
            * Try >> 1 first. If still noisy, try >> 2.
            */
            sample = sample >> 1;

            audio_write_sample(sample);

            played_samples++;

            /*
             * Delay for approximately one 12 kHz sample period.
             */
            usleep(sample_delay_us);
        }

        remaining -= bytes_read;
    }

    /*
     * Send short silence after playback.
     */
    for (int i = 0; i < 2000; i++) {
        audio_write_sample(0);
        usleep(sample_delay_us);
    }

    xil_printf("Playback completed.\r\n");
    xil_printf("Played samples: %u\r\n", (unsigned int)played_samples);
}

/* ---------------- main ---------------- */

int main(void)
{
    FRESULT result;

    uint32_t data_offset = 0U;
    uint32_t data_size = 0U;
    uint32_t sample_rate = 0U;
    uint16_t channels = 0U;
    uint16_t bits_per_sample = 0U;

    xil_printf("\r\n");
    xil_printf("====================================\r\n");
    xil_printf("ZedBoard Full Song SD Audio Player\r\n");
    xil_printf("====================================\r\n");

    xil_printf("AXI IIC base address       : 0x%08x\r\n", (unsigned int)IIC_BASEADDR);
    xil_printf("ADAU1761 I2C address      : 0x%02x\r\n", ADAU1761_I2C_ADDR);
    xil_printf("AXI GPIO audio base address: 0x%08x\r\n", (unsigned int)AUDIO_GPIO_BASEADDR);
    xil_printf("\r\n");

    audio_gpio_init();

    if (adau1761_init_playback() != 0) {
        xil_printf("FAIL: Codec playback initialization failed.\r\n");
        while (1) {
            sleep(1);
        }
    }

    xil_printf("Mounting SD card...\r\n");

    result = f_mount(&fatfs, "0:/", 1);
    if (result != FR_OK) {
        xil_printf("FAIL: f_mount failed. Error code = %d\r\n", result);
        while (1) {
            sleep(1);
        }
    }

    xil_printf("PASS: SD card mounted.\r\n");

    result = f_open(&wav_file, WAV_FILE_NAME, FA_READ);
    if (result != FR_OK) {
        xil_printf("FAIL: Could not open SONG.WAV. Error code = %d\r\n", result);
        while (1) {
            sleep(1);
        }
    }

    xil_printf("PASS: SONG.WAV opened.\r\n");
    xil_printf("File size: %u bytes\r\n", (unsigned int)f_size(&wav_file));

    if (find_wav_data_chunk(&data_offset,
                            &data_size,
                            &sample_rate,
                            &channels,
                            &bits_per_sample) != 0) {
        f_close(&wav_file);
        while (1) {
            sleep(1);
        }
    }

    xil_printf("\r\n");
    xil_printf("WAV information:\r\n");
    xil_printf("------------------------------------\r\n");
    xil_printf("Channels        : %u\r\n", (unsigned int)channels);
    xil_printf("Sample rate     : %u Hz\r\n", (unsigned int)sample_rate);
    xil_printf("Bits per sample : %u\r\n", (unsigned int)bits_per_sample);
    xil_printf("Data offset     : %u bytes\r\n", (unsigned int)data_offset);
    xil_printf("Data size       : %u bytes\r\n", (unsigned int)data_size);
    xil_printf("------------------------------------\r\n");

    xil_printf("PASS: WAV format valid for playback.\r\n");
    xil_printf("Connect speaker/headphone to LINE OUT or HEADPHONE OUT.\r\n");
    xil_printf("Playback will start now.\r\n");
    xil_printf("\r\n");

    play_wav_stream(data_offset, data_size);

    f_close(&wav_file);

    xil_printf("Full-song player finished.\r\n");

    while (1) {
        sleep(1);
    }

    return 0;
}