# ZedBoard Full-Song SD Audio Player

This project builds a **ZedBoard full-song audio player** using **Vivado 2025.2** and **Vitis 2025.2**.

The final result is a full WAV song stored on the SD card and played from the ZedBoard audio outputs:

- Green **LINE OUT**
- Black **HEADPHONE / HEADSET OUT**

The project uses:

- **Zynq Processing System (PS)** for bare-metal C software
- **SD card + FatFs/xilffs** for reading the full song file
- **AXI IIC** for configuring the ADAU1761 audio codec
- **AXI GPIO** for sending PCM samples from ARM software to PL hardware
- **Programmable Logic (PL)** for generating I2S-style audio signals
- **ADAU1761 codec** for converting digital audio into analog output

---

## Table of Contents

1. [Project Goal](#project-goal)
2. [Final Result](#final-result)
3. [Architecture Overview](#architecture-overview)
4. [Hardware and Software Requirements](#hardware-and-software-requirements)
5. [Important Audio Concepts](#important-audio-concepts)
6. [Project Folder Structure](#project-folder-structure)
7. [Prepare the SD Card](#prepare-the-sd-card)
8. [Prepare the Full-Song WAV File](#prepare-the-full-song-wav-file)
9. [Verify WAV File from ZedBoard](#verify-wav-file-from-zedboard)
10. [Vivado Hardware Design](#vivado-hardware-design)
11. [AXI GPIO Sample Streaming Design](#axi-gpio-sample-streaming-design)
12. [Streaming I2S RTL Design](#streaming-i2s-rtl-design)
13. [CDC Timing Constraint](#cdc-timing-constraint)
14. [Build Vivado Hardware](#build-vivado-hardware)
15. [Export Hardware to Vitis](#export-hardware-to-vitis)
16. [Create Vitis Platform](#create-vitis-platform)
17. [Enable xilffs for SD Card Access](#enable-xilffs-for-sd-card-access)
18. [Create Vitis Application](#create-vitis-application)
19. [Bare-Metal C Application](#bare-metal-c-application)
20. [Build and Run](#build-and-run)
21. [Expected UART Output](#expected-uart-output)
22. [Debug Checklist](#debug-checklist)
23. [Problems Faced and Fixes](#problems-faced-and-fixes)
24. [Current Limitation](#current-limitation)
25. [Future Improvements](#future-improvements)

---

## Project Goal

The goal is to play a **full audio song** from the ZedBoard audio outputs.

Unlike the earlier 5-second audio clip player, this project does **not** store the song inside FPGA block RAM. A full song is too large for simple ROM-based playback.

Instead, this project streams samples from the SD card through the ARM processor into PL.

Final playback configuration:

```text
Input source:      MP3 file on PC
Converted format:  WAV
WAV format:        12 kHz mono, 16-bit PCM
Storage:           SD card
File name:         SONG.WAV
Playback method:   ARM reads samples from SD card and writes to AXI GPIO
PL role:           Converts incoming PCM samples to I2S-style audio
MCLK:              12.288 MHz
BCLK:              3.072 MHz
LRCLK:             48 kHz
Output:            LINE OUT and HEADPHONE OUT
```

Success means:

```text
1. SD card is detected by Zynq PS.
2. FatFs/xilffs can mount the SD card.
3. SONG.WAV can be opened and parsed.
4. ADAU1761 codec initializes successfully.
5. ARM streams samples to PL through AXI GPIO.
6. PL sends I2S-style audio to the codec.
7. Full song is heard from LINE OUT and HEADPHONE OUT.
```

---

## Final Result

Confirmed working result:

```text
SD card read:          working
SONG.WAV open:         working
WAV header parsing:    working
Codec initialization:  working
Full song playback:    working
LINE OUT:              working
HEADPHONE OUT:         working
```

The final working path is:

```text
MP3 file
   ↓
Full 12 kHz mono 16-bit WAV
   ↓
SONG.WAV on SD card
   ↓
Zynq PS reads WAV file using FatFs/xilffs
   ↓
ARM writes PCM samples to AXI GPIO
   ↓
audio_stream_i2s_player in PL
   ↓
I2S-style audio signals
   ↓
ADAU1761 codec
   ↓
LINE OUT / HEADPHONE OUT
```

The song was confirmed to play, with slight noise/distortion in the first working version.

---

## Architecture Overview

The project has four major parts.

### 1. Audio Preparation on PC

```text
Boom_Boom.mp3
    |
    | ffmpeg
    v
SONG.WAV
```

The MP3 is converted into a WAV file that the ZedBoard can read directly.

### 2. SD Card File Access

```text
SD card
   |
   | Zynq PS SD0 controller
   v
FatFs / xilffs
   |
   v
Bare-metal C application
```

The ARM Cortex-A9 mounts the SD card, opens `SONG.WAV`, parses the WAV header, and reads PCM samples.

### 3. Codec Control Path

The ARM processor configures the ADAU1761 audio codec through I2C.

```text
ARM Cortex-A9
    |
    | AXI-Lite
    v
AXI IIC
    |
    | I2C SCL/SDA
    v
ADAU1761 audio codec
```

The codec must be configured before audio can be heard.

### 4. Audio Data Path

The ARM sends samples to the PL through AXI GPIO. The PL converts those samples into I2S-style serial audio.

```text
ARM Cortex-A9
    |
    | AXI-Lite writes
    v
AXI GPIO sample register
    |
    | sample_data[15:0]
    | sample_toggle[0]
    v
audio_stream_i2s_player
    |
    | BCLK / LRCLK / SDATA
    v
ADAU1761 codec
    |
    v
LINE OUT / HEADPHONE OUT
```

---

## Hardware and Software Requirements

### Hardware

- ZedBoard
- ZedBoard power adapter
- USB cable for JTAG/programming
- USB-UART connection
- SD card
- SD card reader
- Earphones, headphones, or powered speaker
- 3.5 mm audio cable if testing LINE OUT

### Software

- Vivado 2025.2
- Vitis 2025.2
- ZedBoard board files installed
- FFmpeg
- Serial terminal such as Vitis Serial Monitor, PuTTY, or Tera Term

---

## Important Audio Concepts

### ADAU1761 Codec

The ZedBoard has an ADAU1761 audio codec. The FPGA does not directly drive the headphone or line-out jack. The FPGA sends digital audio to the codec, and the codec converts it into analog audio.

### SD Card

The SD card is connected to the **Zynq PS side**, not directly to the FPGA PL side.

So there is no simple PL “SD card module” in the block design. The SD card is accessed by ARM C software through the Zynq PS SD controller.

### WAV / PCM

The project plays WAV/PCM, not compressed MP3 directly.

```text
MP3       = compressed audio
WAV/PCM   = decoded raw samples
```

For this project, the MP3 is converted on the PC first:

```text
MP3 → WAV → SD card → ZedBoard playback
```

### I2S-Style Audio

The PL audio streamer outputs:

```text
MCLK    Master clock
BCLK    Bit clock
LRCLK   Left/right channel clock
SDATA   Serial audio data
```

In this project:

```text
MCLK_O   = 12.288 MHz
BCLK_O   = 3.072 MHz
LRCLK_O  = 48 kHz
SDATA_O  = audio data from FPGA to codec
SDATA_I  = audio data from codec to FPGA, unused here
```

---

## Project Folder Structure

Example structure:

```text
C:\Zed_Projects
    |
    |-- zed_audio_full_song_player
    |       |-- zed_audio_bd_wrapper.xsa
    |       |-- create_full_song_platform.py
    |       |-- create_full_song_app.py
    |       |
    |       |-- audio_files
    |       |       |-- Boom_Boom.mp3
    |       |       |-- SONG.WAV
    |       |
    |       |-- zed_audio_tone.srcs
    |               |-- sources_1
    |                       |-- new
    |                               |-- audio_stream_i2s_player.v
    |
    |-- zed_audio_full_song_vitis_ws
            |-- zed_audio_full_song_platform
            |-- zed_full_song_player_app
```

---

## Prepare the SD Card

The SD card may already contain important boot files such as:

```text
BOOT.BIN
devicetree_ramdisk.dtb
ramdisk8M.image.gz
README
zImage
```

Do not format or delete the SD card.

First back it up:

```cmd
mkdir C:\Zed_Projects\sd_card_backup
robocopy D:\ C:\Zed_Projects\sd_card_backup /E
```

Then add only the required project files to the root of the SD card.

The final SD card can contain:

```text
D:\
    BOOT.BIN
    devicetree_ramdisk.dtb
    ramdisk8M.image.gz
    README
    zImage
    HELLO.TXT
    SONG.WAV
```

---

## Prepare the Full-Song WAV File

Check FFmpeg:

```cmd
where ffmpeg
```

If FFmpeg is not installed:

```cmd
winget install -e --id Gyan.FFmpeg
```

Convert the full MP3 file to a 12 kHz mono 16-bit PCM WAV:

```cmd
ffmpeg -y -i "C:\Zed_Projects\zed_audio_clip_player\audio_files\Boom_Boom.mp3" -ac 1 -ar 12000 -acodec pcm_s16le "C:\Zed_Projects\zed_audio_clip_player\audio_files\SONG.WAV"
```

Copy the WAV file to the SD card:

```cmd
copy /Y "C:\Zed_Projects\zed_audio_clip_player\audio_files\SONG.WAV" D:\SONG.WAV
```

Verify:

```cmd
dir D:\SONG.WAV
```

Confirmed file size:

```text
SONG.WAV = 5,763,568 bytes
```

---

## Verify WAV File from ZedBoard

Before building the full player, a separate SD read test app was created.

The first test file was:

```text
D:\HELLO.TXT
```

Content:

```text
ZedBoard SD card read test successful.
```

The SD read test confirmed:

```text
PASS: SD card mounted successfully.
PASS: HELLO.TXT opened successfully.
PASS: File read successfully.
```

Then `SONG.WAV` header was parsed on the ZedBoard.

Confirmed WAV information:

```text
Audio format    : 1
Channels        : 1
Sample rate     : 12000 Hz
Bits per sample : 16
Data offset     : 190 bytes
Data size       : 5763378 bytes
Bytes per second: 24000
Approx duration : 240 seconds
```

This confirmed that `SONG.WAV` is suitable for playback.

---

## Vivado Hardware Design

This project was created by copying the working audio clip player project and replacing the ROM-based audio source with a streaming source.

Old clip-player path:

```text
XPM ROM
   ↓
audio_clip_i2s_player
   ↓
I2S
```

New full-song path:

```text
AXI GPIO sample register
   ↓
audio_stream_i2s_player
   ↓
I2S
```

Main block design components:

```text
processing_system7_0          Zynq PS
axi_iic_0                     codec I2C control
clk_wiz_0                     generates 12.288 MHz audio clock
rst_audio_12M                 reset block for audio clock domain
axi_gpio_audio_sample         AXI GPIO sample/toggle register
audio_stream_i2s_player       custom PL streaming I2S player
```

External audio ports:

```text
MCLK_O
BCLK_O
LRCLK_O
SDATA_O
SDATA_I
iic_rtl_scl_io
iic_rtl_sda_io
```

---

## AXI GPIO Sample Streaming Design

An AXI GPIO IP was added and renamed:

```text
axi_gpio_audio_sample
```

Configuration:

```text
Dual channel: enabled

Channel 1:
    Width: 16
    Direction: output
    Purpose: sample_data[15:0]

Channel 2:
    Width: 1
    Direction: output
    Purpose: sample_toggle
```

Address from `xparameters.h`:

```c
#define XPAR_AXI_GPIO_AUDIO_SAMPLE_BASEADDR 0x41200000
#define XPAR_AXI_GPIO_AUDIO_SAMPLE_IS_DUAL 0x1
#define XPAR_AXI_GPIO_AUDIO_SAMPLE_GPIO_WIDTH 0x10
```

Software write order:

```text
1. ARM writes 16-bit PCM sample to channel 1.
2. ARM toggles channel 2 bit.
3. PL detects the toggle and captures the new sample.
```

---

## Streaming I2S RTL Design

The custom RTL module is:

```text
audio_stream_i2s_player.v
```

Its job is to:

```text
1. Receive sample_data[15:0] from AXI GPIO.
2. Detect sample_toggle changes from ARM software.
3. Synchronize AXI GPIO data into the 12.288 MHz audio clock domain.
4. Generate BCLK from 12.288 MHz audio clock.
5. Generate 48 kHz LRCLK.
6. Send the latest mono sample to both left and right channels.
7. Output I2S-style audio to the ADAU1761 codec.
```

Important RTL ports:

```verilog
input  wire        audio_clk;
input  wire        reset;
input  wire [15:0] sample_data;
input  wire        sample_toggle;
input  wire        SDATA_I;
output reg         BCLK_O;
output reg         LRCLK_O;
output reg         SDATA_O;
```

Important CDC synchronizers:

```verilog
(* ASYNC_REG = "TRUE" *) reg        sample_toggle_meta;
(* ASYNC_REG = "TRUE" *) reg        sample_toggle_sync;
(* ASYNC_REG = "TRUE" *) reg [15:0] sample_data_meta;
(* ASYNC_REG = "TRUE" *) reg [15:0] sample_data_sync;
```

The RTL loads new samples only at safe audio frame boundaries to avoid changing audio data in the middle of an I2S frame.

---

## CDC Timing Constraint

The AXI GPIO register is clocked by the 100 MHz PS/AXI clock.

The audio streamer is clocked by the 12.288 MHz Clocking Wizard output.

This creates an intentional clock-domain crossing:

```text
clk_fpga_0  →  clk_out1 from clk_wiz_0
```

The RTL synchronizes this crossing, so the clocks must be constrained as asynchronous.

Add this to the XDC file:

```tcl
## Intentional CDC:
## AXI GPIO sample register is clocked by PS FCLK/AXI clock.
## audio_stream_i2s_player is clocked by the 12.288 MHz Clocking Wizard output.
## sample_data/sample_toggle are synchronized inside audio_stream_i2s_player.

set_clock_groups -asynchronous \
    -group [get_clocks clk_fpga_0] \
    -group [get_clocks -of_objects [get_pins zed_audio_bd_i/clk_wiz_0/inst/mmcm_adv_inst/CLKOUT0]]
```

This fixed the methodology warnings:

```text
TIMING-8  No common period between related clocks
TIMING-16 Large setup violation
TIMING-28 Auto-derived clock referenced by timing constraint
```

Final methodology result:

```text
Checks found: 0
```

---

## Build Vivado Hardware

Run these steps in Vivado:

```text
1. Validate Design
2. Generate Output Products
3. Run Synthesis
4. Run Implementation
5. Generate Bitstream
6. Report Methodology
```

Confirmed result:

```text
Bitstream generated successfully
Methodology checks found: 0
Design state: Fully Routed
```

---

## Export Hardware to Vitis

In Vivado:

```text
File -> Export -> Export Hardware
```

Select:

```text
Include bitstream
```

Expected exported file:

```text
C:\Zed_Projects\zed_audio_full_song_player\zed_audio_bd_wrapper.xsa
```

Overwrite the old copied XSA if Vivado asks.

---

## Create Vitis Platform

Create:

```text
C:\Zed_Projects\zed_audio_full_song_player\create_full_song_platform.py
```

Add:

```python
import os
import vitis

workspace = r"C:\Zed_Projects\zed_audio_full_song_vitis_ws"
xsa_path = r"C:\Zed_Projects\zed_audio_full_song_player\zed_audio_bd_wrapper.xsa"
platform_name = "zed_audio_full_song_platform"

if not os.path.exists(xsa_path):
    raise FileNotFoundError(f"XSA not found: {xsa_path}")

client = vitis.create_client()
client.set_workspace(path=workspace)

platform = client.create_platform_component(
    name=platform_name,
    hw_design=xsa_path,
    os="standalone",
    cpu="ps7_cortexa9_0",
    domain_name="standalone_ps7_cortexa9_0"
)

platform.build()

print("ZedBoard full-song platform created and built successfully.")
print(f"Workspace: {workspace}")
print(f"Platform name: {platform_name}")
print(f"XSA used: {xsa_path}")
```

Run:

```cmd
cd /d C:\Zed_Projects\zed_audio_full_song_player
C:\AMDDesignTools\2025.2\Vitis\bin\vitis.bat -s create_full_song_platform.py
```

Expected:

```text
Platform Build Finished successfully.
ZedBoard full-song platform created and built successfully.
```

---

## Enable xilffs for SD Card Access

The SD controller appeared in `xparameters.h`:

```c
#define XPAR_XSDPS_NUM_INSTANCES 1
#define XPAR_XSDPS_0_BASEADDR 0xe0100000
#define XPAR_XSDPS_0_HAS_CD 0x1
#define XPAR_XSDPS_0_HAS_WP 0x1
```

To read normal files like `SONG.WAV`, the Vitis platform must include `xilffs`.

In Vitis:

```text
zed_audio_full_song_platform
    ↓
vitis-comp.json
    ↓
standalone_ps7_cortexa9_0
    ↓
Board Support Package
    ↓
Supported Libraries
    ↓
Enable xilffs
```

Then:

```text
Ctrl + S
Regenerate BSP
Build zed_audio_full_song_platform
```

Verify:

```cmd
dir /s /b "C:\Zed_Projects\zed_audio_full_song_vitis_ws\zed_audio_full_song_platform\export\zed_audio_full_song_platform\sw\standalone_ps7_cortexa9_0\include\ff.h"
```

Expected:

```text
...\standalone_ps7_cortexa9_0\include\ff.h
```

---

## Create Vitis Application

Create:

```text
C:\Zed_Projects\zed_audio_full_song_player\create_full_song_app.py
```

Add:

```python
import os
import vitis

workspace = r"C:\Zed_Projects\zed_audio_full_song_vitis_ws"
app_name = "zed_full_song_player_app"

xpfm_files = []

for root, dirs, files in os.walk(workspace):
    for file in files:
        if file.endswith(".xpfm"):
            xpfm_files.append(os.path.join(root, file))

if not xpfm_files:
    raise FileNotFoundError(f"No .xpfm file found inside workspace: {workspace}")

platform_path = xpfm_files[0]

print(f"Using platform: {platform_path}")

client = vitis.create_client()
client.set_workspace(path=workspace)

app = client.create_app_component(
    name=app_name,
    platform=platform_path,
    domain="standalone_ps7_cortexa9_0",
    template="empty_application"
)

print("Full song player application created successfully.")
print(f"Workspace: {workspace}")
print(f"Application name: {app_name}")
print(f"Platform used: {platform_path}")
```

Run:

```cmd
cd /d C:\Zed_Projects\zed_audio_full_song_player
C:\AMDDesignTools\2025.2\Vitis\bin\vitis.bat -s create_full_song_app.py
```

Expected:

```text
Full song player application created successfully.
Application name: zed_full_song_player_app
```

---

## Bare-Metal C Application

The full C application does these jobs:

```text
1. Configure AXI GPIO outputs.
2. Configure ADAU1761 codec through AXI IIC.
3. Mount SD card using FatFs/xilffs.
4. Open SONG.WAV.
5. Parse the WAV header.
6. Seek to the data chunk.
7. Read PCM samples from SD card.
8. Write each sample to AXI GPIO channel 1.
9. Toggle AXI GPIO channel 2 for each new sample.
10. PL converts the samples into I2S output.
```

Important software constants:

```c
#define WAV_FILE_NAME       "0:/SONG.WAV"
#define EXPECTED_RATE_HZ    12000U
#define EXPECTED_CHANNELS   1U
#define EXPECTED_BITS       16U

#define AUDIO_GPIO_BASEADDR XPAR_AXI_GPIO_AUDIO_SAMPLE_BASEADDR

#define GPIO_CH1_DATA_OFFSET 0x00U
#define GPIO_CH1_TRI_OFFSET  0x04U
#define GPIO_CH2_DATA_OFFSET 0x08U
#define GPIO_CH2_TRI_OFFSET  0x0CU
```

AXI GPIO initialization:

```c
Xil_Out32(AUDIO_GPIO_BASEADDR + GPIO_CH1_TRI_OFFSET, 0x00000000U);
Xil_Out32(AUDIO_GPIO_BASEADDR + GPIO_CH2_TRI_OFFSET, 0x00000000U);
```

Sample write function:

```c
static void audio_write_sample(int16_t sample)
{
    static uint32_t toggle = 0U;

    Xil_Out32(AUDIO_GPIO_BASEADDR + GPIO_CH1_DATA_OFFSET,
              (uint32_t)((uint16_t)sample));

    toggle ^= 1U;

    Xil_Out32(AUDIO_GPIO_BASEADDR + GPIO_CH2_DATA_OFFSET, toggle);
}
```

Simple timing used in the first working version:

```c
const unsigned int sample_delay_us = 83U;
```

This approximates:

```text
1 / 12000 Hz = 83.33 microseconds
```

---

## Build and Run

Open Vitis workspace:

```text
C:\Zed_Projects\zed_audio_full_song_vitis_ws
```

Build:

```text
zed_full_song_player_app
```

Expected:

```text
Build Finished
0 errors
```

Prepare UART terminal:

```text
Baud rate:    115200
Data bits:    8
Parity:       None
Stop bits:    1
Flow control: None
```

Connect audio output:

```text
Green LINE OUT
Black HEADPHONE / HEADSET OUT
```

Run on hardware:

```text
Run -> Run As -> Launch Hardware
```

Allow Vitis to program the FPGA bitstream.

---

## Expected UART Output

Expected startup:

```text
====================================
ZedBoard Full Song SD Audio Player
====================================
AXI IIC base address       : 0x41600000
ADAU1761 I2C address      : 0x3B
AXI GPIO audio base address: 0x41200000

Starting ADAU1761 playback initialization...
W 0x4000 <- 0x01 ... OK
...
ADAU1761 playback initialization complete.
Mounting SD card...
PASS: SD card mounted.
PASS: SONG.WAV opened.
File size: 5763568 bytes
```

Expected WAV information:

```text
WAV information:
------------------------------------
Channels        : 1
Sample rate     : 12000 Hz
Bits per sample : 16
Data offset     : 190 bytes
Data size       : 5763378 bytes
------------------------------------
PASS: WAV format valid for playback.
Connect speaker/headphone to LINE OUT or HEADPHONE OUT.
Playback will start now.

Seeking to WAV data offset...
Starting full song playback...
Total samples: 2881689
```

Expected physical result:

```text
The full song plays from LINE OUT and HEADPHONE OUT.
```

Observed result:

```text
Song plays successfully.
Small amount of noise/distortion is present in the first working version.
```

---

## Debug Checklist

If no audio is heard, debug in this order.

### 1. Check UART

Confirm:

```text
PASS: SD card mounted.
PASS: SONG.WAV opened.
PASS: WAV format valid for playback.
Starting full song playback...
```

If the code stops before playback, fix SD card or WAV parsing first.

### 2. Check SD Card

Confirm `SONG.WAV` exists in the SD card root:

```cmd
dir D:\SONG.WAV
```

### 3. Check WAV Format

Expected:

```text
Audio format:    1
Channels:        1
Sample rate:     12000 Hz
Bits per sample: 16
```

### 4. Check xilffs

Confirm:

```cmd
dir /s /b "C:\Zed_Projects\zed_audio_full_song_vitis_ws\zed_audio_full_song_platform\export\zed_audio_full_song_platform\sw\standalone_ps7_cortexa9_0\include\ff.h"
```

### 5. Check AXI GPIO Base Address

Expected:

```c
#define XPAR_AXI_GPIO_AUDIO_SAMPLE_BASEADDR 0x41200000
```

### 6. Check Codec Initialization

Every ADAU1761 register write should print:

```text
OK
```

If any write fails, fix I2C/codec configuration first.

### 7. Check FPGA Programming

The PL must be programmed because it contains:

```text
audio_stream_i2s_player
AXI GPIO sample path
I2S signal generation
```

### 8. Check Audio Jacks

Test both:

```text
Green LINE OUT
Black HEADPHONE / HEADSET OUT
```

### 9. Check Methodology

Expected:

```text
Checks found: 0
```

If timing warnings appear between AXI GPIO and audio streamer, check the CDC RTL and XDC clock group constraint.

---

## Problems Faced and Fixes

### Problem 1: Full Song Too Large for XPM ROM

The earlier clip project stored 5 seconds in FPGA ROM.

A full song is much larger:

```text
SONG.WAV data size = 5,763,378 bytes
```

Fix:

```text
Use SD card streaming instead of FPGA ROM storage.
```

---

### Problem 2: SD Card Had Existing Files

The SD card contained important boot/Linux files.

Fix:

```text
Back up the SD card using robocopy.
Do not format or delete existing files.
Only add HELLO.TXT and SONG.WAV.
```

---

### Problem 3: FatFs Header Missing

Initial check:

```text
ff.h not found
```

Cause:

```text
xilffs was not enabled for the standalone application BSP.
```

Fix:

```text
Enable xilffs in the standalone_ps7_cortexa9_0 BSP.
Regenerate BSP.
Rebuild platform.
```

---

### Problem 4: Wrong printf Format for uint32_t

Initial UART output showed huge wrong values for sample rate/data size.

Cause:

```text
xil_printf does not reliably handle %lu.
```

Fix:

```text
Use %u and cast values to unsigned int.
```

---

### Problem 5: Multiple Driver Net in RTL

Implementation initially failed because one register was driven from more than one always block.

Cause:

```text
pending_valid was assigned in two always blocks.
```

Fix:

```text
Rewrite audio_stream_i2s_player so all related registers are driven from one always block.
```

---

### Problem 6: CDC Timing Warnings

Methodology showed:

```text
TIMING-8
TIMING-16
```

Cause:

```text
AXI GPIO runs on 100 MHz PS/AXI clock.
audio_stream_i2s_player runs on 12.288 MHz audio clock.
Vivado timed this crossing as a normal synchronous path.
```

Fix:

```text
Add CDC synchronizers in RTL.
Add set_clock_groups -asynchronous constraint in XDC.
```

---

### Problem 7: TIMING-28 Warning

Methodology showed:

```text
TIMING-28: Auto-derived clock referenced by a timing constraint
```

Cause:

```text
The Clocking Wizard output clock was referenced directly by auto-generated clock name.
```

Fix:

```tcl
Use get_clocks -of_objects [get_pins .../CLKOUT0]
```

Final result:

```text
Methodology checks found: 0
```

---

### Problem 8: xtime_l.h Missing

Build failed with:

```text
fatal error: xtime_l.h: No such file or directory
```

Fix:

```text
Remove xtime_l.h usage.
Use simple usleep(83) timing for the first working version.
```

---

### Problem 9: Slight Noise in Playback

The full song played successfully but with small noise/distortion.

Likely causes:

```text
usleep(83) timing is not perfectly accurate
SD card reads can introduce timing gaps
AXI GPIO is a simple register, not an audio FIFO
Output volume may be slightly high
```

Quick test:

```c
sample = sample >> 1;
```

This reduces volume and can reduce clipping.

Professional fix:

```text
Use FIFO or AXI DMA based audio streaming.
```

---

## Current Limitation

This project is a successful first full-song playback design, but it is not yet a professional real-time audio pipeline.

Current method:

```text
SD card read
   ↓
C loop
   ↓
usleep timing
   ↓
AXI GPIO sample writes
   ↓
PL I2S output
```

This works, but it can have jitter/noise.

A better design would use:

```text
SD card read
   ↓
buffer in DDR
   ↓
AXI DMA or FIFO
   ↓
PL audio streamer
   ↓
I2S output
```

---

## Future Improvements

Possible next improvements:

- Add amplitude scaling / volume control
- Add pause/resume button
- Add restart button
- Add song selection from SD card
- Add stereo WAV support
- Add 48 kHz playback support
- Add FIFO between PS and PL
- Replace AXI GPIO with AXI Stream FIFO
- Use AXI DMA for stable streaming
- Add interrupt-driven playback timing
- Add MP3 decoding using software decoder
- Add OLED/UART menu for selecting songs
- Add audio effects such as fade-in, fade-out, echo, or low-pass filtering

---

## Final Summary

This project successfully plays a full WAV song from the ZedBoard SD card using:

```text
Zynq PS bare-metal C
FatFs / xilffs
AXI IIC
AXI GPIO
Custom PL I2S audio streamer
ADAU1761 codec
```

Confirmed final output:

```text
LINE OUT:              working
HEADPHONE/HEADSET OUT: working
```

The first version has slight noise, but the complete full-song playback pipeline is working end to end.
