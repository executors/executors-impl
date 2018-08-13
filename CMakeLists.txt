
cmake_minimum_required(VERSION 3.10)
project(StdExecutors
  VERSION 7.0.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)

################################################################################

option(EXECUTORS_ENABLE_TESTING "Enable tests." Off)
option(EXECUTORS_ENABLE_EXAMPLES "Build examples." Off)

################################################################################

add_library(executors INTERFACE)
add_library(std::executors ALIAS executors)

target_include_directories(executors INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_compile_features(executors INTERFACE cxx_std_17)

################################################################################

install(TARGETS executors EXPORT executorsTargets
  INCLUDES DESTINATION include
)

install(EXPORT executorsTargets
    FILE executorsTargets.cmake
    NAMESPACE std::
    DESTINATION cmake
)

export(TARGETS executors
    NAMESPACE std::
    FILE executorsTargets.cmake
)

install(DIRECTORY include/experimental DESTINATION include)
install(DIRECTORY include/experimental DESTINATION include/__p0443)

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/StdExecutorsConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/StdExecutorsConfig.cmake
  INSTALL_DESTINATION cmake
)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/StdExecutorsConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/StdExecutorsConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/StdExecutorsConfigVersion.cmake
    DESTINATION cmake
)

################################################################################

if(EXECUTORS_ENABLE_TESTING)
  add_subdirectory(tests)
endif()

if(EXECUTORS_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()
