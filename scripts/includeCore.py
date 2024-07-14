import os
Import("env")
env.BuildSources(
    os.path.join("$BUILD_DIR", "Core"),
    os.path.join("$PROJECT_DIR", "Core", "Src")
)
env.Append(CPPPATH=["$PROJECT_DIR/Core/Inc"])

# from os.path import realpath

# for item in env.GetProjectConfig().parse_multi_values(env.GetProjectOption("using")):
#     if item == "FDCAN":
#         env.Prepend(CPPDEFINES=["_JQB_USE_FDCAN"])
#     if item == "USB_CDC":
#         env.Prepend(CPPDEFINES=["_JQB_USE_USB_CDC"])
#         env.Append(CPPPATH=["USB_Device/App"])
#         env.Append(CPPPATH=["USB_Device/Target"])
#         env.BuildSources(
#             os.path.join("$BUILD_DIR", "USB_Device", "App"),
#             os.path.join("$PROJECT_DIR", "USB_Device", "App")
#         )
#         env.BuildSources(
#             os.path.join("$BUILD_DIR", "USB_Device", "Target"),
#             os.path.join("$PROJECT_DIR", "USB_Device", "Target")
#         )