import os
import vitis

workspace = "C:/Zed_Projects/vitis_workspace"

platform_path = (
    "C:/Zed_Projects/vitis_workspace/"
    "zedboard_ps_platform/export/zedboard_ps_platform/"
    "zedboard_ps_platform.xpfm"
)

if not os.path.exists(platform_path):
    raise FileNotFoundError(f"Platform XPFM not found: {platform_path}")

client = vitis.create_client()
client.set_workspace(path=workspace)

app = client.create_app_component(
    name="ps_led_blink_app",
    platform=platform_path,
    domain="standalone_ps7_cortexa9_0",
    template="empty_application"
)

print("PS LED blink application component created successfully.")