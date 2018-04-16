/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: \\depot\GZL6_TIP\3002\transactive\core\data_access_interface\ats\AtsPracticalDataFactory.h $
 * @author:  Lucky CUizon
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2013/06/14 09:50:03 $
 * Last modified by:  $Author: Lucky Cuizon$
 * 
 * AtsPracticalDataFactory is a singleton that is used to set/retrieve planned objects from the database.
 */

#ifndef _PRACTICAL_TRAIN_DATA_FACTORY_H_
#define _PRACTICAL_TRAIN_DATA_FACTORY_H_

#include <string>
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_IRS_Core
{
	// Forward declaration to help compiler avoid unnecessary compilation
	class IDatabase;

	typedef struct S_PracticalTrainData
	{
		std::string strDate;
		std::string strServiceID;
		std::string strTrainID;
		std::string strDestinationID;
		std::string strStationID;
		std::string strPlatformID;
		std::string strATime;
		std::string strDTime;
		std::string strFlag;
		std::string strType;
		std::string strReserved;
	} PracticalTrainDataInfo ;


	class AtsPracticalDataFactory
	{
		
	public:
		virtual ~AtsPracticalDataFactory() {};

		/**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an AtsPracticalDataFactory object.
         */
		static AtsPracticalDataFactory& getInstance();

		/**
         * createPlannedTrainData
         *
         * Create the Planned Train Data.
         *
         * sets the PlannedTrainData
         */
		void createPracticalTrainData(const PracticalTrainDataInfo& info);
	
	private:

		/**
		 * Disable copy, default and assignment operator
		 */
		AtsPracticalDataFactory() {};
		AtsPracticalDataFactory( const AtsPracticalDataFactory& plannedFactory );
		AtsPracticalDataFactory& operator= (const AtsPracticalDataFactory& );

		static AtsPracticalDataFactory* m_Instance;

	};
}

#endif