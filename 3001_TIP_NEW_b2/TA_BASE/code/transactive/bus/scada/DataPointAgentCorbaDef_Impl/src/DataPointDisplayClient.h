/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointDisplayClient.h $
  * @author  HoaVu
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2012/02/06 16:15:14 $
  * Last modified by : $Author: haijun.li $
  *
  *
  *	Description
  *
  */

#ifndef DATAPOINT_DISPLAY_CLIENT_H
#define DATAPOINT_DISPLAY_CLIENT_H

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>

namespace TA_Base_Core
{
	class StopwatchUtil;
}

namespace TA_Base_Bus
{
	// forward declaration
	class DataPoint;
	class IDataPointAgent;

	class DataPointDisplayClient 
	{
	public:


		DataPointDisplayClient ( unsigned long clientID );
		virtual ~DataPointDisplayClient();

		//
		// operations specific to DataPointDisplayClient
		//


		/**
		  * getObserverID
		  *
		  * Returns the ID of the observer
		  *
		  */	
		unsigned long getClientID() const;

		/**
          * getIdleTime
          *
          * This method returns the number of milliseconds since the last time that
		  * the observer was queried by a remote client for the latest updates of
		  * those DataPoints assigned to the observer
          *
          */
		unsigned long getIdleTime();

	    /**
	      * getCurrrentDisplayUpdates
	      * 
	      * <description>
	      * 
	      * @return TA_Base_Bus::DataPointCorbaDef::DisplayStateSequence* 
	      * 
	      * @exception <exceptions> Optional
	      */
        TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* getCurrrentDisplayUpdates();

	    /**
	      * saveDataPointDisplayUpdateDetails
	      * 
	      * save the current datapoint display update details for this client
	      * 
	      * @return void 
	      * @param : TA_Base_Bus::DataPointCorbaDef::DataPointDisplayState* displayUpdate
	      * 
	      * @exception <exceptions> Optional
	      */
        void saveDataPointDisplayUpdateDetails( boost::shared_ptr<TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState> displayUpdate);

	private:

		unsigned long m_clientID;
		
		TA_Base_Core::StopwatchUtil * m_purgeExistingUpdatesTimer;

        std::vector <TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState *> m_listOfUpdateDetails;
        TA_Base_Core::ReEntrantThreadLockable m_updateDetailsLock;

		DataPointDisplayClient() {};
		DataPointDisplayClient ( const DataPointDisplayClient & theInstance ) {};
		DataPointDisplayClient & operator= ( const DataPointDisplayClient & rhs ) {};
	};
}

#endif

