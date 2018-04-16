/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  adamr
 * @version: $Revision$
 *
 * Last modification: $Date$
 * Last modified by:  $Author$
 *
 */

#include "app/trains/train_agent/src/CctvZone.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>
#include <math.h>

namespace TA_IRS_App
{

    CctvZone::CctvZone()
          : m_doubleRepresentation( 0 ),
            m_currentStage( 0 ),
            m_nextStage( 0 ),
            m_zoneType( CctvZone::NoConflict )
    {
        FUNCTION_ENTRY( "CctvZone" );
        FUNCTION_EXIT;
    }


    CctvZone::CctvZone( double atsValue )
          : m_doubleRepresentation( atsValue ),
            m_currentStage( 0 ),
            m_nextStage( 0 ),
            m_zoneType( CctvZone::NoConflict )
    {
        FUNCTION_ENTRY( "CctvZone" );

        // parse the value into a useable format,
        // this will set m_currentStage, m_nextStage, and m_zoneType
        convertFromDouble();

        FUNCTION_EXIT;
    }
    

    CctvZone::CctvZone( unsigned long atsValue )
        : m_doubleRepresentation( static_cast<double>( atsValue ) ),
          m_currentStage( atsValue ),
          m_nextStage( 0 ),
          m_zoneType( CctvZone::NoConflict )
    {
        FUNCTION_ENTRY( "CctvZone" );
        FUNCTION_EXIT;
    }
    

    CctvZone::CctvZone( const CctvZone& rhs )
          : m_doubleRepresentation( rhs.m_doubleRepresentation ),
            m_currentStage( rhs.m_currentStage ),
            m_nextStage( rhs.m_nextStage ),
            m_zoneType( rhs.m_zoneType )
    {
        FUNCTION_ENTRY( "CctvZone" );
        FUNCTION_EXIT;
    }


    CctvZone::~CctvZone()
    {
        FUNCTION_ENTRY( "~CctvZone" );
        FUNCTION_EXIT;
    }


    void CctvZone::operator=( const double& rhs )
    {
        FUNCTION_ENTRY( "operator=" );

        // Sets m_doubleRepresentation to the given value
        m_doubleRepresentation = rhs;

        // parse the value into a useable format,
        // this will set m_currentStage, m_nextStage, and m_zoneType
        convertFromDouble();

        FUNCTION_EXIT;
    }


    bool CctvZone::operator==( const CctvZone& rhs ) const
    {
        FUNCTION_ENTRY( "operator==" );

        // the double representation contains all the data we need
        // however you cannot compare 2 floating point numbers for equality without some sort of tolerance
        // so best to just compare the rest of the data

        FUNCTION_EXIT;
        return ( ( m_currentStage == rhs.m_currentStage ) &&
                 ( m_nextStage == rhs.m_nextStage ) &&
                 ( m_zoneType == rhs.m_zoneType ) );
    }
    
    
    bool CctvZone::operator!=( const CctvZone& rhs ) const
    {
        FUNCTION_ENTRY( "operator!=" );
        
        // the double representation contains all the data we need
        // however you cannot compare 2 floating point numbers for equality without some sort of tolerance
        // so best to just compare the rest of the data
        
        FUNCTION_EXIT;
        return ( ( m_currentStage != rhs.m_currentStage ) ||
                 ( m_nextStage != rhs.m_nextStage ) ||
                 ( m_zoneType != rhs.m_zoneType ) );
    }


    double CctvZone::operator()() const
    {
        FUNCTION_ENTRY( "double ()" );
        FUNCTION_EXIT;
        return m_doubleRepresentation;
    }


    unsigned long CctvZone::getCurrentStage() const
    {
        FUNCTION_ENTRY( "getCurrentStage" );
        FUNCTION_EXIT;
        return m_currentStage;
    }


    unsigned long CctvZone::getNextStage() const
    {
        FUNCTION_ENTRY( "getNextStage" );
        FUNCTION_EXIT;
        return m_nextStage;
    }


    CctvZone::ECctvZoneType CctvZone::getZoneType() const
    {
        FUNCTION_ENTRY( "getZoneType" );
        FUNCTION_EXIT;
        return m_zoneType;
    }


    void CctvZone::convertFromDouble()
    {
        FUNCTION_ENTRY( "convertFromDouble" );

        // create a copy of m_doubleRepresentation which is positive
        double currentCctvZone = fabs( m_doubleRepresentation );

        // create a std::stringstream variable
        // insert doubleRepresentation into the variable with fixed precision 1
        // this will do any rounding for us to make sure we dont have precision errors,
        // for example 2.0999999 becomes 2.1 as expected
        std::stringstream zoneStr;
        zoneStr << std::fixed << std::setprecision( 1 ) << currentCctvZone;

        // clear the current and next stage
        m_currentStage = 0;
        m_nextStage = 0;

        // this holds the decimal place
        char dot;

        // extract the values
        // eg 2.1 extracts to [2] [.] [1]
        // 4 extracts to [4] [] []
        zoneStr >> m_currentStage;
        zoneStr >> dot;
        zoneStr >> m_nextStage;

        // if there is no next stage, or it matches the current stage
        // then this is not a conflict or pre conflict zone

        if ( ( m_nextStage == 0 ) ||
             ( m_nextStage == m_currentStage ) )
        {
            m_nextStage = m_currentStage;
            m_zoneType = CctvZone::NoConflict;
        }
        // if there is a next stage
        // then this is conflict or pre-conflict
        else
        {
            // m_doubleRepresentation is positive, the zone type is Conflict
            // m_doubleRepresentation is negative, the zone type is PreConflict
            m_zoneType = ( m_doubleRepresentation < 0 ) ? CctvZone::PreConflict : CctvZone::Conflict;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Parsed ATS CCTV Zone [%f] as [%d]->[%d] [%d]",
                     m_doubleRepresentation, m_currentStage, m_nextStage, m_zoneType );

        FUNCTION_EXIT;
    }

} // TA_IRS_App
