/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/CODE_4677_T00060500/transactive/app/radio/RadioTetraAgent/src/RadioSDS.h $
 * @author:  alexis
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2006/12/08 11:20:56 $
 * Last modified by:  $Author: buildteam $
 * 
 */

#ifndef RADIO_SDS_HANDLER_H_INCLUDED
#define RADIO_SDS_HANDLER_H_INCLUDED

#include <string>
#include <vector>
#include "ace/Time_Value.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSInputQueue.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSOutputQueue.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSQueueManager.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSTransactionManager.h"
#include "app/radio/RadioTetraAgent/src/RadioSDS.h"
#include "app/radio/RadioTetraAgent/src/RadioSDSSummary.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
namespace TA_IRS_App
{
class RadioSDSHandler
{
	public:
		RadioSDSHandler();
		virtual ~RadioSDSHandler();

		void sendSDS(unsigned long recipientSSI, std::vector<unsigned char> rawMessage);

		void sendText(const char* session,unsigned long recipientSSI, TextCodingScheme encoding, std::string rawMessage, bool isBroadcast=false);

		RadioSDS* createRadioSDS(RadioSDSSummary* pSummary,const std::string& name, unsigned long passNumber,unsigned long clientSSI, bool isPrimary, ISDSRegistationState* sdsRegState);

		void cleanUpSDS();

		void connectToSDS(const std::string& host, const std::string& port);

		void switchConnection(bool toPrimary);

		void clearSDS();

	private:
    		// To keep all input/output events & requests;
		TA_IRS_App::SDSInputQueue *m_pSDSInputQueue;
		TA_IRS_App::SDSOutputQueue *m_pSDSOutputQueue;

		//	To manage all output/input events & requests;
		TA_IRS_App::SDSQueueManager *m_pSDSQueueManager;

		//	To manage all transactions of events & requests;
		TA_IRS_App::SDTSTransactionManager *m_pSDTSTransactionManager;

		RadioSDS* m_pPrimaryRadioSDS;
		RadioSDS* m_pSecondaryRadioSDS;
		RadioSDS* m_pCurrentRadioSDS;

		/**
		*	Used for safe acess;
		*/
		TA_Base_Core::ReEntrantThreadLockable m_lock;

};
}

#endif
