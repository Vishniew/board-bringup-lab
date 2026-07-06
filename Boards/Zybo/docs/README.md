# Vivado and Zybo Setup Guide

This document combines the general **Vivado Design Suite setup flow** with the **Zybo-specific Zynq-7000 board setup flow**.

It is intended as a beginner-friendly setup reference for creating, configuring, and programming Vivado projects for the **Digilent Zybo** board.

---

## Goal

The goal of this guide is to set up a clean FPGA/Zynq development environment for the Zybo board.

By the end of this guide, the system should be ready for:

* Installing Vivado.
* Selecting the correct device support.
* Installing Digilent board files.
* Creating a new Vivado RTL project.
* Targeting the Zybo board.
* Adding Verilog source files.
* Adding XDC constraint files.
* Generating a bitstream.
* Programming the Zybo board.

---

## Board and Architecture

The Zybo board is based on the **Zynq-7000 SoC**.

The Zynq-7000 device contains two major sections:

```text
Processing System  → ARM Cortex-A9 processor system
Programmable Logic → FPGA fabric
```

In short:

```text
PS → ARM side
PL → FPGA side
```

This is different from pure FPGA boards such as the Nexys A7.

For Zybo projects, Vivado must support:

```text
Zynq-7000 SoC devices
Digilent Zybo board files
```

Without these, Vivado may not correctly recognize the Zybo hardware target.

---

## Phase 1 — Download Vivado

Vivado is the main FPGA design tool used for Xilinx/AMD FPGA and Zynq devices.

### Steps

1. Create an AMD/Xilinx account.
2. Go to the AMD/Xilinx Vivado downloads page.
3. Download the **AMD Unified Installer for FPGAs & Adaptive SoCs**.
4. Prefer the web installer because it downloads only the selected tools and devices.
5. Choose a stable Vivado version suitable for your projects.

---

## Phase 2 — Install Vivado

Run the downloaded installer.

During installation:

1. Sign in using the AMD/Xilinx account.
2. Select **Vivado** as the product.
3. Select **Vivado ML Standard** edition.
4. Select only the required device families.

For this repository, select:

```text
Artix-7   → required for Nexys A7
Zynq-7000 → required for Zybo and ZedBoard
```

Avoid selecting unnecessary families such as:

```text
UltraScale
Versal
Enterprise device families
```

unless they are required.

This keeps the installation smaller and cleaner.

---

## Phase 3 — Add Zynq-7000 Device Support

If Vivado was already installed without Zynq-7000 support, add it later.

### Steps

1. Close Vivado completely.
2. Open the Windows Start Menu.
3. Search for:

```text
Add Design Tools or Devices
```

4. Launch the installer/update tool.
5. Continue until the device selection screen appears.
6. Expand:

```text
Devices
└── SoCs
```

7. Select:

```text
Zynq-7000
```

8. Continue and complete the installation.

This step is required for Zybo because Zybo uses a Zynq-7000 device.

---

## Phase 4 — Install Digilent Board Files

Board files allow Vivado to understand the physical board layout.

They help Vivado know details such as:

* Board name.
* FPGA/Zynq part number.
* DDR configuration.
* Clock routing.
* Peripheral connections.
* Board-level pin mappings.

### Steps

1. Launch Vivado.
2. Go to:

```text
Tools → Vivado Store
```

3. Open the **Boards** tab.
4. Click the refresh icon to update the catalog.
5. Search for:

```text
Zybo
```

6. Select the original Digilent Zybo board.

Important:

```text
Select: Zybo
Do not select: Zybo Z7-10 or Zybo Z7-20
```

unless you are using those newer board revisions.

7. Right-click the board and select **Install**.
8. Close the Vivado Store after installation.

---

## Phase 5 — Create a New Vivado Project

After installing the required device support and board files, create a new Vivado project.

### Steps

1. Open Vivado.
2. Click:

```text
Create Project
```

3. Click **Next**.
4. Enter the project name.
5. Choose the project location.

Important rule:

```text
Do not use spaces in the project path.
```

Good example:

```text
C:\FPGA_Projects\Zybo_Switches_Leds
```

Bad example:

```text
C:\FPGA Projects\Zybo Switches Leds
```

Vivado and Tcl-based tools can fail when paths contain spaces.

---

## Phase 6 — Select Project Type

Select:

```text
RTL Project
```

Then enable:

```text
Do not specify sources at this time
```

This creates a clean empty project.

Sources and constraints can be added later in a controlled way.

---

## Phase 7 — Target the Zybo Board

On the **Default Part** screen:

1. Switch from the **Parts** tab to the **Boards** tab.
2. Search for:

```text
Zybo
```

3. Select the installed Digilent Zybo board.
4. Click **Next**.
5. Review the project summary.
6. Click **Finish**.

Vivado will now create a project targeting the Zybo board.

---

## Phase 8 — Add Verilog Source Files

A Verilog source file describes the digital logic.

### Steps

1. In the Flow Navigator, click:

```text
Add Sources
```

2. Select:

```text
Add or create design sources
```

3. Click **Create File**.
4. Choose file type:

```text
Verilog
```

5. Give the file a meaningful name.

Example:

```text
top_switches_led.v
safe_blink.v
```

6. Click **Finish**.
7. Skip or confirm the port definition popup if needed.
8. Open the file from the Sources pane.
9. Write the Verilog module.

Example structure:

```verilog
module top_module (
    input wire clk,
    output wire led
);

    // Logic goes here

endmodule
```

---

## Phase 9 — Add XDC Constraint Files

An XDC file maps Verilog ports to physical pins on the board.

Without constraints, Vivado does not know which FPGA pin should connect to which board component.

### Steps

1. Click:

```text
Add Sources
```

2. Select:

```text
Add or create constraints
```

3. Click **Create File**.
4. Choose file type:

```text
XDC
```

5. Give the file a meaningful name.

Example:

```text
zybo.xdc
```

6. Open the file from the Constraints folder.
7. Add pin mapping commands.

Example format:

```tcl
set_property PACKAGE_PIN <pin_name> [get_ports <port_name>]
set_property IOSTANDARD LVCMOS33 [get_ports <port_name>]
```

The XDC port names must match the Verilog port names.

Example:

```verilog
output wire led
```

should match:

```tcl
[get_ports led]
```

---

## Phase 10 — Generate Bitstream

After adding source and constraint files, generate the FPGA bitstream.

### Steps

1. In the Flow Navigator, click:

```text
Generate Bitstream
```

2. Vivado will run:

```text
Synthesis
Implementation
Bitstream Generation
```

Meaning:

```text
Synthesis      → converts Verilog into logic gates
Implementation → places and routes logic into FPGA resources
Bitstream      → creates the .bit file used to program the FPGA
```

If the process completes successfully, Vivado creates a `.bit` file.

---

## Phase 11 — Program the Zybo Board

After generating the bitstream:

1. Connect the Zybo board to the computer.
2. Power on the board.
3. Open:

```text
Hardware Manager
```

4. Click:

```text
Open Target → Auto Connect
```

5. Select the generated `.bit` file.
6. Program the device.

After programming, verify the hardware behavior on the board.

---

## Phase 12 — Recommended Project Organization

For this repository, useful files are stored in a clean structure.

### FPGA-only Example Structure

```text
example-name/
├── README.md
├── src/
│   └── design.v
├── constraints/
│   └── zybo.xdc
└── hardware/
    └── design.bit
```

### ARM-only Example Structure

```text
example-name/
├── README.md
├── src/
│   └── main.c
├── scripts/
│   └── create_app.py
└── hardware/
    ├── design.xsa
    └── platform.xpfm
```

### ARM + FPGA Example Structure

```text
example-name/
├── README.md
├── src/
│   └── main.c
├── scripts/
│   └── create_app.py
├── design/
│   └── design_1.bd
└── hardware/
    ├── design.bit
    ├── design.xsa
    └── platform.xpfm
```

---

## Important File Types

### Verilog Source

```text
*.v
```

Used to describe FPGA logic.

---

### Constraint File

```text
*.xdc
```

Used to map Verilog ports to physical board pins.

---

### Bitstream File

```text
*.bit
```

Used to program FPGA fabric.

---

### Hardware Handoff File

```text
*.xsa
```

Used to export Vivado hardware designs to Vitis.

---

### Vitis Platform File

```text
*.xpfm
```

Used by Vitis to build software applications for a hardware platform.

---

### Block Design File

```text
*.bd
```

Used to preserve Vivado block designs, especially for Zynq PS + PL projects.

---

## Common Mistakes to Avoid

### 1. Installing Only Artix-7 Support

If only Artix-7 support is installed, Nexys A7 projects may work, but Zybo projects will not work correctly.

For Zybo, install:

```text
Zynq-7000
```

---

### 2. Selecting the Wrong Zybo Board

Use:

```text
Zybo
```

Do not accidentally select:

```text
Zybo Z7-10
Zybo Z7-20
```

unless that is the exact board being used.

---

### 3. Using Spaces in Project Paths

Avoid paths like:

```text
C:\Users\User Name\FPGA Projects\My Project
```

Use paths like:

```text
C:\FPGA_Projects\My_Project
```

---

### 4. Mismatched Verilog and XDC Port Names

If Verilog has:

```verilog
output wire led
```

then XDC must use:

```tcl
[get_ports led]
```

A mismatch causes implementation errors.

---

### 5. Copying Generated Cache Files into GitHub

Do not commit unnecessary Vivado-generated folders such as:

```text
.Xil/
.cache/
.runs/
.sim/
.hw/
.gen/
.ip_user_files/
```

Only preserve useful source, constraint, design, hardware handoff, bitstream, and script files.

---

## Final Setup Checklist

| Step | Status |
|---|---|
| AMD/Xilinx account created | Required |
| Vivado installed | Required |
| Vivado ML Standard selected | Required |
| Zynq-7000 support installed | Required for Zybo |
| Digilent Zybo board files installed | Required for Zybo |
| Project path has no spaces | Required |
| RTL project created | Required |
| Zybo board selected | Required |
| Verilog source added | Required |
| XDC constraints added | Required |
| Bitstream generated | Required |
| Hardware Manager connected | Required |
| Zybo programmed successfully | Final verification |

---

## Summary

The complete Zybo setup flow is:

```text
Install Vivado
    ↓
Install Zynq-7000 support
    ↓
Install Digilent Zybo board files
    ↓
Create RTL project
    ↓
Select Zybo board
    ↓
Add Verilog source
    ↓
Add XDC constraints
    ↓
Generate bitstream
    ↓
Program Zybo board
    ↓
Verify hardware behavior
```

This setup prepares the environment for both simple FPGA-only Zybo examples and more advanced Zynq PS + PL examples.