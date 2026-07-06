# Nexys A7 Reverse LEDs

This example verifies FPGA-only bring-up on the **Nexys A7** board by routing 16 switch inputs to 16 LED outputs in reverse order.

The design is useful for practicing vector signals, bit indexing, switch-to-LED mapping, XDC constraints, and bitstream generation.

---

## Design Behavior

The design has:

```text
16 switch inputs  → sw[15:0]
16 LED outputs    → led[15:0]
```

The switch values are connected to LEDs in reverse order:

```verilog
assign led[15] = sw[0];
assign led[14] = sw[1];
assign led[13] = sw[2];
assign led[12] = sw[3];
assign led[11] = sw[4];
assign led[10] = sw[5];
assign led[9]  = sw[6];
assign led[8]  = sw[7];
assign led[7]  = sw[8];
assign led[6]  = sw[9];
assign led[5]  = sw[10];
assign led[4]  = sw[11];
assign led[3]  = sw[12];
assign led[2]  = sw[13];
assign led[1]  = sw[14];
assign led[0]  = sw[15];
```

So the behavior is:

```text
Switch pattern → reversed LED pattern
```

Example:

```text
If sw[0] is ON, led[15] turns ON.
If sw[15] is ON, led[0] turns ON.
```

---

## Repository Content

This example contains only the required files:

```text
src/reverse_leds.v
constraints/NexysA7.xdc
hardware/reverse_leds.bit
```

---

## Source File

```text
src/reverse_leds.v
```

This file contains the Verilog design.

The design:

* Takes 16 switch inputs.
* Drives 16 LED outputs.
* Reverses the bit order from switches to LEDs.

---

## Constraint File

```text
constraints/NexysA7.xdc
```

This file maps the Verilog ports to the physical switch and LED pins on the Nexys A7 board.

---

## Hardware File

```text
hardware/reverse_leds.bit
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
* Verified reverse switch-to-LED mapping.

---

## Result

The Reverse LEDs example was tested successfully on the Nexys A7 board.