/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/SectorData.h $
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
