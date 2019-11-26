add_library(orangegrass SHARED orangegrass.cpp)

target_link_libraries(orangegrass Game Engine GLESv2 EGL log android)
