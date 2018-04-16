/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/alarm/alarm_store_library/src/AlarmSortingSet.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmSortingSet.h
//  Implementation of the Class AlarmSortingSet
//  Created on:      24-Jan-2004 04:13:19 PM
///////////////////////////////////////////////////////////

#if !defined(AlarmSortingSet_22692CF3_F6CE_42cc_8A1D_7CB6ED2C2560__INCLUDED_)
#define AlarmSortingSet_22692CF3_F6CE_42cc_8A1D_7CB6ED2C2560__INCLUDED_

#include "IAlarmSort.h"
#include <vector>

namespace TA_Base_Bus
{
	/**
	 * This class manages a collection of sort criteria which are applied to alarms to
	 * determine ordering within the index
	 */
	class AlarmSortingSet
	{
		public:
			AlarmSortingSet();
			virtual ~AlarmSortingSet();
		   
			void AlarmSortingSet::addSort(IAlarmSort*  sort);

			IAlarmSort::ECompareResult compareAlarms(const TA_Base_Core::AlarmDetailCorbaDef& alarm1, 
				const TA_Base_Core::AlarmDetailCorbaDef& alarm2) const;
	
			bool operator == (const AlarmSortingSet& sortSet) const;
			
		private:
			bool sortingRequired() const;
		
			// Not cloneable. Hide copy ctr and assignment operator
			AlarmSortingSet(const AlarmSortingSet& theAlarmSortingSet);
			AlarmSortingSet& operator=(const AlarmSortingSet& rhs);
		private:
			/**
			 * List of sorting classes
			 */
			typedef std::vector<IAlarmSort*> SortList;
			SortList    m_alarmSortList;
	};

};
#endif // !defined(AlarmSortingSet_22692CF3_F6CE_42cc_8A1D_7CB6ED2C2560__INCLUDED_)
