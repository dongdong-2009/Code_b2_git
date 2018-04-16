/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datanode_library/src/SummaryRequest.h $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2014/07/11 18:24:37 $
  * Last modified by:  $Author: haijun.li $
  *
  *	SummaryRequest represents a request to the SummaryProcessor
  * to generate and log a message to the eventlog
  *
  *
  */

#ifndef SUMMARYREQUEST_H
#define SUMMARYREQUEST_H

namespace TA_Base_Bus
{
	class SummaryRequest
	{
	public:
		enum ESummaryRequestType
		{ 
			UpdateActivation = 0,
			ReCalculateRequestFromParent,
			ReCalculateRequestFromChildren,
		};

		SummaryRequest( unsigned long entityKey, ESummaryRequestType requestType)
		:
        m_entityKey( entityKey ),
		m_requestType(requestType)
		{
		}

		~SummaryRequest()
		{
		}

		unsigned long getEntityKey() const
		{ 
			return m_entityKey;
		}

		ESummaryRequestType getRequestType() const
		{ 
			return m_requestType;
		}

		bool operator==( SummaryRequest &other )
		{
			return (m_entityKey == other.getEntityKey() && m_requestType == other.getRequestType());
		}

		bool operator<(const SummaryRequest& other) const
		{
			return (m_entityKey < other.getEntityKey() ||
				    m_requestType < other.getRequestType());
		}

		bool isReplacedBy(SummaryRequest* request)
		{
			return false;
		}

	private:
		unsigned long			m_entityKey;
        ESummaryRequestType     m_requestType;
	};
}
#endif	
