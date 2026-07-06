# Board Bring-up Lab

This repository contains FPGA and Zynq board bring-up examples for multiple development boards.

The goal of this repository is to organize tested hardware examples, source code, constraints, generated hardware files, scripts, setup notes, and documentation in a clean GitHub-friendly structure.

---

## Boards Covered

```text
ZedBoard
Zybo
Nexys A7
```

---

## Repository Structure

```text
board-bringup-lab/
├── README.md
├── .gitignore
└── Boards/
    ├── ZedBoard/
    ├── Zybo/
    └── Nexys-A7/
```

Each board folder contains:

```text
README.md
docs/
example-projects/
```

---

## Vivado Setup Reference

This repository uses **Vivado** for FPGA hardware design and **Vitis** for Zynq bare-metal software development.

Vivado is required for:

* Creating RTL projects.
* Adding Verilog/VHDL source files.
* Adding XDC constraint files.
* Running synthesis and implementation.
* Generating FPGA bitstreams.
* Creating Zynq block designs.
* Exporting XSA hardware handoff files.
* Programming FPGA boards through Hardware Manager.

Vitis is required for:

* Creating bare-metal ARM applications.
* Creating hardware platforms from XSA files.
* Building software applications for Zynq boards.
* Running ARM-side examples on ZedBoard and Zybo.

---

## Vivado Installation Notes

Use the **AMD Unified Installer for FPGAs & Adaptive SoCs**.

During installation, select:

```text
Vivado ML Standard
```

This is suitable for the boards used in this repository.

Recommended device support:

```text
Artix-7   → required for Nexys A7
Zynq-7000 → required for ZedBoard and Zybo
```

Avoid installing unnecessary device families unless needed:

```text
UltraScale
Versal
Enterprise device families
```

This keeps the installation smaller and cleaner.

---

## Required Board Support

### Nexys A7

The Nexys A7 uses an **Artix-7 FPGA**.

Required Vivado device support:

```text
Artix-7
```

---

### ZedBoard

The ZedBoard uses a **Zynq-7000 SoC**.

Required Vivado device support:

```text
Zynq-7000
```

---

### Zybo

The Zybo also uses a **Zynq-7000 SoC**.

Required Vivado device support:

```text
Zynq-7000
```

For Zybo, install the original Digilent board file:

```text
Zybo
```

Do not select the wrong board revision unless that is the exact hardware being used:

```text
Zybo Z7-10
Zybo Z7-20
```

---

## Installing Board Files

Board files help Vivado understand the physical board layout.

They provide information such as:

* Board name.
* FPGA/Zynq part number.
* DDR configuration.
* Clock routing.
* Peripheral connections.
* Board-level pin mappings.

To install board files:

```text
Vivado → Tools → Vivado Store → Boards
```

Then:

1. Refresh the board catalog.
2. Search for the required board.
3. Right-click the board.
4. Select **Install**.

Install board files for:

```text
ZedBoard
Zybo
Nexys A7
```

depending on the board being used.

---

## Creating a Vivado RTL Project

Recommended flow:

1. Open Vivado.
2. Click **Create Project**.
3. Choose a project name.
4. Choose a project location.
5. Select **RTL Project**.
6. Enable:

```text
Do not specify sources at this time
```

7. Select the correct board from the **Boards** tab.
8. Finish project creation.

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

## Adding Source Files

Source files describe the hardware logic.

Typical source file types:

```text
*.v
*.sv
*.vhd
```

For this repository, most FPGA examples use:

```text
*.v
```

Example Verilog module:

```verilog
module top_module (
    input wire clk,
    output wire led
);

    // Logic goes here

endmodule
```

---

## Adding Constraint Files

Constraint files map logical ports to physical board pins.

Typical constraint file:

```text
*.xdc
```

Example XDC format:

```tcl
set_property PACKAGE_PIN <pin_name> [get_ports <port_name>]
set_property IOSTANDARD LVCMOS33 [get_ports <port_name>]
```

The Verilog port name and XDC port name must match.

Example:

```verilog
output wire led
```

should match:

```tcl
[get_ports led]
```

---

## Generating a Bitstream

After adding source and constraint files, run:

```text
Generate Bitstream
```

Vivado performs:

```text
Synthesis
Implementation
Bitstream Generation
```

Meaning:

```text
Synthesis       → converts Verilog into logic gates
Implementation → places and routes logic into FPGA resources
Bitstream       → creates the .bit file used to program the FPGA
```

The final output is:

```text
*.bit
```

---

## Programming the Board

After bitstream generation:

1. Connect the board to the computer.
2. Power on the board.
3. Open Vivado Hardware Manager.
4. Select:

```text
Open Target → Auto Connect
```

5. Select the generated `.bit` file.
6. Program the device.
7. Verify the hardware behavior on the board.

---

## Example Project Categories

The examples are organized into categories based on hardware/software usage.

### FPGA-only

These examples use only FPGA fabric.

Typical files:

```text
src/
constraints/
hardware/
```

Used for:

* Verilog RTL design
* XDC pin constraints
* Bitstream generation
* Direct FPGA programming

---

### ARM-only

These examples run on the ARM Processing System of Zynq boards.

Typical files:

```text
src/
scripts/
hardware/
```

Used for:

* Bare-metal ARM applications
* PS GPIO / MIO control
* Vitis platform generation
* XSA and XPFM based software flow

---

### ARM + FPGA

These examples use both the ARM processor and FPGA fabric.

Typical files:

```text
src/
scripts/
design/
hardware/
```

Used for:

* Zynq PS + PL integration
* AXI GPIO
* AXI Timer
* Interrupts
* Hardware/software communication
* Vivado block designs
* Vitis bare-metal applications

---

## Board Summary

## ZedBoard

Location:

```text
Boards/ZedBoard/
```

The ZedBoard section contains FPGA-only, ARM-only, and ARM + FPGA examples.

### ZedBoard Examples

#### FPGA-only

```text
button-to-led
led-blink
led-counter
```

#### ARM-only

```text
ps-ld9-blink
```

#### ARM + FPGA

```text
axi-gpio-button-interrupt
axi-gpio-leds
axi-gpio-switch-led
axi-timer-interrupt
```

Status:

```text
Completed
```

---

## Zybo

Location:

```text
Boards/Zybo/
```

The Zybo section contains FPGA-only, ARM-only, and ARM + FPGA examples.

### Zybo Examples

#### FPGA-only

```text
switches-to-leds
zybo-safe-blink
```

#### ARM-only

```text
zybo-mio-blink
```

#### ARM + FPGA

```text
axi-gpio-led-blink
zybo-switches-leds
```

Status:

```text
Completed
```

---

## Nexys A7

Location:

```text
Boards/Nexys-A7/
```

The Nexys A7 is an FPGA-only board, so all examples are placed under:

```text
example-projects/fpga-only/
```

### Nexys A7 Examples

#### FPGA-only

```text
hello-fpga
reverse-leds
timer-1
uart
up-down-counter
```

Status:

```text
Completed
```

---

## Tools Used

```text
Vivado
Vitis
Verilog
C
XDC constraints
AXI GPIO
AXI Timer
PS GPIO
Git
GitHub
```

---

## File Types

### Verilog Source Files

```text
*.v
```

Used for FPGA RTL designs.

---

### C Source Files

```text
*.c
```

Used for bare-metal ARM applications.

---

### Constraint Files

```text
*.xdc
```

Used to map logical ports to physical board pins.

---

### Bitstream Files

```text
*.bit
```

Used to program FPGA fabric.

---

### Hardware Handoff Files

```text
*.xsa
```

Used to export Vivado hardware designs to Vitis.

---

### Vitis Platform Files

```text
*.xpfm
```

Used by Vitis to build applications for exported hardware platforms.

---

### Block Design Files

```text
*.bd
```

Used to preserve Vivado block designs for Zynq PS + PL projects.

---

### Automation Scripts

```text
*.py
```

Used for Vitis platform and application generation.

---

## Common Mistakes to Avoid

### 1. Installing the Wrong Device Support

Nexys A7 requires:

```text
Artix-7
```

ZedBoard and Zybo require:

```text
Zynq-7000
```

---

### 2. Selecting the Wrong Board File

Make sure the selected board file matches the physical board.

For Zybo, select:

```text
Zybo
```

Do not accidentally select:

```text
Zybo Z7-10
Zybo Z7-20
```

unless that is the exact board revision being used.

---

### 3. Using Spaces in Project Paths

Avoid:

```text
C:\FPGA Projects\My Project
```

Use:

```text
C:\FPGA_Projects\My_Project
```

---

### 4. Mismatching Verilog and XDC Port Names

If the Verilog port is:

```verilog
output wire led
```

the XDC must use:

```tcl
[get_ports led]
```

---

### 5. Committing Generated Vivado Cache Files

Do not commit unnecessary generated folders such as:

```text
.Xil/
.cache/
.runs/
.sim/
.hw/
.gen/
.ip_user_files/
```

Only preserve useful files such as:

```text
src/
constraints/
design/
hardware/
scripts/
README.md
```

---

## What This Repository Demonstrates

This repository demonstrates practical board bring-up work, including:

* Creating Vivado RTL projects.
* Writing Verilog examples.
* Mapping board pins using XDC constraints.
* Generating FPGA bitstreams.
* Creating Zynq block designs.
* Exporting XSA hardware handoff files.
* Creating Vitis platforms.
* Writing bare-metal ARM C applications.
* Testing PS GPIO and MIO pins.
* Testing AXI GPIO communication.
* Testing AXI Timer interrupts.
* Organizing board projects for GitHub.

---

## Final Progress

| Board | FPGA-only | ARM-only | ARM + FPGA | Status |
|---|---:|---:|---:|---|
| ZedBoard | 3 | 1 | 4 | Completed |
| Zybo | 2 | 1 | 2 | Completed |
| Nexys A7 | 5 | 0 | 0 | Completed |

---

## Final Status

All board bring-up examples and Vivado setup notes are organized, documented, committed, and ready for GitHub reference.