#include <list>
#include "boost/shared_ptr.hpp"
#include "app/ats/AtsAgent/TimetableManager/src/ITimeTableProcessor.h"
#include "app/ats/AtsAgent/TimetableManager/src/DBAccessHelper.h"
#include "app/ats/AtsAgent/TimetableManager/src/TimetableHelper.h"

#include "core/data_access_interface/ats/src/AtsPlannedDataFactory.h"
#include "core/data_access_interface/ats/src/AtsPracticalDataFactory.h"



namespace TA_IRS_App
{	


	class TimetableProcessor : public ITimetableProcessor {
	
	public:
		
		virtual void processPracticalTimetable(std::list<PracticalTrainTimetableInfoPtr>& PracticalTrainDataList);
		virtual void processPlannedTimetable(std::list<PlannedTrainTimetableInfoPtr>& PlannedTrainDataList);
	
	private:
		DBAccessHelper m_dbAccessHelper;
	};

}