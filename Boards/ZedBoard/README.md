# ZedBoard Bring-up Guide

This folder contains ZedBoard bring-up examples developed using **Vivado** and **Vitis**.

The examples cover FPGA-only designs, ARM bare-metal applications, and ARM + FPGA integration using AXI peripherals.

---

## Board Overview

The **ZedBoard** is based on the **Zynq-7000 XC7Z020** SoC.

The Zynq device contains two major parts:

* **Processing System (PS):** ARM Cortex-A9 processor side
* **Programmable Logic (PL):** FPGA fabric side

The bring-up examples in this folder are used to verify both sides of the device and the communication between them.

---

## Tools Used

* **Vivado 2025.2:** Used for hardware design, block design creation, synthesis, implementation, bitstream generation, and XSA export.
* **Vitis 2025.2:** Used for platform generation, bare-metal application development, build, and execution.
* **ZedBoard:** Target development board.
* **Zynq-7000 XC7Z020:** Target SoC device.

---

## Bring-up Flow

The general ZedBoard bring-up flow follows four stages:

```text
Vivado hardware design
        ↓
Bitstream and XSA export
        ↓
Vitis platform and application creation
        ↓
Build, program, and test on board
```

---

## Phase 1: Hardware Design in Vivado

The goal of this phase is to create the hardware design for the Zynq device.

Depending on the example, the design may use only FPGA logic, only the ARM Processing System, or both PS and PL together.

Typical Vivado steps:

1. Create a new Vivado project.
2. Select the correct ZedBoard/Zynq-7000 target.
3. Create a block design if the Processing System or AXI peripherals are required.
4. Add the **ZYNQ7 Processing System**.
5. Run block automation for the Processing System.
6. Add required peripherals such as:
   * AXI GPIO
   * AXI Timer
   * Interrupt connections
7. Create the HDL wrapper.
8. Generate the bitstream if the FPGA fabric is used.
9. Export the hardware as an **XSA** file.
10. Include the bitstream in the exported hardware when the PL is used.

---

## Phase 2: Platform Generation in Vitis

The exported **XSA** file is used by Vitis to understand the hardware design.

The Vitis platform contains the hardware description, processor configuration, BSP, and software domain information required to build bare-metal applications.

Typical Vitis platform steps:

1. Create or select a Vitis workspace.
2. Create a platform from the exported XSA.
3. Select the standalone domain.
4. Select the ARM Cortex-A9 processor.
5. Build the platform.
6. Generate the platform file:

```text
*.xpfm
```

For reproducibility, some examples include Python scripts that automate platform and application creation.

---

## Phase 3: Bare-metal Application Development

After the platform is created, a bare-metal C application is written for the ARM processor.

Common files and headers used in the examples:

* **main.c:** Main bare-metal application source file.
* **xparameters.h:** Hardware address and device ID definitions generated from the hardware design.
* **xgpio.h:** Xilinx AXI GPIO driver.
* **xscugic.h:** Interrupt controller driver.
* **xtmrctr.h:** AXI Timer driver.

The application code depends on the example.

For example:

* LED examples write output values to GPIO.
* Switch examples read input values from GPIO.
* Interrupt examples configure the interrupt controller and handle interrupt service routines.
* Timer examples configure the AXI Timer and respond to periodic interrupts.

---

## Phase 4: Build and Execution

After the application is written, the final step is to build and run it on the ZedBoard.

Typical execution steps:

1. Build the Vitis application.
2. Connect the ZedBoard through USB/JTAG.
3. Power on the board.
4. Program the FPGA if the design uses PL logic.
5. Run the generated ELF application on the ARM processor.
6. Verify the result on the physical board.

Expected outputs may include:

* LED blinking
* Button controlling LED
* Switch values reflected on LEDs
* ARM-controlled FPGA LEDs
* Button interrupt response
* Timer interrupt response

---

## Example Categories

### FPGA-only Examples

These examples use only the FPGA fabric.

* **led-blink:** Blinks an FPGA-side LED using Verilog logic.
* **button-to-led:** Connects an FPGA-side button input to an LED output.
* **led-counter:** Drives FPGA-side LEDs using counter logic.

### ARM-only Examples

These examples use only the ARM Processing System.

* **ps-ld9-blink:** Blinks PS-side LED LD9 using a bare-metal ARM application.

### ARM + FPGA Examples

These examples use both the ARM Processing System and FPGA fabric.

* **axi-gpio-leds:** ARM controls FPGA-side LEDs through AXI GPIO.
* **axi-gpio-switch-led:** ARM reads FPGA-side switches and controls LEDs through AXI GPIO.
* **axi-gpio-button-interrupt:** FPGA-side button interrupt is handled by ARM software.
* **axi-timer-interrupt:** AXI Timer interrupt is handled by ARM software.

---

## Example Folder Convention

Each example keeps only the important project files required for understanding, rebuilding, or documenting the design.

```text
example-name/
├── README.md
├── src/
│   └── main.c or Verilog source files
├── scripts/
│   └── Vitis automation scripts
├── hardware/
│   ├── *.xsa
│   ├── *.xpfm
│   └── *.bit
└── design/
    └── design_1.bd
```

Not every example has all folders. FPGA-only examples may only contain source, constraints, and bitstream files.

---

## Files Kept in This Repository

The repository keeps important source and hardware handoff files such as:

* Verilog source files
* C source files
* XDC constraint files
* Vivado block design files
* XSA hardware export files
* XPFM Vitis platform files
* Bitstream files
* Python automation scripts
* README documentation

Generated cache, build, simulation, and temporary tool folders are not included.

---

## Status

All listed ZedBoard examples were tested successfully and added to this repository.