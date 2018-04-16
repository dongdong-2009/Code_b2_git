
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_Broadcast.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#ifndef CM_BROADCAST_H
#define CM_BROADCAST_H

#include <vector>

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon3.h"

namespace TA_IRS_App {

	class CM_Broadcast : public CMCommon3  
	{
		public:
			CM_Broadcast();
			virtual ~CM_Broadcast();


			CMChannelSelectionMask getChannelSelection();
			CMRepeatRequest getRepeatRequest();
			unsigned int getRegionSelection();

			void setChannelSelection(CMChannelSelectionMask channel);
			void setRepeatRequest(CMRepeatRequest repeatReq);
			void setRegionSelection(unsigned int region);

			bool parseData(const std::vector<unsigned char>& rawData);

		protected:
			virtual void buildMsg();

		private:

			// disabled methods
			CM_Broadcast(const CM_Broadcast&);
			void operator=(const CM_Broadcast&);


		private:

			CMChannelSelectionMask channelSelection;
			CMRepeatRequest repeatRequest;
			unsigned short regionSelection;

		public:

			static const int CM_CHANNEL_INDEX;
			static const int CM_REPEAT_REQ_INDEX;
			static const int CM_REGION_INDEX;
			static const int CM_BROADCAST_TL_LENGTH_INDEX;

	};
};
#endif
