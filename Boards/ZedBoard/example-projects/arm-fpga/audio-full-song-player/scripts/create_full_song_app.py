import os
import vitis

workspace = r"C:\Zed_Projects\zed_audio_full_song_vitis_ws"
app_name = "zed_full_song_player_app"

xpfm_files = []

for root, dirs, files in os.walk(workspace):
    for file in files:
        if file.endswith(".xpfm"):
            xpfm_files.append(os.path.join(root, file))

if not xpfm_files:
    raise FileNotFoundError(f"No .xpfm file found inside workspace: {workspace}")

platform_path = xpfm_files[0]

print(f"Using platform: {platform_path}")

client = vitis.create_client()
client.set_workspace(path=workspace)

app = client.create_app_component(
    name=app_name,
    platform=platform_path,
    domain="standalone_ps7_cortexa9_0",
    template="empty_application"
)

print("Full song player application created successfully.")
print(f"Workspace: {workspace}")
print(f"Application name: {app_name}")
print(f"Platform used: {platform_path}")