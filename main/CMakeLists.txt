file(GLOB_RECURSE srcs "esp32s3dev.c" "src/*.c")
idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "." "./inc")
