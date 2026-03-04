cmake_minimum_required(VERSION 3.12)

project(arithkit VERSION 0.7.0 LANGUAGES C CXX)

option(arithkit_BUILD_TESTING "Build the arithkit tests" ON)
set(arithkit_CXX_STANDARD 20 CACHE STRING "C++ standard for arithkit")

include(GNUInstallDirs)
set(arithkit_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
set(arithkit_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
set(arithkit_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
set(arithkit_INSTALL_CONFDIR ${arithkit_INSTALL_LIBDIR}/cmake/arithkit-${PROJECT_VERSION})

list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)
include(arithkit_deps)

add_subdirectory(src)

add_subdirectory(examples)

if(arithkit_BUILD_TESTING)
  include(CTest)
  if(BUILD_TESTING)
    add_subdirectory(test)
  endif()
endif()

install(EXPORT arithkit_EXPORTS
  NAMESPACE arithkit::
  FILE arithkit-exports.cmake
  DESTINATION ${arithkit_INSTALL_CONFDIR})

configure_file(arithkit-config.cmake.in arithkit-config.cmake)
install(FILES ${PROJECT_BINARY_DIR}/arithkit-config.cmake
  DESTINATION ${arithkit_INSTALL_CONFDIR})
