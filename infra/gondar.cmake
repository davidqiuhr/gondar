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

# Workaround for missing libraries when linking statically with cmake:
# https://github.com/mxe/mxe/issues/1642
function(fix_qt_static_link target)
  include(FindPkgConfig)

  pkg_check_modules(network_extra REQUIRED Qt5Network)
  pkg_check_modules(widgets_extra REQUIRED Qt5Widgets)
  pkg_check_modules(freetype_extra REQUIRED freetype2)

  target_link_libraries(${target} PUBLIC
    Qt5::QWindowsIntegrationPlugin
    Qt5EventDispatcherSupport
    Qt5FontDatabaseSupport
    Qt5ThemeSupport
    ${network_extra_LDFLAGS}
    ${widgets_extra_LDFLAGS}
    ${freetype_extra_LDFLAGS})
endfunction()


# Build gondar.c with a slightly different set of flags since it's a
# partially third-party C file. The result is linked to |target| along
# with any system libs that gondar.c needs.
function(add_win32_usb_support target)
  add_library(win32_usb STATIC src/gondar.cc)
  set_target_properties(win32_usb PROPERTIES CXX_STANDARD 11)
  target_compile_options(win32_usb PRIVATE ${EXTRA_WARNINGS})
  target_link_libraries(app PRIVATE win32_usb setupapi)
endfunction()
