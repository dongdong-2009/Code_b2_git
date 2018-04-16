/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datanode_library/src/SummaryRequest.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *	SummaryRequest represents a request to the SummaryProcessor
  * to generate and log a message to the eventlog
  *
  *
  */

#ifndef SUMMARYREQUEST_H
#define SUMMARYREQUEST_H

#include <string>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
	class SummaryRequest
	{

	public:
        
        typedef enum { ProxyUpdate, UpdateActivation } ESummaryRequestType;

		/**
		 * ListRequest::ListRequest()
		 *
		 * This constructor is for a summary update event
		 *
		 * @param entityKey			The pkey of the entity associated with this request
		 *
		 */
		SummaryRequest( unsigned long entityKey, ScadaEntityUpdateType updateType )
		:
        m_entityKey( entityKey ),
        m_updateType( updateType ),
        m_type( ProxyUpdate )
		{
		};

		SummaryRequest( unsigned long entityKey )
		:
        m_entityKey( entityKey ),
        m_updateType( NotApplicable ),
        m_type( UpdateActivation )
		{
		};

		~SummaryRequest()
		{
		};

		/**
		 * SummaryRequest::getEntityKey()
		 *
		 * @return  The entity key of the DataNode to be processed
		 *
		 */
		unsigned long getEntityKey() const
		{ 
			return m_entityKey;
		};

		ScadaEntityUpdateType getUpdateType() const
		{ 
			return m_updateType;
		};

		ESummaryRequestType getType() const
		{ 
			return m_type;
		};

		bool operator==( SummaryRequest &other )
		{
			return (getEntityKey() == other.getEntityKey() &&
					getUpdateType() == other.getUpdateType() &&
                    getType() == other.getType());
		};

		bool isReplacedBy(SummaryRequest* request)
		{
			return false;
		}

	private:
		unsigned long			m_entityKey;
		ScadaEntityUpdateType   m_updateType;
        ESummaryRequestType     m_type;
	};
}
#endif	
