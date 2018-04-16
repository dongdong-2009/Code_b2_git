/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/bus/ats/ats_agent/CommonATS/src/PowerData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/03/30 11:17:43 $
  * Last modified by:  $Author: grace.koh $
  *
  * This represents the ATS ElectricalSubsection Table
  */
#ifndef POWERDATA_H
#define POWERDATA_H

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"

namespace TA_IRS_Core
{
	class IAtsPowerData;
}

namespace TA_IRS_Bus
{
	class DataPointHelper;

    class PowerData : public TA_Base_Core::ReEntrantThreadLockable
    {
    public:

		/**
		  * Constructor
		  *
		  */
		PowerData(TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
				  unsigned short subsectionId,
				  std::vector<TA_IRS_Core::IAtsPowerData*> powerData, DataPointHelper& dpHelper);

		virtual ~PowerData();

		unsigned short	getSubsectionId();
		unsigned char	getValidityByte();
		unsigned char	getPowerStatusByte();
	
    private:
        // Disable default constructor, copy constructor and assignment operator
		PowerData();
        PowerData( const PowerData& powerData);
        PowerData& operator=(const PowerData &);

	protected:

		//
		// ElectricalSubsection identification
		//
		unsigned short									m_subsectionId;				// <Subsection ID>

		bool											m_valid;					// <Validity>

		std::vector<unsigned long>						m_powerStatusProxies;

		std::vector<unsigned long>						m_bitPositions;		//CL20376, hongzhi, Electrical Subsection data exchange between ATS has a problem

		DataPointHelper&								m_dpHelper;
    };
}

#endif // !defined(TRAINRECORD_H)
