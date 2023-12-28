idf_component_register(
    SRCS "haxxboard.cpp" "util.cpp" "usb.cpp" "led.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES driver
    )


