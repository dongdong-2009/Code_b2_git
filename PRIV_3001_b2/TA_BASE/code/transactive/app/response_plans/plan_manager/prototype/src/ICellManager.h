/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/ICellManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
