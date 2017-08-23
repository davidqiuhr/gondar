
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gdisk)
add_library(gdisk STATIC
  gdisk/basicmbr.cc
  gdisk/bsd.cc
  gdisk/diskio-windows.cc
  gdisk/support.cc
  gdisk/guid.cc
  gdisk/gpt.cc
  gdisk/gptpart.cc
  gdisk/mbrpart.cc
  gdisk/mbr.cc
  gdisk/crc32.cc
  gdisk/parttypes.cc
  gdisk/attributes.cc
  gdisk/diskio.cc)

target_compile_options(gdisk PRIVATE
  -D_FILE_OFFSET_BITS=64)

target_include_directories(gdisk PUBLIC gdisk)
target_link_libraries(gdisk rpcrt4)
