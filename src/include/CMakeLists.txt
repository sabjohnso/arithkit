add_subdirectory(arithkit)

add_library(arithkit_header INTERFACE)
set_target_properties(arithkit_header PROPERTIES EXPORT_NAME header)
target_include_directories(arithkit_header INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${arithkit_INSTALL_INCLUDEDIR}>)
add_library(arithkit::header ALIAS arithkit_header)

install(TARGETS arithkit_header
  EXPORT arithkit_EXPORTS
  RUNTIME DESTINATION ${arithkit_INSTALL_BINDIR}
  LIBRARY DESTINATION ${arithkit_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${arithkit_INSTALL_LIBDIR})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/arithkit/config.hpp
  DESTINATION ${arithkit_INSTALL_INCLUDEDIR}/arithkit)
