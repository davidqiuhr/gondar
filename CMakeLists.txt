cmake_minimum_required(VERSION 3.4)
project(gondar)

include(infra/gondar.cmake)

# Choose between free and paid product downloads
set(CHROMEOVER OFF CACHE BOOL "Build for Chromeover customers")

# This is for the builders, to make sure new warnings don't creep in
set(TREAT_WARNINGS_AS_ERRORS OFF CACHE BOOL "Compile with -Werror")

# This setting is used in minizip's cmake file, setting it to off here
# because we don't need to handle encrypted zip files.
set(USE_AES OFF CACHE BOOL "Unused docstring")

# This is disabled in our release builds so that the ugly console
# doesn't show.
set(WIN32_CONSOLE ON CACHE BOOL "On Windows, show the console")

# Build minizip before setting any project-level compile flags
add_subdirectory(minizip)

# TODO(nicholasbishop): enable more warnings
add_compile_options(-Wall -Wextra)
if(${TREAT_WARNINGS_AS_ERRORS})
  add_compile_options(-Werror)
endif()

# Required Qt components
find_package(Qt5 COMPONENTS Network Test Widgets REQUIRED)

# Static lib with convenience functions that make unzipping easier
add_library(minizip_extra STATIC minizip/minishared.c minizip/miniunz.c)
target_compile_definitions(minizip_extra PRIVATE _LARGEFILE64_SOURCE NOMAIN)
target_compile_options(minizip_extra PRIVATE -Wno-unused-parameter -Wno-format)

# Static lib containing the bulk of gondar, shared between the
# application and test targets
add_library(app STATIC
  gondarwizard.qrc
  src/admin_check_page.cc
  src/chromeover_login_page.cc
  src/deviceguy.cc
  src/diskwritethread.cc
  src/downloader.cc
  src/gondarwizard.cc
  src/image_select_page.cc
  src/log.cc
  src/neverware_unzipper.cc
  src/unzipthread.cc
  src/wizard_page.cc)

if(${CHROMEOVER})
  target_compile_definitions(app PUBLIC CHROMEOVER)
endif()

set_target_properties(app PROPERTIES AUTOMOC ON AUTORCC ON)
target_compile_options(app PUBLIC
  -Wmissing-declarations
  -Wsuggest-override)
target_include_directories(app SYSTEM PUBLIC minizip plog/include)
target_link_libraries(app PUBLIC
  Qt5::Network Qt5::Widgets minizip minizip_extra)

# Gondar application
add_executable(cloudready-usb-creator src/main.cc)
target_link_libraries(cloudready-usb-creator app)

# Test application
add_executable(tests test/test.cc)
target_link_libraries(tests Qt5::Test app)
set_target_properties(tests PROPERTIES AUTOMOC ON)

# Platform-specific build configuration
if(WIN32)
  if(NOT ${WIN32_CONSOLE})
    set_target_properties(cloudready-usb-creator PROPERTIES WIN32_EXECUTABLE ON)
  endif()

  fix_qt_static_link(app)
  add_win32_usb_support(app)
  target_sources(cloudready-usb-creator PRIVATE gondar.rc)
else()
  target_sources(app PRIVATE src/stubs.cc)
endif()
