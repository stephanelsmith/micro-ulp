

```
make BOARD=SS_ULP_S3 USER_C_MODULES=~/ulp/upy/c_modules/esp32.cmake
make USER_C_MODULES=~/ulp/upy/c_modules

py -m esptool --chip esp32s3 --port COM90 write_flash -z 0 micropython_ulp\ports\esp32\build-SS_ULP_S3\firmware.bin
py -m serial.tools.miniterm COM92

py -m esptool --chip esp32s3 --port COM36 write_flash -z 0 micropython_ulp\ports\esp32\build-SS_ULP_S3\firmware.bin
py -m serial.tools.miniterm COM3

import esp32
u = esp32.ULP()
u.set_wakeup_period(1000*1000) # 1s
u.run_embedded()

# get variables here
# cat build-SS_ULP_S3/esp-idf/main/ulp_embedded/ulp_embedded.ld
ulp_var_counter = 0x50000080
u.read(ulp_var_counter)
```

# In `esp32_common.cmake`, add after `idf_component_register`
```
if(DEFINED ulp_embedded_sources)
    list(APPEND MICROPY_DEF_CORE ULP_EMBEDDED_APP=1)
    set(ulp_app_name 
        "ulp_embedded"
    )
    set(ulp_depentants 
        ${ulp_depentants} 
        "esp32_ulp.c"
    )
    message("embedded ULP App sources: " ${ulp_embedded_sources} ",  deps: " ${ulp_depentants})
    ulp_embed_binary(${ulp_app_name} ${ulp_embedded_sources} ${ulp_depentants})
    #set(ULP_LD_DIR ${CMAKE_BINARY_DIR}/esp-idf/main/ulp_embedded)
    #add_custom_command(
      #OUTPUT ${CMAKE_BINARY_DIR}/esp32_ulpconst_qstr.h
      #COMMAND python ${MICROPY_PORT_DIR}/esp32_ulp_qstr.py ${ULP_LD_DIR}/ulp_embedded.ld
      #DEPENDS ${ULP_LD_DIR}/ulp_embedded.ld
      #COMMENT "Parsing ULP headers"
      #VERBATIM
    #)
    #add_library(ULP_CONST INTERFACE ${CMAKE_BINARY_DIR}/esp32_ulpconst_qstr.h)
endif()
```
