/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\PADataSynch.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for PA Data Synchronization
  */

#ifndef _PA_DATA_SYNC_H_
#define _PA_DATA_SYNC_H_

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"
#include "bus/ats/ats_agent/CommonATS/src/DataSynch.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataSync.h"

namespace TA_IRS_Bus
{
	class PADataSynch : public TA_IRS_Bus::DataSynch,
		                public virtual POA_TA_IRS_Bus::IAtsPasCorbaDef
	{
	public:
		PADataSynch(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~PADataSynch();

		/**
		  * getPasInformationList
		  *
		  * This method is used to access the full list of PAS information
		  * and will typlically used to synchronise at startup of the client
		  * application
		  *
		  * @return A sequence of AtsPasMessageCorbaDef for all trains known
		  *			to the ATS agent.
		  */
		virtual TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList* getPasInformationList();

		/**
          * processOccPasUpdate
          *
          * This method is strictly used to receive data from OCC to station
		  * OCC calls this interface for pasing data from OCC to station.
		  * updates
          *
		  * @param	pasUpdate	A sequence of AtsPasMessageCorbaDef for updates  
		  *						to platforms for the station
          */
		virtual void processOccPasUpdate(const TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList& pasUpdate);

		/**
          * processDataSync
          *
          * This method is used to start the processing of the Data Syn
		  *  Only OCC agent calls this method.
          *
          */
		void processDataSync();

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		PADataSynch(const PADataSynch&);
		PADataSynch& operator= (const PADataSynch&);

		/**
          * localProcessOccPasUpdate
          *
          * This method is used for local processing of PA Data update
		  *
          * @param pasUpdate : PasInfoList
          */
		void localProcessOccPasUpdate(const TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList& pasUpdate);
	};
}
#endif