# Copyright (C) 2021 Ben Smith
#
# This software may be modified and distributed under the terms
# of the MIT license.  See the LICENSE file for details.
cmake_minimum_required(VERSION 3.0)
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

if (NOT EMSCRIPTEN)

  set(OpenGL_GL_PREFERENCE "LEGACY")
  find_package(SDL2)
  find_package(OpenGL)

  if (SDL2_FOUND AND OPENGL_FOUND)
    add_executable(binjnes
      src/binjnes.c
      src/common.c
      src/cartdb.c
      src/emulator.c
      src/host.c
      src/host-gl.c
      src/host-ui-simple.c
      src/joypad.c
      src/options.c
      src/rewind.c
    )
    target_link_libraries(binjnes SDL2::SDL2 SDL2::SDL2main ${OPENGL_gl_LIBRARY})

    install(TARGETS binjnes DESTINATION bin)
    target_copy_to_bin(binjnes)
    if (SDL2_DYNAMIC)
      install(FILES ${SDL2_RUNTIME_LIBRARY} DESTINATION bin)
    endif ()
  endif ()

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
  find_package(X11)
  find_package(Threads)
  find_package(ALSA)

  add_executable(binjnes-sokol
    src/main.c
    src/sokol.c
  )
  target_include_directories(binjnes-sokol
    PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
  target_link_libraries(binjnes-sokol
    ${OPENGL_gl_LIBRARY}
    ${X11_X11_LIB}
    ${X11_Xi_LIB}
    ${X11_Xcursor_LIB}
    ALSA::ALSA
    Threads::Threads
  )
  install(TARGETS binjnes-sokol DESTINATION bin)
  target_copy_to_bin(binjnes-sokol)

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

  set(LINK_FLAGS
    --memory-init-file 0
    -s EXPORTED_FUNCTIONS=\"@${EXPORTED_JSON}\"
    -s MALLOC=emmalloc
    -s ASSERTIONS=0
    -s ENVIRONMENT=web
    -s FILESYSTEM=0
    -s EXIT_RUNTIME=0
    -s MODULARIZE=1
    -s EXPORT_NAME="Binjnes"
  )

  string(REPLACE ";" " " LINK_FLAGS_STR "${LINK_FLAGS}")

  set_target_properties(binjnes
    PROPERTIES
    LINK_FLAGS "${LINK_FLAGS_STR}"
    LINK_DEPENDS "${EXPORTED_JSON}"
  )
endif ()
