# Zybo Switches to LEDs

This example verifies basic FPGA input-output control on the **Zybo** board by routing switch inputs directly to LED outputs using simple Verilog logic.

The project is useful as an early FPGA bring-up test to confirm that the Vivado flow, board constraints, bitstream generation, and FPGA programming are working correctly.

---

## Hardware Overview

The design targets the **Zybo** board, which is based on a **Zynq-7000** device.

In this example, only the FPGA fabric is used. The ARM Processing System is not used.

The design connects board switch inputs to board LED outputs through combinational Verilog logic.

---

## Design Behavior

The Verilog logic directly assigns the input signals to the LED outputs:

```verilog
assign led = btn;
```

Although the Verilog input port is named **btn**, the XDC constraints map those signals to the board switches.

So the actual hardware behavior is:

```text
Switches → LEDs
```

When a switch is turned ON, the corresponding LED turns ON.  
When a switch is turned OFF, the corresponding LED turns OFF.

---

## Tech Stack & Tools

### Development Environment

* **Vivado 2025.2:** Used for project creation, synthesis, implementation, bitstream generation, and FPGA programming.

### Hardware

* **Zybo**
* **Zynq-7000**
* FPGA-side switches
* FPGA-side LEDs

---

## Repository Content

This example folder contains only the required files:

* **src/top_switches_led.v:** Verilog source file for routing switch inputs to LED outputs.
* **constraints/zybo.xdc:** Zybo pin constraint file.
* **hardware/top_switches_leds.bit:** Generated bitstream used to program the FPGA.

---

## Source File

```text
src/top_switches_led.v
```

This file contains the Verilog module that connects the input signals to the LED outputs.

The logic is simple combinational routing:

```verilog
assign led = btn;
```

---

## Constraint File

```text
constraints/zybo.xdc
```

This file maps the Verilog ports to the physical Zybo board pins.

The input signals are mapped to board switches, and the output signals are mapped to board LEDs.

---

## Bitstream File

```text
hardware/top_switches_leds.bit
```

This is the generated FPGA bitstream used to program the Zybo board.

---

## What Was Tested

* Created a Vivado project for the Zybo board.
* Added Verilog source code.
* Added Zybo XDC constraints.
* Ran synthesis and implementation.
* Generated the FPGA bitstream successfully.
* Programmed the Zybo FPGA.
* Verified that switch inputs control LED outputs.

---

## Result

The switches-to-LEDs example was tested successfully on the Zybo board.