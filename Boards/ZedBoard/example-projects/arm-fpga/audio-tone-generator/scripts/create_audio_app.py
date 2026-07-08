import os
import vitis

workspace = r"C:\Zed_Projects\zed_audio_vitis_ws"

platform_path = (
    r"C:\Zed_Projects\zed_audio_vitis_ws"
    r"\zed_audio_platform\export\zed_audio_platform"
    r"\zed_audio_platform.xpfm"
)

app_name = "zed_audio_app"

if not os.path.exists(platform_path):
    raise FileNotFoundError(f"Platform XPFM not found: {platform_path}")

client = vitis.create_client()
client.set_workspace(path=workspace)

app = client.create_app_component(
    name=app_name,
    platform=platform_path,
    domain="standalone_ps7_cortexa9_0",
    template="empty_application"
)

print("ZedBoard audio application component created successfully.")
print(f"Workspace: {workspace}")
print(f"Application name: {app_name}")
print(f"Platform used: {platform_path}")