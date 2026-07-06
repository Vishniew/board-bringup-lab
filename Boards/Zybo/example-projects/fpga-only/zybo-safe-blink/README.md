# Zybo Safe Blink

This example verifies basic FPGA clock-driven logic on the **Zybo** board by blinking an FPGA-side LED using a Verilog counter.

The project is useful as an early FPGA bring-up test to confirm that the Vivado flow, board constraints, bitstream generation, FPGA programming, and clock-based sequential logic are working correctly.

---

## Hardware Overview

The design targets the **Zybo** board, which is based on a **Zynq-7000** device.

In this example, only the FPGA fabric is used. The ARM Processing System is not used.

The design uses the board clock as an input and divides it down using a counter to generate a visible LED blink.

---

## Design Behavior

The Verilog design contains a counter that increments on every positive clock edge:

```verilog
always @(posedge clk) begin
    counter <= counter + 1;
end
```

One bit of the counter is connected to the LED output:

```verilog
assign led = counter[26];
```

Because higher counter bits toggle slowly, the LED blinks at a visible rate.

---

## Tech Stack & Tools

### Development Environment

* **Vivado 2025.2:** Used for project creation, synthesis, implementation, bitstream generation, and FPGA programming.

### Hardware

* **Zybo**
* **Zynq-7000**
* FPGA-side LED
* Board clock input

---

## Repository Content

This example folder contains only the required files:

* **src/safe_blink.v:** Verilog source file for LED blink logic.
* **constraints/zybo.xdc:** Zybo pin constraint file.
* **hardware/safe_blink.bit:** Generated bitstream used to program the FPGA.

---

## Source File

```text
src/safe_blink.v
```

This file contains the Verilog counter logic used to blink the LED.

---

## Constraint File

```text
constraints/zybo.xdc
```

This file maps the Verilog clock and LED ports to the physical Zybo board pins.

---

## Bitstream File

```text
hardware/safe_blink.bit
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
* Verified LED blinking on the board.

---

## Result

The safe blink example was tested successfully on the Zybo board.