/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/LibraryVersionListCtrl.cpp $
  * @author Adam Radics
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * This list is formatted to display library versions.
  */

#include "stdafx.h"
#include "LibraryVersionListCtrl.h"

namespace TA_IRS_App
{


    LibraryVersionListCtrl::LibraryVersionListCtrl()
    {
    }


    LibraryVersionListCtrl::~LibraryVersionListCtrl()
    {
    }


    BEGIN_MESSAGE_MAP(LibraryVersionListCtrl, TA_Base_Bus::ListCtrlSelNoFocus)
	    //{{AFX_MSG_MAP(LibraryVersionListCtrl)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void LibraryVersionListCtrl::PreSubclassWindow() 
    {
	    CListCtrl::PreSubclassWindow();

        // set the style
        SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_GRIDLINES );

        setNonSelectableGray(false);
    }


    bool LibraryVersionListCtrl::isSelectable(int rowNumber)
    {
        return false;
    }


} // TA_IRS_App

