import vitis

client = vitis.create_client()

client.set_workspace(path="C:/FPGA_Projects/Zybo_ARM_Blink/vitis_final_ws")

plat = client.create_platform_component(name="zybo_platform", hw_design="C:/FPGA_Projects/Zybo_ARM_Blink/design_1_wrapper.xsa", os="standalone", cpu="ps7_cortexa9_0")

plat.build()