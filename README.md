# Board Bring-up Lab

This repository contains board bring-up examples, hardware designs, software applications, and documentation created while working with FPGA and SoC development boards.

The goal of this repository is to organize practical bring-up work in a clean structure so each board and example can be understood, rebuilt, and referenced easily.

---

## Boards Covered

### ZedBoard

The ZedBoard examples are based on the **Zynq-7000 XC7Z020** device.

Current ZedBoard work includes:

* FPGA-only examples
* ARM bare-metal examples
* ARM + FPGA examples using AXI GPIO and AXI Timer
* Vivado hardware designs
* Vitis bare-metal applications
* Bitstream, XSA, and platform files

### Zybo

Zybo bring-up files and documentation will be added separately.

### Nexys A7

Nexys A7 bring-up files and documentation will be added separately.

---

## Repository Structure

```text
board-bringup-lab/
├── Boards/
│   ├── ZedBoard/
│   │   ├── README.md
│   │   ├── docs/
│   │   └── example-projects/
│   │       ├── fpga-only/
│   │       ├── arm-only/
│   │       └── arm-fpga/
│   ├── Zybo/
│   │   ├── README.md
│   │   ├── docs/
│   │   └── example-projects/
│   └── Nexys-A7/
│       ├── README.md
│       ├── docs/
│       └── example-projects/
└── README.md