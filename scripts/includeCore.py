import os
import shutil
Import("env")

platform = env.PioPlatform()
board = env.BoardConfig()

# Dodaj katalog projektu do ścieżek nagłówkowych
env.Prepend(CPPPATH=[os.path.join(env["PROJECT_DIR"], "Core", "Inc")])

# Skonfiguruj kompilację źródeł projektu
env.BuildSources(
    os.path.join("$BUILD_DIR", "Core", "Src"),
    os.path.join("$PROJECT_DIR", "Core", "Src")
)