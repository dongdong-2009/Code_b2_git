#ifndef STISTRAININFORMATIONMESSAGE_H_INCLUDED
#define STISTRAININFORMATIONMESSAGE_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISTrainInformationMessage.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISMessage.h"

// IDL compiler-produced header file
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

#include "bus/ats/ats_agent/IDL/src/IAtsTisCorbaDef.h"

namespace TA_IRS_App
{

	class STISTrainInformationMessage : public STISMessage
	{
	public:

		STISTrainInformationMessage();

		~STISTrainInformationMessage();

		virtual std::vector<unsigned char> createProtocolMessage();

		virtual void log(bool sendSuccess);

		void setData( TA_IRS_Bus::PlatformIdType platformId, const std::string& stationId, 
			const TA_IRS_Bus::IAtsTisCorbaDef::PlatformInfo& platformInfo, 
			const TA_IRS_Bus::IAtsTisCorbaDef::TrainInfoPair& trainInfoPair);

	private:


        /** 
          * platformInfoToStream
          *
          * Add the platform info to the stream
          */
        void platformInfoToStream(const TA_IRS_Bus::IAtsTisCorbaDef::PlatformInfo& platformInfo,
								  bool nextTrainValid, bool secondNextTrainValid,
                                  std::stringstream& dataStream);

        /** 
          * trainInfoToStream
          *
          * Add the train info to the stream
          */
        void trainInfoToStream(const TA_IRS_Bus::IAtsTisCorbaDef::TrainInfo& trainInfo,
                               std::stringstream& dataStream);


        void booleanToStream(bool value, std::stringstream& dataStream);

		std::string m_stationID;

		TA_IRS_Bus::PlatformIdType m_platformID;

        TA_IRS_Bus::IAtsTisCorbaDef::PlatformInfo m_platformInfo;

        TA_IRS_Bus::IAtsTisCorbaDef::TrainInfoPair m_trainInfo;

	};

}

#endif
