/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/SimpleNodeDisplayClient.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *	Description
  *
  */

#ifndef SIMPLENODE_DISPLAY_CLIENT_H
#define SIMPLENODE_DISPLAY_CLIENT_H

#include <map>


#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "bus/scada/datanode_library/src/IDataNodeObserverNotification.h"


namespace TA_Base_Core
{
	class StopwatchUtil;
}

namespace TA_Base_Bus
{
	// forward declaration
	class SimpleNode;
	class IDataNodeAgent;

	class SimpleNodeDisplayClient
	{
	public:


		SimpleNodeDisplayClient ( unsigned long clientID );
		virtual ~SimpleNodeDisplayClient();

		//
		// operations specific to SimpleNodeDisplayClient
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
		  * those DataNodes assigned to the observer
          *
          */
		unsigned long getIdleTime();

	    /**
	      * getCurrrentDisplayUpdates
	      * 
	      * <description>
	      * 
	      * @return TA_Base_Bus::DataNodeCorbaDef::DisplayStateSequence* 
	      * 
	      * @exception <exceptions> Optional
	      */
        TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* getCurrrentDisplayUpdates();

	    /**
	      * saveDataNodeDisplayUpdateDetails
	      * 
	      * save the current datanode display update details for this client
	      * 
	      * @return void 
	      * @param : TA_Base_Bus::DataNodeCorbaDef::DataNodeDisplayState* displayUpdate
	      * 
	      * @exception <exceptions> Optional
	      */
        void saveDataNodeDisplayUpdateDetails( boost::shared_ptr<TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState> displayUpdate);

	private:

		unsigned long m_clientID;
		
		TA_Base_Core::StopwatchUtil * m_purgeExistingUpdatesTimer;

        std::vector <TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState *> m_listOfUpdateDetails;
        TA_Base_Core::ReEntrantThreadLockable m_updateDetailsLock;

		SimpleNodeDisplayClient() {};
		SimpleNodeDisplayClient ( const SimpleNodeDisplayClient & theInstance ) {};
		SimpleNodeDisplayClient & operator= ( const SimpleNodeDisplayClient & rhs ) {};
	};
}

#endif

