set(3RDPARTY
    unzip/crypt.h
    unzip/ioapi.c
    unzip/ioapi.h
    unzip/iowin32.c
    unzip/iowin32.h
    unzip/unzip.c
    unzip/unzip.h
    unzip/zip.c
    unzip/zip.h
#
    zlib/adler32.c
    zlib/compress.c
    zlib/crc32.c
    zlib/crc32.h
    zlib/deflate.c
    zlib/deflate.h
    zlib/gzio.c
    zlib/infback.c
    zlib/inffast.c
    zlib/inffast.h
    zlib/inffixed.h
    zlib/inflate.c
    zlib/inflate.h
    zlib/inftrees.c
    zlib/inftrees.h
    zlib/trees.c
    zlib/trees.h
    zlib/uncompr.c
    zlib/zconf.h
    zlib/zlib.h
    zlib/zutil.c
    zlib/zutil.h
#
    tinyxml/tinystr.cpp
    tinyxml/tinystr.h
    tinyxml/tinyxml.cpp
    tinyxml/tinyxml.h
    tinyxml/tinyxmlerror.cpp
    tinyxml/tinyxmlparser.cpp
#
    pvrtc/PVRTDecompress.cpp
    pvrtc/PVRTDecompress.cpp
    pvrtc/PVRTDecompress.h
)

add_library(3rdparty_libraries STATIC ${3RDPARTY})

target_include_directories(3rdparty_libraries
PUBLIC
   .
   unzip
   zlib
   tinyxml
   pvrtc
)

if(WIN32)
target_include_directories(3rdparty_libraries PUBLIC glew/include)

target_compile_definitions(3rdparty_libraries PRIVATE
                -D_CRT_SECURE_NO_WARNINGS
                -D_UNICODE
                -DGLEW_STATIC
                -DNOMINMAX
)
endif()
