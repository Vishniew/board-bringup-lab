# Nexys A7 UART Transmitter

This example implements a basic FPGA-only UART transmitter on the **Nexys A7** board using Verilog.

When the send button is pressed, the FPGA transmits the message:

```text
HELLO
```

over the UART TX line.

---

## Design Behavior

The design has:

```text
clk       → 100 MHz board clock
btn_send  → button used to start transmission
tx_out    → UART transmit output
```

When the button is pressed, the top module sends this message:

```text
H E L L O \r \n
```

The UART transmitter sends each character serially at:

```text
115200 baud
```

---

## Source Files

```text
src/top.v
src/uart_tx.v
```

---

## top.v

The `top.v` file acts as the controller.

It contains:

* Message memory.
* Button debouncer.
* Character index counter.
* FSM to send each character.
* Handshake logic with the UART transmitter.

The message is stored as ASCII values:

```verilog
message[0] = 8'h48; // 'H'
message[1] = 8'h45; // 'E'
message[2] = 8'h4C; // 'L'
message[3] = 8'h4C; // 'L'
message[4] = 8'h4F; // 'O'
message[5] = 8'h0D; // '\r'
message[6] = 8'h0A; // '\n'
```

The design waits for a clean button press, then sends one character at a time.

---

## uart_tx.v

The `uart_tx.v` file contains the UART transmit engine.

It uses a baud rate generator:

```verilog
100,000,000 Hz / 115,200 baud = about 868 clock cycles per bit
```

The UART frame format is:

```text
Idle HIGH
Start bit LOW
8 data bits
Stop bit HIGH
```

The transmitter FSM has these states:

```text
IDLE
START_BIT
DATA_BITS
STOP_BIT
```

---

## Design Flow

```text
Button press
    ↓
Debouncer
    ↓
Top-level FSM
    ↓
Load next ASCII character
    ↓
UART TX FSM
    ↓
Serial data on tx_out
```

---

## Repository Content

This example contains only the required files:

```text
src/top.v
src/uart_tx.v
constraints/NexysA7.xdc
hardware/top.bit
```

---

## Constraint File

```text
constraints/NexysA7.xdc
```

This file maps the clock, send button, and UART TX output to the physical Nexys A7 board pins.

---

## Hardware File

```text
hardware/top.bit
```

This is the generated FPGA bitstream used to program the Nexys A7 board.

---

## Tools Used

* Vivado
* Verilog
* XDC constraints
* Nexys A7 board
* UART serial terminal

---

## What Was Tested

* Created a Vivado RTL project.
* Added UART transmitter Verilog source files.
* Added Nexys A7 constraints.
* Generated the bitstream.
* Programmed the Nexys A7 board.
* Opened a serial terminal at 115200 baud.
* Pressed the send button.
* Verified that `HELLO` is transmitted over UART.

---

## Result

The UART transmitter example was tested successfully on the Nexys A7 board.