/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/PowerData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * This represents the ATS ElectricalSubsection Table
  */
#ifndef POWERDATA_H
#define POWERDATA_H

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/data_access_interface/ats/src/IAtsPowerData.h"

#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"

#include "app/ats/AtsAgent/src/RawTable.h"

namespace TA_IRS_App
{
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
