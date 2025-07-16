set(GDCEF_SHARED_SOURCES
	app.hpp app.cpp
	client.hpp client.cpp
	cef_server.hpp cef_server.cpp
)

add_library(
	gdcef-static
	${GDCEF_SHARED_SOURCES}
)

add_library(
	gdcef SHARED
	register_types.hpp register_types.cpp
	# cef_server.hpp cef_server.cpp
)

add_executable(
	gdcef-subprocess
	main.cpp
)

# CEF
set(USE_SANDBOX OFF)
set(CEF_RUNTIME_LIBRARY_FLAG "/MD")

# godot-cpp
set(GODOTCPP_DEBUG_CRT ON)

include(FetchContent)
FetchContent_Declare(
	CEF
	URL https://cef-builds.spotifycdn.com/cef_binary_138.0.21%2Bg54811fe%2Bchromium-138.0.7204.101_windows64.tar.bz2
	URL_HASH SHA1=7b6599a4f1dc726ac39ea892d06ef7f8860cb256
)
FetchContent_MakeAvailable(CEF)

FetchContent_Declare(
	godot-cpp
	GIT_REPOSITORY https://github.com/godotengine/godot-cpp
	GIT_TAG 4.4
)
FetchContent_MakeAvailable(godot-cpp)

message(STATUS "${GODOTCPP_SYSTEM_HEADERS_ATTRIBUTE}")

target_link_directories(gdcef-static PUBLIC ${cef_SOURCE_DIR}/Release)
target_link_libraries(gdcef-static PUBLIC libcef_dll_wrapper libcef godot-cpp)
target_include_directories(gdcef-static PUBLIC ${cef_SOURCE_DIR} ${CMAKE_SOURCE_DIR})

target_link_libraries(gdcef-subprocess PUBLIC gdcef-static)
target_link_libraries(gdcef PUBLIC gdcef-static)

set_target_properties(
	gdcef-static gdcef-subprocess gdcef
	PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	EXPORT_COMPILE_COMMANDS ON
)


set(CEF_RESOURCE_CONFIG Debug)

add_custom_command(TARGET gdcef-static POST_BUILD
	COMMENT "Copying CEF binaries"
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
		${cef_SOURCE_DIR}/${CEF_RESOURCE_CONFIG}
		${cef_SOURCE_DIR}/Resources
		${CMAKE_SOURCE_DIR}/html
		$<TARGET_FILE_DIR:gdcef-static>
)

add_custom_command(TARGET gdcef-static POST_BUILD
	COMMENT "Adding .gdignore file"
	COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/.gdignore
)

if (WIN32)
	# make it MDd_DynamicDebug
	target_compile_options(
		gdcef-static
		PUBLIC
			/MDd
	)

	target_compile_options(
		gdcef
		PUBLIC
			/MDd
	)
endif()