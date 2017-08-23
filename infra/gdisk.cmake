# Copyright 2017 Neverware
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
  gdisk/diskio.cc
  src/gpt_pal.cc)

target_compile_options(gdisk PRIVATE
  -D_FILE_OFFSET_BITS=64)

target_compile_options(gdisk PRIVATE -Wno-shadow)

target_include_directories(gdisk PUBLIC gdisk)
target_link_libraries(gdisk rpcrt4)
