cmake_minimum_required(VERSION 3.16)
project(aobs C)

add_executable(aobs src/aobs.c)

add_library(payload SHARED src/payload.c)
target_link_libraries(payload pthread dl)

set_target_properties(payload PROPERTIES
    OUTPUT_NAME "payload"
    SUFFIX ".dylib"
)
