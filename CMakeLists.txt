


cmake_minimum_required(VERSION 4.0.0 FATAL_ERROR)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
set(CMAKE_CXX_FLAGS "-stdlib=libc++")

project(utils
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(fmt REQUIRED)

set_target_properties(fmt::fmt
  PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
)

add_library(utils)
target_sources(utils PUBLIC
  FILE_SET modules
    TYPE CXX_MODULES
    FILES src/utils.cppm
)
target_compile_options(utils
  PRIVATE
    "-Wall"
    "-Werror"
    # this helps life at client side with Werror
    "-Wno-unused-command-line-argument"
)
target_compile_features(utils
  PRIVATE cxx_std_26
  INTERFACE cxx_std_20
)
target_link_libraries(utils
  PRIVATE
    fmt::fmt
)

install(TARGETS utils
  EXPORT utilsTargets
  FILE_SET modules
    DESTINATION lib/uzleo_utils/miu
  LIBRARY
    DESTINATION lib
  ARCHIVE
    DESTINATION lib/static
  CXX_MODULES_BMI
    DESTINATION lib/uzleo_utils/bmi
)
install(EXPORT utilsTargets
  FILE uzleo_utilsTargets.cmake
  NAMESPACE uzleo::
  DESTINATION lib/cmake/
  CXX_MODULES_DIRECTORY modules
)
install(FILES cmake/uzleo_utilsConfig.cmake
  DESTINATION lib/cmake/
)

