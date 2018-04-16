/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/OccUpdateProcessor.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */
#ifndef OCCUPDATEPROCESSOR_H
#define OCCUPDATEPROCESSOR_H

#include "time.h"

#include <vector>
#include <map>

#include "core/naming/src/NamedObject.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"

#include "app/ats/AtsAgent/src/WatchDogData.h"
#include "app/ats/AtsAgent/src/SectorData.h"

namespace TA_IRS_App
{
	class StationOccUpdateProcessor;

    class OccUpdateProcessor
    {
    public:

        /**
          * Constructor
          */
        OccUpdateProcessor(unsigned long locationKey, TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
						   WatchDogData& watchDogData, SectorData& sectorData);

        /**
          * Destructor
          */
        virtual ~OccUpdateProcessor();

		//
		// ECS
		//
		typedef TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef EcsMessage;

		void addEcsUpdate(const EcsMessage& ecsMessage);

		//
		// Train
		//
		typedef TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef TrainMessage;

		void addTrainUpdate(const TrainMessage& trainMessage);
	
		//
		//	CCTV
		//
		typedef TA_IRS_Bus::IAtsCctvCorbaDef::AtsCctvMessageCorbaDef CctvMessage;

		void addCctvUpdate(const CctvMessage& cctvMessage);

		//
		// TIS
		//
		typedef TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef TisMessage;

		void addTisUpdate(const TisMessage& tisMessage);
		
		//
		// PAS
		//
		typedef TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef PasMessage;

		void addPasUpdate(const PasMessage& pasMessage);
		
		void sendOccUpdates();

	private:
        // Disable default constructor, copy constructor and assignment operator
		OccUpdateProcessor();
        OccUpdateProcessor( const OccUpdateProcessor& occUpdateProcessor);
        OccUpdateProcessor& operator=(const OccUpdateProcessor &);

	public:
		
		typedef TA_Base_Core::NamedObject<TA_IRS_Bus::IAtsCorbaDef,
					 TA_IRS_Bus::IAtsCorbaDef_ptr,
					 TA_IRS_Bus::IAtsCorbaDef_var>	AtsAgentNamedObject;

	protected:

		typedef std::map<unsigned long, StationOccUpdateProcessor*> StationProcessorMap;

		StationProcessorMap				m_stationProcessorMap;

		SectorData&						m_sectorData;
    };
}

#endif // !defined(OCCUPDATEPROCESSOR_H)
