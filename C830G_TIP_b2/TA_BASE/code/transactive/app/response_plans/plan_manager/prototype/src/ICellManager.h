/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/ICellManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef ___ICellManager_h__
#define ___ICellManager_h__

#pragma warning(disable : 4786)

#include <vector>
#include <string>


struct ICellManager
{
    typedef enum
    {
        CT_NOTEDITABLE,
        CT_TEXT,
        CT_NUMBER,
        CT_SELECTION,
        CT_DECISION
    } CellType;

    typedef std::vector<std::string> CellData;


    virtual CellType GetCellType(int listCtrlId, int item, int subItem) = 0;
    virtual CellData GetCellData(int listCtrlId, int item, int subItem) = 0;
    virtual void UpdateCellValue(int listCtrlId, int item, int subItem, CString value) = 0;
};

#endif
