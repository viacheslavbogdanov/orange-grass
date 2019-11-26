
add_executable(SkyCrasher WIN32
    main.cpp
    main.h
    resource.h
    resource.rc
    resources/airassault.ico
)

target_compile_definitions(SkyCrasher PRIVATE
                -D_CRT_SECURE_NO_WARNINGS
                -D_UNICODE
                -DGLEW_STATIC
                -DNOMINMAX
)

if(${CMAKE_SIZEOF_VOID_P} STREQUAL "8")
    set(GLEWPATH ../../../3rdparty/glew/lib/x64)
else()
    set(GLEWPATH ../../../3rdparty/glew/lib/x86)
endif()

target_link_directories(SkyCrasher PRIVATE ${GLEWPATH})

target_link_libraries(SkyCrasher Game Engine glew32s opengl32)

