add_executable(callables_demo "")
add_executable(canvas_drawer "")

target_sources(callables_demo
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/callables_demo.cpp
)

target_sources(canvas_drawer
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/callables_demo.cpp
)
