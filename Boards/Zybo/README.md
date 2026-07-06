# Zybo Bring-up Guide

This folder contains bring-up examples for the **Zybo** board.

The examples are organized from basic FPGA-only designs to ARM-only software examples and ARM + FPGA hardware/software integration examples.

---

## Board Overview

The Zybo board is based on a **Zynq-7000** device.

It contains two major parts:

* **Processing System (PS):** ARM Cortex-A9 processor system.
* **Programmable Logic (PL):** FPGA fabric used for custom digital logic.

This repository uses the Zybo board to practice:

* Pure FPGA design using Verilog.
* Constraint mapping using XDC files.
* Bitstream generation in Vivado.
* Bare-metal ARM application development in Vitis.
* PS GPIO/MIO control.
* AXI GPIO communication between ARM and FPGA fabric.
* Hardware handoff using XSA files.
* Vitis platform generation using XPFM files.

---

## Folder Structure

```text
Zybo/
├── README.md
├── docs/
└── example-projects/
    ├── fpga-only/
    ├── arm-only/
    └── arm-fpga/
```

---

## Example Categories

### 1. FPGA-only Examples

These examples use only the FPGA fabric.  
The ARM processor is not involved.

Location:

```text
example-projects/fpga-only/
```

Current examples:

```text
switches-to-leds/
zybo-safe-blink/
```

---

### 2. ARM-only Examples

These examples run on the ARM processor and use PS-side peripherals.

Location:

```text
example-projects/arm-only/
```

Current example:

```text
zybo-mio-blink/
```

---

### 3. ARM + FPGA Examples

These examples use both the ARM processor and FPGA fabric.

The ARM processor communicates with FPGA-side IP blocks such as AXI GPIO.

Location:

```text
example-projects/arm-fpga/
```

Current examples:

```text
axi-gpio-led-blink/
zybo-switches-leds/
```

---

## Completed Examples

## FPGA-only

### switches-to-leds

Location:

```text
example-projects/fpga-only/switches-to-leds/
```

Description:

This example connects Zybo switch inputs directly to LED outputs using Verilog logic.

Main files:

```text
src/top_switches_led.v
constraints/zybo.xdc
hardware/top_switches_leds.bit
```

Tested result:

```text
Switch input changes are reflected on LEDs.
```

---

### zybo-safe-blink

Location:

```text
example-projects/fpga-only/zybo-safe-blink/
```

Description:

This example blinks an LED using a counter inside FPGA fabric.

Main files:

```text
src/safe_blink.v
constraints/zybo.xdc
hardware/safe_blink.bit
```

Tested result:

```text
LED blinks using FPGA counter logic.
```

---

## ARM-only

### zybo-mio-blink

Location:

```text
example-projects/arm-only/zybo-mio-blink/
```

Description:

This example uses a bare-metal ARM application to control a PS/MIO pin using the Xilinx PS GPIO driver.

Main files:

```text
src/main.c
scripts/build_component.py
scripts/create_app.py
hardware/design_1_wrapper.xsa
hardware/zybo_base_platform.xpfm
```

Tested result:

```text
ARM software toggles a MIO pin using PS GPIO.
```

---

## ARM + FPGA

### axi-gpio-led-blink

Location:

```text
example-projects/arm-fpga/axi-gpio-led-blink/
```

Description:

This example uses ARM software to blink an FPGA-side LED through an AXI GPIO peripheral.

Main files:

```text
src/main.c
scripts/build_platform.py
scripts/create_app.py
design/design_1.bd
hardware/design_1_wrapper.xsa
hardware/design_1_wrapper.bit
hardware/zybo_platform.xpfm
```

Tested result:

```text
ARM software controls an FPGA-side LED through AXI GPIO.
```

---

### zybo-switches-leds

Location:

```text
example-projects/arm-fpga/zybo-switches-leds/
```

Description:

This example reads switch inputs through AXI GPIO and writes the same values to LEDs through AXI GPIO.

Main files:

```text
src/main.c
scripts/build_platform.py
scripts/create_app.py
design/design_1.bd
hardware/design_1_wrapper.xsa
hardware/design_1_wrapper.bit
hardware/switches_leds_platform.xpfm
```

Tested result:

```text
Switch states are read by ARM software and written back to LEDs through AXI GPIO.
```

---

## File Types Used

### Verilog Source Files

```text
*.v
```

Used for FPGA-only RTL logic.

---

### Constraint Files

```text
*.xdc
```

Used to map Verilog ports to physical Zybo board pins.

---

### Bitstream Files

```text
*.bit
```

Used to program the FPGA fabric.

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

Used by Vitis to build bare-metal software applications for the exported hardware design.

---

### Block Design Files

```text
*.bd
```

Used to preserve Vivado block designs for Zynq PS and AXI GPIO based projects.

---

## Tools Used

* Vivado
* Vitis
* Verilog
* Xilinx bare-metal drivers
* AXI GPIO
* PS GPIO
* Git
* GitHub

---

## Bring-up Progress

| Category | Status |
|---|---|
| FPGA-only examples | Completed |
| ARM-only examples | Completed |
| ARM + FPGA examples | Completed |
| Individual example READMEs | Completed |
| Zybo folder README | Completed |

---

## Final Status

The Zybo board bring-up examples are organized, documented, committed, and ready for GitHub reference.