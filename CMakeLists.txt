# Copyright (C) 2021 Ben Smith
#
# This software may be modified and distributed under the terms
# of the MIT license.  See the LICENSE file for details.
cmake_minimum_required(VERSION 2.8)
project(binjnes)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

option(WERROR "Build with warnings as errors" OFF)

add_definitions(
  -Wall -Wextra -Wpointer-arith -Wno-unused-parameter -g
  -Wno-unused-function -Wno-unused-variable
  -Wno-implicit-fallthrough
)
if (WERROR)
  add_definitions(-Werror)
endif ()

function (target_copy_to_bin name)
add_custom_target(${name}-copy-to-bin ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/bin
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${name}> ${PROJECT_SOURCE_DIR}/bin
  DEPENDS ${name}
)
endfunction ()

set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(SDL2)
find_package(OpenGL)

if (SDL2_FOUND AND OPENGL_FOUND)
  add_executable(binjnes
    src/binjnes.c
    src/common.c
    src/emulator.c
    src/host.c
    src/host-gl.c
    src/host-ui-simple.c
    src/joypad.c
  )
  target_link_libraries(binjnes SDL2::SDL2 SDL2::SDL2main ${OPENGL_gl_LIBRARY})

  install(TARGETS binjnes DESTINATION bin)
  target_copy_to_bin(binjnes)
  if (SDL2_DYNAMIC)
    install(FILES ${SDL2_RUNTIME_LIBRARY} DESTINATION bin)
  endif ()
endif ()
