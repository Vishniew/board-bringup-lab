# ZedBoard PS LD9 Blink

This example verifies ARM-side bare-metal bring-up on the **ZedBoard** by blinking the PS-side LED **LD9** using the ARM Cortex-A9 processor.

The project is useful for testing the Zynq Processing System, Vitis platform generation, bare-metal application creation, and GPIO control from the ARM side.

---

## Hardware Overview

The design targets the **ZedBoard**, which is based on the **Zynq-7000 XC7Z020** device.

In this example, only the ARM Processing System is used. The FPGA fabric is not used for LED control.

---

## Tech Stack & Tools

### Development Environment

* **Vivado 2025.2:** Used to create the Zynq Processing System hardware design and export the XSA.
* **Vitis 2025.2:** Used to create the platform and bare-metal application.

### Hardware

* **ZedBoard**
* **Zynq-7000 XC7Z020**
* ARM Cortex-A9 processor
* PS-side LED LD9

---

## Repository Content

This example folder contains only the required files:

* **src/main.c:** Bare-metal C application for blinking PS-side LED LD9.
* **scripts/create_zedboard_platform.py:** Vitis script to create the platform.
* **scripts/create_ps_led_app.py:** Vitis script to create the application.
* **hardware/zedboard_arm_ps_led_blink.xsa:** Exported Vivado hardware design.
* **hardware/zedboard_ps_platform.xpfm:** Generated Vitis platform file.
* **design/design_1.bd:** Vivado block design file.

---

## What Was Tested

* Created a Zynq Processing System design in Vivado.
* Exported the hardware design as XSA.
* Created a Vitis platform from the XSA.
* Created and built a bare-metal ARM application.
* Programmed and ran the application on the ZedBoard.
* Verified PS-side LED LD9 blinking.

---

## Result

The PS LD9 blink example was tested successfully on the ZedBoard.