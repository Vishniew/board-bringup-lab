# 1. Map the 'clk' input to the Zybo's 125MHz oscillator 
set_property PACKAGE_PIN K17 [get_ports {clk}]
set_property IOSTANDARD LVCMOS33 [get_ports {clk}]
create_clock -name clk_125 -period 8.00 [get_ports {clk}]

# 2. Map the 'led' output to the Zybo's physical LED 0 (LD0)
set_property PACKAGE_PIN M14 [get_ports {led}]
set_property IOSTANDARD LVCMOS33 [get_ports {led}]