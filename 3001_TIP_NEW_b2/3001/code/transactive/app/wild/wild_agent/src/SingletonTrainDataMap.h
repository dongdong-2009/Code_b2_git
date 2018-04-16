/* 
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/wild/wild_agent/src/SingletonTrainDataMap.h $
 * @author:  <Andy Siow>
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * This class is a singleton map that allows access by the Data Correlation class
 */

#if !defined(AFX_SINGLETONTRAINDATAMAP_H__6AC0A120_07EA_4F7D_9FC1_E57820D81ED9__INCLUDED_)
#define AFX_SINGLETONTRAINDATAMAP_H__6AC0A120_07EA_4F7D_9FC1_E57820D81ED9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "bus/ats/ats_agent/IDL/src/IAtsWildCorbaDef.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include <time.h>
#include <map>

namespace TA_IRS_App
{
	class SingletonTrainDataMap  
	{
	public:
		//
		// getInstance
		//
		// @return Returns the single instance of the DataPoint Cache
		//
		static SingletonTrainDataMap* getInstance();

		//
		// removeInstance
		//
		// Removes the instance of the class (if already created) and cleans up 
		// the DataPoint Cache.
		//
		static void removeInstance();

		//
		// extractAndInsertIntoMap
		//
		// Extracts the ATSTrainDetailCorbaDef, puts the Train ID information into a struct,
		// and inserts it into a map.
		//
		// @param The ATS Train Message CorbaDef
		//
		void extractAndInsertIntoMap(const TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef& ATSTrainMessage);
	

		typedef struct
		{
			unsigned char PVNumber;
			unsigned long ATCCarNumber;
			bool		  IsRevenue;
		} TrainID;


		bool isMapEmpty()
		{ 
			TA_Base_Core::ThreadGuard guard( m_trainDataLock );
			return m_isEmpty;	
		}


		TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef* retrieveTrainInformation(unsigned long WILDTimestamp);
		
	private:
		// Disable the copy constructor and assignment operator so the compiler won't create its own version.
		// Reference: Effective C++
		SingletonTrainDataMap( const SingletonTrainDataMap& SingletonTrainDataMap);
		SingletonTrainDataMap& operator=(const SingletonTrainDataMap&);

		// Thread guard locks
		static TA_Base_Core::NonReEntrantThreadLockable m_trainDataLock;

		SingletonTrainDataMap();
		virtual ~SingletonTrainDataMap();

		std::map<time_t, TrainID*> m_trainDataMap;

		// The singleton's instance..
		static SingletonTrainDataMap* m_instance;

		unsigned int m_maxItems;
		volatile bool m_isEmpty;
	};
}

#endif // !defined(AFX_SINGLETONTRAINDATAMAP_H__6AC0A120_07EA_4F7D_9FC1_E57820D81ED9__INCLUDED_)
