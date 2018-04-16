/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSTransactionManager.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Manages the SDTS message numbering for the MS/PEI
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSTransactionManager.h"

using namespace std;

namespace TA_IRS_App {


    const int SDTSTransactionManager::MAX_TRANSACTION_COUNT = 100;

    SDTSTransactionManager::SDTSTransactionManager() : m_messageRef(0)
	{
		FUNCTION_ENTRY( "Constructor" );
		FUNCTION_EXIT;
	}


	SDTSTransactionManager::~SDTSTransactionManager()
	{
		FUNCTION_ENTRY( "Destructor" );
		FUNCTION_EXIT;
	}


	unsigned char SDTSTransactionManager::beginTransaction(unsigned long src,unsigned long dst,
                                            CMProtocolIdentifier pi)
	{
		FUNCTION_ENTRY("beginTransaction");

		// lock collection
		TA_Base_Core::ThreadGuard guard( m_lock );
        
        if (m_transactions.size() >= MAX_TRANSACTION_COUNT)
        {
            m_transactions.pop_front();
        }

		// message reference number - 1 byte
        int ref = m_messageRef;
		if (m_messageRef >= 0xFF )
		{
			m_messageRef = 0;
		}
        else
        {
            ++m_messageRef;
        }

        // remove old transaction with same ref# if it exists
        for (list<SDTSTransaction>::iterator it = m_transactions.begin(); it != m_transactions.end(); ++it)
        {
            if (it->messageRef == ref)
            {
                it=m_transactions.erase(it);
            }
        }

		SDTSTransaction oTransaction;
		oTransaction.src = src;
		oTransaction.dst = dst;
		oTransaction.pi  = pi;
		oTransaction.timestamp = ACE_OS::gettimeofday();
		oTransaction.messageRef = ref;

		m_transactions.push_back(oTransaction);

		FUNCTION_EXIT;

		return ref;
	}
		
	bool SDTSTransactionManager::findTransaction(unsigned long src, unsigned long dst,
			CMProtocolIdentifier pi, unsigned char messageRef)
	{
		FUNCTION_ENTRY("findTransaction");

		TA_Base_Core::ThreadGuard guard( m_lock );
		
		list<SDTSTransaction>::iterator iter;
        bool found = false;

		for(iter = m_transactions.begin();iter !=m_transactions.end() ;++iter)
		{
			if (((*iter).dst == dst) && ((*iter).src == src) && 
                ((*iter).messageRef == messageRef) && ((*iter).pi == pi))
			{
                found = true;
                break;
			} 
		}

		FUNCTION_EXIT;
        return found;
	}

	void SDTSTransactionManager::endTransaction(unsigned long src, unsigned long dst,
			CMProtocolIdentifier pi, unsigned char messageRef)
	{
		FUNCTION_ENTRY("endTransaction");

		TA_Base_Core::ThreadGuard guard( m_lock );
		
		list<SDTSTransaction>::iterator iter;

		for(iter = m_transactions.begin();iter !=m_transactions.end() ;++iter)
		{
			if (((*iter).dst == dst) && ((*iter).src == src) && 
                ((*iter).messageRef == messageRef) && ((*iter).pi == pi))
			{
				iter=m_transactions.erase(iter);
				break;
			} 
		}

		FUNCTION_EXIT;
	}

	void SDTSTransactionManager::clearTransaction()
	{
		FUNCTION_ENTRY("clearTransaction");
		TA_Base_Core::ThreadGuard guard( m_lock );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removing %u transactions from SDS Transaction Manager", m_transactions.size());
		m_transactions.clear();

		FUNCTION_EXIT;
	}
}

