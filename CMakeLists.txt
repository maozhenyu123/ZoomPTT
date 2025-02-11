cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(ZRadio
	VERSION "1.0.0"
	DESCRIPTION "HAHAHA"
	HOMEPAGE_URL "https://www.zhener.pw"
	LANGUAGES "C" "CXX"
)

option(AH_HA "Build tests" OFF)


set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)


if(WIN32)
	add_definitions(
		"-DOS_WINDOWS"
		"-DNOMINMAX"
		"-DWIN32_LEAN_AND_MEAN"
		"-D_CRT_SECURE_NO_WARNINGS"
			"-DSO_EXCLUSIVEADDRUSE=yes"
	)
else()
	add_definitions(
		"-DOS_UNIX"
		"-DSO_EXCLUSIVEADDRUSE=yes"
	)
endif()

if(MSVC)
	add_compile_options(
		# https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus
		"/Zc:__cplusplus"
	)
endif()

if(APPLE)
    FIND_LIBRARY (CARBON_LIBRARY Carbon.framework)
    MARK_AS_ADVANCED (CARBON_LIBRARY)
endif()

find_package(SFML COMPONENTS system window audio CONFIG REQUIRED)

add_executable(ZRadio
        main.cpp
)
#target_include_directories(ZRadio PRIVATE include/)
target_link_libraries(ZRadio PRIVATE
        sfml-system sfml-window sfml-audio
)