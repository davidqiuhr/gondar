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
  add_library(win32_usb STATIC src/gondar.c)
  target_compile_options(win32_usb PRIVATE
    -Wwrite-strings
    # TODO(nicholasbishop): consider fixing these warnings instead of
    # hiding them
    -Wno-format
    -Wno-sign-compare)
  target_link_libraries(app PRIVATE win32_usb setupapi)
endfunction()
