/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/AtsFixedBlockMap.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * AtsFixedBlockMap is an implementation of IAtsFixedBlockMap. It holds the data specific to an AtsFixedBlockMap entry
  * in the database, and allows read-only access to that data.
  */

#if !defined(ATSFIXEDBLOCKMAP_H_INCLUDED)
#define ATSFIXEDBLOCKMAP_H_INCLUDED

#include "core/data_access_interface/ats/src/IAtsFixedBlockMap.h"
#include "core/data_access_interface/ats/src/AtsFbMapRecord.h"
#include <string>
#include <vector>
#include <map>

namespace TA_IRS_Core
{
    // stores a map of <branchIds, zoneDetails structure>
    typedef std::map<unsigned long, IAtsFbMapRecords> BranchZoneMap;

    class AtsFixedBlockMap : public IAtsFixedBlockMap
    {

    public:

        /** 
          * AtsFixedBlockMap
          *
          * Constructor for AtsFixedBlockMap
          *
          * @param branchZoneMap The data that maps each branch to a vector of zone details
          */
        AtsFixedBlockMap(BranchZoneMap branchZoneMap);

        ~AtsFixedBlockMap();

        /**
         * getZoneId
         *
         * Returns the zone Id that maps to the branchID and abscissa value.  The zone 
         * Id could be a whole number of a decimal number to indicate an overlap zone.
         *
         * @param branchId - The branch Id
         * @param fixBlockAbsicissa - The abscissa of Fixed Block from origin of Branch
         * @param headCarAbscissa - The abscissa of Head Car along Fixed Block
         *
         * @return The zone Id.  This may be in the form of an integer to indicate a 
         *         normal zone or a decimal number like 3.5 to indicate it is in the
         *         pre-conflict state in the stage overlap zone "3/4".
         *
         * @exception DataException A DataException will be thrown if there is no such
         *            branch in the zone type or if the abscissa is out of range (i.e.
         *            it does not fall into any of the zones along the branch).
         */
        virtual double getZoneId(unsigned long branchId, unsigned long fixBlockAbscissa, unsigned long headCarAbscissa);

		/**
         * getTrack
         *
         * Returns the track that maps to the branchID and abscissa value.  
         *
         * @param branchId - The branch Id
         * @param fixBlockAbsicissa - The abscissa of Fixed Block from origin of Branch
         * @param headCarAbscissa - The abscissa of Head Car along Fixed Block
         *
         * @return The track. 
         *
         * @exception DataException A DataException will be thrown if there is no such
         *            branch in the zone type or if the abscissa is out of range (i.e.
         *            it does not fall into any of the zones along the branch).
         */
        virtual std::string getTrack(unsigned long branchId, unsigned long fixBlockAbscissa, unsigned long headCarAbscissa);

	protected:

		/**
         * getAtsFbMapRecord
         *
         * Returns the branch detail that maps to the branchID and abscissa value.  
         *
         * @param branchId - The branch Id
         * @param fixBlockAbsicissa - The abscissa of Fixed Block from origin of Branch
         * @param headCarAbscissa - The abscissa of Head Car along Fixed Block
         *
         * @return The branch detail. 
         *
         * @exception DataException A DataException will be thrown if there is no such
         *            branch in the zone type or if the abscissa is out of range (i.e.
         *            it does not fall into any of the zones along the branch).
         */
		virtual IAtsFbMapRecord* getAtsFbMapRecord(unsigned long branchId, unsigned long fixBlockAbscissa, unsigned long headCarAbscissa);

    private:
        // Copy constructor and assignment operator are not used so they are made private
        AtsFixedBlockMap( const AtsFixedBlockMap& theAtsFixedBlockMap);            
		AtsFixedBlockMap& operator=(const AtsFixedBlockMap &);

        // This stores the map of branch and abscissas to the zones
        BranchZoneMap m_branchZoneMap;

    };
} // closes TA_Core

#endif // !defined(ATSFIXEDBLOCKMAP_H_INCLUDED)
