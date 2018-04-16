/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSTransactionManager.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Manages the SDTS message numbering for the MS/PEI
  */

#ifndef __SDS_TransactionManager_H
#define __SDS_TransactionManager_H

#include <list>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSTransaction.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"

namespace TA_IRS_App 
{

	class SDTSTransactionManager
	{
		public:
            SDTSTransactionManager();
			virtual ~SDTSTransactionManager();

			unsigned char beginTransaction(unsigned long src, unsigned long dst,
                        					CMProtocolIdentifier pi);

            bool findTransaction(unsigned long src, unsigned long dst,
        					CMProtocolIdentifier pi, unsigned char messageRef);

			void endTransaction(unsigned long src, unsigned long dst,
		        			CMProtocolIdentifier pi, unsigned char messageRef);

			void clearTransaction();

        private:
            // disabled methods:
            SDTSTransactionManager(const SDTSTransactionManager&);
            void operator=(const SDTSTransactionManager&);

		private:

            /**
	          * Collection that maintains a list of active/pending transactions
              */
			std::list<SDTSTransaction>  m_transactions;

            /**
	          * Lock for the collection
              */
			TA_Base_Core::ReEntrantThreadLockable m_lock;

			int m_messageRef;


            /**
	          * Max size of the queue
              */
            static const int MAX_TRANSACTION_COUNT;
	};

};

#endif //__SDS_TransactionManager_H
