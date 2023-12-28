idf_component_register(
    SRCS "haxxboard.c" "util.c" "util.h" "key_maps.h"
    INCLUDE_DIRS "."
    PRIV_REQUIRES driver
    )
