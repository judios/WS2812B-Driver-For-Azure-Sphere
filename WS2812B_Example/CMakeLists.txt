#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.8)
project (WS2812B_Example C)

# Create executable
add_executable (${PROJECT_NAME} main.c WS2812B_Driver/ws2812b.c WS2811_Driver/ws2811.c)
target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c)

# Add MakeImage post-build command
include (${AZURE_SPHERE_MAKE_IMAGE_FILE})
