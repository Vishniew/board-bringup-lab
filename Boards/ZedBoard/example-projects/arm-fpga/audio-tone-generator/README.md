# ZedBoard Audio Tone Generator

This project builds a complete **ZedBoard audio tone generator** using **Vivado 2025.2** and **Vitis 2025.2**.

The final result is a continuous audible tone from the ZedBoard:

- Green **LINE OUT**
- Black **HEADPHONE / HEADSET OUT**

The project uses:

- **Zynq Processing System (PS)** for bare-metal C software
- **AXI IIC** for configuring the ADAU1761 audio codec
- **Programmable Logic (PL)** for generating audio clocks and I2S-style serial audio data
- **ADAU1761 audio codec** for converting digital audio into analog output

---

## Table of Contents

1. [Project Goal](#project-goal)
2. [Architecture Overview](#architecture-overview)
3. [Hardware and Software Requirements](#hardware-and-software-requirements)
4. [Important Audio Concepts](#important-audio-concepts)
5. [Create a Fresh Vivado Project](#create-a-fresh-vivado-project)
6. [Create the Block Design](#create-the-block-design)
7. [Configure the Zynq Processing System](#configure-the-zynq-processing-system)
8. [Add AXI IIC for Codec Configuration](#add-axi-iic-for-codec-configuration)
9. [Add Clocking Wizard for Audio MCLK](#add-clocking-wizard-for-audio-mclk)
10. [Add Custom I2S Tone Generator RTL](#add-custom-i2s-tone-generator-rtl)
11. [Connect the Audio RTL Module](#connect-the-audio-rtl-module)
12. [Add Audio Pin Constraints](#add-audio-pin-constraints)
13. [Validate, Generate Wrapper, Build Bitstream](#validate-generate-wrapper-build-bitstream)
14. [Export Hardware to Vitis](#export-hardware-to-vitis)
15. [Create Vitis Platform Using Python Script](#create-vitis-platform-using-python-script)
16. [Create Vitis Application Using Python Script](#create-vitis-application-using-python-script)
17. [Add Bare-Metal C Application](#add-bare-metal-c-application)
18. [Build and Run in Vitis](#build-and-run-in-vitis)
19. [Expected Output](#expected-output)
20. [Debug Checklist](#debug-checklist)
21. [Future Improvements](#future-improvements)

---

## Project Goal

The goal is to generate a continuous tone on the ZedBoard and hear it from the physical audio jacks.

The project generates:

```text
Tone frequency: 1 kHz
Sample rate:    48 kHz
MCLK:           12.288 MHz
BCLK:           3.072 MHz
LRCLK:          48 kHz
Audio format:   I2S-style stereo
Output:         LINE OUT and HEADPHONE/HEADSET OUT
```

Success means:

```text
1. Vivado bitstream is generated successfully.
2. Vitis bare-metal application builds successfully.
3. UART shows codec initialization success.
4. A steady tone is heard from the ZedBoard LINE OUT and HEADPHONE OUT.
```

---

## Architecture Overview

The project has two main paths.

### 1. Codec Control Path

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

The codec must be configured before audio can be heard. Without I2C configuration, the codec may stay muted or may not route I2S input to analog output.

### 2. Audio Data Path

The PL generates the audio clock and I2S-style audio signals.

```text
100 MHz FCLK_CLK0 from Zynq PS
    |
    v
Clocking Wizard
    |
    | 12.288 MHz
    v
audio_tone_i2s
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
- Power adapter for ZedBoard
- USB cable for JTAG/programming
- USB-UART connection
- Earphones/headphones or powered speaker
- 3.5 mm audio cable if using external speaker

### Software

- Vivado 2025.2
- Vitis 2025.2
- ZedBoard board files installed
- Serial terminal:
  - Vitis Serial Monitor
  - PuTTY
  - Tera Term
  - or any equivalent UART terminal

### Board Selection

When creating the Vivado project, select the **ZedBoard board file**, not only the raw FPGA part.

Use:

```text
Board: ZedBoard
```

Do not manually select only:

```text
xc7z020clg484-1
```

unless you know exactly how to manually configure the Zynq PS.

---

## Important Audio Concepts

### ADAU1761 Codec

The ZedBoard has an ADAU1761 audio codec. The FPGA does not directly drive headphones. The FPGA sends digital audio to the codec, and the codec converts it to analog audio.

### I2C

I2C is used only for control/configuration.

In this project, the ARM writes codec registers through AXI IIC.

```text
I2C codec address used: 0x3B
```

### I2S-Style Audio

I2S-style serial audio uses:

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

## Create a Fresh Vivado Project

1. Open Vivado 2025.2.

2. Create a new project:

```text
File -> Project -> New
```

3. Project name:

```text
zed_audio_tone
```

Example location:

```text
C:\Zed_Projects\zed_audio_tone
```

4. Project type:

```text
RTL Project
```

5. Select:

```text
Do not specify sources at this time
```

6. Board selection:

```text
Boards -> ZedBoard
```

7. Finish project creation.

---

## Create the Block Design

1. In Vivado Flow Navigator, click:

```text
IP Integrator -> Create Block Design
```

2. Name the block design:

```text
zed_audio_bd
```

3. Click **OK**.

---

## Configure the Zynq Processing System

### Add Zynq Processing System

1. In the block design, click the **+** button.
2. Search:

```text
ZYNQ7 Processing System
```

3. Add it to the design.
4. Click:

```text
Run Block Automation
```

5. Keep the ZedBoard preset/default options.
6. Click **OK**.

Vivado should create external ports:

```text
DDR
FIXED_IO
```

### Confirm FCLK_CLK0

1. Double-click:

```text
processing_system7_0
```

2. Go to:

```text
Clock Configuration -> PL Fabric Clocks
```

3. Confirm:

```text
FCLK_CLK0 enabled
FCLK_CLK0 frequency = 100 MHz
```

### Enable M_AXI_GP0

1. In the Zynq PS configuration window, go to:

```text
PS-PL Configuration
```

2. Expand:

```text
AXI Non Secure Enablement
```

3. Expand:

```text
GP Master AXI Interface
```

4. Enable:

```text
M AXI GP0 Interface
```

This is required because the ARM processor must access AXI IIC through AXI-Lite.

Click **OK**.

---

## Add AXI IIC for Codec Configuration

### Add AXI IIC

1. In the block design, click **+**.
2. Search:

```text
AXI IIC
```

3. Add it.

The block should be named:

```text
axi_iic_0
```

### Configure AXI IIC

1. Double-click `axi_iic_0`.
2. Set I2C/SCL frequency to:

```text
100 kHz
```

3. Keep the default AXI4-Lite mode.
4. Click **OK**.

### Connect AXI IIC to Zynq PS

1. Click:

```text
Run Connection Automation
```

2. Select:

```text
axi_iic_0/S_AXI
```

3. Keep default options.
4. Click **OK**.

Vivado should create/connect:

```text
processing_system7_0/M_AXI_GP0
        |
        v
AXI SmartConnect
        |
        v
axi_iic_0/S_AXI
```

It should also connect AXI clocks and resets.

### Make IIC External

1. On `axi_iic_0`, right-click the `IIC` interface.
2. Select:

```text
Make External
```

3. Rename the external interface to:

```text
iic_rtl
```

The final top-level I2C pins will become:

```text
iic_rtl_scl_io
iic_rtl_sda_io
```

---

## Add Clocking Wizard for Audio MCLK

The codec needs a 12.288 MHz master clock for 48 kHz audio.

### Add Clocking Wizard

1. In the block design, click **+**.
2. Search:

```text
Clocking Wizard
```

3. Add it.

The block should be named:

```text
clk_wiz_0
```

### Configure Clocking Wizard

1. Double-click `clk_wiz_0`.

2. In **Clocking Options**, set/check:

```text
Input clock frequency = 100.000 MHz
```

3. Set the input clock source/buffer to:

```text
No buffer
```

This is important because the input clock comes internally from the Zynq PS FCLK, not from an external FPGA pin.

4. Go to **Output Clocks**.

5. Enable only `clk_out1`.

6. Set:

```text
clk_out1 requested frequency = 12.288 MHz
```

7. Click **OK**.

### Connect Clocking Wizard

Connect:

```text
processing_system7_0/FCLK_CLK0 -> clk_wiz_0/clk_in1
```

Connect the reset:

```text
rst_ps7_0_100M/peripheral_reset -> clk_wiz_0/reset
```

### Make MCLK External

1. Right-click:

```text
clk_wiz_0/clk_out1
```

2. Select:

```text
Make External
```

3. Rename the external port to:

```text
MCLK_O
```

---

## Add Custom I2S Tone Generator RTL

### Create Verilog File

In Vivado:

```text
Sources -> Design Sources
Right-click Design Sources
Add Sources
Add or create design sources
Create File
```

Set:

```text
File type: Verilog
File name: audio_tone_i2s
```

Replace the generated file contents with the following code.

### `audio_tone_i2s.v`

```verilog
`timescale 1ns / 1ps

module audio_tone_i2s (
    input  wire audio_clk,   // 12.288 MHz audio master clock
    input  wire reset,       // active-high reset

    input  wire SDATA_I,     // codec ADC data, unused in this playback-only project

    output reg  BCLK_O,      // I2S bit clock: 3.072 MHz
    output reg  LRCLK_O,     // I2S left/right clock: 48 kHz
    output reg  SDATA_O      // I2S DAC data to codec
);

    // 12.288 MHz / 4 = 3.072 MHz BCLK
    // 3.072 MHz / 64 = 48 kHz LRCLK
    // 48 samples per sine period at 48 kHz = 1 kHz tone

    reg [1:0] mclk_div;
    reg [5:0] frame_bit;
    reg [5:0] sample_index;
    reg signed [15:0] active_sample;

    reg [5:0] next_bit;
    reg [5:0] next_sample_index;

    function signed [15:0] sine_sample;
        input [5:0] idx;
        begin
            case (idx)
                6'd0:  sine_sample = 16'sd0;
                6'd1:  sine_sample = 16'sd2088;
                6'd2:  sine_sample = 16'sd4141;
                6'd3:  sine_sample = 16'sd6123;
                6'd4:  sine_sample = 16'sd8000;
                6'd5:  sine_sample = 16'sd9740;
                6'd6:  sine_sample = 16'sd11314;
                6'd7:  sine_sample = 16'sd12694;
                6'd8:  sine_sample = 16'sd13856;
                6'd9:  sine_sample = 16'sd14782;
                6'd10: sine_sample = 16'sd15455;
                6'd11: sine_sample = 16'sd15863;
                6'd12: sine_sample = 16'sd16000;
                6'd13: sine_sample = 16'sd15863;
                6'd14: sine_sample = 16'sd15455;
                6'd15: sine_sample = 16'sd14782;
                6'd16: sine_sample = 16'sd13856;
                6'd17: sine_sample = 16'sd12694;
                6'd18: sine_sample = 16'sd11314;
                6'd19: sine_sample = 16'sd9740;
                6'd20: sine_sample = 16'sd8000;
                6'd21: sine_sample = 16'sd6123;
                6'd22: sine_sample = 16'sd4141;
                6'd23: sine_sample = 16'sd2088;
                6'd24: sine_sample = 16'sd0;
                6'd25: sine_sample = -16'sd2088;
                6'd26: sine_sample = -16'sd4141;
                6'd27: sine_sample = -16'sd6123;
                6'd28: sine_sample = -16'sd8000;
                6'd29: sine_sample = -16'sd9740;
                6'd30: sine_sample = -16'sd11314;
                6'd31: sine_sample = -16'sd12694;
                6'd32: sine_sample = -16'sd13856;
                6'd33: sine_sample = -16'sd14782;
                6'd34: sine_sample = -16'sd15455;
                6'd35: sine_sample = -16'sd15863;
                6'd36: sine_sample = -16'sd16000;
                6'd37: sine_sample = -16'sd15863;
                6'd38: sine_sample = -16'sd15455;
                6'd39: sine_sample = -16'sd14782;
                6'd40: sine_sample = -16'sd13856;
                6'd41: sine_sample = -16'sd12694;
                6'd42: sine_sample = -16'sd11314;
                6'd43: sine_sample = -16'sd9740;
                6'd44: sine_sample = -16'sd8000;
                6'd45: sine_sample = -16'sd6123;
                6'd46: sine_sample = -16'sd4141;
                6'd47: sine_sample = -16'sd2088;
                default: sine_sample = 16'sd0;
            endcase
        end
    endfunction

    always @(posedge audio_clk) begin
        if (reset) begin
            mclk_div      <= 2'd0;
            BCLK_O        <= 1'b0;
            LRCLK_O       <= 1'b0;
            SDATA_O       <= 1'b0;
            frame_bit     <= 6'd0;
            sample_index  <= 6'd0;
            active_sample <= 16'sd0;
        end else begin
            if (mclk_div == 2'd1) begin
                mclk_div <= 2'd0;
                BCLK_O   <= ~BCLK_O;

                // Update data on falling edge of BCLK.
                // Codec samples data on the opposite edge.
                if (BCLK_O == 1'b1) begin
                    if (frame_bit == 6'd63) begin
                        next_bit = 6'd0;

                        if (sample_index == 6'd47)
                            next_sample_index = 6'd0;
                        else
                            next_sample_index = sample_index + 6'd1;

                        sample_index  <= next_sample_index;
                        active_sample <= sine_sample(next_sample_index);
                        frame_bit     <= 6'd0;
                    end else begin
                        next_bit = frame_bit + 6'd1;
                        frame_bit <= next_bit;
                    end

                    // LRCLK low = left channel, high = right channel
                    LRCLK_O <= (next_bit >= 6'd32);

                    // I2S-style one-bit delay:
                    // bit 0 and bit 32 are dummy/transition bits.
                    if ((next_bit >= 6'd1) && (next_bit <= 6'd16)) begin
                        SDATA_O <= active_sample[6'd16 - next_bit];
                    end else if ((next_bit >= 6'd33) && (next_bit <= 6'd48)) begin
                        SDATA_O <= active_sample[6'd48 - next_bit];
                    end else begin
                        SDATA_O <= 1'b0;
                    end
                end
            end else begin
                mclk_div <= mclk_div + 2'd1;
            end
        end
    end

endmodule
```

---

## Connect the Audio RTL Module

### Add Module to Block Design

1. Open the block design.
2. Right-click in an empty area.
3. Select:

```text
Add Module
```

4. Choose:

```text
audio_tone_i2s
```

The block should appear as:

```text
audio_tone_i2s_0
```

### Add Audio Reset Block

Add another Processor System Reset block for the 12.288 MHz audio clock domain.

Open Tcl Console and run:

```tcl
create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 rst_audio_12M
```

Connect it:

```tcl
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins rst_audio_12M/slowest_sync_clk]
connect_bd_net [get_bd_pins processing_system7_0/FCLK_RESET0_N] [get_bd_pins rst_audio_12M/ext_reset_in]
connect_bd_net [get_bd_pins clk_wiz_0/locked] [get_bd_pins rst_audio_12M/dcm_locked]
```

### Connect Audio Module

Run:

```tcl
connect_bd_net [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins audio_tone_i2s_0/audio_clk]
connect_bd_net [get_bd_pins rst_audio_12M/peripheral_reset] [get_bd_pins audio_tone_i2s_0/reset]

make_bd_pins_external -name BCLK_O  [get_bd_pins audio_tone_i2s_0/BCLK_O]
make_bd_pins_external -name LRCLK_O [get_bd_pins audio_tone_i2s_0/LRCLK_O]
make_bd_pins_external -name SDATA_O [get_bd_pins audio_tone_i2s_0/SDATA_O]
make_bd_pins_external -name SDATA_I [get_bd_pins audio_tone_i2s_0/SDATA_I]
```

After this, the design should expose:

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

## Add Audio Pin Constraints

Create an XDC file:

```text
zed_audio_pins.xdc
```

Add:

```tcl
## ZedBoard ADAU1761 Audio Codec Pins

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

## I2C codec control interface timing exception
## AXI IIC uses these as slow bidirectional open-drain control pins.
## Do not analyze board-level I2C input/output paths as synchronous FPGA timing paths.

set_false_path -from [get_ports {iic_rtl_scl_io iic_rtl_sda_io}]
set_false_path -to   [get_ports {iic_rtl_scl_io iic_rtl_sda_io}]
```

---

## Validate, Generate Wrapper, Build Bitstream

### Assign Address

Open:

```text
Address Editor
```

Click:

```text
Assign All
```

Confirm AXI IIC has a valid address. The expected address from this project is:

```text
0x41600000
```

### Validate Design

Run:

```text
Tools -> Validate Design
```

Expected:

```text
Validation successful
```

### Create HDL Wrapper

In Sources:

```text
Design Sources
Right-click zed_audio_bd.bd
Create HDL Wrapper
```

Select:

```text
Let Vivado manage wrapper and auto-update
```

Expected wrapper:

```text
zed_audio_bd_wrapper.v
```

### Generate Output Products

Right-click the block design:

```text
zed_audio_bd.bd
```

Select:

```text
Generate Output Products
```

Use default/global synthesis option.

### Run Synthesis

```text
Flow Navigator -> Synthesis -> Run Synthesis
```

Expected:

```text
Synthesis completed successfully
```

### Run Implementation

```text
Flow Navigator -> Implementation -> Run Implementation
```

Expected:

```text
Implementation completed successfully
```

### Check Timing

Open Timing Summary.

Good result:

```text
WNS >= 0
TNS = 0
WHS >= 0
THS = 0
Failed Routes = 0
```

### Check Methodology

Run:

```text
Open Implemented Design -> Report Methodology
```

Expected:

```text
Checks found: 0
```

### Generate Bitstream

```text
Flow Navigator -> Program and Debug -> Generate Bitstream
```

Expected:

```text
Bitstream generation completed successfully
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
zed_audio_bd_wrapper.xsa
```

Example path:

```text
C:\Zed_Projects\zed_audio_tone\zed_audio_bd_wrapper.xsa
```

---

## Create Vitis Platform Using Python Script

In this setup, Vitis components are created using Python scripts.

### Folder Structure

Recommended:

```text
C:\Zed_Projects
    |
    |-- zed_audio_tone
    |       |-- zed_audio_bd_wrapper.xsa
    |
    |-- zed_audio_vitis_ws
    |
    |-- vitis_scripts
            |-- create_audio_platform.py
            |-- create_audio_app.py
```

### Create Platform Script

Create:

```text
C:\Zed_Projects\vitis_scripts\create_audio_platform.py
```

Add:

```python
import os
import vitis

workspace = r"C:\Zed_Projects\zed_audio_vitis_ws"
xsa_path = r"C:\Zed_Projects\zed_audio_tone\zed_audio_bd_wrapper.xsa"
platform_name = "zed_audio_platform"

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

print("ZedBoard audio platform created and built successfully.")
print(f"Workspace: {workspace}")
print(f"Platform name: {platform_name}")
print(f"XSA used: {xsa_path}")
```

### Run Platform Script

Open Command Prompt:

```cmd
cd /d C:\Zed_Projects\vitis_scripts
C:\AMDDesignTools\2025.2\Vitis\bin\vitis.bat -s create_audio_platform.py
```

Expected output:

```text
ZedBoard audio platform created and built successfully.
Workspace: C:\Zed_Projects\zed_audio_vitis_ws
Platform name: zed_audio_platform
XSA used: C:\Zed_Projects\zed_audio_tone\zed_audio_bd_wrapper.xsa
```

---

## Create Vitis Application Using Python Script

Create:

```text
C:\Zed_Projects\vitis_scripts\create_audio_app.py
```

Add:

```python
import os
import vitis

workspace = r"C:\Zed_Projects\zed_audio_vitis_ws"

platform_path = (
    r"C:\Zed_Projects\zed_audio_vitis_ws"
    r"\zed_audio_platform\export\zed_audio_platform"
    r"\zed_audio_platform.xpfm"
)

app_name = "zed_audio_app"

if not os.path.exists(platform_path):
    raise FileNotFoundError(f"Platform XPFM not found: {platform_path}")

client = vitis.create_client()
client.set_workspace(path=workspace)

app = client.create_app_component(
    name=app_name,
    platform=platform_path,
    domain="standalone_ps7_cortexa9_0",
    template="empty_application"
)

print("ZedBoard audio application component created successfully.")
print(f"Workspace: {workspace}")
print(f"Application name: {app_name}")
print(f"Platform used: {platform_path}")
```

Run:

```cmd
cd /d C:\Zed_Projects\vitis_scripts
C:\AMDDesignTools\2025.2\Vitis\bin\vitis.bat -s create_audio_app.py
```

Expected output:

```text
ZedBoard audio application component created successfully.
```

---

## Add Bare-Metal C Application

Open the Vitis workspace:

```text
C:\Zed_Projects\zed_audio_vitis_ws
```

Open:

```text
zed_audio_app/src
```

Create:

```text
main.c
```

Paste the following code.

### `main.c`

```c
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
     * PL provides:
     *   MCLK   = 12.288 MHz
     *   BCLK   = 3.072 MHz
     *   LRCLK  = 48 kHz
     *   SDATA  = I2S-style stereo data into codec
     *
     * Codec role:
     *   I2S slave
     *   route serial input L0/R0 to DAC L/R
     *   enable DAC/playback path
     *   enable headphone and line output paths
     */

    const codec_reg_t init_seq[] = {
        {0x4000, 0x01},   // Clock control: enable core clock

        {0x401C, 0x21},   // Playback mixer left
        {0x401E, 0x41},   // Playback mixer right

        {0x4020, 0x05},   // Playback LR mixer left
        {0x4021, 0x11},   // Playback LR mixer right

        {0x4023, 0xE7},   // Headphone left output
        {0x4024, 0xE7},   // Headphone right output

        {0x4025, 0xE6},   // Line output left
        {0x4026, 0xE6},   // Line output right

        {0x4029, 0x03},   // Playback power management
        {0x402A, 0x03},   // DAC control

        {0x40F2, 0x01},   // Route serial input L0/R0 to DAC L/R
        {0x40F3, 0x01},   // Serial output route

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
    xil_printf("ZedBoard Audio Tone Generator\r\n");
    xil_printf("====================================\r\n");

    xil_printf("AXI IIC base address: 0x%08x\r\n", (unsigned int)IIC_BASEADDR);
    xil_printf("ADAU1761 I2C address: 0x%02x\r\n", ADAU1761_I2C_ADDR);

    xil_printf("\r\n");
    xil_printf("PL is generating I2S tone after FPGA programming.\r\n");
    xil_printf("Configuring ADAU1761 codec to route I2S input to analog outputs.\r\n");
    xil_printf("\r\n");

    if (adau1761_init_playback() != 0) {
        xil_printf("FAIL: Codec playback initialization failed.\r\n");
        while (1) {
            sleep(1);
        }
    }

    xil_printf("\r\n");
    xil_printf("PASS: Codec configured successfully.\r\n");
    xil_printf("You should now hear a steady tone from the ZedBoard audio output.\r\n");
    xil_printf("Test green LINE OUT first. Also test black HEADPHONE OUT.\r\n");

    while (1) {
        sleep(1);
    }

    return 0;
}
```

---

## Build and Run in Vitis

### Build

In Vitis:

1. Select:

```text
zed_audio_app
```

2. Click:

```text
Build
```

Expected:

```text
Build Finished
0 errors
```

### Prepare UART Terminal

Open a serial terminal with:

```text
Baud rate:    115200
Data bits:    8
Parity:       None
Stop bits:    1
Flow control: None
```

### Connect Audio Output

Connect earphones or speaker to:

```text
Green LINE OUT
```

Also test:

```text
Black HEADPHONE / HEADSET OUT
```

Start with low volume if using powered speakers or sensitive earphones.

### Run

In Vitis:

```text
FLOW -> Run
```

Make sure hardware run/programming uses:

```text
Target: ps7_cortexa9_0
Application: zed_audio_app.elf
Bitstream: included/program FPGA enabled
```

---

## Expected Output

UART output should look like:

```text
====================================
ZedBoard Audio Tone Generator
====================================
AXI IIC base address: 0x41600000
ADAU1761 I2C address: 0x3B

PL is generating I2S tone after FPGA programming.
Configuring ADAU1761 codec to route I2S input to analog outputs.

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
You should now hear a steady tone from the ZedBoard audio output.
Test green LINE OUT first. Also test black HEADPHONE OUT.
```

Expected physical result:

```text
A steady tone is heard from LINE OUT and HEADPHONE OUT.
```

---

## Debug Checklist

If no sound is heard, debug in this order.

### 1. Check UART

Confirm the program prints:

```text
PASS: Codec configured successfully.
```

If not, fix the software/I2C issue first.

### 2. Check AXI IIC Base Address

UART should show:

```text
AXI IIC base address: 0x41600000
```

If the value is different, check Vivado Address Editor and regenerate/export hardware.

### 3. Check Codec I2C Address

The application uses:

```text
0x3B
```

If I2C writes fail, check:

- `iic_rtl_scl_io` constraint
- `iic_rtl_sda_io` constraint
- AXI IIC is connected to PS
- bitstream is programmed
- board is powered correctly

### 4. Check FPGA Programming

Make sure Vitis programmed the FPGA bitstream before running the ELF.

The PL must be configured because it generates:

```text
MCLK
BCLK
LRCLK
SDATA
```

### 5. Check Audio Jacks

Test both:

```text
Green LINE OUT
Black HEADPHONE / HEADSET OUT
```

### 6. Check Audio Clocks with Logic Analyzer or Oscilloscope

Expected:

```text
MCLK   = 12.288 MHz
BCLK   = 3.072 MHz
LRCLK  = 48 kHz
```

### 7. Check Reset

The audio module reset must come from:

```text
rst_audio_12M/peripheral_reset
```

not from the 100 MHz reset block.

### 8. Check Codec Register Writes

Every codec register write should print:

```text
OK
```

If one fails, codec initialization did not complete.

---

## Future Improvements

After the basic tone works, possible improvements are:

- Add AXI-Lite registers to control tone frequency from C
- Add multiple tones: 500 Hz, 1 kHz, 2 kHz
- Add push-button tone selection
- Add volume control through ADAU1761 registers
- Add square, triangle, and sawtooth waveforms
- Add UART command interface to change tone live
- Add audio input loopback
- Add AXI DMA based audio streaming
- Build a small synthesizer

---

## Final Result

This project successfully generates an audible tone from the ZedBoard audio jacks using:

```text
Zynq PS bare-metal C
AXI IIC
Custom PL I2S tone generator
ADAU1761 codec
```

Confirmed outputs:

```text
LINE OUT:              working
HEADPHONE/HEADSET OUT: working
```
