project(rtow)

cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/rtow-bin)

IF(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
ENDIF()

string(TOUPPER ${CMAKE_CXX_COMPILER_ID} CXX_COMPILER_ID_UPPER)
if(${CXX_COMPILER_ID_UPPER} STREQUAL MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /MP")
endif()

IF(WIN32)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	add_definitions(-DNOMINMAX)
	add_definitions(-D_USE_MATH_DEFINES)
ENDIF(WIN32)

set(RTOW_SRC
	camera.h
	hitable.h
	hitable_list.h
	main.cc
	material.h
	ray.h
	sphere.h
	vec3.h
)

add_executable(rtow_ch12 ${RTOW_SRC})
add_executable(rtow_ch12_omp ${RTOW_SRC})

IF(WIN32)
	set_target_properties(rtow_ch12_omp PROPERTIES COMPILE_FLAGS "/openmp")
ENDIF(WIN32)
IF(UNIX AND NOT APPLE)
	set_target_properties(rtow_ch12_omp PROPERTIES COMPILE_FLAGS "-fopenmp")
	set_target_properties(rtow_ch12_omp PROPERTIES LINK_FLAGS "-fopenmp")
ENDIF(UNIX AND NOT APPLE)


add_subdirectory(rtow_ch06_antialiasing)
add_subdirectory(rtow_ch07_diffuse)
add_subdirectory(rtow_ch08_metal)
add_subdirectory(rtow_ch09_dielectrics)
add_subdirectory(rtow_ch10_camera)
add_subdirectory(rtow_ch11_defocus)
