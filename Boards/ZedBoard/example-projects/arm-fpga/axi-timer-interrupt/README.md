# ZedBoard AXI Timer Interrupt

This example verifies ARM + FPGA timer interrupt handling on the **ZedBoard** using an **AXI Timer** connected to the ARM Cortex-A9 processor.

The project is useful for testing PS-PL integration, AXI Timer configuration, interrupt routing, Vitis platform generation, and bare-metal interrupt-based application execution.

---

## Hardware Overview

The design targets the **ZedBoard**, which is based on the **Zynq-7000 XC7Z020** device.

In this example, the ARM Processing System communicates with FPGA fabric through AXI. An AXI Timer in the FPGA fabric generates interrupts that are handled by software running on the ARM processor.

---

## Tech Stack & Tools

### Development Environment

* **Vivado 2025.2:** Used to create the Zynq PS + AXI Timer hardware design, generate the bitstream, and export the XSA.
* **Vitis 2025.2:** Used to create the platform and bare-metal application.

### Hardware

* **ZedBoard**
* **Zynq-7000 XC7Z020**
* ARM Cortex-A9 processor
* AXI Timer IP
* FPGA-side interrupt connection

---

## Repository Content

This example folder contains only the required files:

* **src/main.c:** Bare-metal C application for handling AXI Timer interrupts.
* **scripts/create_timer_platform.py:** Vitis script to create the platform.
* **scripts/create_timer_app.py:** Vitis script to create the application.
* **hardware/zedboard_axi_timer_interrupt.xsa:** Exported Vivado hardware design.
* **hardware/zedboard_axi_timer_platform.xpfm:** Generated Vitis platform file.
* **hardware/design_1_wrapper.bit:** Generated FPGA bitstream.
* **design/design_1.bd:** Vivado block design file.

---

## What Was Tested

* Created a Zynq PS + AXI Timer hardware design in Vivado.
* Connected the AXI Timer interrupt to the ARM Processing System.
* Generated the FPGA bitstream successfully.
* Exported the hardware design as XSA.
* Created a Vitis platform from the XSA.
* Created and built a bare-metal ARM interrupt application.
* Programmed the FPGA and ran the application on the ZedBoard.
* Verified AXI Timer interrupt handling on the ARM processor.

---

## Result

The AXI Timer interrupt example was tested successfully on the ZedBoard.