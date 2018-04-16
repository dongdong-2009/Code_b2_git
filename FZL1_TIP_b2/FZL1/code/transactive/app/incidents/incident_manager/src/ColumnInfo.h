/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/incidents/incident_manager/src/ColumnInfo.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  ColumnInfo.h
//  Implementation of the Class ColumnInfo
//  Created on:      16-May-2005 12:18:52 PM
//  Original author: San Teo
///////////////////////////////////////////////////////////

#if !defined(ColumnInfo_5BF5878F_56D0_40bd_9606_207614F71947__INCLUDED_)
#define ColumnInfo_5BF5878F_56D0_40bd_9606_207614F71947__INCLUDED_

#include "stdafx.h"
#include "ColumnType.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"

/**
 * Suggested structure to maintain the information for each column
 * @author San Teo
 * @version 1.0
 * @created 16-May-2005 12:18:52 PM
 */
namespace TA_IRS_App
{	
	struct ColumnInfo
	{
	/**
	* The column type
		*/
		ColumnType type;
		/**
		* The width of the column
		*/
		unsigned int width;
		/**
		* The string Id in the resource's string table 
		*/
		unsigned int name;
		/**
		* The way that the column is sorted by
		*/
		TA_Base_Bus::AutoSortListCtrl::ESortType sortBy;
		
	};
}// end TA_IRS_App
#endif // !defined(ColumnInfo_5BF5878F_56D0_40bd_9606_207614F71947__INCLUDED_)