## ZedBoard ADAU1761 Audio Codec Pins

set_property PACKAGE_PIN AB2 [get_ports {MCLK_O}]
set_property PACKAGE_PIN AA6 [get_ports {BCLK_O}]
set_property PACKAGE_PIN Y6  [get_ports {LRCLK_O}]
set_property PACKAGE_PIN Y8  [get_ports {SDATA_O}]
set_property PACKAGE_PIN AA7 [get_ports {SDATA_I}]

set_property PACKAGE_PIN AB4 [get_ports {iic_rtl_scl_io}]
set_property PACKAGE_PIN AB5 [get_ports {iic_rtl_sda_io}]

set_property IOSTANDARD LVCMOS33 [get_ports {MCLK_O}]
set_property IOSTANDARD LVCMOS33 [get_ports {BCLK_O}]
set_property IOSTANDARD LVCMOS33 [get_ports {LRCLK_O}]
set_property IOSTANDARD LVCMOS33 [get_ports {SDATA_O}]
set_property IOSTANDARD LVCMOS33 [get_ports {SDATA_I}]
set_property IOSTANDARD LVCMOS33 [get_ports {iic_rtl_scl_io}]
set_property IOSTANDARD LVCMOS33 [get_ports {iic_rtl_sda_io}]

## I2C codec control interface timing exception
## AXI IIC uses these as slow bidirectional open-drain control pins.
## Do not analyze board-level I2C input/output paths as synchronous FPGA timing paths.

set_false_path -from [get_ports {iic_rtl_scl_io iic_rtl_sda_io}]
set_false_path -to   [get_ports {iic_rtl_scl_io iic_rtl_sda_io}]

## Intentional CDC:
## AXI GPIO sample register is clocked by PS FCLK/AXI clock.
## audio_stream_i2s_player is clocked by the 12.288 MHz Clocking Wizard output.
## sample_data/sample_toggle are synchronized inside audio_stream_i2s_player.

set_clock_groups -asynchronous \
    -group [get_clocks clk_fpga_0] \
    -group [get_clocks -of_objects [get_pins zed_audio_bd_i/clk_wiz_0/inst/mmcm_adv_inst/CLKOUT0]]