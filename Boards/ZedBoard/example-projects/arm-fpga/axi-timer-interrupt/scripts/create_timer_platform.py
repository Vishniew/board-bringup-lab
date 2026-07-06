import os
import vitis

workspace = "C:/Zed_Projects/vitis_axi_timer_ws"

xsa_path = (
    "C:/Zed_Projects/zedboard_axi_timer_interrupt/"
    "zedboard_axi_timer_interrupt.xsa"
)

if not os.path.exists(xsa_path):
    raise FileNotFoundError(f"XSA not found: {xsa_path}")

client = vitis.create_client()
client.set_workspace(path=workspace)

platform = client.create_platform_component(
    name="zedboard_axi_timer_platform",
    hw_design=xsa_path,
    os="standalone",
    cpu="ps7_cortexa9_0"
)

platform.build()

print("ZedBoard AXI Timer interrupt platform created and built successfully.")

vitis.dispose()