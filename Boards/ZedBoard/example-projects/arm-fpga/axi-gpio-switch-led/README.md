# ZedBoard AXI GPIO Switch LED

This example verifies ARM + FPGA communication on the **ZedBoard** by reading FPGA-side switches and controlling FPGA-side LEDs through **AXI GPIO**.

The project is useful for testing PS-PL integration, AXI GPIO input/output handling, Vitis platform generation, and bare-metal application execution.

---

## Hardware Overview

The design targets the **ZedBoard**, which is based on the **Zynq-7000 XC7Z020** device.

In this example, the ARM Processing System communicates with FPGA fabric through AXI GPIO. FPGA-side switches are read by software running on the ARM processor, and FPGA-side LEDs are driven based on the switch values.

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
* FPGA-side switches
* FPGA-side LEDs

---

## Repository Content

This example folder contains only the required files:

* **src/main.c:** Bare-metal C application for reading switches and controlling LEDs through AXI GPIO.
* **scripts/create_switch_led_platform.py:** Vitis script to create the platform.
* **scripts/create_switch_led_app.py:** Vitis script to create the application.
* **hardware/zedboard_axi_gpio_switch_led.xsa:** Exported Vivado hardware design.
* **hardware/zedboard_switch_led_platform.xpfm:** Generated Vitis platform file.
* **hardware/design_1_wrapper.bit:** Generated FPGA bitstream.
* **design/design_1.bd:** Vivado block design file.

---

## What Was Tested

* Created a Zynq PS + AXI GPIO hardware design in Vivado.
* Connected AXI GPIO to FPGA-side switches and LEDs.
* Generated the FPGA bitstream successfully.
* Exported the hardware design as XSA.
* Created a Vitis platform from the XSA.
* Created and built a bare-metal ARM application.
* Programmed the FPGA and ran the application on the ZedBoard.
* Verified that switch input controls LED output through ARM + AXI GPIO.

---

## Result

The AXI GPIO switch-to-LED example was tested successfully on the ZedBoard.