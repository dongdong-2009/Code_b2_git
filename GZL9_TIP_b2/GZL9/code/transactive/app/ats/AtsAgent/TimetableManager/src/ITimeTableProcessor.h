#ifndef _I_TIMETABLE_PROCESSROR_H_
#define _I_TIMETABLE_PROCESSROR_H_

#include "boost/shared_ptr.hpp"
#include "core/data_access_interface/ats/src/AtsPlannedDataFactory.h"
#include "core/data_access_interface/ats/src/AtsPracticalDataFactory.h"
#include "app/ats/AtsAgent/TimetableManager/src/TimetableHelper.h"

class ITimetableProcessor
{

public:
	
	virtual void processPracticalTimetable(std::list<PracticalTrainTimetableInfoPtr>& PracticalTrainDataList) = 0;
	virtual void processPlannedTimetable(std::list<PlannedTrainTimetableInfoPtr>& PlannedTrainDataList) = 0;
};

#endif