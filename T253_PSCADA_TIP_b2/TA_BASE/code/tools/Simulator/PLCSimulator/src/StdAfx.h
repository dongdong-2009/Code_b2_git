// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9DA8FC80_189F_48C8_B507_403AD201BDBA__INCLUDED_)
#define AFX_STDAFX_H__9DA8FC80_189F_48C8_B507_403AD201BDBA__INCLUDED_

#pragma warning (disable : 4786)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9DA8FC80_189F_48C8_B507_403AD201BDBA__INCLUDED_)
#include <strstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <boost/regex.hpp>
#include <iostream>
#include <tchar.h>
#include <map>
#include <utility>
#include <string>
#include <bitset>
#include <vector>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Log_Msg.h"
#include "ace/Task.h"
#include "ace/ACE.h"
#include <sys/timeb.h>
#include <sys/types.h>
#include <stack>
typedef std::pair<int, int> index;
enum CMDCLASS { READNB, READNW, WRITENB, WRITENW,READCOIL,READ_HOLD_REGISTERS,FAILED, WRITE1B, WRITE1W};
enum EXPTYPE {NOTALLOWACT = 1, NOTALLOWADDR, NOTALLOWVAL};
enum SETTYPE {BYBIT, BYBYTE, BYWORD, BYDWORD};
enum PROTOCALTYPE {C830MODBUS, STANDARDMODBUS};
