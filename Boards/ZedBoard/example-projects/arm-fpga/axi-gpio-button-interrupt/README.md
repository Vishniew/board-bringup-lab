# ZedBoard AXI GPIO Button Interrupt

This example verifies ARM + FPGA interrupt handling on the **ZedBoard** by using an FPGA-side button input connected through **AXI GPIO** to generate an interrupt for the ARM Cortex-A9 processor.

The project is useful for testing PS-PL integration, AXI GPIO input handling, interrupt routing, Vitis platform generation, and bare-metal interrupt-based application execution.

---

## Hardware Overview

The design targets the **ZedBoard**, which is based on the **Zynq-7000 XC7Z020** device.

In this example, the ARM Processing System communicates with FPGA fabric through AXI GPIO. A button input from the FPGA side is used to trigger an interrupt that is handled by software running on the ARM processor.

---

## Tech Stack & Tools

### Development Environment

* **Vivado 2025.2:** Used to create the Zynq PS + AXI GPIO interrupt hardware design, generate the bitstream, and export the XSA.
* **Vitis 2025.2:** Used to create the platform and bare-metal application.

### Hardware

* **ZedBoard**
* **Zynq-7000 XC7Z020**
* ARM Cortex-A9 processor
* AXI GPIO IP
* FPGA-side button input
* FPGA-side interrupt connection

---

## Repository Content

This example folder contains only the required files:

* **src/main.c:** Bare-metal C application for handling AXI GPIO button interrupts.
* **scripts/create_button_interrupt_platform.py:** Vitis script to create the platform.
* **scripts/create_button_interrupt_app.py:** Vitis script to create the application.
* **hardware/zedboard_axi_gpio_button_interrupt.xsa:** Exported Vivado hardware design.
* **hardware/zedboard_button_interrupt_platform.xpfm:** Generated Vitis platform file.
* **hardware/design_1_wrapper.bit:** Generated FPGA bitstream.
* **design/design_1.bd:** Vivado block design file.

---

## What Was Tested

* Created a Zynq PS + AXI GPIO hardware design in Vivado.
* Connected FPGA-side button input through AXI GPIO.
* Configured interrupt support between the FPGA fabric and ARM Processing System.
* Generated the FPGA bitstream successfully.
* Exported the hardware design as XSA.
* Created a Vitis platform from the XSA.
* Created and built a bare-metal ARM interrupt application.
* Programmed the FPGA and ran the application on the ZedBoard.
* Verified button interrupt handling on the ARM processor.

---

## Result

The AXI GPIO button interrupt example was tested successfully on the ZedBoard.