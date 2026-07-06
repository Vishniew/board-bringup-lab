# Nexys A7 Timer 1

This example implements a simple FPGA-only timer on the **Nexys A7** board using Verilog.

The design counts once per second and displays the count on the board's seven-segment display.

---

## Design Behavior

The design uses the Nexys A7 clock input and generates two internal timing signals:

```text
1 Hz tick   → used to increment the timer once per second
1 kHz tick  → used to multiplex the seven-segment display digits
```

The timer counts from:

```text
0000 to 9999
```

A reset button clears the counter back to:

```text
0000
```

---

## Main Logic Blocks

### 1. 1 Hz Clock Divider

The design uses a counter to generate a 1 Hz tick from the board clock.

```verilog
if (count_1Hz == 99_999_999) begin
    count_1Hz <= 0;
    tick_1Hz <= 1;
end
```

This tick is used to increment the timer once per second.

---

### 2. BCD Counter

The timer uses four BCD digits:

```text
ones
tens
hundreds
thousands
```

The counter rolls over properly from 9 to 0 and carries into the next digit.

---

### 3. Seven-Segment Multiplexing

The design rapidly switches between display digits using a 1 kHz tick.

```text
digit_select = 00 → ones
digit_select = 01 → tens
digit_select = 10 → hundreds
digit_select = 11 → thousands
```

Only one digit is active at a time, but fast switching creates the visual effect that all digits are ON together.

---

### 4. Seven-Segment Encoding

Each BCD digit is converted into seven-segment pattern values.

Example:

```verilog
4'd0: seg = 7'b1000000;
4'd1: seg = 7'b1111001;
```

---

## Repository Content

This example contains only the required files:

```text
src/Timer_1.v
constraints/NexysA7.xdc
hardware/timer.bit
```

---

## Source File

```text
src/Timer_1.v
```

This file contains the Verilog timer design.

The design:

* Generates a 1 Hz timer tick.
* Generates a 1 kHz display refresh tick.
* Counts from 0000 to 9999.
* Drives the seven-segment display.
* Supports reset using a button input.

---

## Constraint File

```text
constraints/NexysA7.xdc
```

This file maps the clock, reset button, anode pins, and segment pins to the physical Nexys A7 board pins.

---

## Hardware File

```text
hardware/timer.bit
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
* Added Verilog timer source.
* Added Nexys A7 constraints.
* Generated the bitstream.
* Programmed the Nexys A7 board.
* Verified timer counting on the seven-segment display.
* Verified reset button behavior.

---

## Result

The Timer 1 example was tested successfully on the Nexys A7 board.