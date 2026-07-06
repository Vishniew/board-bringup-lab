# Board Bring-up Lab

This repository contains FPGA and Zynq board bring-up examples for multiple development boards.

The goal of this repository is to organize tested hardware examples, source code, constraints, generated hardware files, scripts, and documentation in a clean GitHub-friendly structure.

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

All board bring-up examples are organized, documented, committed, and ready for GitHub reference.