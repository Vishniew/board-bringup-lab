import os
import vitis

workspace = "C:/Zed_Projects/vitis_switch_led_ws"

platform_path = (
    "C:/Zed_Projects/vitis_switch_led_ws/"
    "zedboard_switch_led_platform/export/zedboard_switch_led_platform/"
    "zedboard_switch_led_platform.xpfm"
)

if not os.path.exists(platform_path):
    raise FileNotFoundError(f"Platform XPFM not found: {platform_path}")

client = vitis.create_client()
client.set_workspace(path=workspace)

app = client.create_app_component(
    name="switch_led_app",
    platform=platform_path,
    domain="standalone_ps7_cortexa9_0",
    template="empty_application"
)

print("Switch-to-LED AXI GPIO application created successfully.")

vitis.dispose()