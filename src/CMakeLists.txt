cmake_minimum_required(VERSION 3.13)

set(BEAM_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/arena.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/beam.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/node.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/node_visitor.c"
  PARENT_SCOPE
  )

set(SDL_RENDERER_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/renderer/sdl2.c"
  PARENT_SCOPE
  )

set(GLFW_RENDERER_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/renderer/glfw.c"
  PARENT_SCOPE
  )

message(STATUS "Loaded: ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
message(STATUS "BEAM_SOURCES: ${BEAM_SOURCES}")
message(STATUS "SDL_RENDERER_SOURCES: ${SDL_RENDERER_SOURCES}")
message(STATUS "GLFW_RENDERER_SOURCES: ${GLFW_RENDERER_SOURCES}")

