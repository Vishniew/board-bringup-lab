import vitis

client = vitis.create_client()

client.set_workspace(path="C:/FPGA_Projects/Zybo_ARM_Blink/vitis_final_ws")

app = client.create_app_component(name="zybo_blink_app", platform="C:/FPGA_Projects/Zybo_ARM_Blink/vitis_final_ws/zybo_platform/export/zybo_platform/zybo_platform.xpfm", domain="standalone_ps7_cortexa9_0")