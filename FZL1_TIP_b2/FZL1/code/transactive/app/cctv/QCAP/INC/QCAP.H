#ifndef QCAP_H
#define QCAP_H

#ifdef _QCAP_VB_EXPORT

#define QCAP_EXPORT __stdcall

#else // _QCAP_VB_EXPORT

#define QCAP_EXPORT __cdecl

#endif // _QCAP_VB_EXPORT

// LABVIEW 2012 SUPPORT
// 
#ifdef _QCAP_LABVIEW_IMPORT

#define AFX_EXT_API __declspec(dllimport)

#define QCAP_EXPORT __cdecl

typedef void * PVOID;

typedef unsigned int UINT;

typedef unsigned long ULONG;

typedef unsigned long DWORD;

typedef unsigned long long ULONGLONG;

typedef char CHAR;

typedef unsigned char BYTE;

#endif // _QCAP_LABVIEW_IMPORT

// RESULT OF FUNCTION
//
typedef enum {

	QCAP_RS_SUCCESSFUL						= 0x00000000,

	QCAP_RS_ERROR_GENERAL					= 0x00000001,

	QCAP_RS_ERROR_OUT_OF_MEMORY				= 0x00000002,

	QCAP_RS_ERROR_OUT_OF_RESOURCE			= 0x00000003,

	QCAP_RS_ERROR_INVALID_DEVICE			= 0x00000004,

	QCAP_RS_ERROR_INVALID_PARAMETER			= 0x00000005,

	QCAP_RS_ERROR_NON_SUPPORT				= 0x00000006,

	QCAP_RS_ERROR_TIMEOUT					= 0x00000007,

	QCAP_RS_ERROR_INVALID_ANIMATION_SCRIPT	= 0x00000008,

} QRESULT;

// RETURN OF CALLBACK
//
typedef enum {

	QCAP_RT_OK					= 0x00000000,

	QCAP_RT_FAIL				= 0x00000001,

	QCAP_RT_SKIP_RECORD_NUM_00	= 0x00010002, // ONLY FOR PF_VIDEO_PREVIEW_CALLBACK / PF_AUDIO_PREVIEW_CALLBACK / PF_VIDEO_BROADCAST_CLIENT_CALLBACK / PF_AUDIO_BROADCAST_CLIENT_CALLBACK

	QCAP_RT_SKIP_RECORD_NUM_01	= 0x00020002, // ONLY FOR PF_VIDEO_PREVIEW_CALLBACK / PF_AUDIO_PREVIEW_CALLBACK

	QCAP_RT_SKIP_RECORD_NUM_02	= 0x00040002, // ONLY FOR PF_VIDEO_PREVIEW_CALLBACK / PF_AUDIO_PREVIEW_CALLBACK

	QCAP_RT_SKIP_RECORD_NUM_03	= 0x00080002, // ONLY FOR PF_VIDEO_PREVIEW_CALLBACK / PF_AUDIO_PREVIEW_CALLBACK

	QCAP_RT_SKIP_DISPLAY		= 0x00000004, // ONLY FOR PF_VIDEO_PREVIEW_CALLBACK / PF_AUDIO_PREVIEW_CALLBACK / 
											  //
											  //		  PF_VIDEO_BROADCAST_CLIENT_CALLBACK / PF_AUDIO_BROADCAST_CLIENT_CALLBACK / PF_VIDEO_DECODER_BROADCAST_CLIENT_CALLBACK / PF_AUDIO_DECODER_BROADCAST_CLIENT_CALLBACK /
											  //
											  //		  PF_VIDEO_DECODER_FILE_CALLBACK / PF_AUDIO_DECODER_FILE_CALLBACK
											  
	QCAP_RT_SKIP_CLONE_DISPLAY	= 0x00000008, // ONLY FOR PF_VIDEO_PREVIEW_CALLBACK / PF_AUDIO_PREVIEW_CALLBACK

} QRETURN;

// DEVICE ENUMERATION TYPE
//
enum {

	QCAP_ENUM_TYPE_DEVICE_NAME		= 0,

	QCAP_ENUM_TYPE_SERIAL_NUMBER	= 1,
};

// INPUT VIDEO SOURCE
//
enum {

	QCAP_INPUT_TYPE_COMPOSITE	= 0,

	QCAP_INPUT_TYPE_SVIDEO		= 1,

	QCAP_INPUT_TYPE_HDMI		= 2,

	QCAP_INPUT_TYPE_DVI_D		= 3,

	QCAP_INPUT_TYPE_COMPONENTS	= 4,

	QCAP_INPUT_TYPE_YCBCR		= 4,

	QCAP_INPUT_TYPE_DVI_A		= 5,

	QCAP_INPUT_TYPE_RGB			= 5,

	QCAP_INPUT_TYPE_VGA			= 5,

	QCAP_INPUT_TYPE_SDI			= 6,

	QCAP_INPUT_TYPE_AUTO		= 7
};

// INPUT AUDIO SOURCE
//
enum {	

	QCAP_INPUT_TYPE_EMBEDDED_AUDIO			= 0,

	QCAP_INPUT_TYPE_LINE_IN					= 1,

	QCAP_INPUT_TYPE_SOUNDCARD_MICROPHONE	= 2,

	QCAP_INPUT_TYPE_SOUNDCARD_LINE_IN		= 3
};

// VIDEO STANDARD TYPE
//
enum {	

	QCAP_STANDARD_TYPE_NTSC_M		= 0x00000001,

	QCAP_STANDARD_TYPE_NTSC_M_J		= 0x00000002,

	QCAP_STANDARD_TYPE_NTSC_433		= 0x00000004,

	QCAP_STANDARD_TYPE_PAL_M		= 0x00000200,

	QCAP_STANDARD_TYPE_PAL_60		= 0x00000800,

	QCAP_STANDARD_TYPE_PAL_B		= 0x00000010,

	QCAP_STANDARD_TYPE_PAL_D		= 0x00000020,

	QCAP_STANDARD_TYPE_PAL_G		= 0x00000040,

	QCAP_STANDARD_TYPE_PAL_H		= 0x00000080,

	QCAP_STANDARD_TYPE_PAL_I		= 0x00000100,

	QCAP_STANDARD_TYPE_PAL_N		= 0x00000400,

	QCAP_STANDARD_TYPE_PAL_N_COMBO	= 0x00100000,

	QCAP_STANDARD_TYPE_AUTO			= 0x80000000, // DEFAULT, [P.S. SC310N8/N16            (CX2581 PCI) DON'T SUPPORT AUTO STANDARD DETECTION]
												  //
												  //		  [P.S. SC290N4/SC390N4/N8/N16 (AH8400 PCI) DON'T SUPPORT AUTO STANDARD DETECTION]
												  //
												  //		  [P.S. SC2B0N4/SC3B0N4/N8/N16 (TW5864 PCI) DON'T SUPPORT AUTO STANDARD DETECTION]
												  //
												  //		  [P.S. SC3A0N4/N8/N16         (FH8735 PCI) DON'T SUPPORT AUTO STANDARD DETECTION]
};

// VIDEO DEINTERLACE TYPE
//
enum {	

	QCAP_SOFTWARE_DEINTERLACE_TYPE_BLENDING			= 0x00000000, // DEFAULT

	QCAP_SOFTWARE_DEINTERLACE_TYPE_MOTIONAD_APTIVE	= 0x00000001,

	QCAP_SOFTWARE_DEINTERLACE_TYPE_FILTER_TRIANGLE	= 0x00000002,

	QCAP_SOFTWARE_DEINTERLACE_TYPE_MEDICAL_FILTER	= 0x00000003,
};

// COLORSPACE TYPE
//
enum {

	QCAP_COLORSPACE_TYEP_RGB24	= 0,		  //   0xBBGGRR -> R0 G0 B0 R1 G1 B1 R2 G2 B2 ... >>

	QCAP_COLORSPACE_TYEP_BGR24	= 1,		  //   0xRRGGBB -> B0 G0 R0 B1 G1 R1 B2 G2 R2 ... >>

	QCAP_COLORSPACE_TYEP_ARGB32	= 2,		  // 0xAABBGGRR -> R0 G0 B0 A0 R1 G1 B1 A1 R2 G2 B2 A2 ... >>

	QCAP_COLORSPACE_TYEP_ABGR32	= 3,		  // 0xAARRGGBB -> B0 G0 R0 A0 B1 G1 R1 A1 B2 G2 R2 A2 ... >>

	QCAP_COLORSPACE_TYEP_YUY2	= 0x32595559, // 0x32595559 -> MAKEFOURCC('Y', 'U', 'Y', '2')

	QCAP_COLORSPACE_TYEP_UYVY	= 0x59565955, // 0x59565955 -> MAKEFOURCC('U', 'Y', 'V', 'Y')

	QCAP_COLORSPACE_TYEP_YV12	= 0x32315659, // 0x32315659 -> MAKEFOURCC('Y', 'V', '1', '2')

	QCAP_COLORSPACE_TYEP_I420	= 0x30323449, // 0x30323449 -> MAKEFOURCC('I', '4', '2', '0')

	QCAP_COLORSPACE_TYEP_H264	= 0x34363248, // 0x34363248 -> MAKEFOURCC('H', '2', '6', '4')
};

// ENCODER TYPE (RECORD PROPERTY)
//
enum {

	QCAP_ENCODER_TYPE_SOFTWARE			= 0,

	QCAP_ENCODER_TYPE_HARDWARE			= 1, // IT IS ONLY USED FOR HARDWARE COMPRESSION CAPTURE CARD, SUCH AS SC580 & SC590

	QCAP_ENCODER_TYPE_INTEL_MEDIA_SDK	= 2,

	QCAP_ENCODER_TYPE_AMD_STREAM		= 3, // DEVELOPING ...

	QCAP_ENCODER_TYPE_NVIDIA_CUDA		= 4,

	QCAP_ENCODER_TYPE_NVIDIA_NVENC		= 5, // DEVELOPING ...
};

// ENCODER VIDEO FORMAT (RECORD PROPERTY)
//
enum {

	QCAP_ENCODER_FORMAT_H264		= 0,
};

// ENCODER AUDIO FORMAT (RECORD PROPERTY)
//
enum {

	QCAP_ENCODER_FORMAT_PCM			= 0,

	QCAP_ENCODER_FORMAT_AAC			= 1,

	QCAP_ENCODER_FORMAT_AAC_RAW		= 1,

	QCAP_ENCODER_FORMAT_AAC_ADTS	= 2,
};

// RECORD PROFILE (RECORD PROPERTY)
//
enum {

	QCAP_RECORD_PROFILE_BASELINE	= 0, // DEFAULT

	QCAP_RECORD_PROFILE_MAIN		= 1,

	QCAP_RECORD_PROFILE_HIGH		= 2
};

// RECORD LEVEL (RECORD PROPERTY)
//
enum {

	QCAP_RECORD_LEVEL_1		= 10,

	QCAP_RECORD_LEVEL_1B	=  9,

	QCAP_RECORD_LEVEL_11	= 11,

	QCAP_RECORD_LEVEL_12	= 12,

	QCAP_RECORD_LEVEL_13	= 13,

	QCAP_RECORD_LEVEL_2		= 20,

	QCAP_RECORD_LEVEL_21	= 21,

	QCAP_RECORD_LEVEL_22	= 22,

	QCAP_RECORD_LEVEL_3		= 30,

	QCAP_RECORD_LEVEL_31	= 31,

	QCAP_RECORD_LEVEL_32	= 32,

	QCAP_RECORD_LEVEL_4		= 40,

	QCAP_RECORD_LEVEL_41	= 41, // DEFAULT

	QCAP_RECORD_LEVEL_42	= 42,

	QCAP_RECORD_LEVEL_50	= 50,

	QCAP_RECORD_LEVEL_51	= 51,

	QCAP_RECORD_LEVEL_52	= 52,
};

// RECORD ENTROPY (RECORD PROPERTY)
//
enum {

	QCAP_RECORD_ENTROPY_CAVLC	= 0, // DEFAULT

	QCAP_RECORD_ENTROPY_CABAC	= 1,
};

// RECORD COMPLEXITY (RECORD PROPERTY)
//
enum {

	QCAP_RECORD_COMPLEXITY_0	= 0, // BEST SPEED, DEFAULT

	QCAP_RECORD_COMPLEXITY_1	= 1,

	QCAP_RECORD_COMPLEXITY_2	= 2,

	QCAP_RECORD_COMPLEXITY_3	= 3,

	QCAP_RECORD_COMPLEXITY_4	= 4,

	QCAP_RECORD_COMPLEXITY_5	= 5,

	QCAP_RECORD_COMPLEXITY_6	= 6, // BEST QUALITY
};

// RECORD MODE (RECORD PROPERTY)
//
enum {

	QCAP_RECORD_MODE_VBR	= 0,

	QCAP_RECORD_MODE_CBR	= 1,

	QCAP_RECORD_MODE_ABR	= 2,
};

// DOWNSCALE MODE (RECORD PROPERTY)
// 
enum {

	QCAP_DOWNSCALE_MODE_OFF	= 0, // (1920, 1080) * (1 / 1) = (1920, 1080)

	QCAP_DOWNSCALE_MODE_2_3	= 1, // (1920, 1080) * (2 / 3) = (1280,  720)

	QCAP_DOWNSCALE_MODE_1_2	= 2, // (1920, 1080) * (1 / 2) = ( 960,  540)

	QCAP_DOWNSCALE_MODE_1_4	= 3, // (1920, 1080) * (1 / 4) = ( 480,  270)
};

// RECORD FLAG (RECORD PROPERTY)
//
enum {

	QCAP_RECORD_FLAG_FULL		= 0x00000007,

	QCAP_RECORD_FLAG_FILE		= 0x00000001,

	QCAP_RECORD_FLAG_ENCODE		= 0x00000002,

	QCAP_RECORD_FLAG_DISPLAY	= 0x00000004,

	QCAP_RECORD_FLAG_DECODE		= 0x00000008,

	QCAP_RECORD_FLAG_VIDEO_ONLY	= 0x00000010,

	QCAP_RECORD_FLAG_AUDIO_ONLY	= 0x00000020,
};

// BROADCAST FLAG (BROADCAST PROPERTY)
//
enum {

	QCAP_BROADCAST_FLAG_FULL	= 0x00000007,

	QCAP_BROADCAST_FLAG_NETWORK	= 0x00000001,

	QCAP_BROADCAST_FLAG_ENCODE	= 0x00000002,

	QCAP_BROADCAST_FLAG_DISPLAY	= 0x00000004,

	QCAP_BROADCAST_FLAG_DECODE	= 0x00000008,
};

// BROADCAST PROTOCOL (BROADCAST PROPERTY)
//
enum {

	QCAP_BROADCAST_PROTOCOL_UDP	 = 0x00000000,

	QCAP_BROADCAST_PROTOCOL_TCP	 = 0x00000001,

	QCAP_BROADCAST_PROTOCOL_HTTP = 0x00000002,
};

// OSD FONT STYLE (OSD PROPERTY)
//
typedef enum {

    QCAP_FONT_STYLE_REGULAR     = 0x00000000,
    
	QCAP_FONT_STYLE_BOLD        = 0x00000001,
    
	QCAP_FONT_STYLE_ITALIC      = 0x00000002,
    
	QCAP_FONT_STYLE_BOLDITALIC  = 0x00000003,
    
	QCAP_FONT_STYLE_UNDERLINE   = 0x00000004,
    
	QCAP_FONT_STYLE_STRIKEOUT   = 0x00000008,
};

// OSD SEQUENCE STYLE (OSD PROPERTY)
//
typedef enum {

	//  GENERAL DEVICE: FOREMOST -> PREVIEW CALLBACK -> RECORDING (ENCODING) -> AFTERMOST
	//
	// SHARE RECORDING: FOREMOST ->                     RECORDING (ENCODING) -> AFTERMOST
	//
	// SHARE STREAMING: FOREMOST ->                     STREAMING (ENCODING) -> AFTERMOST
	//
	QCAP_SEQUENCE_STYLE_FOREMOST		= 0x00000000,

    QCAP_SEQUENCE_STYLE_BEFORE_ENCODE	= 0x00000001,
    
	QCAP_SEQUENCE_STYLE_AFTERMOST		= 0x00000002,
};

// DECODER TYPE (PLAYBACK PROPERTY)
//
enum {

	QCAP_DECODER_TYPE_SOFTWARE			= 0,

	QCAP_DECODER_TYPE_HARDWARE			= 1, // DEVELOPING ...

	QCAP_DECODER_TYPE_INTEL_MEDIA_SDK	= 2, // DEVELOPING ...

	QCAP_DECODER_TYPE_AMD_STREAM		= 3, // DEVELOPING ...

	QCAP_DECODER_TYPE_NVIDIA_CUDA		= 4, // DEVELOPING ...

	QCAP_DECODER_TYPE_NVIDIA_NVENC		= 5, // DEVELOPING ...
};

// CALLBACK FUNCTION TYPES //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
typedef QRETURN (QCAP_EXPORT *PF_NO_SIGNAL_DETECTED_CALLBACK)( PVOID pDevice /*IN*/, ULONG nVideoInput /*IN*/, ULONG nAudioInput /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_SIGNAL_REMOVED_CALLBACK)( PVOID pDevice /*IN*/, ULONG nVideoInput /*IN*/, ULONG nAudioInput /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_FORMAT_CHANGED_CALLBACK)( PVOID pDevice /*IN*/, ULONG nVideoInput /*IN*/, ULONG nAudioInput /*IN*/, ULONG nVideoWidth /*IN*/, ULONG nVideoHeight /*IN*/, BOOL bVideoIsInterleaved /*IN*/, ULONG nVideoFrameRate /*IN*/, ULONG nAudioChannels /*IN*/, ULONG nAudioBitsPerSample /*IN*/, ULONG nAudioSampleFrequency /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_SNAPSHOT_DONE_CALLBACK)( PVOID pDevice /*IN*/, CHAR * pszFilePathName /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_RECORD_DONE_CALLBACK)( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, CHAR * pszFilePathName /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_SHARE_RECORD_DONE_CALLBACK)( UINT iRecNum /*IN*/, CHAR * pszFilePathName /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_BROADCAST_CLIENT_RECORD_DONE_CALLBACK)( PVOID pClient /*IN*/, CHAR * pszFilePathName /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_BROADCAST_CLIENT_CONNECTED_CALLBACK)( PVOID pClient /*IN*/, ULONG nVideoWidth /*IN*/, ULONG nVideoHeight /*IN*/, BOOL bVideoIsInterleaved /*IN*/, ULONG nVideoFrameRate /*IN*/, ULONG nAudioChannels /*IN*/, ULONG nAudioBitsPerSample /*IN*/, ULONG nAudioSampleFrequency /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_VIDEO_PREVIEW_CALLBACK)( PVOID pDevice /*IN*/, double dSampleTime /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_AUDIO_PREVIEW_CALLBACK)( PVOID pDevice /*IN*/, double dSampleTime /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_VIDEO_HARDWARE_ENCODER_CALLBACK)( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, double dSampleTime /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, BOOL bIsKeyFrame /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_VIDEO_RECORD_CALLBACK)( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, double dSampleTime /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, BOOL bIsKeyFrame /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_AUDIO_RECORD_CALLBACK)( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, double dSampleTime /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_VIDEO_SHARE_RECORD_CALLBACK)( UINT iRecNum /*IN*/, double dSampleTime /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, BOOL bIsKeyFrame /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_AUDIO_SHARE_RECORD_CALLBACK)( UINT iRecNum /*IN*/, double dSampleTime /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_VIDEO_DECODER_SHARE_RECORD_CALLBACK)( UINT iRecNum /*IN*/, double dSampleTime /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_AUDIO_DECODER_SHARE_RECORD_CALLBACK)( UINT iRecNum /*IN*/, double dSampleTime /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_VIDEO_BROADCAST_SERVER_CALLBACK)( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, double dSampleTime /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, BOOL bIsKeyFrame /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_AUDIO_BROADCAST_SERVER_CALLBACK)( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, double dSampleTime /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_VIDEO_DECODER_BROADCAST_SERVER_CALLBACK)( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, double dSampleTime /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_AUDIO_DECODER_BROADCAST_SERVER_CALLBACK)( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, double dSampleTime /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_VIDEO_BROADCAST_CLIENT_CALLBACK)( PVOID pClient /*IN*/, double dSampleTime /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, BOOL bIsKeyFrame /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_AUDIO_BROADCAST_CLIENT_CALLBACK)( PVOID pClient /*IN*/, double dSampleTime /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_VIDEO_DECODER_BROADCAST_CLIENT_CALLBACK)( PVOID pClient /*IN*/, double dSampleTime /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_AUDIO_DECODER_BROADCAST_CLIENT_CALLBACK)( PVOID pClient /*IN*/, double dSampleTime /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_VIDEO_DECODER_FILE_CALLBACK)( PVOID pFile /*IN*/, double dSampleTime /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, PVOID pUserData /*IN*/ );

typedef QRETURN (QCAP_EXPORT *PF_AUDIO_DECODER_FILE_CALLBACK)( PVOID pFile /*IN*/, double dSampleTime /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, PVOID pUserData /*IN*/ );

// SYSTEM FUNCTIONS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_SYSTEM_CONFIGURATION( BOOL  bEnableMultipleUsersAccess = TRUE,

															   BOOL  bEnableVideoPreviewDevice = TRUE,

															   BOOL  bEnableAudioPreviewDevice = TRUE,

															   BOOL  bEnableVideoHardwareMainEncoderDevice = TRUE,

															   BOOL  bEnableVideoHardwareSubEncoderDevice = TRUE,

															   ULONG nAutoInputDetectionTimeout = 3000 ); // 3 SEC

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_QUERY_ENCODER_TYPE_CAP( ULONG nEncoderType );

// DEVICE FUNCTIONS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_DEVICE_ENUMERATION( ULONGLONG * * ppVideoDeviceList /*OUT*/, ULONG * pVideoDeviceSize /*OUT*/, ULONGLONG * * ppVideoEncoderDeviceList /*OUT*/, ULONG * pVideoEncoderDeviceSize /*OUT*/, ULONGLONG * * ppAudioDeviceList /*OUT*/, ULONG * pAudioDeviceSize /*OUT*/, ULONGLONG * * ppAudioEncoderDeviceList /*OUT*/, ULONG * pAudioEncoderDeviceSize /*OUT*/, ULONG nDeviceEnumType = QCAP_ENUM_TYPE_DEVICE_NAME /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE( CHAR * pszDevName /*IN*/, UINT iDevNum /*IN*/, HWND hAttachedWindow /*IN*/, PVOID * ppDevice /*OUT*/, BOOL bThumbDraw = FALSE /*IN*/, BOOL bMaintainAspectRatio = FALSE /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_DESTROY( PVOID pDevice /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_RUN( PVOID pDevice /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_STOP( PVOID pDevice /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_INPUT( PVOID pDevice /*IN*/, ULONG * pInput /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_INPUT( PVOID pDevice /*IN*/, ULONG nInput /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_AUDIO_INPUT( PVOID pDevice /*IN*/, ULONG * pInput /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_AUDIO_INPUT( PVOID pDevice /*IN*/, ULONG nInput /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_STANDARD( PVOID pDevice /*IN*/, ULONG * pStandard /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_STANDARD( PVOID pDevice /*IN*/, ULONG nStandard /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_DEFAULT_FORMAT( PVOID pDevice /*IN*/, ULONG * pWidth /*OUT*/, ULONG * pHeight /*OUT*/, BOOL * pIsInterleaved /*OUT*/, ULONG * pFrameRate /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_DEFAULT_FORMAT( PVOID pDevice /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, BOOL bIsInterleaved /*IN*/, ULONG nFrameRate /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_DEINTERLACE_TYPE( PVOID pDevice /*IN*/, ULONG * pType /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_DEINTERLACE_TYPE( PVOID pDevice /*IN*/, ULONG nType /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_DEINTERLACE( PVOID pDevice /*IN*/, BOOL * pEnable /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_DEINTERLACE( PVOID pDevice /*IN*/, BOOL bEnable /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_BRIGHTNESS( PVOID pDevice /*IN*/, ULONG * pValue /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_BRIGHTNESS( PVOID pDevice /*IN*/, ULONG nValue /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_CONTRAST( PVOID pDevice /*IN*/, ULONG * pValue /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_CONTRAST( PVOID pDevice /*IN*/, ULONG nValue /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_HUE( PVOID pDevice /*IN*/, ULONG * pValue /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_HUE( PVOID pDevice /*IN*/, ULONG nValue /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_SATURATION( PVOID pDevice /*IN*/, ULONG * pValue /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_SATURATION( PVOID pDevice /*IN*/, ULONG nValue /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_SHARPNESS( PVOID pDevice /*IN*/, ULONG * pValue /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_SHARPNESS( PVOID pDevice /*IN*/, ULONG nValue /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_AUDIO_VOLUME( PVOID pDevice /*IN*/, ULONG * pVolume /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_AUDIO_VOLUME( PVOID pDevice /*IN*/, ULONG nVolume /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_DEVICE_CUSTOM_PROPERTY( PVOID pDevice /*IN*/, ULONG nProperty /*IN*/, ULONG * pValue /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_DEVICE_CUSTOM_PROPERTY( PVOID pDevice /*IN*/, ULONG nProperty /*IN*/, ULONG nValue /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_DEVICE_CUSTOM_PROPERTY_EX( PVOID pDevice /*IN*/, ULONG nProperty /*IN*/, BYTE * pValue /*OUT*/, ULONG nBytes /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_DEVICE_CUSTOM_PROPERTY_EX( PVOID pDevice /*IN*/, ULONG nProperty /*IN*/, BYTE * pValue /*IN*/, ULONG nBytes /*IN*/ );

// CALLBACK FUNCTIONS PART.I ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_NO_SIGNAL_DETECTED_CALLBACK( PVOID pDevice /*IN*/, PF_NO_SIGNAL_DETECTED_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_SIGNAL_REMOVED_CALLBACK( PVOID pDevice /*IN*/, PF_SIGNAL_REMOVED_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_FORMAT_CHANGED_CALLBACK( PVOID pDevice /*IN*/, PF_FORMAT_CHANGED_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_SNAPSHOT_DONE_CALLBACK( PVOID pDevice /*IN*/, PF_SNAPSHOT_DONE_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_RECORD_DONE_CALLBACK( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, PF_RECORD_DONE_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_SHARE_RECORD_DONE_CALLBACK( UINT iRecNum /*IN*/, PF_SHARE_RECORD_DONE_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_BROADCAST_CLIENT_RECORD_DONE_CALLBACK( PVOID pClient /*IN*/, PF_BROADCAST_CLIENT_RECORD_DONE_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_BROADCAST_CLIENT_CONNECTED_CALLBACK( PVOID pClient /*IN*/, PF_BROADCAST_CLIENT_CONNECTED_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

// CALLBACK FUNCTIONS PART.II ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_VIDEO_PREVIEW_CALLBACK( PVOID pDevice /*IN*/, PF_VIDEO_PREVIEW_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_AUDIO_PREVIEW_CALLBACK( PVOID pDevice /*IN*/, PF_AUDIO_PREVIEW_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_VIDEO_HARDWARE_ENCODER_CALLBACK( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, PF_VIDEO_HARDWARE_ENCODER_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

// CALLBACK FUNCTIONS PART.III //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_VIDEO_RECORD_CALLBACK( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, PF_VIDEO_RECORD_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_AUDIO_RECORD_CALLBACK( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, PF_AUDIO_RECORD_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_VIDEO_SHARE_RECORD_CALLBACK( UINT iRecNum /*IN*/, PF_VIDEO_SHARE_RECORD_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_AUDIO_SHARE_RECORD_CALLBACK( UINT iRecNum /*IN*/, PF_AUDIO_SHARE_RECORD_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_VIDEO_DECODER_SHARE_RECORD_CALLBACK( UINT iRecNum /*IN*/, PF_VIDEO_DECODER_SHARE_RECORD_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_AUDIO_DECODER_SHARE_RECORD_CALLBACK( UINT iRecNum /*IN*/, PF_AUDIO_DECODER_SHARE_RECORD_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_VIDEO_BROADCAST_SERVER_CALLBACK( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, PF_VIDEO_BROADCAST_SERVER_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_AUDIO_BROADCAST_SERVER_CALLBACK( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, PF_AUDIO_BROADCAST_SERVER_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_VIDEO_DECODER_BROADCAST_SERVER_CALLBACK( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, PF_VIDEO_DECODER_BROADCAST_SERVER_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_AUDIO_DECODER_BROADCAST_SERVER_CALLBACK( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, PF_AUDIO_DECODER_BROADCAST_SERVER_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_VIDEO_BROADCAST_CLIENT_CALLBACK( PVOID pClient /*IN*/, PF_VIDEO_BROADCAST_CLIENT_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_AUDIO_BROADCAST_CLIENT_CALLBACK( PVOID pClient /*IN*/, PF_AUDIO_BROADCAST_CLIENT_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_VIDEO_DECODER_BROADCAST_CLIENT_CALLBACK( PVOID pClient /*IN*/, PF_VIDEO_DECODER_BROADCAST_CLIENT_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_AUDIO_DECODER_BROADCAST_CLIENT_CALLBACK( PVOID pClient /*IN*/, PF_AUDIO_DECODER_BROADCAST_CLIENT_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_VIDEO_DECODER_FILE_CALLBACK( PVOID pFile /*IN*/, PF_VIDEO_DECODER_FILE_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_REGISTER_AUDIO_DECODER_FILE_CALLBACK( PVOID pFile /*IN*/, PF_AUDIO_DECODER_FILE_CALLBACK pCB /*IN*/, PVOID pUserData /*IN*/ );

// SNAPSHOT FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SNAPSHOT_BMP( PVOID pDevice /*IN*/, CHAR * pszFilePathName /*IN*/, BOOL bIsAsync = TRUE /*IN*/, ULONG nMilliseconds = 0 /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SNAPSHOT_JPG( PVOID pDevice /*IN*/, CHAR * pszFilePathName /*IN*/, ULONG nQuality /*IN*/, BOOL bIsAsync = TRUE /*IN*/, ULONG nMilliseconds = 0 /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SNAPSHOT_BMP_EX( PVOID pDevice /*IN*/, CHAR * pszFilePathName /*IN*/, ULONG nCropX /*IN*/, ULONG nCropY /*IN*/, ULONG nCropW /*IN*/, ULONG nCropH /*IN*/, ULONG nDstW /*IN*/, ULONG nDstH /*IN*/, BOOL bIsAsync = TRUE /*IN*/, ULONG nMilliseconds = 0 /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SNAPSHOT_JPG_EX( PVOID pDevice /*IN*/, CHAR * pszFilePathName /*IN*/, ULONG nCropX /*IN*/, ULONG nCropY /*IN*/, ULONG nCropW /*IN*/, ULONG nCropH /*IN*/, ULONG nDstW /*IN*/, ULONG nDstH /*IN*/, ULONG nQuality /*IN*/, BOOL bIsAsync = TRUE /*IN*/, ULONG nMilliseconds = 0 /*IN*/ );

// CHANNEL RECORDING FUNCTIONS (SUPPORT MULTIPLE-STREAMS CHANNEL RECORDING) (RECNUM: 0 ~ 3) /////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_RECORD_PROPERTY( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, ULONG * pEncoderType /*OUT*/, ULONG * pEncoderFormat /*OUT*/, ULONG * pRecordMode /*OUT*/, ULONG * pQuality /*OUT*/, ULONG * pBitRate /*OUT*/, ULONG * pGOP /*OUT*/, ULONG * pAspectRatioX /*OUT*/, ULONG * pAspectRatioY /*OUT*/, ULONG * pDownscaleMode /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_RECORD_PROPERTY( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, ULONG   nEncoderType /*IN*/,  ULONG   nEncoderFormat /*IN*/,  ULONG   nRecordMode /*IN*/,  ULONG   nQuality /*IN*/,  ULONG   nBitRate /*IN*/,  ULONG   nGOP /*IN*/,  ULONG   nAspectRatioX /*IN*/,  ULONG   nAspectRatioY /*IN*/,  ULONG   nDownscaleMode /*IN*/  );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_RECORD_PROPERTY_EX( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, ULONG * pEncoderType /*OUT*/, ULONG * pEncoderFormat /*OUT*/, ULONG * pRecordProfile /*OUT*/ /*DEFAULT = BASELINE*/, ULONG * pRecordLevel /*OUT*/ /*DEFAULT = 41*/, ULONG * pRecordEntropy /*OUT*/ /*DEFAULT = CAVLC*/, ULONG * pRecordComplexity /*OUT*/ /*DEFAULT = 0*/, ULONG * pRecordMode /*OUT*/, ULONG * pQuality /*OUT*/, ULONG * pBitRate /*OUT*/, ULONG * pGOP /*OUT*/, ULONG * pBFrames /*OUT*/ /*DEFAULT = 0*/, ULONG * pAspectRatioX /*OUT*/, ULONG * pAspectRatioY /*OUT*/, ULONG * pCropX /*OUT*/, ULONG * pCropY /*OUT*/, ULONG * pCropW /*OUT*/, ULONG * pCropH /*OUT*/, ULONG * pDstW /*OUT*/, ULONG * pDstH /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_RECORD_PROPERTY_EX( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, ULONG   nEncoderType /*IN*/,  ULONG   nEncoderFormat /*IN*/,  ULONG   nRecordProfile /*IN*/  /*DEFAULT = BASELINE*/, ULONG   nRecordLevel /*IN*/  /*DEFAULT = 41*/, ULONG   nRecordEntropy /*IN*/  /*DEFAULT = CAVLC*/, ULONG   nRecordComplexity /*IN*/  /*DEFAULT = 0*/, ULONG   nRecordMode /*IN*/,  ULONG   nQuality /*IN*/,  ULONG   nBitRate /*IN*/,  ULONG   nGOP /*IN*/,  ULONG   nBFrames /*IN*/  /*DEFAULT = 0*/, ULONG   nAspectRatioX /*IN*/,  ULONG   nAspectRatioY /*IN*/,  ULONG   nCropX /*IN*/,  ULONG   nCropY /*IN*/,  ULONG   nCropW /*IN*/,  ULONG   nCropH /*IN*/,  ULONG   nDstW /*IN*/,  ULONG   nDstH /*IN*/  );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_AUDIO_RECORD_PROPERTY( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, ULONG * pEncoderType /*OUT*/, ULONG * pEncoderFormat /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_AUDIO_RECORD_PROPERTY( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, ULONG   nEncoderType /*IN*/,  ULONG   nEncoderFormat /*IN*/  );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_START_RECORD( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, CHAR * pszFilePathName /*IN*/, DWORD dwFlags = QCAP_RECORD_FLAG_FULL /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_STOP_RECORD( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/, BOOL bIsAsync = TRUE /*IN*/, ULONG nMilliseconds = 0 /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_RESUME_RECORD( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_PAUSE_RECORD( PVOID pDevice /*IN*/, UINT iRecNum /*IN*/ );

// CLONE FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_CLONE( PVOID pDevice /*IN*/, HWND hAttachedWindow /*IN*/, PVOID * ppCloneDevice /*OUT*/, BOOL bThumbDraw = FALSE /*IN*/, BOOL bMaintainAspectRatio = FALSE /*IN*/ );

// OSD FUNCTIONS (SUPPORT MULTIPLE-LAYERS OSD OBJECTS) (OSDNUM: 0 ~ 15) /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_OSD_TEXT( PVOID pDevice /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, CHAR * pszString /*IN*/, CHAR * pszFontFamilyName /*IN*/, ULONG nFontStyle /*IN*/, ULONG nFontSize /*IN*/, DWORD dwFontColor /*IN*/, DWORD dwBackgroundColor /*IN*/, ULONG nTransparent /*IN*/, ULONG nSequenceStyle = QCAP_SEQUENCE_STYLE_FOREMOST /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_OSD_PICTURE( PVOID pDevice /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, CHAR * pszFilePathName /*IN*/, ULONG nTransparent /*IN*/, ULONG nSequenceStyle = QCAP_SEQUENCE_STYLE_FOREMOST /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_OSD_BUFFER( PVOID pDevice /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, ULONG nColorSpaceType /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameWidth /*IN*/, ULONG nFrameHeight /*IN*/, ULONG nFramePitch /*IN*/, ULONG nTransparent /*IN*/, ULONG nSequenceStyle = QCAP_SEQUENCE_STYLE_FOREMOST /*IN*/ );

// SHARE RECORDING FUNCTIONS (SUPPORT MULTIPLE-STREAMS SHARE RECORDING) (RECNUM: 0 ~ 15) ////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_SHARE_RECORD_PROPERTY( UINT iRecNum /*IN*/, ULONG * pEncoderType /*OUT*/, ULONG * pEncoderFormat /*OUT*/, ULONG * pColorSpaceType /*OUT*/, ULONG * pWidth /*OUT*/, ULONG * pHeight /*OUT*/, ULONG * pFrameRate /*OUT*/, ULONG * pRecordMode /*OUT*/, ULONG * pQuality /*OUT*/, ULONG * pBitRate /*OUT*/, ULONG * pGOP /*OUT*/, ULONG * pAspectRatioX /*OUT*/, ULONG * pAspectRatioY /*OUT*/, HWND * pAttachedWindow = NULL /*OUT*/, BOOL * pThumbDraw = NULL /*OUT*/, BOOL * pMaintainAspectRatio = NULL /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_SHARE_RECORD_PROPERTY( UINT iRecNum /*IN*/, ULONG   nEncoderType /*IN*/,  ULONG   nEncoderFormat /*IN*/,  ULONG	 nColorSpaceType /*IN*/,  ULONG   nWidth /*IN*/,  ULONG   nHeight /*IN*/,  ULONG   nFrameRate /*IN*/,  ULONG   nRecordMode /*IN*/,  ULONG   nQuality /*IN*/,  ULONG   nBitRate /*IN*/,  ULONG   nGOP /*IN*/,  ULONG   nAspectRatioX /*IN*/,  ULONG   nAspectRatioY /*IN*/,  HWND   hAttachedWindow = NULL /*IN*/,  BOOL   bThumbDraw = FALSE /*IN*/, BOOL   bMaintainAspectRatio = FALSE /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_SHARE_RECORD_PROPERTY_EX( UINT iRecNum /*IN*/, ULONG * pEncoderType /*OUT*/, ULONG * pEncoderFormat /*OUT*/, ULONG * pColorSpaceType /*OUT*/, ULONG * pWidth /*OUT*/, ULONG * pHeight /*OUT*/, ULONG * pFrameRate /*OUT*/, ULONG * pRecordProfile /*OUT*/ /*DEFAULT = BASELINE*/, ULONG * pRecordLevel /*OUT*/ /*DEFAULT = 41*/, ULONG * pRecordEntropy /*OUT*/ /*DEFAULT = CAVLC*/, ULONG * pRecordComplexity /*OUT*/ /*DEFAULT = 0*/, ULONG * pRecordMode /*OUT*/, ULONG * pQuality /*OUT*/, ULONG * pBitRate /*OUT*/, ULONG * pGOP /*OUT*/, ULONG * pBFrames /*OUT*/ /*DEFAULT = 0*/, ULONG * pAspectRatioX /*OUT*/, ULONG * pAspectRatioY /*OUT*/, HWND * pAttachedWindow = NULL /*OUT*/, BOOL * pThumbDraw = NULL /*OUT*/, BOOL * pMaintainAspectRatio = NULL /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_SHARE_RECORD_PROPERTY_EX( UINT iRecNum /*IN*/, ULONG   nEncoderType /*IN*/,  ULONG   nEncoderFormat /*IN*/,  ULONG	nColorSpaceType /*IN*/,  ULONG   nWidth /*IN*/,  ULONG   nHeight /*IN*/,  ULONG   nFrameRate /*IN*/,  ULONG   nRecordProfile /*IN*/  /*DEFAULT = BASELINE*/, ULONG   nRecordLevel /*IN*/  /*DEFAULT = 41*/, ULONG   nRecordEntropy /*IN*/  /*DEFAULT = CAVLC*/, ULONG   nRecordComplexity /*IN*/  /*DEFAULT = 0*/, ULONG   nRecordMode /*IN*/,  ULONG   nQuality /*IN*/,  ULONG   nBitRate /*IN*/,  ULONG   nGOP /*IN*/,  ULONG   nBFrames /*IN*/  /*DEFAULT = 0*/, ULONG   nAspectRatioX /*IN*/,  ULONG   nAspectRatioY /*IN*/,  HWND   hAttachedWindow = NULL /*IN*/,  BOOL   bThumbDraw = FALSE /*IN*/, BOOL   bMaintainAspectRatio = FALSE /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_AUDIO_SHARE_RECORD_PROPERTY( UINT iRecNum /*IN*/, ULONG * pEncoderType /*OUT*/, ULONG * pEncoderFormat /*OUT*/, ULONG * pChannels /*OUT*/, ULONG * pBitsPerSample /*OUT*/, ULONG * pSampleFrequency /*OUT*/, ULONG * pVolume /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_AUDIO_SHARE_RECORD_PROPERTY( UINT iRecNum /*IN*/, ULONG   nEncoderType /*IN*/,  ULONG   nEncoderFormat /*IN*/,  ULONG   nChannels /*IN*/,  ULONG   nBitsPerSample /*IN*/,  ULONG   nSampleFrequency /*IN*/,  ULONG   nVolume /*IN*/  );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_SHARE_RECORD_UNCOMPRESSION_BUFFER( UINT iRecNum /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, double dSampleTime = 0.0 /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_AUDIO_SHARE_RECORD_UNCOMPRESSION_BUFFER( UINT iRecNum /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, double dSampleTime = 0.0 /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_SHARE_RECORD_COMPRESSION_BUFFER( UINT iRecNum /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, ULONG bIsKeyFrame /*IN*/, double dSampleTime = 0.0 /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_AUDIO_SHARE_RECORD_COMPRESSION_BUFFER( UINT iRecNum /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, double dSampleTime = 0.0 /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_START_SHARE_RECORD( UINT iRecNum /*IN*/, CHAR * pszFilePathName /*IN*/, DWORD dwFlags = QCAP_RECORD_FLAG_FULL /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_STOP_SHARE_RECORD( UINT iRecNum /*IN*/, BOOL bIsAsync = TRUE /*IN*/, ULONG nMilliseconds = 0 /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_RESUME_SHARE_RECORD( UINT iRecNum /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_PAUSE_SHARE_RECORD( UINT iRecNum /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_OSD_SHARE_RECORD_TEXT( UINT iRecNum /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, CHAR * pszString /*IN*/, CHAR * pszFontFamilyName /*IN*/, ULONG nFontStyle /*IN*/, ULONG nFontSize /*IN*/, DWORD dwFontColor /*IN*/, DWORD dwBackgroundColor /*IN*/, ULONG nTransparent /*IN*/, ULONG nSequenceStyle = QCAP_SEQUENCE_STYLE_FOREMOST /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_OSD_SHARE_RECORD_PICTURE( UINT iRecNum /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, CHAR * pszFilePathName /*IN*/, ULONG nTransparent /*IN*/, ULONG nSequenceStyle = QCAP_SEQUENCE_STYLE_FOREMOST /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_OSD_SHARE_RECORD_BUFFER( UINT iRecNum /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, ULONG nColorSpaceType /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameWidth /*IN*/, ULONG nFrameHeight /*IN*/, ULONG nFramePitch /*IN*/, ULONG nTransparent /*IN*/, ULONG nSequenceStyle = QCAP_SEQUENCE_STYLE_FOREMOST /*IN*/ );

// MULTIPLE CHANNELS SYNCHRONIZED RECORDING FUNCTIONS (SYNCHRONIZED RECORDING) //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_START_SYNCHRONIZED_RECORD( ULONG nFileArgs /*IN*/, CHAR * pszFilePathName1 /*IN*/, PVOID pDevice1 /*IN*/, UINT iRecNum1 /*IN*/, 
															   
                                                                                        CHAR * pszFilePathName2 /*IN*/, PVOID pDevice2 /*IN*/, UINT iRecNum2 /*IN*/, ... );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_STOP_SYNCHRONIZED_RECORD( BOOL bIsAsync /*IN*/, ULONG nMilliseconds /*IN*/, ULONG nFileArgs /*IN*/, PVOID pDevice1 /*IN*/, UINT iRecNum1 /*IN*/, 
															  
                                                                                                                                         PVOID pDevice2 /*IN*/, UINT iRecNum2 /*IN*/, ... );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_RESUME_SYNCHRONIZED_RECORD( ULONG nFileArgs /*IN*/, PVOID pDevice1 /*IN*/, UINT iRecNum1 /*IN*/, 
															   
                                                                                         PVOID pDevice2 /*IN*/, UINT iRecNum2 /*IN*/, ... );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_PAUSE_SYNCHRONIZED_RECORD( ULONG nFileArgs /*IN*/, PVOID pDevice1 /*IN*/, UINT iRecNum1 /*IN*/, 
															   
                                                                                        PVOID pDevice2 /*IN*/, UINT iRecNum2 /*IN*/, ... );
// SYNCHRONIZED RECORDING EXAMPLE
//
// QCAP_START_SYNCHRONIZED_RECORD( 4, "SR_CH01.MP4", pDevices[ 0 ], 0, 
//
//								      "SR_CH02.MP4", pDevices[ 1 ], 0, 
//
//								      "SR_CH03.MP4", pDevices[ 2 ], 0, 
//
//								      "SR_CH04.MP4", pDevices[ 3 ], 0 );
//
// QCAP_STOP_SYNCHRONIZED_RECORD( TRUE, 0, 4, pDevices[ 0 ], 0, 
//
//										      pDevices[ 1 ], 0, 
//
//										      pDevices[ 2 ], 0, 
//
//										      pDevices[ 3 ], 0 );

// BROADCASTING SERVER FUNCTIONS (SHARE STREAMING TX FUNCTIONS) (SVRNUM: 0 ~ 63) ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_BROADCAST_RTSP_SERVER( UINT iSvrNum /*IN*/, ULONG nTotalSessions /*IN*/, PVOID * ppServer /*OUT*/, CHAR * pszAccount = NULL /*IN*/, CHAR * pszPassword = NULL /*IN*/, ULONG nNetworkPort_RTSP = 554 /*IN*/, ULONG nNetworkPort_RTSPOverHTTP = 0 /*IN 0 = TURN OFF*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_BROADCAST_RTMP_SERVER( UINT iSvrNum /*IN*/, ULONG nTotalSessions /*IN*/, PVOID * ppServer /*OUT*/, CHAR * pszAccount = NULL /*IN*/, CHAR * pszPassword = NULL /*IN*/, ULONG nNetworkPort_RTMP = 1935 /*IN*/, ULONG nNetworkPort_RTMPOverHTTP = 0 /*IN 0 = TURN OFF*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_BROADCAST_RTMP_WEB_PORTAL_SERVER( UINT iSvrNum /*IN*/, CHAR * pszURL, PVOID * ppServer /*OUT*/ ); // TOTOAL SESSION = 1

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_BROADCAST_HLS_SERVER( UINT iSvrNum /*IN*/, ULONG nTotalSessions /*IN*/, PVOID * ppServer /*OUT*/, CHAR * pszWebServerRootFolderPath /*IN*/, CHAR * pszSubFolderPath /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_DESTROY_BROADCAST_SERVER( PVOID pServer /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_START_BROADCAST_SERVER( PVOID pServer /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_STOP_BROADCAST_SERVER( PVOID pServer /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_BROADCAST_SERVER_PROPERTY( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, ULONG * pEncoderType /*OUT*/, ULONG * pEncoderFormat /*OUT*/, ULONG * pColorSpaceType /*OUT*/, ULONG * pWidth /*OUT*/, ULONG * pHeight /*OUT*/, ULONG * pFrameRate /*OUT*/, ULONG * pRecordMode /*OUT*/, ULONG * pQuality /*OUT*/, ULONG * pBitRate /*OUT*/, ULONG * pGOP /*OUT*/, ULONG * pAspectRatioX /*OUT*/, ULONG * pAspectRatioY /*OUT*/, HWND * pAttachedWindow = NULL /*OUT*/, BOOL * pThumbDraw = NULL /*OUT*/, BOOL * pMaintainAspectRatio = NULL /*OUT*/, DWORD * pFlags = NULL /*OUT*/                    );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_BROADCAST_SERVER_PROPERTY( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, ULONG   nEncoderType /*IN*/,  ULONG   nEncoderFormat /*IN*/,  ULONG   nColorSpaceType /*IN*/,  ULONG   nWidth /*IN*/,  ULONG   nHeight /*IN*/,  ULONG   nFrameRate /*IN*/,  ULONG   nRecordMode /*IN*/,  ULONG   nQuality /*IN*/,  ULONG   nBitRate /*IN*/,  ULONG   nGOP /*IN*/,  ULONG   nAspectRatioX /*IN*/,  ULONG   nAspectRatioY /*IN*/,  HWND   hAttachedWindow = NULL /*IN*/,  BOOL   bThumbDraw = FALSE /*IN*/, BOOL   bMaintainAspectRatio = FALSE /*IN*/, DWORD  dwFlags = QCAP_BROADCAST_FLAG_FULL /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_BROADCAST_SERVER_PROPERTY_EX( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, ULONG * pEncoderType /*OUT*/, ULONG * pEncoderFormat /*OUT*/, ULONG * pColorSpaceType /*OUT*/, ULONG * pWidth /*OUT*/, ULONG * pHeight /*OUT*/, ULONG * pFrameRate /*OUT*/, ULONG * pRecordProfile /*OUT*/ /*DEFAULT = BASELINE*/, ULONG * pRecordLevel /*OUT*/ /*DEFAULT = 41*/, ULONG * pRecordEntropy /*OUT*/ /*DEFAULT = CAVLC*/, ULONG * pRecordComplexity /*OUT*/ /*DEFAULT = 0*/, ULONG * pRecordMode /*OUT*/, ULONG * pQuality /*OUT*/, ULONG * pBitRate /*OUT*/, ULONG * pGOP /*OUT*/, ULONG * pBFrames /*OUT*/ /*DEFAULT = 0*/, ULONG * pAspectRatioX /*OUT*/, ULONG * pAspectRatioY /*OUT*/, HWND * pAttachedWindow = NULL /*OUT*/, BOOL * pThumbDraw = NULL /*OUT*/, BOOL * pMaintainAspectRatio = NULL /*OUT*/, DWORD * pFlags = NULL /*OUT*/                    );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_BROADCAST_SERVER_PROPERTY_EX( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, ULONG   nEncoderType /*IN*/,  ULONG   nEncoderFormat /*IN*/,  ULONG   nColorSpaceType /*IN*/,  ULONG   nWidth /*IN*/,  ULONG   nHeight /*IN*/,  ULONG   nFrameRate /*IN*/,  ULONG   nRecordProfile /*IN*/  /*DEFAULT = BASELINE*/, ULONG   nRecordLevel /*IN*/  /*DEFAULT = 41*/, ULONG   nRecordEntropy /*IN*/  /*DEFAULT = CAVLC*/, ULONG   nRecordComplexity /*IN*/  /*DEFAULT = 0*/, ULONG   nRecordMode /*IN*/,  ULONG   nQuality /*IN*/,  ULONG   nBitRate /*IN*/,  ULONG   nGOP /*IN*/,  ULONG   nBFrames /*IN*/  /*DEFAULT = 0*/, ULONG   nAspectRatioX /*IN*/,  ULONG   nAspectRatioY /*IN*/,  HWND   hAttachedWindow = NULL /*IN*/,  BOOL   bThumbDraw = FALSE /*IN*/, BOOL   bMaintainAspectRatio = FALSE /*IN*/, DWORD  dwFlags = QCAP_BROADCAST_FLAG_FULL /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_AUDIO_BROADCAST_SERVER_PROPERTY( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, ULONG * pEncoderType /*OUT*/, ULONG * pEncoderFormat /*OUT*/, ULONG * pChannels /*OUT*/, ULONG * pBitsPerSample /*OUT*/, ULONG * pSampleFrequency /*OUT*/, ULONG * pVolume /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_AUDIO_BROADCAST_SERVER_PROPERTY( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, ULONG   nEncoderType /*IN*/,  ULONG   nEncoderFormat /*IN*/,  ULONG   nChannels /*IN*/,  ULONG   nBitsPerSample /*IN*/,  ULONG   nSampleFrequency /*IN*/,  ULONG   nVolume /*IN*/  );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_BROADCAST_SERVER_UNCOMPRESSION_BUFFER( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, double dSampleTime = 0.0 /*IN*/ /*HLS NEED IT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_AUDIO_BROADCAST_SERVER_UNCOMPRESSION_BUFFER( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, double dSampleTime = 0.0 /*IN*/ /*HLS NEED IT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_BROADCAST_SERVER_COMPRESSION_BUFFER( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, ULONG bIsKeyFrame /*IN*/, double dSampleTime = 0.0 /*IN*/ /*HLS NEED IT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_AUDIO_BROADCAST_SERVER_COMPRESSION_BUFFER( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, BYTE * pStreamBuffer /*IN*/, ULONG nStreamBufferLen /*IN*/, double dSampleTime = 0.0 /*IN*/ /*HLS NEED IT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_OSD_BROADCAST_SERVER_TEXT( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, CHAR * pszString /*IN*/, CHAR * pszFontFamilyName /*IN*/, ULONG nFontStyle /*IN*/, ULONG nFontSize /*IN*/, DWORD dwFontColor /*IN*/, DWORD dwBackgroundColor /*IN*/, ULONG nTransparent /*IN*/, ULONG nSequenceStyle = QCAP_SEQUENCE_STYLE_FOREMOST /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_OSD_BROADCAST_SERVER_PICTURE( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, CHAR * pszFilePathName /*IN*/, ULONG nTransparent /*IN*/, ULONG nSequenceStyle = QCAP_SEQUENCE_STYLE_FOREMOST /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_OSD_BROADCAST_SERVER_BUFFER( PVOID pServer /*IN*/, UINT iSessionNum /*IN*/, UINT iOsdNum /*IN*/, INT x /*IN*/, INT y /*IN*/, INT w /*IN*/, INT h /*IN*/, ULONG nColorSpaceType /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameWidth /*IN*/, ULONG nFrameHeight /*IN*/, ULONG nFramePitch /*IN*/, ULONG nTransparent /*IN*/, ULONG nSequenceStyle = QCAP_SEQUENCE_STYLE_FOREMOST /*IN*/ );

// BROADCASTING RTSP SERVER EXAMPLE:
//
// STEP#01 QCAP_CREATE_BROADCAST_RTSP_SERVER( 0, 4 /*4 CHANNELS*/, &pServer, "root", "root", 554 );
//
//         CH01 URL -> rtsp://root:root@127.0.0.1:554/session0.mpg
//
//         CH02 URL -> rtsp://root:root@127.0.0.1:554/session1.mpg
//
//         CH03 URL -> rtsp://root:root@127.0.0.1:554/session2.mpg
//
//         CH04 URL -> rtsp://root:root@127.0.0.1:554/session3.mpg
// 
// STEP#02 QCAP_SET_VIDEO_BROADCAST_SERVER_PROPERTY( pServer, 0 /*CH01*/, QCAP_ENCODER_TYPE_INTEL_MEDIA_SDK, QCAP_ENCODER_FORMAT_H264, QCAP_COLORSPACE_TYEP_YUY2, 480, 270, 30, QCAP_RECORD_MODE_CBR, 8000, 1000000, 30, 0, 0, NULL, FALSE, QCAP_BROADCAST_FLAG_NETWORK | QCAP_BROADCAST_FLAG_ENCODE );
//
//         QCAP_SET_VIDEO_BROADCAST_SERVER_PROPERTY( pServer, 1 /*CH02*/, QCAP_ENCODER_TYPE_INTEL_MEDIA_SDK, QCAP_ENCODER_FORMAT_H264, QCAP_COLORSPACE_TYEP_YUY2, 480, 270, 30, QCAP_RECORD_MODE_CBR, 8000, 1000000, 30, 0, 0, NULL, FALSE, QCAP_BROADCAST_FLAG_NETWORK | QCAP_BROADCAST_FLAG_ENCODE );
//
//         QCAP_SET_VIDEO_BROADCAST_SERVER_PROPERTY( pServer, 2 /*CH03*/, QCAP_ENCODER_TYPE_INTEL_MEDIA_SDK, QCAP_ENCODER_FORMAT_H264, QCAP_COLORSPACE_TYEP_YUY2, 480, 270, 30, QCAP_RECORD_MODE_CBR, 8000, 1000000, 30, 0, 0, NULL, FALSE, QCAP_BROADCAST_FLAG_NETWORK | QCAP_BROADCAST_FLAG_ENCODE );
//
//         QCAP_SET_VIDEO_BROADCAST_SERVER_PROPERTY( pServer, 3 /*CH04*/, QCAP_ENCODER_TYPE_INTEL_MEDIA_SDK, QCAP_ENCODER_FORMAT_H264, QCAP_COLORSPACE_TYEP_YUY2, 480, 270, 30, QCAP_RECORD_MODE_CBR, 8000, 1000000, 30, 0, 0, NULL, FALSE, QCAP_BROADCAST_FLAG_NETWORK | QCAP_BROADCAST_FLAG_ENCODE );
//
//         QCAP_SET_AUDIO_BROADCAST_SERVER_PROPERTY( pServer, 0 /*CH01*/, QCAP_ENCODER_TYPE_SOFTWARE, QCAP_ENCODER_FORMAT_AAC, 2, 16, 48000, 0 );
//
//         QCAP_SET_AUDIO_BROADCAST_SERVER_PROPERTY( pServer, 1 /*CH02*/, QCAP_ENCODER_TYPE_SOFTWARE, QCAP_ENCODER_FORMAT_AAC, 2, 16, 48000, 0 );
//
//         QCAP_SET_AUDIO_BROADCAST_SERVER_PROPERTY( pServer, 2 /*CH03*/, QCAP_ENCODER_TYPE_SOFTWARE, QCAP_ENCODER_FORMAT_AAC, 2, 16, 48000, 0 );
//
//         QCAP_SET_AUDIO_BROADCAST_SERVER_PROPERTY( pServer, 3 /*CH04*/, QCAP_ENCODER_TYPE_SOFTWARE, QCAP_ENCODER_FORMAT_AAC, 2, 16, 48000, 0 );
//
// STEP#03 QCAP_START_BROADCAST_SERVER( pServer );
//
// STEP#04 QCAP_SET_VIDEO_BROADCAST_SERVER_UNCOMPRESSION_BUFFER( pServer, 0 /*CH01*/, QCAP_COLORSPACE_TYEP_YUY2, 1920, 1080, pFrameBufferA, nFrameBufferLenA );
//
//         QCAP_SET_VIDEO_BROADCAST_SERVER_UNCOMPRESSION_BUFFER( pServer, 1 /*CH02*/, QCAP_COLORSPACE_TYEP_YUY2, 1920, 1080, pFrameBufferB, nFrameBufferLenB );
//
//         QCAP_SET_VIDEO_BROADCAST_SERVER_UNCOMPRESSION_BUFFER( pServer, 2 /*CH03*/, QCAP_COLORSPACE_TYEP_YUY2, 1920, 1080, pFrameBufferC, nFrameBufferLenC );
//
//         QCAP_SET_VIDEO_BROADCAST_SERVER_UNCOMPRESSION_BUFFER( pServer, 3 /*CH04*/, QCAP_COLORSPACE_TYEP_YUY2, 1920, 1080, pFrameBufferD, nFrameBufferLenD );
//
//         QCAP_SET_AUDIO_BROADCAST_SERVER_UNCOMPRESSION_BUFFER( pServer, 0 /*CH01*/, pFrameBufferA, nFrameBufferLenA );
//
//         QCAP_SET_AUDIO_BROADCAST_SERVER_UNCOMPRESSION_BUFFER( pServer, 1 /*CH02*/, pFrameBufferB, nFrameBufferLenB );
//
//         QCAP_SET_AUDIO_BROADCAST_SERVER_UNCOMPRESSION_BUFFER( pServer, 2 /*CH03*/, pFrameBufferC, nFrameBufferLenC );
//
//         QCAP_SET_AUDIO_BROADCAST_SERVER_UNCOMPRESSION_BUFFER( pServer, 3 /*CH04*/, pFrameBufferD, nFrameBufferLenD );
//
// STEP#05 QCAP_STOP_BROADCAST_SERVER( pServer );
//
// STEP#06 QCAP_DRSTROY_BROADCAST_SERVER( pServer );

// BROADCASTING RTMP SERVER EXAMPLE:
//
// STEP#01 QCAP_CREATE_BROADCAST_RTMP_SERVER( 0, 4 /*4 CHANNELS*/, &pServer, NULL, NULL, 1935 );
//
//         CH01 URL -> rtmp://xxx.xxx.xxx.xxx:1935/flvplayback/session0.mpg (HERE, xxx.xxx.xxx.xxx IS YOUR WEB SERVER ADDRESS)
//
//         CH02 URL -> rtmp://xxx.xxx.xxx.xxx:1935/flvplayback/session1.mpg
//
//         CH03 URL -> rtmp://xxx.xxx.xxx.xxx:1935/flvplayback/session2.mpg
//
//         CH04 URL -> rtmp://xxx.xxx.xxx.xxx:1935/flvplayback/session3.mpg

// BROADCASTING HLS SERVER EXAMPLE:
//
// STEP#01 QCAP_CREATE_BROADCAST_HLS_SERVER( 0, 4 /*4 CHANNELS*/, &pServer, "C:\\AppServ\\www\\" "hls\\" ); // HERE, C:\\AppServ\\www\\ IS YOUR WEB SERVER ROOT FOLDER, hls\\ is SUB FOLDER TO SAVE HLS FILES
//
//         CH01 URL -> http://xxx.xxx.xxx.xxx/hls/session0.m3u8 (HERE, xxx.xxx.xxx.xxx IS YOUR WEB SERVER ADDRESS)
//
//         CH02 URL -> http://xxx.xxx.xxx.xxx/hls/session1.m3u8
//
//         CH03 URL -> http://xxx.xxx.xxx.xxx/hls/session2.m3u8
//
//         CH04 URL -> http://xxx.xxx.xxx.xxx/hls/session3.m3u8

// BROADCASTING CLIENT FUNCTIONS (SHARE STREAMING RX FUNCTIONS) (CLINUM: 0 ~ 63) ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_BROADCAST_CLIENT( UINT iCliNum /*IN*/, CHAR * pszURL /*IN*/, PVOID * ppClient /*OUT*/, HWND hAttachedWindow = NULL /*IN*/, BOOL bThumbDraw = FALSE /*IN*/, BOOL bMaintainAspectRatio = FALSE /*IN*/ ); // ONLY FOR RTSP

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_DESTROY_BROADCAST_CLIENT( PVOID pClient /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_START_BROADCAST_CLIENT( PVOID pClient /*IN*/, ULONG nProtocol = QCAP_BROADCAST_PROTOCOL_UDP );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_STOP_BROADCAST_CLIENT( PVOID pClient /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_START_BROADCAST_CLIENT_RECORD( PVOID pClient /*IN*/, CHAR * pszFilePathName /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_STOP_BROADCAST_CLIENT_RECORD( PVOID pClient /*IN*/, BOOL bIsAsync = TRUE /*IN*/, ULONG nMilliseconds = 0 /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_RESUME_BROADCAST_CLIENT_RECORD( PVOID pClient /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_PAUSE_BROADCAST_CLIENT_RECORD( PVOID pClient /*IN*/ );

// BROADCASTING CLIENT EXAMPLE:
//
// STEP#01 QCAP_CREATE_BROADCAST_CLIENT( 0, "rtsp://root:root@127.0.0.1:554/session0.mpg", &pClient, hWindow, TRUE );
//
// STEP#02 QCAP_REGISTER_VIDEO_BROADCAST_CLIENT_CALLBACK( pClient, on_process_video_stream, this );
//
//         QCAP_REGISTER_AUDIO_BROADCAST_CLIENT_CALLBACK( pClient, on_process_audio_stream, this );
//
// STEP#03 QCAP_START_BROADCAST_CLIENT( pClient );
//
// STEP#04 QCAP_STOP_BROADCAST_CLIENT( pClient );
//
// STEP#05 QCAP_DESTROY_BROADCAST_CLIENT( pClient );

// ANIMATION FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_ANIMATION_CLIP( CHAR * pszAniFileName /*IN*/, PVOID * ppClip /*OUT*/, ULONG * pTotalFrames /*OUT*/, ULONG * pTotalSprites /*OUT*/, ULONG * pColorSpaceType /*OUT*/, ULONG * pWidth /*OUT*/, ULONG * pHeight /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_DESTROY_ANIMATION_CLIP( PVOID pClip /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_START_ANIMATION_CLIP( PVOID pClip /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_STOP_ANIMATION_CLIP( PVOID pClip /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_STEP_ANIMATION_CLIP( PVOID pClip /*IN*/, UINT * pFrameNum /*OUT*/, BYTE * * ppFrameBuffer /*OUT*/, ULONG * pFrameBufferLen /*OUT*/, BOOL bClearBackground = TRUE /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SEEK_ANIMATION_CLIP( PVOID pClip /*IN*/, UINT iFrameNum /*IN*/, BYTE * * ppFrameBuffer /*OUT*/, ULONG * pFrameBufferLen /*OUT*/, BOOL bClearBackground = TRUE /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_ANIMATION_CLIP_SPRITE_PICTURE( PVOID pClip /*IN*/, UINT iSpriteNum /*IN*/, CHAR * pszFilePathName /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_ANIMATION_CLIP_SPRITE_BUFFER( PVOID pClip /*IN*/, UINT iSpriteNum /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, BOOL bCloneCopy = FALSE /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_ANIMATION_CLIP_SPRITE_BUFFER_EX( PVOID pClip /*IN*/, UINT iSpriteNum /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, ULONG nCropX /*IN*/, ULONG nCropY /*IN*/, ULONG nCropW /*IN*/, ULONG nCropH /*IN*/, BOOL bCloneCopy = FALSE /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_ANIMATION_CLIP_SPRITE_SOURCE( PVOID pClip /*IN*/, UINT iSpriteNum /*IN*/, PVOID pDevice /*IN*/, ULONG nSequenceStyle = QCAP_SEQUENCE_STYLE_FOREMOST /*IN*/, BOOL bCloneCopy = FALSE /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_ANIMATION_CLIP_SPRITE_SOURCE_EX( PVOID pClip /*IN*/, UINT iSpriteNum /*IN*/, PVOID pDevice /*IN*/, ULONG nCropX /*IN*/, ULONG nCropY /*IN*/, ULONG nCropW /*IN*/, ULONG nCropH /*IN*/, ULONG nSequenceStyle = QCAP_SEQUENCE_STYLE_FOREMOST /*IN*/, BOOL bCloneCopy = FALSE /*IN*/ );

// ANIMATION EXAMPLE:
//
// STEP#01 QCAP_CREATE_ANIMATION_CLIP( "EXAMPLE.FADEOUT.XML", &pClip, &nFrames, &nSprites, &nColorSpaceType, &nWidth, &nHeight );
//
// STEP#02 QCAP_START_ANIMATION_CLIP( pClip );
//
// STEP#03 QCAP_SET_ANIMATION_CLIP_SPRITE_BUFFER( pClip, 0 /*CH01*/, QCAP_COLORSPACE_TYEP_YUY2, 1920, 1080, pFrameBufferA, nFrameBufferLenA );
// 
//         QCAP_SET_ANIMATION_CLIP_SPRITE_SOURCE( pClip, 1 /*CH02*/, QCAP_COLORSPACE_TYEP_YUY2, 1920, 1080, pFrameBufferB, nFrameBufferLenB );
//
// STEP#04 QCAP_STEP_ANIMATION_CLIP( pClip, &iFrameNum, &pFrameBuffer, &nFrameBufferLen, TRUE );
//
// STEP#05 QCAP_STOP_ANIMATION_CLIP( pClip );
//
// STEP#06 QCAP_DESTROY_ANIMATION_CLIP( pClip );

// VIRTUAL CAMERA FUNCTIONS (CAMNUM: 0 ~ 63) ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_CREATE_VIRTUAL_CAMERA( UINT iCamNum /*IN*/, PVOID * ppCamera /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_DESTROY_VIRTUAL_CAMERA( PVOID pCamera /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_VIDEO_VIRTUAL_CAMERA_PROPERTY( PVOID pCamera /*IN*/, ULONG * pColorSpaceType /*OUT*/, ULONG * pWidth /*OUT*/, ULONG * pHeight /*OUT*/, ULONG * pFrameRate /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_VIRTUAL_CAMERA_PROPERTY( PVOID pCamera /*IN*/, ULONG	  nColorSpaceType /*IN*/,  ULONG   nWidth /*IN*/,  ULONG   nHeight /*IN*/,  ULONG   nFrameRate /*IN*/  );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_AUDIO_VIRTUAL_CAMERA_PROPERTY( PVOID pCamera /*IN*/, ULONG * pChannels /*OUT*/, ULONG * pBitsPerSample /*OUT*/, ULONG * pSampleFrequency /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_AUDIO_VIRTUAL_CAMERA_PROPERTY( PVOID pCamera /*IN*/, ULONG   nChannels /*IN*/,  ULONG   nBitsPerSample /*IN*/,  ULONG   nSampleFrequency /*IN*/  );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_VIDEO_VIRTUAL_CAMERA_UNCOMPRESSION_BUFFER( PVOID pCamera /*IN*/, ULONG nColorSpaceType /*IN*/, ULONG nWidth /*IN*/, ULONG nHeight /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, double dSampleTime = 0.0 /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_AUDIO_VIRTUAL_CAMERA_UNCOMPRESSION_BUFFER( PVOID pCamera /*IN*/, BYTE * pFrameBuffer /*IN*/, ULONG nFrameBufferLen /*IN*/, double dSampleTime = 0.0 /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_START_VIRTUAL_CAMERA( PVOID pCamera /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_STOP_VIRTUAL_CAMERA( PVOID pCamera /*IN*/ );

// FILE PLAYBACK/EDITING FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_OPEN_FILE( CHAR * pszFileName /*IN*/, PVOID * ppFile /*OUT*/, ULONG nDecoderType /*IN*/, ULONG * pVideoEncoderFormat /*OUT*/, ULONG * pVideoWidth /*OUT*/, ULONG * pVideoHeight /*OUT*/, double * pVideoFrameRate /*OUT*/, ULONG * pAudioEncoderFormat /*OUT*/, ULONG * pAudioChannels /*OUT*/, ULONG * pAudioBitsPerSample /*OUT*/, ULONG * pAudioSampleFrequency /*OUT*/, double * pTotalDurationTimes /*OUT*/, ULONG * pTotalVideoFrames /*OUT*/, ULONG * pTotalAudioFrames /*OUT*/, HWND hAttachedWindow /*IN*/, BOOL bThumbDraw = FALSE /*IN*/, BOOL bMaintainAspectRatio = FALSE /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_DESTROY_FILE( PVOID pFile /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_PLAY_FILE( PVOID pFile /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_PAUSE_FILE( PVOID pFile /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_STOP_FILE( PVOID pFile /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_FILE_POSITION( PVOID pFile /*IN*/, double * pSampleTime /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_FILE_POSITION( PVOID pFile /*IN*/, double dSampleTime /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_FILE_PLAYBACK_SPEED( PVOID pFile /*IN*/, double * pSpeed /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_FILE_PLAYBACK_SPEED( PVOID pFile /*IN*/, double dSpeed /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_FILE_VIDEO_STREAM_BUFFER( PVOID pFile /*IN*/, UINT iFrameNum /*IN*/, BYTE * pStreamBuffer /*OUT*/, ULONG * pStreamBufferLen /*IN/OUT*/, BOOL * pIsKeyFrame /*OUT*/, double * pSampleTime /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_GET_FILE_AUDIO_STREAM_BUFFER( PVOID pFile /*IN*/, UINT iFrameNum /*IN*/, BYTE * pStreamBuffer /*OUT*/, ULONG * pStreamBufferLen /*IN/OUT*/, double * pSampleTime /*OUT*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_EXPORT_FILE( PVOID pFile /*IN*/, double dStartSampleTime /*IN*/, double dStopSampleTime /*IN*/, CHAR * pszExportedFileName /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_MERGE_FILES( PVOID pFrontEndFile /*IN*/, PVOID pBackEndFile /*IN*/, CHAR * pszMergedFileName /*IN*/ );

// CAMERA FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_CAMERA_ENUMERATION( CHAR * * ppszCameraDevName /*OUT*/, BOOL bNext = FALSE /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_DEFAULT_CAMERA( CHAR * pszCameraDevName /*IN*/ );

// SOUNCARD FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SOUNDCARD_ENUMERATION( CHAR * * ppszSoundCardDevName /*OUT*/, BOOL bNext = FALSE /*IN*/ );

AFX_EXT_API QRESULT QCAP_EXPORT QCAP_SET_DEFAULT_SOUNDCARD( CHAR * pszMicrophoneDevName /*IN*/, CHAR * pszLineInDevName /*IN*/ );

// SOUNCARD EXAMPLE:
// 
// STEP#01 QCAP_CREATE( "SOUNDCARD", 0, hWindow, &pDevice, TRUE );
// 
// STEP#02 QCAP_SET_AUDIO_INPUT( pDevice, QCAP_INPUT_TYPE_SOUNDCARD_LINE_IN );
// 
// STEP#03 QCAP_RUN( pDevice );
// 
// STEP#04 QCAP_STOP( pDevice );
// 
// STEP#05 QCAP_DESTROY( pDevice );

// HELPER FUNCTIONS (FOR .NET DEVELOPER) ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
AFX_EXT_API ULONG QCAP_EXPORT QCAP_HELPER_OBJPTR( PVOID pObj );

#endif // QCAP_H