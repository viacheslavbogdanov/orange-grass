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
)

target_link_directories(SkyCrasher PRIVATE 
                        engine/sdk/OpenGL2/lib
)

target_link_libraries(SkyCrasher Game Engine glew32 opengl32)
