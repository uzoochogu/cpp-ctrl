cmake_minimum_required(VERSION 3.27)
project(cpp-ctrl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# list of modules
set(module_dirs "1-callables-and-callbacks" )


foreach(dir IN LISTS module_dirs)
    # defines targets and sources per module 
    add_subdirectory(${dir})    
endforeach()
