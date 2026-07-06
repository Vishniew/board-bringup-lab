# ZedBoard Button to LED



This example verifies basic FPGA input-output control on the **ZedBoard** by connecting a push button input to an FPGA-side LED output using Verilog logic.



The project is useful for testing board constraints, GPIO-style FPGA input handling, output driving, and basic Vivado bitstream generation.



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

* FPGA-side push button

* FPGA-side LED



---



## Repository Content



This example folder contains only the required files:



* **src/button_to_led.v:** Verilog source file for button-to-LED logic.

* **constraints/zedboard.xdc:** ZedBoard pin constraint file.

* **hardware/button_to_led.bit:** Generated bitstream used to program the FPGA.



---



## What Was Tested



* Created a Vivado project for the ZedBoard.

* Added Verilog source and XDC constraints.

* Generated the FPGA bitstream successfully.

* Programmed the ZedBoard FPGA.

* Verified that pressing the button controls the LED.



---



## Result



The button-to-LED example was tested successfully on the ZedBoard.

