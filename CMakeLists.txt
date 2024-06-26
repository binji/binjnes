# Copyright (C) 2021 Ben Smith
#
# This software may be modified and distributed under the terms
# of the MIT license.  See the LICENSE file for details.
cmake_minimum_required(VERSION 3.0)
project(binjnes)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

option(WERROR "Build with warnings as errors" OFF)
option(WASM_SIMD "Use SIMD" OFF)

if (UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif ()

include(CheckIncludeFile)
CHECK_INCLUDE_FILE("stdatomic.h" HAVE_C11_ATOMIC)
CHECK_INCLUDE_FILE("threads.h" HAVE_C11_THREADS)
CHECK_INCLUDE_FILE("pthread.h" HAVE_PTHREADS)

if (HAVE_C11_THREADS)
  add_definitions(-DBINJNES_THREAD_C11)
elseif (HAVE_PTHREADS)
  add_definitions(-DBINJNES_THREAD_PTHREADS)
else ()
  message(WARNING "No thread library found")
endif ()

if (HAVE_C11_ATOMIC)
  add_definitions(-DBINJNES_ATOMIC_C11)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_definitions(-DBINJNES_ATOMIC_MSVC)
else ()
  message(WARNING "No atomic library found")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_definitions(-DBINJNES_CLANG)
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_definitions(-DBINJNES_GCC)
  endif ()
  add_definitions(
    -Wall -Wextra -Wpointer-arith -Wno-unused-parameter -g
    -Wno-unused-function -Wno-unused-variable
    -Wno-implicit-fallthrough -Wno-unknown-pragmas
  )
  if (WERROR)
    add_definitions(-Werror)
  endif ()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_definitions(
    /DBINJNES_MSVC
    /D_CRT_SECURE_NO_WARNINGS
    /std:c11
    /wd4146  # unary minus operator applied to unsigned type
    /wd4098  # 'void' function returning a value
  )
endif()

function (target_copy_to_bin name)
add_custom_target(${name}-copy-to-bin ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/bin
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${name}> ${PROJECT_SOURCE_DIR}/bin
  DEPENDS ${name}
)
endfunction ()

if (NOT EMSCRIPTEN)
  add_executable(binjnes-tester
    src/common.c
    src/cartdb.c
    src/options.c
    src/emulator.c
    src/joypad.c
    src/tester.c
  )
  install(TARGETS binjnes-tester DESTINATION bin)
  target_copy_to_bin(binjnes-tester)


  # Sokol stuff
  if (LINUX)
    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(OpenGL)
    find_package(X11)
    find_package(Threads)
    find_package(ALSA)
  endif()

  add_executable(binjnes
    src/binjnes.c
    src/cartdb.c
    src/common.c
    src/emulator.c
    src/joypad.c
    src/options.c
    src/rewind.c
    src/sokol.c
  )
  target_include_directories(binjnes
    PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
  if (LINUX)
    target_link_libraries(binjnes
      OpenGL::GL
      ${X11_X11_LIB}
      ${X11_Xi_LIB}
      ${X11_Xcursor_LIB}
      ALSA::ALSA
      Threads::Threads
    )
  endif ()
  install(TARGETS binjnes DESTINATION bin)
  target_copy_to_bin(binjnes)

else (EMSCRIPTEN)

  add_definitions(-Wno-format-security)

  add_executable(binjnes
    src/memory.c
    src/cartdb.c
    src/emulator.c
    src/joypad.c
    src/rewind.c
    src/emscripten/wrapper.c)
  set(EXPORTED_JSON ${PROJECT_SOURCE_DIR}/src/emscripten/exported.json)
  target_include_directories(binjnes PUBLIC ${PROJECT_SOURCE_DIR}/src)

  if (WASM_SIMD)
    set(EXPORT_NAME "BinjnesSimd")
    set_target_properties(binjnes PROPERTIES OUTPUT_NAME "binjnes-simd")
    target_compile_options(binjnes PUBLIC -msimd128)
  else ()
    set(EXPORT_NAME "Binjnes")
  endif ()

  set(LINK_FLAGS
    --memory-init-file 0
    -s EXPORTED_FUNCTIONS=\"@${EXPORTED_JSON}\"
    -s MALLOC=emmalloc
    -s ASSERTIONS=0
    -s ENVIRONMENT=web
    -s FILESYSTEM=0
    -s EXIT_RUNTIME=0
    -s MODULARIZE=1
    -s EXPORT_NAME="${EXPORT_NAME}"
  )

  string(REPLACE ";" " " LINK_FLAGS_STR "${LINK_FLAGS}")

  set_target_properties(binjnes
    PROPERTIES
    LINK_FLAGS "${LINK_FLAGS_STR}"
    LINK_DEPENDS "${EXPORTED_JSON}"
  )
endif ()
