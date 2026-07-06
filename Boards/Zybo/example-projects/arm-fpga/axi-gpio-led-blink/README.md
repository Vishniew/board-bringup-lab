# Zybo AXI GPIO LED Blink

This example verifies ARM + FPGA communication on the **Zybo** board by blinking an FPGA-side LED from the ARM processor through an **AXI GPIO** peripheral.

The project is useful for testing PS-PL integration, AXI GPIO output control, Vitis platform generation, bare-metal application execution, bitstream programming, and basic ARM-to-FPGA communication.

---

## Hardware Overview

The design targets the **Zybo** board, which is based on a **Zynq-7000** device.

In this example, the ARM Processing System communicates with FPGA fabric through AXI GPIO.

The LED is not controlled directly by pure Verilog logic. Instead, software running on the ARM processor writes values to the AXI GPIO peripheral, and the AXI GPIO drives the FPGA-side LED.

---

## Design Behavior

The ARM application initializes the AXI GPIO peripheral and configures the GPIO channel as output.

The software repeatedly writes:

```c
XGpio_DiscreteWrite(&led_device, 1, 1);
```

to turn the LED ON, then writes:

```c
XGpio_DiscreteWrite(&led_device, 1, 0);
```

to turn the LED OFF.

A delay is added between ON and OFF states using:

```c
usleep(500000);
```

So the actual hardware behavior is:

```text
ARM software → AXI GPIO → FPGA-side LED blink
```

---

## Tech Stack & Tools

### Development Environment

* **Vivado 2025.2:** Used to create the Zynq PS + AXI GPIO hardware design, generate the bitstream, and export the XSA.
* **Vitis 2025.2:** Used to create the platform and bare-metal application.

### Hardware

* **Zybo**
* **Zynq-7000**
* ARM Cortex-A9 processor
* AXI GPIO IP
* FPGA-side LED

---

## Repository Content

This example folder contains only the required files:

* **src/main.c:** Bare-metal C application for blinking an FPGA-side LED through AXI GPIO.
* **scripts/build_platform.py:** Vitis script to create/build the platform.
* **scripts/create_app.py:** Vitis script to create the application.
* **hardware/design_1_wrapper.xsa:** Exported Vivado hardware design.
* **hardware/zybo_platform.xpfm:** Generated Vitis platform file.
* **hardware/design_1_wrapper.bit:** Generated FPGA bitstream.
* **design/design_1.bd:** Vivado block design file.

---

## Source File

```text
src/main.c
```

This file contains the bare-metal ARM application.

The application:

* Initializes AXI GPIO.
* Configures GPIO channel 1 as output.
* Writes HIGH and LOW values to the GPIO.
* Uses delays to create visible LED blinking.
* Prints status messages over the debug console.

---

## Hardware Files

```text
hardware/design_1_wrapper.xsa
hardware/zybo_platform.xpfm
hardware/design_1_wrapper.bit
```

These files are used for hardware handoff, Vitis platform creation, and FPGA programming.

---

## Design File

```text
design/design_1.bd
```

This file contains the Vivado block design used for the Zynq Processing System and AXI GPIO connection.

---

## What Was Tested

* Created a Zynq PS + AXI GPIO hardware design in Vivado.
* Generated the FPGA bitstream successfully.
* Exported the hardware design as XSA.
* Created a Vitis platform from the XSA.
* Created and built a bare-metal ARM application.
* Programmed the FPGA.
* Ran the application on the Zybo board.
* Verified that the ARM processor blinks the FPGA-side LED through AXI GPIO.

---

## Result

The AXI GPIO LED blink example was tested successfully on the Zybo board.