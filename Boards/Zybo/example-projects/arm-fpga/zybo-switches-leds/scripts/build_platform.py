import vitis

client = vitis.create_client()

# Point to your new clean workspace
client.set_workspace(path="C:/FPGA_Projects/Zybo_Switches_LEDs/vitis_ws")

# Command the compiler to build the platform using your new hardware blueprint
plat = client.create_platform_component(name="switches_leds_platform", hw_design="C:/FPGA_Projects/Zybo_Switches_LEDs/design_1_wrapper.xsa", os="standalone", cpu="ps7_cortexa9_0")

plat.build()