import os
Import("env")

env.Prepend(CPPPATH=["$PROJECT_DIR/Core/Inc"])
env.BuildSources(
    os.path.join("$BUILD_DIR", "Core", "Src"),
    os.path.join("$PROJECT_DIR", "Core", "Src")
)

for item in env.GetProjectConfig().parse_multi_values(env.GetProjectOption("using", "")):
    if item == "FDCAN":
        env.Prepend(CPPDEFINES=["_JQB_USE_FDCAN"])
    if item == "CAN":
        env.Prepend(CPPDEFINES=["_JQB_USE_CAN"])
    if item == "USB_CDC":
        env.Prepend(CPPDEFINES=["_JQB_USE_USB_CDC"])

        env.Prepend(CPPPATH=["$PROJECT_DIR/USB_Device/App"])
        env.Prepend(CPPPATH=["$PROJECT_DIR/USB_Device/Target"])
        
        env.BuildSources(
            os.path.join("$BUILD_DIR", "USB_Device", "App"),
            os.path.join("$PROJECT_DIR", "USB_Device", "App")
        )
        env.BuildSources(
            os.path.join("$BUILD_DIR", "USB_Device", "Target"),
            os.path.join("$PROJECT_DIR", "USB_Device", "Target")
        )