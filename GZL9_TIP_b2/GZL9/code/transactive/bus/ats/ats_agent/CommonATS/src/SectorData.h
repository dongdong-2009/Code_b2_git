/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/bus/ats/ats_agent/CommonATS/src/SectorData.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #3 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2010/03/30 11:17:43 $
  * Last modified by:  $Author: grace.koh $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */
#ifndef SECTORDATA_H
#define SECTORDATA_H

#include <vector>
#include <map>

namespace TA_IRS_Bus
{
    class SectorData
    {
    public:

        /**
          * Constructor
          */
        SectorData();

        /**
          * Destructor
          */
        virtual ~SectorData();

		std::vector<unsigned long> getStationsInRelatedMssSector(unsigned long station);
		std::vector<unsigned long> getAllStations();
		void addLastSectorIfRequired(std::vector<unsigned long>& stations, unsigned long lastLocation);

	private:
        // Disable copy constructor and assignment operator
        SectorData( const SectorData& sectorData);
        SectorData& operator=(const SectorData &);

	protected:

		typedef std::map<unsigned long, unsigned long> StationToMssMap;
		
		StationToMssMap					m_stationToMssMap;

		typedef std::map<unsigned long, std::vector<unsigned long> > MssToStationMap;

		MssToStationMap					m_mssToStationMap;
    };
}

#endif // !defined(SECTORDATA_H)
