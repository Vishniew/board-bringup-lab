# ZedBoard LED Blink



This example verifies basic FPGA programming on the **ZedBoard** by blinking an FPGA-side LED using simple Verilog logic.



The project is useful as a first bring-up test to confirm that the Vivado flow, board constraints, bitstream generation, and FPGA programming are working correctly.



---



## Hardware Overview



The design targets the **ZedBoard**, which is based on the **Zynq-7000 XC7Z020** device.



In this example, only the FPGA fabric is used. The ARM processing system is not used.



---



## Tech Stack & Tools



### Development Environment



* **Vivado 2025.2:** Used for project creation, synthesis, implementation, bitstream generation, and FPGA programming.



### Hardware



* **ZedBoard**

* **Zynq-7000 XC7Z020**

* FPGA-side LED



---



## Repository Content



This example folder contains only the required files:



* **src/led_blink.v:** Verilog source file for LED blinking logic.

* **constraints/zedboard.xdc:** ZedBoard pin constraint file.

* **hardware/led_blink.bit:** Generated bitstream used to program the FPGA.



---



## What Was Tested



* Created a Vivado project for the ZedBoard.

* Added Verilog source and XDC constraints.

* Generated the FPGA bitstream successfully.

* Programmed the ZedBoard FPGA.

* Verified LED blinking on the board.



---



## Result



The LED blink example was tested successfully on the ZedBoard.

