# Build shell as a static library
add_library(shell_lib STATIC
    executor.c
    parser.c
    history.c
    filemanager.c
    # ... other .c files
)

# Make headers available to tests
target_include_directories(shell_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

