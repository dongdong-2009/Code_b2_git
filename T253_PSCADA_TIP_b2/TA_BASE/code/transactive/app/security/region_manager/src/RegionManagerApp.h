#if !defined(AFX_REGIONMANAGERAPP_H__99C223D4_5B94_44BB_A668_E2F5EFF212D6__INCLUDED_)
#define AFX_REGIONMANAGERAPP_H__99C223D4_5B94_44BB_A668_E2F5EFF212D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionManagerApp.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/01/23 17:57:33 $
  * Last modified by:  $Author: builder $
  *
  * This class provides the entry point into the application.
  * It instantiates RegionManagerGUI, but leaves it to the
  * GenericGUI to display.
  *
  */

namespace TA_Base_App
{
    class RegionManagerApp : public TA_Base_Bus::TransActiveWinApp
    {
    // Operations
    public:
        RegionManagerApp();
        ~RegionManagerApp();


    // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(RegionManagerApp)
    protected:
        //}}AFX_VIRTUAL


    // Implementation
    protected:
        //{{AFX_MSG(RegionManagerApp)
	        // NOTE - the ClassWizard will add and remove member functions here.
	        //    DO NOT EDIT what you see in these blocks of generated code !
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_REGIONMANAGERAPP_H__99C223D4_5B94_44BB_A668_E2F5EFF212D6__INCLUDED_)
