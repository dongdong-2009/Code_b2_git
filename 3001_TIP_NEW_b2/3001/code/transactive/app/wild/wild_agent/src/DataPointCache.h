/* DataPointCache
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/wild/wild_agent/src/DataPointCache.h $
 * @author:  <Andy Siow>
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * This class is responsible for managing the contents of real and virtual datapoints 
 * for the WILD Agent. This class implements the Singleton design pattern.
 */

#if !defined(AFX_DATAPOINTCACHE_H__30C9D7AF_BFFF_4378_81F0_CF4F73C70E54__INCLUDED_)
#define AFX_DATAPOINTCACHE_H__30C9D7AF_BFFF_4378_81F0_CF4F73C70E54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/alarm/src/AlarmReceiver.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/ats/ats_agent/IDL/src/IAtsWildCorbaDef.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

#include <map>
// Forward declarations
namespace TA_Base_Core
{
	class AlarmHelper;
	class StateUpdateMessageSender;
	class ConfigUpdateDetails;
}

namespace TA_Base_Bus
{
	class IDataPointProxy;
}

namespace TA_IRS_App
{
	//Forward declarations
	class DataCorrelation;
	 
	class DataPointCache : public TA_Base_Bus::IEntityUpdateEventProcessor, 
						   public TA_Base_Core::AlarmReceiver
	{
	public:
	
	    //
		// getInstance
		//
		// @return Returns the single instance of the DataPoint Cache
		//
		static DataPointCache* getInstance();

		//
		// removeInstance
		//
		// Removes the instance of the class (if already created) and cleans up 
		// the DataPoint Cache.
		//
		static void removeInstance();

		//
		// insertIntoVirtualList
		//
		// This method inserts the virtual data point into the local map, it uses 2 unique 
		// identifiers i.e. 2 unique keys will point to the same object. 
		//
		// @param The entity key of the data point to be inserted.
		//
		// @param The wheelID which conforms to a predefined naming convention i.e 1101120
		//		  correponds to OCC, train 10, Car 1, Bog 1, Axle 2 and Side A
		// 
		// @param The data point to be inserted
		//
		void insertIntoVirtualDataPointList(unsigned long entityKey, unsigned long wheelID, TA_Base_Bus::DataPoint& dataPoint);

		//
		// updateAndRetrieveDataPoint
		//
		// Method finds the virtual data point, updates its value and returns a pointer to the 
		// data point
		//
		// @return  The pointer to the data point
		//
		// @param	entityKey	The new state of the DataPoint
		//
		// @param The boolean value of the virtual data point to udpate. 
		//
		TA_Base_Bus::DataPoint* updateAndRetrieveDataPoint(unsigned long wildDpKey, unsigned long wheelID, bool value, long timeStamp);

		//
		// initialiseDataPointProxies
		//
		// Initialises the datapoint proxies. 
		//
		// @pre-cond The data point maps have already been initialised.
		//
		// @exception DataException
		// 
		// @exception DatabaseException
		//
		void initialiseDataPointProxies();

	    //
		// processEntityUpdateEvent
		//
		// Process the update messages received from the proxy
		//
		// @param 	entityKey		The entity that has been updated
		//
		// @param 	updateType		The type of update that has occurred
		//
		void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

	    //
		// submitInvalidConfigAlarm
		//
		// Submits an Invalid WILD configuration Alarm
		//
		// @param The description of the alarm
		//
		void submitInvalidConfigAlarm(const std::string& description);

		void closeAlarm( const TA_Base_Core::MessageType& type ); //TD15198 azenitha

		//
		// submitWILDErrorAlarm
		//
		// The helper method which submits a WILD error alarm 
		// train
		//
		// @param The WILD error description to be displayed.
		// 
		void submitWILDErrorAlarm(const std::string& description);

		//
		// receiveSpecialisedMessage
		//
		// Implements the abstract method for the SpecialisedMessageSubscriber base class
		//
		// @param The Comms message to process
		//
		//virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef&  message);


		//
		// updateDataPointConfiguration
		//
		// Updates the configuration paramters for the virtual data points
		//
		// @param	updateEvent	The configuration update  
		//
		void updateDataPointConfiguration(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		//
		// setDataPointsToControlMode
		//
		// Sets the virtual data points in the local map to control mode.
		//
		// @param Sets the data points to control mode if TRUE, otherwise it sets it to monitor mode
		//
		void setDataPointsMode(bool isControl);


		//
		// convertAddressIntoID
		//
		// Converts an address into a unique ID which represents the Train's
		// wheel data point. 
		//
		// @return Returns an unsigned long key to the data point's map.
		//
		// @param The data point's address
		//
		unsigned long convertAddressIntoID(std::string& address);


		//
		// receiveAlarmMessage
		//
		// Receive alarm state updates. i.e in this case, close messages.
		//
		void receiveAlarmMessage( const TA_Base_Core::AlarmMessageCorbaDef& alarmMessage );

		//
		// subscribeForAlarmUpdates
		//
		// Subscribes for alarm updates i.e close messages.
		//
		void subscribeForAlarmUpdates();


		//
		// unsubscribeForAlarmUpdates
		//
		// Unsubscribes for alarm updates.
		//
		void unsubscribeForAlarmUpdates();

        // ss++ TD16650
        unsigned long getProxyEntityKeyFromInhibitMonitorList(unsigned int index);
        unsigned long getCAEntityKeyFromInhibitMonitorList();
        // ++ss

	private:
		// Disable the copy constructor and assignment operator so the compiler won't create its own version.
		// Reference: Effective C++
		DataPointCache( const DataPointCache& DataPointCache);
		DataPointCache& operator=(const DataPointCache &);

		// Thread guard locks
		static TA_Base_Core::NonReEntrantThreadLockable m_virtualDataPointLock;

		// The singleton's instance..
		static DataPointCache* m_instance;

		// The data point containers
		// Note m_virtualDataPointList contains 2 different keys for the same object, and it is important 
		// when iterating through the whole container that we do not perform the same action twice for
		// the same DataPoint object. One key is the data point's entity key and the second key is the
		// search ID which can be easily constructed so as to reduce hits to the data base and minimise
		// configuration dependence.
		std::map<unsigned long, TA_Base_Bus::DataPoint*> m_virtualDataPointList;

		std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*> m_dpProxyMap;
		
		TA_Base_Core::StateUpdateMessageSender* m_stateUpdateSender;
		//TD18093, jianghp
		//TA_Base_Core::AlarmHelper& m_alarmHelper;
		//TD18093, jianghp
		bool m_isControlMode;
		bool m_isInitialStartUp;

		DataCorrelation* m_dataCorrelator;

		bool m_raisedInvalidWILDConfigurationAlarm; //TD15198 azenitha

		// Private Functions
		DataPointCache();
		virtual ~DataPointCache();

		//
		// cleanUp
		//
		// Helper method whichs cleans up the class's objects. 
		//
		void cleanUp();

        // ss++ TD16650
        void unInhibitTrainDatapoints(std::vector<TA_Base_Bus::DataPoint*>& inhibitedList);
        void initialiseIndexToProxyEntityMap();
        unsigned long findProxyEntityKey(std::string proxyName);

        std::map<unsigned long, std::vector<TA_Base_Bus::DataPoint*> > m_alarmInhitedProxyVirtualDpMap;

        std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*> m_dpProxyInhibitMonitorMap;

        std::map<unsigned int, unsigned long> m_indexToProxyEntityMap;

        std::map<std::string, unsigned long> m_entityNameToEntityKeyMap;

        // ++ss
	};
}

#endif // !defined(AFX_DATAPOINTCACHE_H__30C9D7AF_BFFF_4378_81F0_CF4F73C70E54__INCLUDED_)
