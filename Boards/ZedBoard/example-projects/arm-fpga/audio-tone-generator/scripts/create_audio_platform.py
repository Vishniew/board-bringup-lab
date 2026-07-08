import os
import vitis

workspace = r"C:\Zed_Projects\zed_audio_vitis_ws"
xsa_path = r"C:\Zed_Projects\zed_audio_tone\zed_audio_bd_wrapper.xsa"
platform_name = "zed_audio_platform"

if not os.path.exists(xsa_path):
    raise FileNotFoundError(f"XSA not found: {xsa_path}")

client = vitis.create_client()
client.set_workspace(path=workspace)

platform = client.create_platform_component(
    name=platform_name,
    hw_design=xsa_path,
    os="standalone",
    cpu="ps7_cortexa9_0",
    domain_name="standalone_ps7_cortexa9_0"
)

platform.build()

print("ZedBoard audio platform created and built successfully.")
print(f"Workspace: {workspace}")
print(f"Platform name: {platform_name}")
print(f"XSA used: {xsa_path}")