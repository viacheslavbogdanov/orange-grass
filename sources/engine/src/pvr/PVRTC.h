#pragma once

#include "OpenGL2.h"
#include <cstdio>


// Describes the header of a PVR header-texture
typedef struct PVR_Header_Texture_TAG
{
    unsigned int dwHeaderSize;			/*!< size of the structure */
    unsigned int dwHeight;				/*!< height of surface to be created */
    unsigned int dwWidth;				/*!< width of input surface */
    unsigned int dwMipMapCount;			/*!< number of mip-map levels requested */
    unsigned int dwpfFlags;				/*!< pixel format flags */
    unsigned int dwTextureDataSize;		/*!< Total size in bytes */
    unsigned int dwBitCount;			/*!< number of bits per pixel  */
    unsigned int dwRBitMask;			/*!< mask for red bit */
    unsigned int dwGBitMask;			/*!< mask for green bits */
    unsigned int dwBBitMask;			/*!< mask for blue bits */
    unsigned int dwAlphaBitMask;		/*!< mask for alpha channel */
    unsigned int dwPVR;					/*!< magic number identifying pvr file */
    unsigned int dwNumSurfs;			/*!< the number of surfaces present in the pvr */
} PVR_Texture_Header;


typedef struct TextureMIPLevelData_TAG
{
    unsigned char* pData;
    unsigned int dwDataSize;
    unsigned int dwMIPLevel;
    unsigned int dwSizeX;
    unsigned int dwSizeY;
} TextureMIPLevelData;


typedef struct TextureImageData_TAG
{
    TextureMIPLevelData* pLevels;
    GLenum textureFormat;
    GLenum textureType;
    bool isCompressed;
    unsigned int dwMipLevels;
    unsigned int dwWidth;
    unsigned int dwHeight;
} TextureImageData;


// Loads the whole texture from PVR.
TextureImageData* LoadTextureFromPVR(const char* const filename);
