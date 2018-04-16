/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Definition file for TableAddressRegister class
  */

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"

#include <stdexcept>
#include <limits.h>

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"

using namespace TA_IRS_App::ATS_Sim;

#define DEFAULT_ATS_BASE_ADDRESS 30000
#define DEFAULT_ISCS_BASE_ADDRESS 50000

// TODO: So ugly. Use an xml file, or other plain text.
#define ATS_OCC_WATCHDOG_OFFSET 0
#define ATS_OCC_TRAIN_OFFSET    2
#define ATS_OCC_PLATFORM_OFFSET 9506
#define ATS_OCC_WILD_OFFSET     14906
#define ATS_OCC_DSS_OFFSET      14996

#define ATS_STATION_WATCHDOG_OFFSET 0
#define ATS_STATION_TRAIN_OFFSET    2
#define ATS_STATION_PLATFORM_OFFSET 1922
#define ATS_STATION_DSS_OFFSET      3082

#define ATS_DEPOT_WATCHDOG_OFFSET 0
#define ATS_DEPOT_TRAIN_OFFSET    2
#define ATS_DEPOT_DSS_OFFSET      9506
#define ATS_DEPOT_WASH_OFFSET     9794

#define ISCS_OCC_WATCHDOG_OFFSET    0
#define ISCS_OCC_TRAIN_OFFSET       2
#define ISCS_OCC_ELECTRICAL_OFFSET  2774
#define ISCS_OCC_ATCDATA_OFFSET     3774

#define ISCS_MSS_WATCHDOG_OFFSET    0
#define ISCS_MSS_TRAIN_OFFSET       2
#define ISCS_MSS_PLATFORM_OFFSET    282
#define ISCS_MSS_ELECTRICAL_OFFSET  342
#define ISCS_MSS_ATCDATA_OFFSET     402

#define ISCS_SS1_WATCHDOG_OFFSET    1000
#define ISCS_SS1_TRAIN_OFFSET       1002
#define ISCS_SS1_PLATFORM_OFFSET    1282
#define ISCS_SS1_ELECTRICAL_OFFSET  1342
#define ISCS_SS1_ATCDATA_OFFSET     1402

#define ISCS_SS2_WATCHDOG_OFFSET    2000
#define ISCS_SS2_TRAIN_OFFSET       2002
#define ISCS_SS2_PLATFORM_OFFSET    2282
#define ISCS_SS2_ELECTRICAL_OFFSET  2342
#define ISCS_SS2_ATCDATA_OFFSET     2402

#define ISCS_DEPOT_WATCHDOG_OFFSET  0
#define ISCS_DEPOT_TRAIN_OFFSET     2
#define ISCS_DEPOT_WASH_OFFSET      2774
#define ISCS_DEPOT_ATCDATA_OFFSET   2806

/**
 * Searches through the map of registered table addresses
 */
unsigned short TableAddressRegister::getAddress(TableIdentifier tableId, LocationType location)
{
	unsigned short address = INVALID_ADDRESS;
    unsigned long key = makeKey(tableId, location);
    TableAddressMap::iterator found = m_tableAddresses.find(key);

    if (found != m_tableAddresses.end())
    {
		address = (*found).second;

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugNorm,
            "Found table address for table id=%d at location %d", tableId, location);
    }
    else
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
            "Couldn't find table address for table id=%d at location %d", tableId, location);
    }

	return address;
}


bool TableAddressRegister::registerLocationIdentifier
(   LocationType location,
    const std::string & identifier )
{
    bool registeredOk = false;
    LocationIdentifierMap::const_iterator found = m_locationIdentifiers.find(identifier);

    if ( found == m_locationIdentifiers.end() )
    {
        LocationIdentifierMap::value_type value(identifier, location);
        m_locationIdentifiers.insert( value );

        // here we delay supposing the registration is Ok until the last
        // possible exception has been thrown
        registeredOk = true;
    }

	return registeredOk;
}

int TableAddressRegister::getLocationIdentifiers
(   LocationType location,
    std::list<std::string> & identifiers )
{
    //
    // continue to loop finding all identifiers for a location
    for ( LocationIdentifierMap::iterator iter = m_locationIdentifiers.begin() ;
          iter != m_locationIdentifiers.end() ;
          iter++
        )
    {
        if ( (*iter).second == location )
        {
            identifiers.push_back( (*iter).first );
        }
    }

	return identifiers.size();
}

LocationType TableAddressRegister::findLocation(const std::string & identifier)
{
    LocationType location = NO_LOCATION;
    LocationIdentifierMap::const_iterator found = m_locationIdentifiers.find(identifier);

    if ( found != m_locationIdentifiers.end() )
    {
        location = (*found).second;
    }

    return location;
}

TableAddressRegister & TableAddressRegister::getInstance()
{
	if ( m_instance == 0 )
	{
		m_instance = new TableAddressRegister;
	}

    return *m_instance;
}


TableAddressRegister::TableAddressRegister()
: m_baseAddressIscs(DEFAULT_ISCS_BASE_ADDRESS)
, m_baseAddressAts(DEFAULT_ATS_BASE_ADDRESS)
{
    // Ats Occ
    m_tableAddresses[makeKey(AtsWatchdog,OCC)] = m_baseAddressAts+ATS_OCC_WATCHDOG_OFFSET;
    m_tableAddresses[makeKey(AtsTrain,OCC)] = m_baseAddressAts+ATS_OCC_TRAIN_OFFSET;
    m_tableAddresses[makeKey(AtsPlatform,OCC)] = m_baseAddressAts+ATS_OCC_PLATFORM_OFFSET;
    m_tableAddresses[makeKey(AtsWILD,OCC)] = m_baseAddressAts+ATS_OCC_WILD_OFFSET;
    m_tableAddresses[makeKey(AtsDSS,OCC)] = m_baseAddressAts+ATS_OCC_DSS_OFFSET;

    // Ats Station Mss
    m_tableAddresses[makeKey(AtsWatchdog,STATION)] = m_baseAddressAts+ATS_STATION_WATCHDOG_OFFSET;
    m_tableAddresses[makeKey(AtsTrain,STATION)] = m_baseAddressAts+ATS_STATION_TRAIN_OFFSET;
    m_tableAddresses[makeKey(AtsPlatform,STATION)] = m_baseAddressAts+ATS_STATION_PLATFORM_OFFSET;
    m_tableAddresses[makeKey(AtsDSS,STATION)] = m_baseAddressAts+ATS_STATION_DSS_OFFSET;

    // Ats Depot
    m_tableAddresses[makeKey(AtsWatchdog,DEPOT)] = m_baseAddressAts+ATS_DEPOT_WATCHDOG_OFFSET;
    m_tableAddresses[makeKey(AtsTrain,DEPOT)] = m_baseAddressAts+ATS_DEPOT_TRAIN_OFFSET;
    m_tableAddresses[makeKey(AtsWash,DEPOT)] = m_baseAddressAts+ATS_DEPOT_WASH_OFFSET;
    m_tableAddresses[makeKey(AtsDSS,DEPOT)] = m_baseAddressAts+ATS_DEPOT_DSS_OFFSET;

    // Iscs Occ
    m_tableAddresses[makeKey(IscsWatchdog,OCC)] = m_baseAddressIscs+ISCS_OCC_WATCHDOG_OFFSET;
    m_tableAddresses[makeKey(IscsTrain,OCC)] = m_baseAddressIscs+ISCS_OCC_TRAIN_OFFSET;
    m_tableAddresses[makeKey(IscsElectricalSubsection,OCC)] = m_baseAddressIscs+ISCS_OCC_ELECTRICAL_OFFSET;
    m_tableAddresses[makeKey(IscsAtcData,OCC)] = m_baseAddressIscs+ISCS_OCC_ATCDATA_OFFSET;

    // Iscs Mss
    m_tableAddresses[makeKey(IscsWatchdog,MSS)] = m_baseAddressIscs+ISCS_MSS_WATCHDOG_OFFSET;
    m_tableAddresses[makeKey(IscsTrain,MSS)] = m_baseAddressIscs+ISCS_MSS_TRAIN_OFFSET;
    m_tableAddresses[makeKey(IscsPlatform,MSS)] = m_baseAddressIscs+ISCS_MSS_PLATFORM_OFFSET;
    m_tableAddresses[makeKey(IscsElectricalSubsection,MSS)] = m_baseAddressIscs+ISCS_MSS_ELECTRICAL_OFFSET;
    m_tableAddresses[makeKey(IscsAtcData,MSS)] = m_baseAddressIscs+ISCS_MSS_ATCDATA_OFFSET;

    // Iscs Ss1
    m_tableAddresses[makeKey(IscsWatchdog,SS1)] = m_baseAddressIscs+ISCS_SS1_WATCHDOG_OFFSET;
    m_tableAddresses[makeKey(IscsTrain,SS1)] = m_baseAddressIscs+ISCS_SS1_TRAIN_OFFSET;
    m_tableAddresses[makeKey(IscsPlatform,SS1)] = m_baseAddressIscs+ISCS_SS1_PLATFORM_OFFSET;
    m_tableAddresses[makeKey(IscsElectricalSubsection,SS1)] = m_baseAddressIscs+ISCS_SS1_ELECTRICAL_OFFSET;
    m_tableAddresses[makeKey(IscsAtcData,SS1)] = m_baseAddressIscs+ISCS_SS1_ATCDATA_OFFSET;

    // Iscs Ss2
    m_tableAddresses[makeKey(IscsWatchdog,SS2)] = m_baseAddressIscs+ISCS_SS2_WATCHDOG_OFFSET;
    m_tableAddresses[makeKey(IscsTrain,SS2)] = m_baseAddressIscs+ISCS_SS2_TRAIN_OFFSET;
    m_tableAddresses[makeKey(IscsPlatform,SS2)] = m_baseAddressIscs+ISCS_SS2_PLATFORM_OFFSET;
    m_tableAddresses[makeKey(IscsElectricalSubsection,SS2)] = m_baseAddressIscs+ISCS_SS2_ELECTRICAL_OFFSET;
    m_tableAddresses[makeKey(IscsAtcData,SS2)] = m_baseAddressIscs+ISCS_SS2_ATCDATA_OFFSET;

    // Iscs Depot
    m_tableAddresses[makeKey(IscsWatchdog,DEPOT)] = m_baseAddressIscs+ISCS_DEPOT_WATCHDOG_OFFSET;
    m_tableAddresses[makeKey(IscsTrain,DEPOT)] = m_baseAddressIscs+ISCS_DEPOT_TRAIN_OFFSET;
    m_tableAddresses[makeKey(IscsWash,DEPOT)] = m_baseAddressIscs+ISCS_DEPOT_WASH_OFFSET;
    m_tableAddresses[makeKey(IscsAtcData,DEPOT)] = m_baseAddressIscs+ISCS_DEPOT_ATCDATA_OFFSET;

    registerLocationIdentifier(MSS, "MSS");
    registerLocationIdentifier(SS1, "SS1");
    registerLocationIdentifier(SS2, "SS2");
    registerLocationIdentifier(OCC, "OCC");
    registerLocationIdentifier(DEPOT, "DEPOT");
}

TableAddressRegister::~TableAddressRegister()
{
}

unsigned long TableAddressRegister::makeKey(TableIdentifier tid, LocationType loc)
{
    // shift the location 16bits left then bitwise-or it with the tid
    return ( (unsigned long)((unsigned short)tid | ((unsigned short)loc) << 16) );
}

bool TableAddressRegister::addAddress
(   TableIdentifier tid,
    LocationType loc,
    unsigned short address )
{
    bool addOk = false;
    unsigned long key = makeKey(tid, loc);
    TableAddressMap::iterator found = m_tableAddresses.find(key);

    if (found == m_tableAddresses.end())
    {
        m_tableAddresses[key] = address;
        addOk = true;
    }
    
    return addOk;
}

TableAddressRegister * TableAddressRegister::m_instance = 0;

const unsigned short TableAddressRegister::INVALID_ADDRESS = USHRT_MAX;
const unsigned short TableAddressRegister::NOTINUSE_ADDRESS = USHRT_MAX-1;
