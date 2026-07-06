import os
import vitis

workspace = "C:/Zed_Projects/vitis_button_interrupt_ws"

platform_path = (
    "C:/Zed_Projects/vitis_button_interrupt_ws/"
    "zedboard_button_interrupt_platform/export/"
    "zedboard_button_interrupt_platform/"
    "zedboard_button_interrupt_platform.xpfm"
)

if not os.path.exists(platform_path):
    raise FileNotFoundError(f"Platform not found: {platform_path}")

client = vitis.create_client()
client.set_workspace(path=workspace)

app = client.create_app_component(
    name="button_interrupt_app",
    platform=platform_path,
    domain="standalone_ps7_cortexa9_0",
    template="empty_application"
)

print("Button interrupt application created successfully.")

vitis.dispose()