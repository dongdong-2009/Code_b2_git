

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Mon Mar 26 19:09:02 2018
 */
/* Compiler settings for .\src\ATSBridgeUtil.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IATSIntList,0x4C007031,0x097D,0x499B,0xAD,0x44,0xA4,0x4C,0x03,0xCA,0xA6,0xE1);


MIDL_DEFINE_GUID(IID, IID_IATSStringList,0xE69ED143,0xBFFB,0x45B7,0xB0,0x2E,0x1B,0xB5,0x3F,0x97,0x9A,0x83);


MIDL_DEFINE_GUID(IID, IID_IATSDateTime,0xDAB92A0C,0xC43C,0x4057,0x9A,0x7B,0x78,0xC2,0x05,0xC7,0xD2,0xE6);


MIDL_DEFINE_GUID(IID, IID_IATSEvent,0x1C1E314D,0x182C,0x4ED9,0x91,0x65,0x97,0xCE,0xB4,0xE0,0xE1,0x4F);


MIDL_DEFINE_GUID(IID, IID_IATSEventList,0x1F63D5C7,0x09DB,0x4514,0xA0,0xDB,0xA1,0xCE,0x0B,0x00,0xF5,0xD2);


MIDL_DEFINE_GUID(IID, IID_IATSEventSystem,0xED06D7FD,0x69BE,0x4D64,0x80,0x1A,0x1C,0x9F,0xA0,0xE3,0xCB,0x2D);


MIDL_DEFINE_GUID(IID, IID_IATSEventSystemList,0xBCB1B984,0xE91F,0x4210,0xAA,0x2C,0x95,0x68,0x67,0x13,0xF0,0x22);


MIDL_DEFINE_GUID(IID, IID_IATSFilterCriteria,0x71ADC320,0x6B34,0x4D62,0x86,0xD5,0x2B,0x82,0x03,0x4F,0x88,0xD3);


MIDL_DEFINE_GUID(IID, IID_IATSBridgeUtility,0xCA7A7E62,0x9A1D,0x487C,0xB8,0xB7,0xE8,0xE2,0x4A,0x4D,0x0A,0x26);


MIDL_DEFINE_GUID(IID, IID_IATSEventSubscription,0x68A6F3FD,0x1A22,0x4F98,0xB2,0x9F,0xF1,0xD8,0x96,0x14,0x65,0xF7);


MIDL_DEFINE_GUID(IID, LIBID_ATSBridgeUtilLib,0x56D26B63,0x492F,0x46F9,0xB1,0x6F,0x04,0xC3,0x50,0x3A,0x93,0x76);


MIDL_DEFINE_GUID(CLSID, CLSID_ATSIntList,0x78F5E6DA,0x3BE1,0x4066,0x91,0x1C,0x3E,0x2D,0x5E,0xEC,0x98,0x4F);


MIDL_DEFINE_GUID(CLSID, CLSID_ATSStringList,0x77C5CBB3,0x7529,0x4A6E,0x95,0x22,0x2E,0x28,0x97,0x5F,0xA0,0x43);


MIDL_DEFINE_GUID(CLSID, CLSID_ATSEvent,0xAA1D6690,0x6FDA,0x4B5E,0x98,0x6D,0x93,0x87,0xDB,0x65,0xB8,0x0E);


MIDL_DEFINE_GUID(CLSID, CLSID_ATSDateTime,0x8E89F9A8,0xDBB2,0x4B6B,0xA5,0x24,0xCE,0x6E,0xCE,0x71,0x5E,0xDE);


MIDL_DEFINE_GUID(CLSID, CLSID_ATSEventList,0xC68EFDF1,0x91D1,0x4DB4,0x96,0x98,0xE2,0x8C,0xDE,0xCC,0x69,0xF5);


MIDL_DEFINE_GUID(CLSID, CLSID_ATSEventSystem,0xC988D859,0xE440,0x4797,0xAF,0x37,0x2F,0x32,0xDE,0x98,0x40,0xBF);


MIDL_DEFINE_GUID(CLSID, CLSID_ATSEventSystemList,0x65203707,0xC1FA,0x45AE,0x94,0x92,0xE7,0x7B,0x2C,0xB9,0xD8,0xE7);


MIDL_DEFINE_GUID(CLSID, CLSID_ATSFilterCriteria,0x45A9A577,0x30B8,0x4DC1,0xB3,0xF0,0x73,0x7C,0xCD,0x0E,0x65,0x57);


MIDL_DEFINE_GUID(CLSID, CLSID_ATSBridgeUtility,0x23B16F05,0x40CA,0x47B2,0x8F,0xB4,0x9D,0xE3,0x7C,0x67,0xCA,0xDE);


MIDL_DEFINE_GUID(IID, DIID__IATSEventSubscriptionEvents,0x3EC07762,0xAE4B,0x4B52,0x8D,0x4F,0x9B,0xE3,0x74,0x60,0x14,0x3E);


MIDL_DEFINE_GUID(CLSID, CLSID_ATSEventSubscription,0xDF8AE25F,0x804E,0x48FE,0x9C,0x0D,0x40,0x0F,0x0A,0x25,0xFA,0x49);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



