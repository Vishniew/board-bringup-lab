# ZedBoard Audio Clip Player

This project builds a complete **ZedBoard audio clip player** using **Vivado 2025.2** and **Vitis 2025.2**.

The final result is a short stored audio clip playing repeatedly from the ZedBoard:

- Green **LINE OUT**
- Black **HEADPHONE / HEADSET OUT**

The project uses:

- **Zynq Processing System (PS)** for bare-metal C software
- **AXI IIC** for configuring the ADAU1761 audio codec
- **Programmable Logic (PL)** for generating the audio stream
- **XPM block ROM** for storing PCM samples inside FPGA fabric
- **I2S-style audio signals** for sending digital audio to the ADAU1761 codec
- **ADAU1761 codec** for converting digital audio into analog output

---

## Table of Contents

1. [Project Goal](#project-goal)
2. [Final Result](#final-result)
3. [Architecture Overview](#architecture-overview)
4. [Hardware and Software Requirements](#hardware-and-software-requirements)
5. [Important Audio Concepts](#important-audio-concepts)
6. [Project Folder Structure](#project-folder-structure)
7. [Prepare the Audio File](#prepare-the-audio-file)
8. [Convert WAV to Memory File](#convert-wav-to-memory-file)
9. [Vivado Hardware Design](#vivado-hardware-design)
10. [Audio Clip RTL Design](#audio-clip-rtl-design)
11. [Why XPM ROM Was Required](#why-xpm-rom-was-required)
12. [Build Vivado Hardware](#build-vivado-hardware)
13. [Export Hardware to Vitis](#export-hardware-to-vitis)
14. [Create Vitis Platform](#create-vitis-platform)
15. [Create Vitis Application](#create-vitis-application)
16. [Bare-Metal C Application](#bare-metal-c-application)
17. [Build and Run](#build-and-run)
18. [Expected UART Output](#expected-uart-output)
19. [Debug Checklist](#debug-checklist)
20. [Problems Faced and Fixes](#problems-faced-and-fixes)
21. [Future Improvements](#future-improvements)

---

## Project Goal

The goal is to play a short real audio clip from the ZedBoard audio outputs.

The audio clip is not streamed from SD card, USB, Ethernet, or DDR. Instead, the clip is converted into a memory initialization file and stored directly in FPGA block memory.

Final playback configuration:

```text
Input file:       MP3 audio file
Clip duration:    5 seconds
Stored format:    12 kHz mono, 16-bit PCM
Stored samples:   60,000 samples
Output rate:      48 kHz I2S frame rate
Playback method:  each 12 kHz sample repeated 4 times
MCLK:             12.288 MHz
BCLK:             3.072 MHz
LRCLK:            48 kHz
Output:           LINE OUT and HEADPHONE OUT
```

Success means:

```text
1. MP3 is converted into a valid PCM memory file.
2. Vivado bitstream is generated successfully.
3. Vitis bare-metal application builds successfully.
4. UART shows codec initialization success.
5. Audio is heard from both LINE OUT and HEADPHONE OUT.
```

---

## Final Result

Confirmed working outputs:

```text
LINE OUT:              working
HEADPHONE/HEADSET OUT: working
```

The final working path is:

```text
MP3 file
   ↓
5-second 12 kHz mono WAV
   ↓
audio_samples.mem
   ↓
XPM block ROM in PL
   ↓
audio_clip_i2s_player RTL
   ↓
I2S-style audio signals
   ↓
ADAU1761 codec
   ↓
LINE OUT / HEADPHONE OUT
```

---

## Architecture Overview

The project has three main parts.

### 1. Audio Preparation on PC

```text
Boom_Boom.mp3
    |
    | ffmpeg
    v
Boom_Boom_5sec_12k_mono.wav
    |
    | Python wav_to_mem.py
    v
audio_samples.mem
```

`audio_samples.mem` contains one signed 16-bit PCM sample per line in hexadecimal form.

### 2. Codec Control Path

The ARM processor configures the ADAU1761 codec through I2C.

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

The codec must be configured before audio can be heard. Without I2C configuration, the codec may stay muted or may not route I2S input to analog output.

### 3. Audio Data Path

The PL reads samples from XPM ROM and sends them to the codec as I2S-style serial audio.

```text
100 MHz FCLK_CLK0 from Zynq PS
    |
    v
Clocking Wizard
    |
    | 12.288 MHz
    v
audio_clip_i2s_player
    |
    | MCLK / BCLK / LRCLK / SDATA
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
- Earphones, headphones, or powered speaker
- 3.5 mm audio cable if testing LINE OUT

### Software

- Vivado 2025.2
- Vitis 2025.2
- ZedBoard board files installed
- FFmpeg
- Python 3
- Serial terminal such as Vitis Serial Monitor, PuTTY, or Tera Term

---

## Important Audio Concepts

### ADAU1761 Codec

The ZedBoard has an ADAU1761 audio codec. The FPGA does not directly drive the headphone jack or line-out jack. The FPGA sends digital audio to the codec, and the codec converts it into analog audio.

### I2C

I2C is used only for codec control.

```text
ADAU1761 I2C address: 0x3B
AXI IIC base address: 0x41600000
```

### I2S-Style Audio

The audio stream uses these signals:

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

### Why 12 kHz Samples Were Used

A 5-second 48 kHz mono clip requires:

```text
48,000 × 5 = 240,000 samples
```

That is large for a simple block-ROM based project.

So the clip was converted to:

```text
12,000 × 5 = 60,000 samples
```

The hardware still outputs a 48 kHz I2S stream by repeating each stored sample 4 times:

```text
12 kHz × 4 = 48 kHz
```

---

## Project Folder Structure

Example structure:

```text
C:\Zed_Projects
    |
    |-- zed_audio_clip_player
    |       |-- zed_audio_bd_wrapper.xsa
    |       |-- create_audio_clip_platform.py
    |       |-- create_audio_clip_app.py
    |       |
    |       |-- audio_files
    |       |       |-- Boom_Boom.mp3
    |       |       |-- Boom_Boom_5sec_12k_mono.wav
    |       |       |-- wav_to_mem.py
    |       |       |-- audio_samples.mem
    |       |
    |       |-- zed_audio_tone.srcs
    |               |-- sources_1
    |                       |-- new
    |                               |-- audio_clip_i2s_player.v
    |                               |-- audio_samples.mem
    |
    |-- zed_audio_clip_vitis_ws
            |-- zed_audio_clip_platform
            |-- zed_audio_clip_app
```

---

## Prepare the Audio File

Create an audio file folder:

```cmd
mkdir C:\Zed_Projects\zed_audio_clip_player\audio_files
```

Place the MP3 file inside it:

```text
C:\Zed_Projects\zed_audio_clip_player\audio_files\Boom_Boom.mp3
```

Check FFmpeg:

```cmd
where ffmpeg
```

If FFmpeg is not installed:

```cmd
winget install -e --id Gyan.FFmpeg
```

Convert the MP3 to a 5-second, 12 kHz, mono, 16-bit PCM WAV:

```cmd
ffmpeg -y -i "C:\Zed_Projects\zed_audio_clip_player\audio_files\Boom_Boom.mp3" -t 5 -ac 1 -ar 12000 -acodec pcm_s16le "C:\Zed_Projects\zed_audio_clip_player\audio_files\Boom_Boom_5sec_12k_mono.wav"
```

Expected WAV properties:

```text
Duration:      5 seconds
Channels:      mono
Sample rate:   12,000 Hz
Sample width:  16-bit PCM
Samples:       60,000
```

---

## Convert WAV to Memory File

Create:

```text
C:\Zed_Projects\zed_audio_clip_player\audio_files\wav_to_mem.py
```

Add:

```python
import wave
import struct
from pathlib import Path

INPUT_WAV = Path("Boom_Boom_5sec_12k_mono.wav")
OUTPUT_MEM = Path("audio_samples.mem")

EXPECTED_CHANNELS = 1
EXPECTED_SAMPLE_WIDTH = 2
EXPECTED_SAMPLE_RATE = 12000
EXPECTED_SAMPLES = 60000

with wave.open(str(INPUT_WAV), "rb") as wav:
    channels = wav.getnchannels()
    sample_width = wav.getsampwidth()
    sample_rate = wav.getframerate()
    frames = wav.getnframes()

    print(f"Channels: {channels}")
    print(f"Sample width: {sample_width} bytes")
    print(f"Sample rate: {sample_rate} Hz")
    print(f"Total frames: {frames}")

    if channels != EXPECTED_CHANNELS:
        raise ValueError(f"Expected mono WAV, got {channels} channels")
    if sample_width != EXPECTED_SAMPLE_WIDTH:
        raise ValueError(f"Expected 16-bit WAV, got {sample_width * 8}-bit")
    if sample_rate != EXPECTED_SAMPLE_RATE:
        raise ValueError(f"Expected {EXPECTED_SAMPLE_RATE} Hz, got {sample_rate} Hz")

    raw_data = wav.readframes(frames)

samples = struct.unpack("<" + "h" * frames, raw_data)

if len(samples) != EXPECTED_SAMPLES:
    raise ValueError(f"Expected {EXPECTED_SAMPLES} samples, got {len(samples)}")

with open(OUTPUT_MEM, "w") as mem:
    for sample in samples:
        mem.write(f"{sample & 0xFFFF:04x}\n")

print(f"Samples written: {len(samples)}")
print(f"Duration: {len(samples) / EXPECTED_SAMPLE_RATE:.2f} seconds")
print("Conversion complete.")
```

Run:

```cmd
cd /d C:\Zed_Projects\zed_audio_clip_player\audio_files
python wav_to_mem.py
```

Expected output:

```text
Channels: 1
Sample width: 2 bytes
Sample rate: 12000 Hz
Total frames: 60000
Samples written: 60000
Duration: 5.00 seconds
Conversion complete.
```

---

## Vivado Hardware Design

This project was created by copying the already working audio tone generator design and replacing only the audio source logic.

Main block design components:

```text
processing_system7_0       Zynq PS
axi_iic_0                  codec I2C control
clk_wiz_0                  generates 12.288 MHz audio clock
rst_audio_12M              reset block for audio clock domain
audio_clip_i2s_player_0    custom PL audio sample player
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

Important pin constraints:

```tcl
set_property PACKAGE_PIN AB2 [get_ports {MCLK_O}]
set_property PACKAGE_PIN AA6 [get_ports {BCLK_O}]
set_property PACKAGE_PIN Y6  [get_ports {LRCLK_O}]
set_property PACKAGE_PIN Y8  [get_ports {SDATA_O}]
set_property PACKAGE_PIN AA7 [get_ports {SDATA_I}]

set_property PACKAGE_PIN AB4 [get_ports {iic_rtl_scl_io}]
set_property PACKAGE_PIN AB5 [get_ports {iic_rtl_sda_io}]

set_property IOSTANDARD LVCMOS33 [get_ports {MCLK_O}]
set_property IOSTANDARD LVCMOS33 [get_ports {BCLK_O}]
set_property IOSTANDARD LVCMOS33 [get_ports {LRCLK_O}]
set_property IOSTANDARD LVCMOS33 [get_ports {SDATA_O}]
set_property IOSTANDARD LVCMOS33 [get_ports {SDATA_I}]
set_property IOSTANDARD LVCMOS33 [get_ports {iic_rtl_scl_io}]
set_property IOSTANDARD LVCMOS33 [get_ports {iic_rtl_sda_io}]

set_false_path -from [get_ports {iic_rtl_scl_io iic_rtl_sda_io}]
set_false_path -to   [get_ports {iic_rtl_scl_io iic_rtl_sda_io}]
```

---

## Audio Clip RTL Design

The module is:

```text
audio_clip_i2s_player.v
```

Its job is to:

```text
1. Generate BCLK from 12.288 MHz audio clock.
2. Generate 48 kHz LRCLK.
3. Read 16-bit samples from XPM ROM.
4. Repeat each 12 kHz sample 4 times.
5. Send the same mono sample to left and right channels.
6. Loop back to sample 0 after 60,000 samples.
```

Important parameters:

```verilog
localparam integer SAMPLE_COUNT  = 60000;
localparam integer ADDR_WIDTH    = 16;
localparam integer ROM_DEPTH     = 65536;
localparam integer SAMPLE_REPEAT = 4;
```

The ROM is created using:

```verilog
xpm_memory_sprom #(
    .ADDR_WIDTH_A(ADDR_WIDTH),
    .MEMORY_INIT_FILE("audio_samples.mem"),
    .MEMORY_PRIMITIVE("block"),
    .MEMORY_SIZE(ROM_DEPTH * 16),
    .READ_DATA_WIDTH_A(16),
    .READ_LATENCY_A(2),
    .USE_MEM_INIT(1)
) sample_rom_inst (...);
```

Copy `audio_samples.mem` into the Vivado source folder:

```cmd
copy /Y "C:\Zed_Projects\zed_audio_clip_player\audio_files\audio_samples.mem" "C:\Zed_Projects\zed_audio_clip_player\zed_audio_tone.srcs\sources_1\new\audio_samples.mem"
```

Verify line count:

```cmd
find /c /v "" "C:\Zed_Projects\zed_audio_clip_player\zed_audio_tone.srcs\sources_1\new\audio_samples.mem"
```

Expected:

```text
60000
```

Also add `audio_samples.mem` in Vivado as a **memory file**.

---

## Why XPM ROM Was Required

The first version used a large inferred Verilog ROM:

```verilog
reg [15:0] sample_rom [0:SAMPLE_COUNT-1];
initial begin
    $readmemh("audio_samples.mem", sample_rom);
end
```

Vivado accepted synthesis but implementation failed with RAMB36E1 cascade errors:

```text
[DRC REQP-1962] Cascade RAMB36E1 ADDR15 pin check
```

This was not an I2S problem, not a codec problem, and not an audio file problem. It was a memory implementation problem caused by a large inferred ROM.

The fix was:

```text
Replace inferred ROM with Xilinx xpm_memory_sprom.
```

After the XPM ROM fix:

```text
Synthesis:      successful
Implementation: successful
Timing:         passed
Methodology:    clean
Bitstream:      generated
```

---

## Build Vivado Hardware

Run these steps in Vivado:

```text
1. Validate Design
2. Generate Output Products
3. Run Synthesis
4. Run Implementation
5. Check Timing Summary
6. Report Methodology
7. Generate Bitstream
```

Confirmed timing result:

```text
WNS = 3.600 ns
TNS = 0.000
WHS = 0.041 ns
THS = 0.000
Failed Routes = 0
```

Confirmed methodology result:

```text
Checks found: 0
```

Confirmed bitstream result:

```text
write_bitstream Complete
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
C:\Zed_Projects\zed_audio_clip_player\zed_audio_bd_wrapper.xsa
```

---

## Create Vitis Platform

Create:

```text
C:\Zed_Projects\zed_audio_clip_player\create_audio_clip_platform.py
```

Add:

```python
import os
import vitis

workspace = r"C:\Zed_Projects\zed_audio_clip_vitis_ws"
xsa_path = r"C:\Zed_Projects\zed_audio_clip_player\zed_audio_bd_wrapper.xsa"
platform_name = "zed_audio_clip_platform"

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

print("ZedBoard audio clip platform created and built successfully.")
print(f"Workspace: {workspace}")
print(f"Platform name: {platform_name}")
print(f"XSA used: {xsa_path}")
```

Run:

```cmd
cd /d C:\Zed_Projects\zed_audio_clip_player
C:\AMDDesignTools\2025.2\Vitis\bin\vitis.bat -s create_audio_clip_platform.py
```

Expected:

```text
Platform Build Finished successfully.
ZedBoard audio clip platform created and built successfully.
```

---

## Create Vitis Application

Create:

```text
C:\Zed_Projects\zed_audio_clip_player\create_audio_clip_app.py
```

Add this robust version, which finds the `.xpfm` automatically:

```python
import os
import vitis

workspace = r"C:\Zed_Projects\zed_audio_clip_vitis_ws"
app_name = "zed_audio_clip_app"

if not os.path.exists(workspace):
    raise FileNotFoundError(f"Workspace not found: {workspace}")

xpfm_files = []

for root, dirs, files in os.walk(workspace):
    for file in files:
        if file.endswith(".xpfm"):
            xpfm_files.append(os.path.join(root, file))

if not xpfm_files:
    raise FileNotFoundError(f"No .xpfm platform file found inside workspace: {workspace}")

platform_path = xpfm_files[0]

print(f"Found platform XPFM: {platform_path}")

client = vitis.create_client()
client.set_workspace(path=workspace)

app = client.create_app_component(
    name=app_name,
    platform=platform_path,
    domain="standalone_ps7_cortexa9_0",
    template="empty_application"
)

print("ZedBoard audio clip application component created successfully.")
print(f"Workspace: {workspace}")
print(f"Application name: {app_name}")
print(f"Platform used: {platform_path}")
```

Run:

```cmd
cd /d C:\Zed_Projects\zed_audio_clip_player
C:\AMDDesignTools\2025.2\Vitis\bin\vitis.bat -s create_audio_clip_app.py
```

Expected:

```text
Found platform XPFM: ...zed_audio_clip_platform.xpfm
ZedBoard audio clip application component created successfully.
```

---

## Bare-Metal C Application

The C application does not play the audio samples. The PL hardware plays the samples.

The C application only does this:

```text
ARM software
    ↓
AXI IIC
    ↓
ADAU1761 register writes
    ↓
codec routes I2S input to LINE OUT and HEADPHONE OUT
```

Use the proven codec initialization from the working tone generator project, but update the printed messages to say **Audio Clip Player**.

Final `main()` message section:

```c
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
```

Important rule:

```text
Do not keep backup .c files inside zed_audio_clip_app/src.
```

Vitis compiles every `.c` file in `src`. A backup such as `main_clip_backup.c` can cause duplicate `main()` linker errors.

---

## Build and Run

Open Vitis workspace:

```text
C:\Zed_Projects\zed_audio_clip_vitis_ws
```

Build:

```text
zed_audio_clip_app
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

```text
====================================
ZedBoard Audio Clip Player
====================================
AXI IIC base address: 0x41600000
ADAU1761 I2C address: 0x3B

PL is playing stored PCM audio samples from audio_samples.mem.
The samples are stored in XPM block ROM inside the FPGA fabric.
ARM software is configuring the ADAU1761 codec to route I2S input to analog outputs.

Starting ADAU1761 playback initialization...
W 0x4000 <- 0x01 ... OK
W 0x401c <- 0x21 ... OK
W 0x401e <- 0x41 ... OK
W 0x4020 <- 0x05 ... OK
W 0x4021 <- 0x11 ... OK
W 0x4023 <- 0xe7 ... OK
W 0x4024 <- 0xe7 ... OK
W 0x4025 <- 0xe6 ... OK
W 0x4026 <- 0xe6 ... OK
W 0x4029 <- 0x03 ... OK
W 0x402a <- 0x03 ... OK
W 0x40f2 <- 0x01 ... OK
W 0x40f3 <- 0x01 ... OK
W 0x40f9 <- 0x7f ... OK
W 0x40fa <- 0x03 ... OK
ADAU1761 playback initialization complete.

PASS: Codec configured successfully.
You should now hear the stored 5-second audio clip looping continuously.
Audio output verified through green LINE OUT and black HEADPHONE OUT.
```

Expected physical result:

```text
The stored 5-second audio clip loops continuously from both LINE OUT and HEADPHONE OUT.
```

---

## Debug Checklist

If no audio is heard, debug in this order.

### 1. Check UART

Confirm:

```text
PASS: Codec configured successfully.
```

If not, fix I2C/software first.

### 2. Check AXI IIC Base Address

Expected:

```text
0x41600000
```

### 3. Check Codec Address

Expected:

```text
0x3B
```

### 4. Check FPGA Programming

The PL must be programmed because it provides:

```text
MCLK
BCLK
LRCLK
SDATA
audio_samples.mem playback
```

### 5. Check Audio Jacks

Test both:

```text
Green LINE OUT
Black HEADPHONE / HEADSET OUT
```

If headphone works but line out does not, the PL and I2S are working. The issue is likely codec analog routing or the line-out cable/speaker setup.

### 6. Check Audio Sample Memory

Verify:

```cmd
find /c /v "" "C:\Zed_Projects\zed_audio_clip_player\zed_audio_tone.srcs\sources_1\new\audio_samples.mem"
```

Expected:

```text
60000
```

### 7. Check Block Design Module

The block design should contain:

```text
audio_clip_i2s_player_0
```

not:

```text
audio_tone_i2s_0
```

### 8. Check Methodology and Timing

Expected:

```text
Methodology checks found: 0
Failed routes: 0
WNS >= 0
WHS >= 0
```

---

## Problems Faced and Fixes

### Problem 1: Large Inferred ROM Failed Implementation

Error:

```text
[DRC REQP-1962] Cascade RAMB36E1 ADDR15 pin check
```

Cause:

```text
Large inferred ROM from reg-array and $readmemh.
```

Fix:

```text
Use xpm_memory_sprom instead of inferred ROM.
```

---

### Problem 2: 48 kHz Memory Was Too Large

Original sample count:

```text
48,000 × 5 = 240,000 samples
```

Fix:

```text
Use 12 kHz mono samples and repeat each sample 4 times in RTL.
```

Final sample count:

```text
12,000 × 5 = 60,000 samples
```

---

### Problem 3: Vitis Compiled Backup C File

Error:

```text
multiple definition of main
```

Cause:

```text
main.c and main_clip_backup.c were both inside src.
```

Fix:

```text
Do not keep backup .c files inside src.
```

---

### Problem 4: Vitis Metadata Got Polluted

Error:

```text
Cannot find source file: main_clip_backup.c
```

Cause:

```text
Vitis added the backup file into UserConfig.cmake/CMake metadata.
```

Clean fix:

```text
Delete and recreate the Vitis platform/application workspace.
```

---

### Problem 5: Headphone Worked but Line Out Did Not

Cause:

```text
Codec register sequence was not exactly the proven working line-out configuration.
```

Fix:

```text
Reuse the proven ADAU1761 codec initialization code from the working tone project.
Update only printed messages.
```

---

## Future Improvements

Possible next improvements:

- Add AXI-Lite register to start/stop playback
- Add button-controlled pause/resume
- Add switch-controlled clip restart
- Add codec volume control
- Store multiple short clips and select between them
- Add stereo audio sample support
- Read samples from DDR instead of block ROM
- Use AXI DMA for longer audio playback
- Read WAV files from SD card
- Add effects such as echo, fade-in, fade-out, or low-pass filtering

---

## Final Summary

This project successfully plays a stored 5-second audio clip from the ZedBoard audio jacks using:

```text
Zynq PS bare-metal C
AXI IIC
XPM block ROM
Custom PL I2S audio clip player
ADAU1761 codec
```

Confirmed final output:

```text
LINE OUT:              working
HEADPHONE/HEADSET OUT: working
```
