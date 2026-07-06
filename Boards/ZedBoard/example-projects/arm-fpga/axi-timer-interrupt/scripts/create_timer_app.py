import os
import vitis

workspace = "C:/Zed_Projects/vitis_axi_timer_ws"

platform_path = (
    "C:/Zed_Projects/vitis_axi_timer_ws/"
    "zedboard_axi_timer_platform/export/"
    "zedboard_axi_timer_platform/"
    "zedboard_axi_timer_platform.xpfm"
)

if not os.path.exists(platform_path):
    raise FileNotFoundError(f"Platform not found: {platform_path}")

client = vitis.create_client()
client.set_workspace(path=workspace)

app = client.create_app_component(
    name="axi_timer_led_app",
    platform=platform_path,
    domain="standalone_ps7_cortexa9_0",
    template="empty_application"
)

print("AXI Timer LED application created successfully.")

vitis.dispose()