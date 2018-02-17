cmake_minimum_required(VERSION 3.0)

include(thirdparty/PrecompiledHeader.cmake)

project(gexnetpy)

set(library_HEADERS
	include/pch.hpp
)
set(library_SOURCES
	src/pch.cpp
	src/main.cpp
    src/network.cpp	
)

add_subdirectory(thirdparty/pybind11)

pybind11_add_module(gexnetpy ${library_SOURCES} ${library_HEADERS})
target_link_libraries(gexnetpy PUBLIC gexnet)
target_include_directories(gexnetpy
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

add_precompiled_header(gexnetpy pch.hpp SOURCE_CXX src/pch.cpp FORCEINCLUDE)