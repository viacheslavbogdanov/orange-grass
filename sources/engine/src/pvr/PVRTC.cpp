#include "OrangeGrass.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "OpenGL2.h"
#include "ogresourcefile.h"
#include "PVRTC.h"
#include "PVRTDecompress.h"


#define PT_INDEX    (2)	/*The Punch-through index*/
#define BLK_Y_SIZE 	(4) /*always 4 for all 2D block types*/
#define BLK_X_MAX	(8)	/*Max X dimension for blocks*/
#define BLK_X_2BPP	(8) /*dimensions for the two formats*/
#define BLK_X_4BPP	(4)

#define WRAP_COORD(Val, Size) ((Val) & ((Size)-1))

#define POWER_OF_2(X)   IsPowerOf2(X)

// Define an expression to either wrap or clamp large or small vals to the
// legal coordinate range
#define CLAMP(X, lower, upper)   (OG_MIN(OG_MAX((X),(lower)), (upper)))
#define LIMIT_COORD(Val, Size, AssumeImageTiles) \
      ((AssumeImageTiles)? WRAP_COORD((Val), (Size)): CLAMP((Val), 0, (Size)-1))


// define GLES-specific constants on desktops
#ifndef GL_IMG_texture_compression_pvrtc
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03
#endif


typedef enum iPixelType_TAG
{
    MGLPT_ARGB_4444 = 0x00,
    MGLPT_ARGB_1555,
    MGLPT_RGB_565,
    MGLPT_RGB_555,
    MGLPT_RGB_888,
    MGLPT_ARGB_8888,
    MGLPT_ARGB_8332,
    MGLPT_I_8,
    MGLPT_AI_88,
    MGLPT_1_BPP,
    MGLPT_VY1UY0,
    MGLPT_Y1VY0U,
    MGLPT_PVRTC2,
    MGLPT_PVRTC4,
    MGLPT_PVRTC2_2,
    MGLPT_PVRTC2_4,

    OGL_RGBA_4444= 0x10,
    OGL_RGBA_5551,
    OGL_RGBA_8888,
    OGL_RGB_565,
    OGL_RGB_555,
    OGL_RGB_888,
    OGL_I_8,
    OGL_AI_88,
    OGL_PVRTC2,
    OGL_PVRTC4,
    OGL_PVRTC2_2,
    OGL_PVRTC2_4,

    D3D_DXT1 = 0x20,
    D3D_DXT2,
    D3D_DXT3,
    D3D_DXT4,
    D3D_DXT5,

    D3D_RGB_332,
    D3D_AI_44,
    D3D_LVU_655,
    D3D_XLVU_8888,
    D3D_QWVU_8888,

    //10 bits per channel
    D3D_ABGR_2101010,
    D3D_ARGB_2101010,
    D3D_AWVU_2101010,

    //16 bits per channel
    D3D_GR_1616,
    D3D_VU_1616,
    D3D_ABGR_16161616,

    //HDR formats
    D3D_R16F,
    D3D_GR_1616F,
    D3D_ABGR_16161616F,

    //32 bits per channel
    D3D_R32F,
    D3D_GR_3232F,
    D3D_ABGR_32323232F,

    // Ericsson
    ETC_RGB_4BPP,
    ETC_RGBA_EXPLICIT,
    ETC_RGBA_INTERPOLATED,

    MGLPT_NOTYPE = 0xff
} iPixelType;


const unsigned int PVRTEX_MIPMAP		= (1<<8);		// has mip map levels
const unsigned int PVRTEX_TWIDDLE		= (1<<9);		// is twiddled
const unsigned int PVRTEX_BUMPMAP		= (1<<10);		// has normals encoded for a bump map
const unsigned int PVRTEX_TILING		= (1<<11);		// is bordered for tiled pvr
const unsigned int PVRTEX_CUBEMAP		= (1<<12);		// is a cubemap/skybox
const unsigned int PVRTEX_FALSEMIPCOL	= (1<<13);		//
const unsigned int PVRTEX_VOLUME		= (1<<14);
const unsigned int PVRTEX_PIXELTYPE		= 0xff;			// pixel type is always in the last 16bits of the flags
const unsigned int PVRTEX_IDENTIFIER	= 0x21525650;	// the pvr identifier is the characters 'P','V','R'
const unsigned int PVRTEX_V1_HEADER_SIZE = 44;			// old header size was 44 for identification purposes
const unsigned int PVRTC2_MIN_TEXWIDTH	= 16;
const unsigned int PVRTC2_MIN_TEXHEIGHT	= 8;
const unsigned int PVRTC4_MIN_TEXWIDTH	= 8;
const unsigned int PVRTC4_MIN_TEXHEIGHT	= 8;
const unsigned int ETC_MIN_TEXWIDTH		= 4;
const unsigned int ETC_MIN_TEXHEIGHT	= 4;


static bool CheckPVRTCSupport()
{
    const char *extensions = (const char*)glGetString(GL_EXTENSIONS);
    return (strstr(extensions, "GL_IMG_texture_compression_pvrtc") != 0);
}


// filename			Filename of the .PVR file to load the texture from
// Allows textures to be stored in binary PVR files and loaded in. Loads the whole texture
TextureImageData* LoadTextureFromPVR(const char* const filename)
{
    COGResourceFile TexFile;
    if (!TexFile.OpenForRead(filename)) 
        return NULL;

    PVR_Texture_Header* psPVRHeader = (PVR_Texture_Header*)TexFile.DataPtr();
    const void* const texPtr = NULL;

    // perform checks for old PVR psPVRHeader
    if(psPVRHeader->dwHeaderSize!=sizeof(PVR_Texture_Header))
    {	
        // Header V1
        if(psPVRHeader->dwHeaderSize==PVRTEX_V1_HEADER_SIZE)
        {	
            // react to old psPVRHeader: i.e. fill in numsurfs as this is missing from old header
            OG_LOG_WARNING("LoadTextureFromPVR: this is an old pvr.");
            if(psPVRHeader->dwpfFlags & PVRTEX_CUBEMAP)
            {
                OG_LOG_ERROR("LoadTextureFromPVR: cubmap textures are not supported.");
                return NULL;
            }
        }
        else
        {	
            // not a pvr at all
            OG_LOG_ERROR("LoadTextureFromPVR: not a valid pvr.");
            return NULL;
        }
    }
    else
    {	
        // Header V2
        if(psPVRHeader->dwNumSurfs<1)
        {	
            // encoded with old version of PVRTexTool before zero numsurfs bug found.
            if(psPVRHeader->dwpfFlags & PVRTEX_CUBEMAP)
            {
                OG_LOG_ERROR("LoadTextureFromPVR: cubmap textures are not supported.");
                return NULL;
            }
        }
    }

    GLenum textureFormat = 0;
    GLenum textureType = GL_RGB;

    bool IsPVRTCSupported = CheckPVRTCSupport();
    bool IsCompressedFormatSupported = false, IsCompressedFormat = false;

    /* Only accept untwiddled data UNLESS texture format is PVRTC */
    if ( ((psPVRHeader->dwpfFlags & PVRTEX_TWIDDLE) == PVRTEX_TWIDDLE)
        && ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)!=OGL_PVRTC2)
        && ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)!=OGL_PVRTC4) )
    {
        // We need to load untwiddled textures -- hw will twiddle for us.
        OG_LOG_ERROR("LoadTextureFromPVR: texture should be untwiddled.");
        return NULL;
    }

    unsigned int szElement = 0;
    switch(psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)
    {
    case OGL_RGBA_4444:
        szElement = 2;
        textureFormat = GL_UNSIGNED_SHORT_4_4_4_4;
        textureType = GL_RGBA;
        break;

    case OGL_RGBA_5551:
        szElement = 2;
        textureFormat = GL_UNSIGNED_SHORT_5_5_5_1;
        textureType = GL_RGBA;
        break;

    case OGL_RGBA_8888:
        szElement = 4;
        textureFormat = GL_UNSIGNED_BYTE;
        textureType = GL_RGBA;
        break;

    case OGL_RGB_565:
        szElement = 2;
        textureFormat = GL_UNSIGNED_SHORT_5_6_5;
        textureType = GL_RGB;
        break;

    case OGL_RGB_888:
        szElement = 3;
        textureFormat = GL_UNSIGNED_BYTE;
        textureType = GL_RGB;
        break;

    case OGL_I_8:
        szElement = 1;
        textureFormat = GL_UNSIGNED_BYTE;
        textureType = GL_LUMINANCE;
        break;

    case OGL_AI_88:
        szElement = 2;
        textureFormat = GL_UNSIGNED_BYTE;
        textureType = GL_LUMINANCE_ALPHA;
        break;

    case OGL_PVRTC2:
        if(IsPVRTCSupported)
        {
            IsCompressedFormatSupported = IsCompressedFormat = true;
            textureFormat = psPVRHeader->dwAlphaBitMask==0 ? GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG ;	// PVRTC2
            OG_LOG_INFO("Loading compressed PVRTC2 texture: %s", filename);
        }
        else
        {
            IsCompressedFormatSupported = false;
            IsCompressedFormat = true;
            textureFormat = GL_UNSIGNED_BYTE;
            textureType = GL_RGBA;
        }
        break;

    case OGL_PVRTC4:
        if(IsPVRTCSupported)
        {
            IsCompressedFormatSupported = IsCompressedFormat = true;
            textureFormat = psPVRHeader->dwAlphaBitMask==0 ? GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG ;	// PVRTC4
            OG_LOG_INFO("Loading compressed PVRTC4 texture: %s", filename);
        }
        else
        {
            IsCompressedFormatSupported = false;
            IsCompressedFormat = true;
            textureFormat = GL_UNSIGNED_BYTE;
            textureType = GL_RGBA;
        }
        break;

    // NOT SUPPORTED
    case OGL_RGB_555:
    default:
        OG_LOG_ERROR("LoadTextureFromPVR: pixel type not supported.");
        return NULL;
    }

    char *theTexturePtr = (texPtr ? (char*)texPtr : (char*)psPVRHeader + psPVRHeader->dwHeaderSize);
    char *theTextureToLoad = 0;
    int	nMIPMapLevel;
    int nTextureLevelsNeeded = (psPVRHeader->dwpfFlags & PVRTEX_MIPMAP) ? psPVRHeader->dwMipMapCount : 0;
    unsigned int nSizeX = psPVRHeader->dwWidth, nSizeY = psPVRHeader->dwHeight;
    unsigned int CompressedImageSize = 0;

    TextureImageData* pTexImageData = (TextureImageData*)malloc(sizeof(TextureImageData));
    pTexImageData->dwMipLevels = nTextureLevelsNeeded;
    pTexImageData->dwWidth = nSizeX;
    pTexImageData->dwHeight = nSizeY;
    pTexImageData->textureFormat = textureFormat;
    pTexImageData->textureType = textureType;
    pTexImageData->isCompressed = (IsCompressedFormat && IsCompressedFormatSupported);
    pTexImageData->pLevels = (TextureMIPLevelData*)malloc(sizeof(TextureMIPLevelData) * (pTexImageData->dwMipLevels+1));
    for(nMIPMapLevel = 0; nMIPMapLevel <= nTextureLevelsNeeded; nSizeX=OG_MAX(nSizeX/2, 1), nSizeY=OG_MAX(nSizeY/2, 1), nMIPMapLevel++)
    {
        // Do Alpha-swap if needed
        theTextureToLoad = theTexturePtr;

        unsigned int LevelSize = 0;

        // Load the Texture
        /* If the texture is PVRTC then use GLCompressedTexImage2D */
        if(IsCompressedFormat)
        {
            /* Calculate how many bytes this MIP level occupies */
            if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC2)
            {
                // PVRTC2 case
                CompressedImageSize = ( OG_MAX(nSizeX, PVRTC2_MIN_TEXWIDTH) * OG_MAX(nSizeY, PVRTC2_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount + 7) / 8;
            }
            else
            {
                // PVRTC4 case
                CompressedImageSize = ( OG_MAX(nSizeX, PVRTC4_MIN_TEXWIDTH) * OG_MAX(nSizeY, PVRTC4_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount + 7) / 8;
            }

            LevelSize = CompressedImageSize;

            if((int)nMIPMapLevel >= 0)
            {
                if(IsCompressedFormatSupported)
                {
                    pTexImageData->pLevels[nMIPMapLevel].dwDataSize = CompressedImageSize;
                    pTexImageData->pLevels[nMIPMapLevel].dwMIPLevel = nMIPMapLevel;
                    pTexImageData->pLevels[nMIPMapLevel].dwSizeX = nSizeX;
                    pTexImageData->pLevels[nMIPMapLevel].dwSizeY = nSizeY;
                    pTexImageData->pLevels[nMIPMapLevel].pData = (unsigned char*)malloc(CompressedImageSize);
                    memcpy(pTexImageData->pLevels[nMIPMapLevel].pData, theTextureToLoad, CompressedImageSize);
                }
                else
                {
                    // Convert PVRTC to 32-bit
                    pTexImageData->pLevels[nMIPMapLevel].dwDataSize = nSizeX*nSizeY*4*sizeof(unsigned char);
                    pTexImageData->pLevels[nMIPMapLevel].dwMIPLevel = nMIPMapLevel;
                    pTexImageData->pLevels[nMIPMapLevel].dwSizeX = nSizeX;
                    pTexImageData->pLevels[nMIPMapLevel].dwSizeY = nSizeY;
                    pTexImageData->pLevels[nMIPMapLevel].pData = (unsigned char*)malloc(pTexImageData->pLevels[nMIPMapLevel].dwDataSize);
                    memset(pTexImageData->pLevels[nMIPMapLevel].pData, 0, pTexImageData->pLevels[nMIPMapLevel].dwDataSize);
                    if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC2)
                    {
                        // PVRTC2 case
                        pvr::PVRTDecompressPVRTC(theTextureToLoad, 1, nSizeX, nSizeY, pTexImageData->pLevels[nMIPMapLevel].pData);
                    }
                    else
                    {
                        // PVRTC4 case
                        pvr::PVRTDecompressPVRTC(theTextureToLoad, 0, nSizeX, nSizeY, pTexImageData->pLevels[nMIPMapLevel].pData);
                    }
                }
            }
        }
        else
        {
            LevelSize = (nSizeX * nSizeY * psPVRHeader->dwBitCount + 7) / 8;

            if((int)nMIPMapLevel >= 0)
            {
                /* Load uncompressed texture data */
                pTexImageData->pLevels[nMIPMapLevel].dwDataSize = LevelSize;
                pTexImageData->pLevels[nMIPMapLevel].dwMIPLevel = nMIPMapLevel;
                pTexImageData->pLevels[nMIPMapLevel].dwSizeX = nSizeX;
                pTexImageData->pLevels[nMIPMapLevel].dwSizeY = nSizeY;
                pTexImageData->pLevels[nMIPMapLevel].pData = (unsigned char*)malloc(pTexImageData->pLevels[nMIPMapLevel].dwDataSize);
                memcpy(pTexImageData->pLevels[nMIPMapLevel].pData, theTextureToLoad, pTexImageData->pLevels[nMIPMapLevel].dwDataSize);
            }
        }

        theTexturePtr += LevelSize;
    }

    return pTexImageData; 
}
