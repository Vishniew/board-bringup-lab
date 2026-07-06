# Nexys A7 Up/Down Counter

This example implements an FPGA-only up/down counter on the **Nexys A7** board using Verilog.

The design uses push buttons to increment, decrement, and reset a four-digit BCD value displayed on the seven-segment display.

---

## Design Behavior

The design has:

```text
clk        → 100 MHz board clock
btn_up     → increment button
btn_down   → decrement button
btn_reset  → reset button
an[7:0]    → seven-segment anode control
seg[6:0]   → seven-segment segment control
```

The counter displays values from:

```text
0000 to 9999
```

Button behavior:

```text
btn_up     → count increases by 1
btn_down   → count decreases by 1
btn_reset  → count resets to 0000
```

The design stops decrementing at `0000`.

---

## Main Logic Blocks

### 1. Button Debouncers

The design uses debounce counters to clean the physical button inputs.

```verilog
if (debounce_up_cnt == 1_000_000) begin
    btn_up_clean <= btn_up;
end
```

This prevents one physical button press from being detected as multiple presses.

---

### 2. Edge Detectors

The design creates a single pulse when a cleaned button press is detected.

```verilog
assign pulse_up = (btn_up_clean == 1) && (btn_up_last == 0);
assign pulse_down = (btn_down_clean == 1) && (btn_down_last == 0);
```

This ensures the counter changes only once per button press.

---

### 3. BCD Up/Down Counter

The counter uses four BCD digits:

```text
ones
tens
hundreds
thousands
```

Increment logic handles carry from one digit to the next.

Decrement logic handles borrowing from higher digits.

---

### 4. Seven-Segment Multiplexing

The display is refreshed using a 1 kHz tick.

```text
digit_select = 00 → ones
digit_select = 01 → tens
digit_select = 10 → hundreds
digit_select = 11 → thousands
```

Only one digit is enabled at a time, but fast switching makes the display appear stable.

---

## Repository Content

This example contains only the required files:

```text
src/up_down_counter.v
constraints/NexysA7.xdc
hardware/up_down_counter.bit
```

---

## Source File

```text
src/up_down_counter.v
```

This file contains the Verilog design.

The design:

* Cleans button inputs using debouncing.
* Detects button press edges.
* Increments the BCD counter using `btn_up`.
* Decrements the BCD counter using `btn_down`.
* Resets the counter using `btn_reset`.
* Displays the count on the seven-segment display.

---

## Constraint File

```text
constraints/NexysA7.xdc
```

This file maps the clock, buttons, anodes, and segment pins to the physical Nexys A7 board pins.

---

## Hardware File

```text
hardware/up_down_counter.bit
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
* Verified increment button behavior.
* Verified decrement button behavior.
* Verified reset button behavior.
* Verified seven-segment display output.

---

## Result

The Up/Down Counter example was tested successfully on the Nexys A7 board.