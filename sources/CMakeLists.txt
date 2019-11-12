cmake_minimum_required(VERSION 3.8)

project(SkyCrasher)

add_subdirectory(skycrasher/game)
add_subdirectory(engine)

add_executable(SkyCrasher WIN32
    skycrasher/launcher/win32/main.cpp
    skycrasher/launcher/win32/main.h
    skycrasher/launcher/win32/resource.h
    skycrasher/launcher/win32/resource.rc
    skycrasher/launcher/win32/resources/airassault.ico
)

target_compile_definitions(SkyCrasher PRIVATE
                -D_CRT_SECURE_NO_WARNINGS
                -D_UNICODE
                -DGLEW_STATIC
                -DNOMINMAX
)

if(${CMAKE_SIZEOF_VOID_P} STREQUAL "8")
    set(GLEWPATH engine/sdk/OpenGL2/lib/x64)
else()
    set(GLEWPATH engine/sdk/OpenGL2/lib/x86)
endif()

target_link_directories(SkyCrasher PRIVATE ${GLEWPATH})

target_link_libraries(SkyCrasher Game Engine glew32s opengl32)

set(SkyCrasherDir $<TARGET_FILE_DIR:SkyCrasher>)

add_custom_command(TARGET SkyCrasher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../assets ${CMAKE_CURRENT_BINARY_DIR}/assets/
)
