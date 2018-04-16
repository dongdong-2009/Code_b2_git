/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/alarm/alarm_list_control/src/columns/ColumnAsset.h $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * This class provides all the information for the asset column.
 */

#if !defined(AFX_ColumnAsset_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
#define AFX_ColumnAsset_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/alarm/alarm_list_control/src/columns/AbstractAlarmColumn.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnIdentifiers.h"

namespace TA_Base_Bus
{

    class ColumnAsset : public AbstractAlarmColumn
	{
	public:

        
        /**
         * Constructor
         *
         * @param int - The width of the column
         * @param bool - Whether the column is currently enabled
         * @param Sort* - The ascending sort for the column. If the sort is NULL then the default sort will be used
         * @param Sort* - The descending sort for the column. If the sort is NULL then the default sort will be used
         * @param IAlarmAction* - The action to perform when the user double-clicks on the column. This can be NULL
         *                        signifying no action.
         */
        ColumnAsset(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction);


        virtual ~ColumnAsset()
        {
        };


        /**
         * getColumnIdentifier
         *
         * @return long - A unique identifier for this column. This is defined in the file ColumnIdentifiers.h
         */
        virtual long getColumnIdentifier() const
        {
            return COL_ASSET;
        }


        /**
         * getColumnHeaderText
         *
         * @return CString - Returns the column header text
         */
        virtual CString getColumnHeaderText() const
        {
            return "Asset";
        }


        /**
         * getItemText
         *
         * @param AlarmDetailCorbaDef* - The alarm to retrieve information for
         * @param DatabaseCache - This object can be used to retrieve database information if necessary
         *
         * @return CString - This returns the text to display in the column for the specified alarm.
         */
		virtual const std::string getItemText(AlarmVectorWrap* pAlarmWrap, unsigned long ulIndex);

    private:
        /**
         * operator= - Hidden as not used
         */
        ColumnAsset& operator=(const ColumnAsset& rhs);

        /**
         * Copy constructor - Hidden as not used
         */
        ColumnAsset(const ColumnAsset&);
 
	};
} //End of TA_Base_Bus


#endif // !defined(AFX_ColumnAsset_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
