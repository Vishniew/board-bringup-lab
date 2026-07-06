import vitis

client = vitis.create_client()

client.set_workspace(path="C:/FPGA_Projects/Zybo_Switches_LEDs/vitis_ws")

app = client.create_app_component(name="switches_leds_app", platform="C:/FPGA_Projects/Zybo_Switches_LEDs/vitis_ws/switches_leds_platform/export/switches_leds_platform/switches_leds_platform.xpfm", domain="standalone_ps7_cortexa9_0")