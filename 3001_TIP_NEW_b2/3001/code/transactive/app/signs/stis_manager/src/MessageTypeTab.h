/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/MessageTypeTab.h $
  * @author:  Robin Ashcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * The tab control that holds the
  * predefined, adhoc and ratis pages.
  *
  */

#if !defined(AFX_MESSAGETYPETAB_H__7FAD8E32_5931_45C3_AFC6_8E96E4DB0EA1__INCLUDED_)
#define AFX_MESSAGETYPETAB_H__7FAD8E32_5931_45C3_AFC6_8E96E4DB0EA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "bus\mfc_extensions\src\tab_dialog_control\TabDialogCtrl.h"
#include "PredefinedPage.h"
#include "FreeTextPage.h"
#include "stismanager.h"
#include "TisDialogCtrl.h"

namespace TA_IRS_App
{

    class CMessageTypeTab : public CTisDialogCtrl
    {

    public:
	    CMessageTypeTab();
        virtual ~CMessageTypeTab();

		void InitAll();

        /** 
          * setMessageSelectionListener
          *
          * Sets the object to notify of selection changes
          *
          * @param messageSelectionListener
          */
        void setMessageSelectionListener( IMessageSelectionListener* messageSelectionListener);


        /** 
          * getPredefinedMessage
          *
          * Gets the selected predefined message
          *
          * @pre The predefined page must be active
          *      There must be a valid message selected
          */
        TA_Base_Core::PredefinedMessage getPredefinedMessage() const;


        /** 
          * getFreeTextMessage
          *
          * Gets all attributes from the free text page
          *
          * @pre The free text page must be active
          */
	    FreeTextMessage getFreeTextMessage() const;



        /** 
          * getTimeTypeLisener
          *
          * Gets the page that is listening for time type changes.
          *
          * @return  An ITimeTypeListener reference.
          */
        ITimeTypeListener& getTimeTypeLisener();

		//TD 15349
		//zhou yuan++
		bool findAndSelectStationMessage(const std::string& messageName);
		//++zhou yuan

    protected:

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CMessageTypeTab)
	    public:
	    //}}AFX_VIRTUAL

	    //{{AFX_MSG(CMessageTypeTab)
        afx_msg BOOL OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	    //}}AFX_MSG

        DECLARE_MESSAGE_MAP()


    private:
	    CPredefinedPage m_predefinedPage;
	    CFreeTextPage m_freeTextPage;

		bool m_isOcc;
		int m_previousSelect;
    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_MESSAGETYPETAB_H__7FAD8E32_5931_45C3_AFC6_8E96E4DB0EA1__INCLUDED_)
