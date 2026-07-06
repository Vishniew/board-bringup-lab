# ZedBoard AXI GPIO LEDs

This example verifies ARM + FPGA communication on the **ZedBoard** by controlling FPGA-side LEDs from the ARM Cortex-A9 processor through an **AXI GPIO** peripheral.

The project is useful for testing PS-PL integration, AXI communication, FPGA-side GPIO control, Vitis platform generation, and bare-metal application execution.

---

## Hardware Overview

The design targets the **ZedBoard**, which is based on the **Zynq-7000 XC7Z020** device.

In this example, the ARM Processing System communicates with FPGA fabric through AXI GPIO. The FPGA-side LEDs are controlled by software running on the ARM processor.

---

## Tech Stack & Tools

### Development Environment

* **Vivado 2025.2:** Used to create the Zynq PS + AXI GPIO hardware design, generate the bitstream, and export the XSA.
* **Vitis 2025.2:** Used to create the platform and bare-metal application.

### Hardware

* **ZedBoard**
* **Zynq-7000 XC7Z020**
* ARM Cortex-A9 processor
* AXI GPIO IP
* FPGA-side LEDs LD0–LD7

---

## Repository Content

This example folder contains only the required files:

* **src/main.c:** Bare-metal C application for controlling FPGA-side LEDs through AXI GPIO.
* **scripts/create_axi_gpio_platform.py:** Vitis script to create the platform.
* **scripts/create_axi_gpio_app.py:** Vitis script to create the application.
* **hardware/zedboard_axi_gpio_leds.xsa:** Exported Vivado hardware design.
* **hardware/zedboard_axi_gpio_platform.xpfm:** Generated Vitis platform file.
* **hardware/design_1_wrapper.bit:** Generated FPGA bitstream.
* **design/design_1.bd:** Vivado block design file.

---

## What Was Tested

* Created a Zynq PS + AXI GPIO hardware design in Vivado.
* Connected AXI GPIO to FPGA-side LEDs.
* Generated the FPGA bitstream successfully.
* Exported the hardware design as XSA.
* Created a Vitis platform from the XSA.
* Created and built a bare-metal ARM application.
* Programmed the FPGA and ran the application on the ZedBoard.
* Verified that the ARM processor controls FPGA-side LEDs through AXI GPIO.

---

## Result

The AXI GPIO LEDs example was tested successfully on the ZedBoard.