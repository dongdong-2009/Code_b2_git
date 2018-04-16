#if !defined(ATS_SIM_TABLE_ADDRESS_REGISTER_H)
#define ATS_SIM_TABLE_ADDRESS_REGISTER_H
/*
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 */

/**
  * Declaration file for ATSSimTable class
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  */

#ifdef WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4284)
#endif // #ifdef WIN32


#include <map>
#include <list>
#include <string>

#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommon.h"

namespace TA_IRS_App
{
    
namespace ATS_Sim
{
    /**
     *
     */
    class TableAddressRegister
    {

    public:
        /**
          *
          */
        static TableAddressRegister & getInstance();

        /** fetches the start address of the given table at the given location
          *
          * @param tableId as defined in the anonymous enumeration in 
          *                ATSSimCommon.h
          * @param location the location
          * @return the start address, if the tableNumber and location are a
          *         valid combination, otherwise -1 (0xFFFF, 65535)
          */
        unsigned short getAddress(TableIdentifier tableId, LocationType location);


        /** Associates a location with an identifier. Fails if the identifier
          * is not unique, ie. is already in use.
          *
          * @param location the location to use with the identifier
          * @param identifier the string to identify with the location
          * @return true if registration was successful, false if another
          *         location had previously registered that identifier.
          */
        bool registerLocationIdentifier(LocationType location, const std::string & identifier);

        /** A kind of reverse-key lookup. For the location type given,
          * retrieves all the identifiers registered.
          *
          * @param location the location for which to retrieve identifiers
          * @param identifiers the list of identifier strings to populate
          * @return the number of identifiers, basically identifiers.size()
          */
        int getLocationIdentifiers(LocationType location, std::list<std::string> & identifiers);

        /** Retrieves the location associated with the given identifier. Simple
          * map lookup
          *
          * @param identifier the identifier string
          * @return the LocationType registered for the given identifier, or
          *         NO_LOCATION if no such identifier was registered
          */
        LocationType findLocation(const std::string & identifier);

        ///
        /// specially for indicating the table doesn't exist.
        static const unsigned short INVALID_ADDRESS;

        ///
        /// for indicating when the address is NOT IN USE (usually in
        /// a UserQuery)
        static const unsigned short NOTINUSE_ADDRESS;


    private:
        /** Morphs a TableIdentifier and a LocationType into a single
          * unsigned long, for use as a key into the TableAddressMap
          * 
          * @param tid the TableIdentifier
          * @param loc the LocationType
          * @return unsigned long usable as a key into the TableAddressMap
          */
        unsigned long makeKey(TableIdentifier tid, LocationType loc);

        /** 
          *
          */
        bool addAddress(TableIdentifier tid, LocationType loc, unsigned short address);

        /**
          *
          */
        typedef std::map< unsigned long, unsigned short > TableAddressMap;

        /**
          * A map of table addresses, keyed by a combination of tableNumber
          * and location
          */
        TableAddressMap m_tableAddresses;


        /** The value of 'Y' in the PMF document, from which the addresses
          * of the Iscs tables are based
          */
        unsigned short m_baseAddressIscs;


        /** The value of 'X' in the PMF document, from which the addresses
          * of the Ats tables are based
          */
        unsigned short m_baseAddressAts;

        /**
          *
          */
        typedef std::map< std::string, LocationType > LocationIdentifierMap;

        /**
          *
          */
        LocationIdentifierMap m_locationIdentifiers;

    private:
        /**
         * Hidden constructor, as class is instantiated by self
         */
        TableAddressRegister();

        /**
         * Hidden destructor, as class is destroyed by self
         */
        virtual ~TableAddressRegister();

        /**
         * 
         */
        static TableAddressRegister * m_instance;

    }; // class TableAddressRegister

} // namespace ATS_Sim

} // namespace TA_IRS_App

#endif // !defined(ATS_SIM_TABLE_ADDRESS_REGISTER_H)
