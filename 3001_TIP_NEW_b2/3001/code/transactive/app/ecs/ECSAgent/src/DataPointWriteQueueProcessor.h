/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/DataPointWriteQueueProcessor.h $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2012/02/06 16:15:14 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  *	The DataPointWriteQueueProcessor object is responsible for processing write data commands to the PMS RTU
  *
  */

#ifndef DataPointWriteQueueProcessor_H
#define DataPointWriteQueueProcessor_H

#include <map>
#include "core/threads/src/Thread.h"
#include "bus/scada/datapoint_library/src/DataPointWriteQueue.h"
#include "bus/scada/common_library/src/EventProcessor.h"

namespace TA_Base_Bus
{
	class DataPoint;
}

class ItaEcsCachedConfig;


class DataPointWriteQueueProcessor : public EventProcessor<TA_Base_Bus::DataPointWriteRequest>
{
private:

	DataPointWriteQueueProcessor ( const DataPointWriteQueueProcessor & obj );
	DataPointWriteQueueProcessor & operator= ( const DataPointWriteQueueProcessor & rhs );

// operations
public:

	DataPointWriteQueueProcessor (ItaEcsCachedConfig& theConfig);
	virtual ~DataPointWriteQueueProcessor();

    void registerOiAssociation(TA_Base_Bus::DataPoint* outputDataPoint, TA_Base_Bus::DataPoint* inputDataPoint);
    void deregisterOiAssociation(TA_Base_Bus::DataPoint* outputDataPoint);

// operations
private:

    /**
      * processEvent
      *
      * Process the specified DataPoint write request
	  *
      */
    virtual void processEvent( TA_Base_Bus::DataPointWriteRequest * newEvent );

// attributes
protected:

    typedef std::map<TA_Base_Bus::DataPoint*, TA_Base_Bus::DataPoint*>      OiPairMap;
    typedef OiPairMap::iterator                                             OiPairMapIt;

    TA_Base_Core::ReEntrantThreadLockable    m_lock;
    ItaEcsCachedConfig&                         m_theConfig;

    OiPairMap                                   m_oiPairs;
};

#endif
