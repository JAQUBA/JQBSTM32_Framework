import os
import shutil
Import("env")

platform = env.PioPlatform()
board = env.BoardConfig()
MCU = board.get("build.mcu", "")
MCU_FAMILY = MCU[0:7]

# Ścieżka do domyślnego pliku HAL
framework_dir = platform.get_package_dir("framework-stm32cube%s" % MCU[5:7])
default_conf_h_path = os.path.join(
    framework_dir,
    "Drivers",
    MCU_FAMILY.upper() + "xx_HAL_Driver",
    "Inc",
    MCU_FAMILY + "xx_hal_conf.h"
)

# Ścieżka do niestandardowego pliku konfiguracyjnego w projekcie
custom_conf_h_path = os.path.join(env["PROJECT_DIR"], "Core", "Inc", MCU_FAMILY + "xx_hal_conf.h")

# Usuń oryginalny plik i skopiuj niestandardowy plik na jego miejsce
if os.path.isfile(default_conf_h_path):
    os.remove(default_conf_h_path)
shutil.copy(custom_conf_h_path, default_conf_h_path)

# Dodaj katalog projektu do ścieżek nagłówkowych
env.Prepend(CPPPATH=[os.path.join(env["PROJECT_DIR"], "Core", "Inc")])

# Skonfiguruj kompilację źródeł projektu
env.BuildSources(
    os.path.join("$BUILD_DIR", "Core", "Src"),
    os.path.join("$PROJECT_DIR", "Core", "Src")
)


for item in env.GetProjectConfig().parse_multi_values(env.GetProjectOption("custom_using", "")):
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