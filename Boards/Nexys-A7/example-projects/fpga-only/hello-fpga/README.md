# Nexys A7 Hello FPGA

This example verifies basic FPGA bring-up on the **Nexys A7** board using a simple combinational logic design.

The design implements an AND gate using two switch inputs and one LED output.

---

## Design Behavior

The Verilog module has:

```text
switch_a
switch_b
led_out
```

The LED output is driven by this logic:

```verilog
assign led_out = switch_a & switch_b;
```

So the behavior is:

```text
switch_a = 0, switch_b = 0 → led_out = 0
switch_a = 0, switch_b = 1 → led_out = 0
switch_a = 1, switch_b = 0 → led_out = 0
switch_a = 1, switch_b = 1 → led_out = 1
```

The LED turns ON only when both switches are ON.

---

## Repository Content

This example contains only the required files:

```text
src/hello_led.v
constraints/NexysA7.xdc
hardware/hello_led.bit
```

---

## Source File

```text
src/hello_led.v
```

This file contains the Verilog design.

The design:

* Takes two switch inputs.
* Performs AND logic.
* Drives one LED output.

---

## Constraint File

```text
constraints/NexysA7.xdc
```

This file maps the Verilog ports to physical pins on the Nexys A7 board.

---

## Hardware File

```text
hardware/hello_led.bit
```

This is the generated FPGA bitstream used to program the Nexys A7 board.

---

## Tools Used

* Vivado
* Verilog
* XDC constraints
* Nexys A7 board

---

## What Was Tested

* Created a Vivado RTL project.
* Added Verilog source.
* Added Nexys A7 constraints.
* Generated the bitstream.
* Programmed the Nexys A7 board.
* Verified switch-to-LED AND gate behavior.

---

## Result

The Hello FPGA example was tested successfully on the Nexys A7 board.