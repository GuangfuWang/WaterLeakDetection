## Author: Guangfu WANG.
## Date: 2023-08-20.
#set cpp version used in this project.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#this is equivalently to -fPIC in cxx_flags.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(DEPLOY_LIB_NAME "water_leak")
set(MAIN_NAME "water_leak_main")

set(CMAKE_INSTALL_RPATH "\$ORIGIN")
set(CMAKE_INSTALL_PREFIX "install")
add_link_options("-Wl,--as-needed")


