# Nexys A7 Bring-up Guide

This folder contains bring-up examples for the **Nexys A7** FPGA development board.

The Nexys A7 is an FPGA-only board, so all examples are organized under:

```text
example-projects/fpga-only/
```

There are no ARM-only or ARM + FPGA examples for this board.

---

## Board Overview

The Nexys A7 board is based on a **Xilinx Artix-7 FPGA**.

This repository uses the Nexys A7 board to practice:

* Pure FPGA RTL design using Verilog.
* Switch, LED, button, clock, UART, and seven-segment display usage.
* XDC constraint mapping.
* Vivado synthesis and implementation.
* Bitstream generation.
* FPGA programming and hardware verification.

---

## Folder Structure

```text
Nexys-A7/
├── README.md
├── docs/
└── example-projects/
    └── fpga-only/
```

---

## Example Categories

### FPGA-only Examples

These examples use only FPGA fabric.

Location:

```text
example-projects/fpga-only/
```

Current examples:

```text
hello-fpga/
reverse-leds/
timer-1/
uart/
up-down-counter/
```

---

## Completed Examples

## FPGA-only

### hello-fpga

Location:

```text
example-projects/fpga-only/hello-fpga/
```

Description:

This example implements a simple AND gate using two switch inputs and one LED output.

Main files:

```text
src/hello_led.v
constraints/NexysA7.xdc
hardware/hello_led.bit
```

Tested result:

```text
LED turns ON only when both switches are ON.
```

---

### reverse-leds

Location:

```text
example-projects/fpga-only/reverse-leds/
```

Description:

This example routes 16 switch inputs to 16 LED outputs in reverse order.

Main files:

```text
src/reverse_leds.v
constraints/NexysA7.xdc
hardware/reverse_leds.bit
```

Tested result:

```text
Switch pattern is displayed on LEDs in reverse order.
```

---

### timer-1

Location:

```text
example-projects/fpga-only/timer-1/
```

Description:

This example implements a timer using the Nexys A7 clock and displays the count on the seven-segment display.

Main files:

```text
src/Timer_1.v
constraints/NexysA7.xdc
hardware/timer.bit
```

Tested result:

```text
Timer counts on the seven-segment display and resets using a button.
```

---

### uart

Location:

```text
example-projects/fpga-only/uart/
```

Description:

This example implements a UART transmitter. When a button is pressed, the FPGA sends `HELLO` over UART.

Main files:

```text
src/top.v
src/uart_tx.v
constraints/NexysA7.xdc
hardware/top.bit
```

Tested result:

```text
HELLO is transmitted through UART at 115200 baud.
```

---

### up-down-counter

Location:

```text
example-projects/fpga-only/up-down-counter/
```

Description:

This example implements a button-controlled up/down BCD counter on the seven-segment display.

Main files:

```text
src/up_down_counter.v
constraints/NexysA7.xdc
hardware/up_down_counter.bit
```

Tested result:

```text
Buttons increment, decrement, and reset the displayed counter value.
```

---

## File Types Used

### Verilog Source Files

```text
*.v
```

Used for RTL logic design.

---

### Constraint Files

```text
*.xdc
```

Used to map Verilog ports to physical Nexys A7 board pins.

---

### Bitstream Files

```text
*.bit
```

Used to program the FPGA.

---

## Tools Used

* Vivado
* Verilog
* XDC constraints
* Git
* GitHub
* Nexys A7 board

---

## Bring-up Progress

| Category | Status |
|---|---|
| FPGA-only examples | Completed |
| Individual example READMEs | Completed |
| Nexys A7 folder README | Completed |

---

## Final Status

The Nexys A7 FPGA-only examples are organized, documented, committed, and ready for GitHub reference.