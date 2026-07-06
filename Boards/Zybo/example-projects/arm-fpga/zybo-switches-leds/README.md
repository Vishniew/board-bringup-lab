# Zybo AXI GPIO Switches to LEDs

This example verifies ARM + FPGA communication on the **Zybo** board by reading switch inputs through AXI GPIO and writing the same values to LEDs through AXI GPIO.

The project is useful for testing PS-PL communication, AXI GPIO input/output configuration, Vitis platform generation, bare-metal application execution, bitstream programming, and basic hardware-software interaction.

---

## Hardware Overview

The design targets the **Zybo** board, which is based on a **Zynq-7000** device.

In this example, the ARM Processing System communicates with FPGA fabric through an **AXI GPIO** peripheral.

The switches and LEDs are connected through AXI GPIO channels:

```text
Channel 1 → LEDs  → Output
Channel 2 → Switches → Input
```

The ARM processor reads the switch state from AXI GPIO channel 2 and writes the same value to AXI GPIO channel 1.

---

## Design Behavior

The application initializes the AXI GPIO peripheral:

```c
XGpio_Initialize(&my_bridge, XPAR_AXI_GPIO_0_BASEADDR);
```

Then it configures the GPIO directions:

```c
XGpio_SetDataDirection(&my_bridge, 1, 0x0);
XGpio_SetDataDirection(&my_bridge, 2, 0xF);
```

Here:

```text
0x0 → output
0xF → input for 4 bits
```

Inside the infinite loop, the software reads the switch state:

```c
switch_data = XGpio_DiscreteRead(&my_bridge, 2);
```

Then it writes the same value to the LEDs:

```c
XGpio_DiscreteWrite(&my_bridge, 1, switch_data);
```

So the actual behavior is:

```text
Switch input → AXI GPIO → ARM software → AXI GPIO → LED output
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
* Board switches
* Board LEDs

---

## Repository Content

This example folder contains only the required files:

* **src/main.c:** Bare-metal C application that reads switches and drives LEDs through AXI GPIO.
* **scripts/build_platform.py:** Vitis script to create/build the platform.
* **scripts/create_app.py:** Vitis script to create the application.
* **hardware/design_1_wrapper.xsa:** Exported Vivado hardware design.
* **hardware/switches_leds_platform.xpfm:** Generated Vitis platform file.
* **hardware/design_1_wrapper.bit:** Generated FPGA bitstream.
* **design/design_1.bd:** Vivado block design file.

---

## Source File

```text
src/main.c
```

This file contains the bare-metal ARM application.

The application:

* Initializes the AXI GPIO peripheral.
* Configures AXI GPIO channel 1 as LED output.
* Configures AXI GPIO channel 2 as switch input.
* Continuously reads the switch state.
* Continuously writes the switch state to the LEDs.

---

## Hardware Files

```text
hardware/design_1_wrapper.xsa
hardware/switches_leds_platform.xpfm
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
* Connected switches and LEDs through AXI GPIO.
* Generated the FPGA bitstream successfully.
* Exported the hardware design as XSA.
* Created a Vitis platform from the XSA.
* Created and built a bare-metal ARM application.
* Programmed the FPGA.
* Ran the application on the Zybo board.
* Verified that switch states are reflected on LEDs through ARM + AXI GPIO.

---

## Result

The AXI GPIO switches-to-LEDs example was tested successfully on the Zybo board.