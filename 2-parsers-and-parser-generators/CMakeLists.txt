
# find_package(ctpg 1 REQUIRED)
add_executable(value_parser "")
add_executable(value_spirit_parser "")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


target_sources(value_parser
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/value_ctpg.cpp
)

target_sources(value_spirit_parser
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/value_spirit.cpp
)
