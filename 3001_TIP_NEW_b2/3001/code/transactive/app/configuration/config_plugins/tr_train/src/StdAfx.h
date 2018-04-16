// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9CDFGDF_9875_43FD_8180_C9DFGFDG9683__INCLUDED_)
#define AFX_STDAFX_H__9CDFGDF_9875_43FD_8180_C9DFGFDG9683__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include "bus\resources\resource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MultipleChangeDialog.h"

#include "app/configuration/config_plugins/tr_train/src/TrTrainComponent.h"
#include "app/configuration/config_plugins/tr_train/src/resource.h"
#include "app/configuration/config_plugins/tr_train/src/TrTrainFrameworkView.h"
#include "app/configuration/config_plugins/tr_train/src/TrTrainDatabaseAccessor.h"
#include "app/configuration/config_plugins/tr_train/src/TrTrainDetailView.h"
#include "app/configuration/config_plugins/tr_train/src/TrTrainListView.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9CDFGDF_9875_43FD_8180_C9DFGFDG9683__INCLUDED_)
