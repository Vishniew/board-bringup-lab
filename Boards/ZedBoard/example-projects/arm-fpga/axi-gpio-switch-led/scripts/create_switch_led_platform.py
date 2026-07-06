import os
import vitis

workspace = "C:/Zed_Projects/vitis_switch_led_ws"

xsa_path = "C:/Zed_Projects/zedboard_axi_gpio_switch_led/zedboard_axi_gpio_switch_led.xsa"

if not os.path.exists(xsa_path):
    raise FileNotFoundError(f"XSA not found: {xsa_path}")

client = vitis.create_client()
client.set_workspace(path=workspace)

platform = client.create_platform_component(
    name="zedboard_switch_led_platform",
    hw_design=xsa_path,
    os="standalone",
    cpu="ps7_cortexa9_0"
)

platform.build()

print("ZedBoard switch-to-LED AXI GPIO platform created and built successfully.")

vitis.dispose()