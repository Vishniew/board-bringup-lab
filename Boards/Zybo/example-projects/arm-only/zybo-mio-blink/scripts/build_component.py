import vitis

client = vitis.create_client()

# Pointing to today's clean workspace
client.set_workspace(path="C:/FPGA_Projects/Zybo_MIO_Blink/vitis_ws")

# Pulling yesterday's hardware blueprint to get the RAM timings
plat = client.create_platform_component(name="zybo_base_platform", hw_design="C:/FPGA_Projects/Zybo_Switches_LEDs/design_1_wrapper.xsa", os="standalone", cpu="ps7_cortexa9_0")

plat.build()