idf_component_register(
        SRCS "haxxboard.cpp" "key_scan.cpp" "usb.cpp" "trackpad.cpp"
        INCLUDE_DIRS "."
        PRIV_REQUIRES driver
)


