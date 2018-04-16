/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: \\depot\GZL6_TIP\3002\transactive\core\data_access_interface\ats\src\AtsPlannedDataFactory.h $
 * @author:  Lucky CUizon
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2013/06/14 09:50:03 $
 * Last modified by:  $Author: Lucky Cuizon$
 * 
 * AtsPlannedDataFactory is a singleton that is used to set/retrieve planned objects from the database.
 */

#ifndef _PLANNED_TRAIN_DATA_FACTORY_H_
#define _PLANNED_TRAIN_DATA_FACTORY_H_

#include <string>
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_IRS_Core
{
	// Forward declaration to help compiler avoid unnecessary compilation
	class IDatabase;

	typedef struct S_PlannedTrainData
	{
		std::string strDate;
		std::string strTrainID;
		std::string strServiceID;
		std::string strDestinationID;
		std::string strStationID;
		std::string strPlatformID;
		std::string strATime;
		std::string strDTime;
		std::string strFlag;
		std::string strReserved;
	} PlannedTrainDataInfo ;


	class AtsPlannedDataFactory
	{
		
	public:
		virtual ~AtsPlannedDataFactory() {};

		/**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an AtsPlannedDataFactory object.
         */
		static AtsPlannedDataFactory& getInstance();

		/**
         * createPlannedTrainData
         *
         * Create the Planned Train Data.
         *
         * sets the PlannedTrainData
         */
		void createPlannedTrainData(const PlannedTrainDataInfo& info);
	
	private:

		/**
		 * Disable copy, default and assignment operator
		 */
		AtsPlannedDataFactory() {};
		AtsPlannedDataFactory( const AtsPlannedDataFactory& plannedFactory );
		AtsPlannedDataFactory& operator= (const AtsPlannedDataFactory& );

		static AtsPlannedDataFactory* m_Instance;

	};
}

#endif