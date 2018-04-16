/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/SectorData.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #2 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */
#ifndef SECTORDATA_H
#define SECTORDATA_H

#include <vector>
#include <map>

namespace TA_IRS_App
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
