# Zybo MIO Blink

This example verifies ARM-side GPIO control on the **Zybo** board by blinking a PS/MIO-connected output using a bare-metal ARM application.

The project is useful for testing the Zynq Processing System, PS GPIO access, Vitis platform generation, bare-metal application execution, and basic ARM-side bring-up.

---

## Hardware Overview

The design targets the **Zybo** board, which is based on a **Zynq-7000** device.

In this example, the ARM Processing System is used to control a MIO pin through PS GPIO.

The FPGA fabric is not used for the LED control logic.

---

## Design Behavior

The application uses the Xilinx PS GPIO driver:

```c
#include "xgpiops.h"
```

The code configures MIO pin **7** as an output:

```c
XGpioPs_SetDirectionPin(&ps_gpio, 7, 1);
XGpioPs_SetOutputEnablePin(&ps_gpio, 7, 1);
```

Then it repeatedly writes HIGH and LOW values to the pin:

```c
XGpioPs_WritePin(&ps_gpio, 7, 1);
usleep(500000);

XGpioPs_WritePin(&ps_gpio, 7, 0);
usleep(500000);
```

So the actual behavior is:

```text
ARM software → PS GPIO/MIO pin → blink output
```

---

## Tech Stack & Tools

### Development Environment

* **Vitis 2025.2:** Used to create the platform and bare-metal application.
* **Vivado 2025.2:** Used for the hardware handoff/XSA flow.

### Hardware

* **Zybo**
* **Zynq-7000**
* ARM Cortex-A9 processor
* PS GPIO / MIO pin

---

## Repository Content

This example folder contains only the required files:

* **src/main.c:** Bare-metal C application for blinking a PS/MIO output.
* **scripts/build_component.py:** Vitis script used for component/platform generation.
* **scripts/create_app.py:** Vitis script used to create the application.
* **hardware/design_1_wrapper.xsa:** Exported hardware handoff file.
* **hardware/zybo_base_platform.xpfm:** Generated Vitis platform file.

---

## Source File

```text
src/main.c
```

This file contains the bare-metal ARM application.

The application:

* Initializes the PS GPIO driver.
* Configures MIO pin 7 as output.
* Enables output on the pin.
* Toggles the pin HIGH and LOW.
* Uses delays to create visible blinking.

---

## Hardware Files

```text
hardware/design_1_wrapper.xsa
hardware/zybo_base_platform.xpfm
```

These files are used for hardware handoff and Vitis platform creation.

---

## What Was Tested

* Created the required hardware handoff for the Zybo board.
* Created a Vitis platform from the XSA.
* Created and built a bare-metal ARM application.
* Configured PS GPIO/MIO from software.
* Ran the application on the Zybo board.
* Verified MIO pin blinking behavior.

---

## Result

The MIO blink example was tested successfully on the Zybo board.