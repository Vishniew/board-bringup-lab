# ZedBoard LED Counter

This example verifies counter-based FPGA logic on the **ZedBoard** by driving FPGA-side LEDs using a Verilog counter.

The project is useful for testing clock-based sequential logic, LED output control, Vivado synthesis, implementation, bitstream generation, and FPGA programming.

---

## Hardware Overview

The design targets the **ZedBoard**, which is based on the **Zynq-7000 XC7Z020** device.

In this example, only the FPGA fabric is used. The ARM processing system is not used.

---

## Tech Stack & Tools

### Development Environment

* **Vivado 2025.2:** Used for project creation, synthesis, implementation, bitstream generation, and FPGA programming.

### Hardware

* **ZedBoard**
* **Zynq-7000 XC7Z020**
* FPGA-side LEDs

---

## Repository Content

This example folder contains only the required files:

* **src/led_counter.v:** Verilog source file for LED counter logic.
* **constraints/zedboard.xdc:** ZedBoard pin constraint file.
* **hardware/led_counter.bit:** Generated bitstream used to program the FPGA.

---

## What Was Tested

* Created a Vivado project for the ZedBoard.
* Added Verilog source and XDC constraints.
* Generated the FPGA bitstream successfully.
* Programmed the ZedBoard FPGA.
* Verified LED counter output on the board.

---

## Result

The LED counter example was tested successfully on the ZedBoard.