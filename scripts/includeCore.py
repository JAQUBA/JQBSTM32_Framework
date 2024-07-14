import os
Import("env")
env.BuildSources(
    os.path.join("$BUILD_DIR", "Core"),
    os.path.join("$PROJECT_DIR", "Core", "Src")
)