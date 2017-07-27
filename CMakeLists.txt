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

# TODO(nicholasbishop): enable more warnings
set(EXTRA_WARNINGS ${EXTRA_WARNINGS}
  # Warnings limited to C++
  $<$<COMPILE_LANGUAGE:CXX>:-Wsuggest-override>

  # Warnings for both C and C++
  -Wall
  -Wextra
  -Wmissing-declarations
  -Wunused-macros
  -Wwrite-strings)

set(METRICS_API_KEY CACHE STRING "metrics API key")
configure_file(src/config.h.in src/config.h @ONLY)

if(${TREAT_WARNINGS_AS_ERRORS})
  set(EXTRA_WARNINGS ${EXTRA_WARNINGS} -Werror)
endif()

# This mingw define adds more modern string formatting than what
# msvcrt provides (C99/C11 vs C89). This makes extended format
# specifiers like "%lld" work.
add_definitions(-D__USE_MINGW_ANSI_STDIO=1)

# Build minizip
add_subdirectory(minizip)

# Required Qt components
find_package(Qt5 COMPONENTS Network Test Widgets REQUIRED)

# Static lib with convenience functions that make unzipping easier
add_library(minizip_extra STATIC minizip/minishared.c minizip/miniunz.c)
target_compile_definitions(minizip_extra PRIVATE _LARGEFILE64_SOURCE NOMAIN)
target_compile_options(minizip_extra PRIVATE
  ${EXTRA_WARNINGS}
  -Wno-format
  -Wno-missing-declarations
  -Wno-unused-macros)

# Static lib containing the bulk of gondar, shared between the
# application and test targets
add_library(app STATIC
  resources/gondarwizard.qrc
  src/about_dialog.cc
  src/admin_check_page.cc
  src/chromeover_login_page.cc
  src/device.cc
  src/diskwritethread.cc
  src/download_progress_page.cc
  src/downloader.cc
  src/error_page.cc
  src/gondarsite.cc
  src/gondarwizard.cc
  src/image_select_page.cc
  src/log.cc
  src/meepo.cc
  src/metric.cc
  src/neverware_unzipper.cc
  src/site_select_page.cc
  src/unzipthread.cc
  src/util.cc
  src/wizard_page.cc)

if(${CHROMEOVER})
  target_compile_definitions(app PUBLIC CHROMEOVER)
endif()

set_target_properties(app PROPERTIES AUTOMOC ON AUTORCC ON)
target_compile_options(app PRIVATE ${EXTRA_WARNINGS})
target_include_directories(app SYSTEM PUBLIC minizip plog/include)
target_include_directories(app PRIVATE ${CMAKE_BINARY_DIR}/src)
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
  target_sources(cloudready-usb-creator PRIVATE resources/gondar.rc)
else()
  target_sources(app PRIVATE src/stubs.cc)
endif()
