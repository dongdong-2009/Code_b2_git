/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TLCommon.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#ifndef TLCOMMON_H
#define TLCOMMON_H

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSProtocolEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h"


namespace TA_IRS_App {

    // forward declaration
    class CMCommon3;

	class TLCommon : public SDTSProtocolEvent  
	{
		public:
			TLCommon(TLPDUType pduType);
			virtual ~TLCommon();

			CMCommon3* getCMHeader();
			void setCMHeader(CMCommon3* pCMCommon3);

			TLPDUType getTL_PDUType();

		private:
			// disabled methods
            TLCommon();
			TLCommon(const TLCommon&);
			void operator=(const TLCommon&);

			CMCommon3 *m_cmData;
    
			TLPDUType m_tlPDUType;

		public:

			static const int TL_DATA_PDU_TYPE_INDEX;
			static const int TL_BROADCAST_PDU_TYPE_INDEX;

	};
};
#endif
