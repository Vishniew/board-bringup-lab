import vitis

client = vitis.create_client()

client.set_workspace(path="C:/FPGA_Projects/Zybo_MIO_Blink/vitis_ws")

app = client.create_app_component(name="mio_blink_app", platform="C:/FPGA_Projects/Zybo_MIO_Blink/vitis_ws/zybo_base_platform/export/zybo_base_platform/zybo_base_platform.xpfm", domain="standalone_ps7_cortexa9_0")