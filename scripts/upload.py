Import("env")

env.Append(
    BOOTUPLOADERFLAGS=[
        "-P", "$UPLOAD_PORT"
    ],
    FUSESUPLOADERFLAGS=[
        "-P", "$UPLOAD_PORT"
    ]
)