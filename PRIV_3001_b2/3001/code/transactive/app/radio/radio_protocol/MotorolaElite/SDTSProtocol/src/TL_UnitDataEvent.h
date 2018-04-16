/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_UnitDataEvent.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#ifndef TL_UNITDATAEVENT_H
#define TL_UNITDATAEVENT_H

#include <vector>
#include <string>
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TLCommon.h"

namespace TA_IRS_App {

	class TL_UnitDataEvent : public TLCommon  
	{
		public:
			TL_UnitDataEvent();
			virtual ~TL_UnitDataEvent();

			unsigned int getTLDataLength();

			std::string getMessageData();
			void setMessageData(const std::string& userData);

		protected:
			virtual void buildMsg();

		private:
			// disabled methods
			TL_UnitDataEvent(const TL_UnitDataEvent&);
			void operator=(const TL_UnitDataEvent&);

			std::string  m_msgData;
	};
};
#endif 
