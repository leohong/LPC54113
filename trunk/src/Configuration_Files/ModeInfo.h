#ifndef MODEINFO_H
#define MODEINFO_H
// ==============================================================================
// FILE NAME: MODEINFO_H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 19/06/2018, chris written
// --------------------
// ==============================================================================


#include "CommonDef.h"

#define AUDIO_CHANNEL_STATUS_NUMBERS    (5)
#define INPUT_PORT_NUMBERS              (4)
#define AR_SQUARE_PIX                   (0)
#define AR_IS_4_3                       (0x00040003)
#define AR_IS_16_9                      (0x00100009)
#define MODE_FLAG_RB                    (1)
#define MODE_FLAG_420                   (2)

typedef enum
{
    eMODE_TYPE_UK,          // undefined: 0 = mode definition incomplete
    eMODE_TYPE_PALi,        // 50Hz SD   includes SECAM
    eMODE_TYPE_NTSCi,       // 60Hz SD
    eMODE_TYPE_PALp,        // 50Hz SD
    eMODE_TYPE_NTSCp,       // 60Hz SD
    eMODE_TYPE_720p,
    eMODE_TYPE_1080i,
    eMODE_TYPE_1080sfp,     // ie 1080i 48Hz 2:2 cadence Segmented Frame Progressive
    eMODE_TYPE_1080p,
    eMODE_TYPE_2160p,
    eMODE_TYPE_GRAPHIC,     // anything not TV oriented

    eMODE_TYPE_NUMBERS,
} eMODE_TYPE;

typedef enum
{
    ePIX_RPT_1X,
    ePIX_RPT_2X,
    ePIX_RPT_4X,

    ePIX_RPT_NUMBERS,
} ePIX_RPT;

typedef enum
{
    eSYNCPOL_NEG_POL,
    eSYNCPOL_POS_POL,
    eSYNCPOL_NUMBERS
} eSYNCPOL;

typedef enum
{
    eSCANTYPE_PROG_SCAN,
    eSCANTYPE_INTER_SCAN
} eSCANTYPE;

typedef enum
{
    eCOLOR_SAMPLING_RGB444,
    eCOLOR_SAMPLING_YUV422,
    eCOLOR_SAMPLING_YUV444,
    eCOLOR_SAMPLING_YUV420,
    eCOLOR_SAMPLING_NUMBERS
} eCOLOR_SAMPLING;


typedef enum
{
    eCOLOR_RANGE_FULL,
    eCOLOR_RANGE_LIMITED,
    eCOLOR_RANGE_DEFAULT
} eCOLOR_RANGE;

typedef enum
{
    eCOLOR_DEPTH_8,         //color depth 24
    eCOLOR_DEPTH_10,        //color depth 30
    eCOLOR_DEPTH_12,        //color depth 36
    eCOLOR_DEPTH_16,        //color depth 48

    eCOLOR_DEPTH_NUMBERS
} eCOLOR_DEPTH;

typedef enum
{
    eASPECT_RATIO_DEFAULT,
    eASPECT_RATIO_4_3,
    eASPECT_RATIO_16_9,
    eASPECT_RATIO_RESERVED
} eASPECT_RATIO;

typedef struct
{
    BYTE ch_val;
    BYTE cea_val;
} sAUDIOSAMPLESIZE;

//Audio Sample Sizes (CEA-861-F)
enum
{
    CEA_SS_RESERVED,
    CEA_SS_16BIT,
    CEA_SS_18BIT,
    CEA_SS_20BIT,
    CEA_SS_24BIT
};

//Audio Coding TYPE
enum
{
    AUDIO_TYPE_RESERVED,
    AUDIO_TYPE_NLPCM,
    AUDIO_TYPE_LPCM,
    AUDIO_TYPE_HBR,
    AUDIO_TYPE_DSD,
};

static sAUDIOSAMPLESIZE AudioSampleSizeFromHeader[] =
{
    {0x00, CEA_SS_RESERVED},
    {0x01, CEA_SS_RESERVED},
    {0x02, CEA_SS_16BIT},
    {0x03, CEA_SS_20BIT},
    {0x0A, CEA_SS_20BIT},
    {0x0B, CEA_SS_24BIT}
};

// Audio Sample Frequencies (CEA-861-F)
enum
{
    CEA_SF_RESERVED,
    CEA_SF_32000,
    CEA_SF_44100,
    CEA_SF_48000,
    CEA_SF_88200,
    CEA_SF_96000,
    CEA_SF_176400,
    CEA_SF_192000,
    
    CEA_SF_NOT_SUPPORT
};

static BYTE CEAAudioFreqToChannelStatusBits[] =
{
    2,  //default CEA_SF_48000
    3,  //CEA_SF_32000
    0,  //CEA_SF_44100
    2,  //CEA_SF_48000
    8,  //CEA_SF_88200
    10, //CEA_SF_96000
    2,  //CEA_SF_176400 => CEA_SF_48000
    14, //CEA_SF_192000

    /*
    CEA_SF_44100,
    CEA_SF_RESERVED,
    CEA_SF_48000,
    CEA_SF_32000,
    CEA_SF_RESERVED,
    CEA_SF_RESERVED,
    CEA_SF_RESERVED,
    CEA_SF_RESERVED,
    CEA_SF_88200,
    CEA_SF_RESERVED,
    CEA_SF_96000,
    CEA_SF_RESERVED,
    CEA_SF_RESERVED,
    CEA_SF_RESERVED,
    CEA_SF_192000
    */
};

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

typedef struct
{

    DWORD dwPixClock;               // Pixel clock in Hz
    //DWORD ulHFreq;                // Horizontal frequency in Hz
    //DWORD ulVFreq;                // Vertical frequency in Hz * 1000
    WORD wHTol;                     // Total horizontal pixels
    WORD wVTol;                     // Total vertical pixels
    WORD wHAct;                     // Active horizontal pixels
    WORD wVAct;                     // Acive vertical pixels
    WORD wHBack;                    // Horizontal back porch
    WORD wVBack;                    // Vertical back porch
    WORD wHFront;                   // Horizontal Front porch
    WORD wVFront;                   // Vertical Front porch
    WORD wHSync;                    // Horizontal sync width
    WORD wVSync;                    // Vertical sync width;
    BYTE cHPol;                     // Horizontal sync polarity
    BYTE cVPol;                     // Vertical sync polarity
    BYTE cScanType;                 // PROG_SCAN or INTER_SCAN
    //BYTE     ucCEAMode;           // Detected CEA mode (0 = unknown or non-CEA)

} sVIDEO_TIMING;

typedef struct
{
    //---Main Source
    BYTE                cHdmiMode;      // TRUE if HDMI mode or FALSE for DVI mode
    BYTE                cHdcpVersion;   // TRUE if HDCP encrypted
    eCOLOR_SAMPLING     eColorSpace;
    eCOLOR_RANGE        eColorRange;
    eCOLOR_DEPTH        eColorDepth;
    BYTE                cScrambling;
    eASPECT_RATIO       eAspectRatio;
} sVIDEO_FORMAT;

typedef struct
{
    //---Main Source
    BYTE            cStable;        // Input is Stable
    sVIDEO_TIMING   sTimingInfo;
    sVIDEO_FORMAT   sFormatInfo;
} sVIDEO_INFO;

typedef struct
{
    BYTE            cIsAudio;
    BYTE            cAudioType;           // Audio type
    BYTE            cChannelCount;        // Audio channel count
    //BYTE          cSpeakMapping;        // Speaker mapping
    //BYTE          cDownmixLevel;        // Downmix level
    //BYTE          cDownmixPermit;       // Permission to downmix?
    //BYTE          cChannelMask;         // Channel mask
    //BYTE          caChannelStatus[AUDIO_CHANNEL_STATUS_NUMBERS];    // Channel Status bits
    BYTE            cSampleSize;          // Sample size
    BYTE            cSampleFreq;          // Sample frequency
    //BOOL           bMuteSPDIF;            // Mute SPDIF output flag
    //BOOL           bForceSPDIF;           // Force output to use SPDIF as source

} sAUDIO_INFO;

typedef struct
{
    BYTE cVideoInfoChange;
    BYTE cAudioInfoChange;
    BYTE cPortStatusChange;

} sSYSTEM_STATUS;

#pragma pack(pop)

#endif // MODEINFO_H
