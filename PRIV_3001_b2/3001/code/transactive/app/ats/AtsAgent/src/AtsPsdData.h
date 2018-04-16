/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/AtsPsdData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This represents the ATS PSD Data
  */
#ifndef ATSPSDDATA_H
#define ATSPSDDATA_H

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/data_access_interface/ats/src/IAtsPSDData.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"
#include "bus/scada/datapoint_library/src/DataPointWriteQueue.h"

namespace TA_Bus
{
    class DataPoint;
}

namespace TA_IRS_App
{
	class DataPointHelper;

    class AtsPsdData : public TA_Base_Core::ReEntrantThreadLockable
    {
    public:

		/**
		  * Constructor
		  *
		  */
		AtsPsdData(TA_IRS_Bus::PlatformIdType platformId, unsigned long stationId, unsigned long entityKey, 
						  unsigned long locationKey, TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
						  DataPointHelper& dpHelper);

		virtual ~AtsPsdData();

		bool attachDataPoint(TA_Base_Bus::DataPoint* dataPoint);
		void addPsdData(TA_IRS_Core::IAtsPSDData* psdData);
		bool configurationComplete();

		void updatePsdClosePreWarning(bool closePreWarning);
		void updatePsdClosedAndLocked(bool psdClosedAndLocked);
		void updatePsdInhibitionStatus(unsigned int doorIndex, bool inhibitionStatus);

		TA_IRS_Bus::PlatformIdType getPlatformId();
		bool getPsdCloseRequest();
		bool getPsdOpenRequest();
		bool getDoorStatus(unsigned int index);
	
		bool checkWriteQueue(TA_Base_Bus::DataPointWriteRequest * writeReq);
		
		void processUpdates();
		void processUpdatesWhenDoubleATSFail();		//TD17760, hongzhi

	protected:
		bool checkAttribute(std::string address);
		void updateDataPoint(TA_Base_Bus::DataPoint* dataPoint);
		void updateDataPointWhenDoubleATSFail(TA_Base_Bus::DataPoint* dataPoint);		//TD17760,hongzhi
		bool findDataPoint(const TA_Base_Bus::DataPoint & theDataPoint);

		TA_IRS_Bus::IAtsPsdCorbaDef::AtsPsdMessageCorbaDef buildAtsPsdMessage();
	
    private:
        // Disable default constructor, copy constructor and assignment operator
		AtsPsdData();
        AtsPsdData( const AtsPsdData& atsPlatformRecord);
        AtsPsdData& operator=(const AtsPsdData &);

	protected:

		//
		// Platform ID
		//
		TA_IRS_Bus::IAtsPsdCorbaDef::AtsPsdMessageCorbaDef
												m_atsPsdMessage;

		bool									m_psdOpenRequest;	// <Open Request>
		bool									m_psdCloseRequest;	// <Close Request>

		bool									m_psdClosePreWarningChanged;
        std::vector<unsigned long>				m_setValueFailed; //lichao++
		std::vector<unsigned long>				m_changedPsdInhibitStatus;

		unsigned long							m_entityKey;
		unsigned long							m_locationKey;
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType		
												m_locationType;

		std::map<long, unsigned long>			m_psdDoorInhibitDp;
        unsigned long							m_psdPreCloseWarningDp;
		std::map<long, unsigned long>			m_psdDoorStatusDp;

		std::map<long, bool>  m_psdDoorNumToStatus;   // TD15264
		std::vector<TA_Base_Bus::DataPoint*>			m_dataPoints;

		DataPointHelper&						m_dpHelper;
    };
}

#endif // !defined(TRAINRECORD_H)
