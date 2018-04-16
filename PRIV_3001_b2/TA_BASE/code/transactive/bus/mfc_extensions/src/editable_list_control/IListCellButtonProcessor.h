/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/mfc_extensions/src/editable_list_control/IListCellButtonProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#ifndef ___IListCellButtonProcessorh__
#define ___IListCellButtonProcessorh__

#pragma warning(disable : 4786)

#include "ICellManager.h"

// This guard is so that this MFC extension can be used in another
// MFC extension DLL
//

class CEditableListCtrl;

//class IListCellButtonProcessor
#ifdef IMPORT_EXTENSIONS
struct __declspec(dllimport) IListCellButtonProcessor
#else
struct AFX_EXT_CLASS IListCellButtonProcessor
#endif
{
public: 

    virtual ~IListCellButtonProcessor() {}

    // Process the button click from the ListCellButton
    virtual void processButtonClick(int item, int subItem, CEditableListCtrl* parent, ICellManager* manager) = 0;

};

#endif
