#include "core/utilities/src/DebugUtil.h"
#include "app/ats/AtsAgent/TimetableManager/src/TimetableProcessor.h"



namespace TA_IRS_App
{

	void TimetableProcessor::processPlannedTimetable(std::list<PlannedTrainTimetableInfoPtr>& PlannedTrainDataList)
	{	
		FUNCTION_ENTRY("processPlannedTimetable");
		
		std::list<PlannedTrainTimetableInfoPtr>::iterator listIterator = PlannedTrainDataList.begin();
		for(;listIterator != PlannedTrainDataList.end(); ++listIterator)
		{
			PlannedTrainDataInfo* PlannedData = (*listIterator).get();
			m_dbAccessHelper.createPlannedTrainData(PlannedData);
		}

		FUNCTION_EXIT;
	}

	void TimetableProcessor::processPracticalTimetable(std::list<PracticalTrainTimetableInfoPtr>& PracticalTrainDataList)
	{
		FUNCTION_ENTRY("processPracticalTimetable");
		std::list<PracticalTrainTimetableInfoPtr>::iterator listIterator = PracticalTrainDataList.begin();
		for(;listIterator != PracticalTrainDataList.end(); ++listIterator)
		{
			PracticalTrainDataInfo* PracticalData = (*listIterator).get();
			m_dbAccessHelper.createPraticalTrainData(PracticalData);
		}

		FUNCTION_EXIT;
	}

}