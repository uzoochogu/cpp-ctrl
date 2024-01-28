
# find_package(ctpg 1 REQUIRED)
add_executable(value_parser "")


target_sources(value_parser
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/value_ctpg.cpp
)