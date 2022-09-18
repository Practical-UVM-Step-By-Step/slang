# ~~~
# SPDX-FileCopyrightText: Michael Popoloski
# SPDX-License-Identifier: MIT
# ~~~

set(find_pkg_args "")
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24.0")
  set(find_pkg_args "FIND_PACKAGE_ARGS" "9.0.0")
endif()

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 9.0.0
  GIT_SHALLOW ON
  ${find_pkg_args})

# Force fmtlib to always be a static lib if we pull it via FetchContent (i.e.
# not intercepted by find_package), since it's a private dependency that's
# mostly header-only already.
set(saved_build_shared_libs ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailable(fmt)
set(BUILD_SHARED_LIBS ${saved_build_shared_libs})

if(SLANG_INCLUDE_PYLIB OR BUILD_SHARED_LIBS)
  set_target_properties(fmt PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

FetchContent_Declare(
  unordered_dense
  GIT_REPOSITORY https://github.com/martinus/unordered_dense.git
  GIT_TAG v1.3.2
  GIT_SHALLOW ON)
FetchContent_MakeAvailable(unordered_dense)

if(SLANG_INCLUDE_INSTALL)
  install(
    DIRECTORY ../external/ieee1800/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ieee1800
    COMPONENT slang_Development)
  install(
    FILES ../external/flat_hash_map.hpp ../external/span.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT slang_Development)
  install(
    DIRECTORY ${unordered_dense_SOURCE_DIR}/include/ankerl
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT slang_Development)

  install(
    TARGETS fmt
    EXPORT slangTargets
    LIBRARY COMPONENT slang_Development
    ARCHIVE COMPONENT slang_Development
    PUBLIC_HEADER EXCLUDE_FROM_ALL
    PRIVATE_HEADER EXCLUDE_FROM_ALL)

  install(TARGETS unordered_dense EXPORT slangTargets)
endif()