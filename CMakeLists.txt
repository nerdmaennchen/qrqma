cmake_minimum_required(VERSION 3.12)
project(qrqma VERSION 0.9.5 LANGUAGES CXX)
include(GNUInstallDirs)

add_library(qrqma)
add_library(qrqma::qrqma ALIAS qrqma)
target_compile_features(qrqma PUBLIC cxx_std_17)
target_sources(qrqma PRIVATE demangle.cpp loader.cpp template.cpp)
target_include_directories(qrqma PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(DEST_DIR "${CMAKE_INSTALL_PREFIX}")
set(VERSION "${PROJECT_VERSION}")

if(VERSION STREQUAL "")
    message(FATAL_ERROR "cannot generate version string")
endif()

message("qrqma version: " ${VERSION})

configure_file(qrqma.pc.in qrqma.pc @ONLY)

install(TARGETS qrqma DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES
    loader.h
    template.h
    unique_function.h
    symbol.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qrqma)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qrqma.pc
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pkgconfig)
install(FILES LICENSE
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/licenses/${PROJECT_NAME})
