/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableLogData.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * \class ATSSimTableLogData
  * Implementation file for the ATSSimTableLogData class.
  *
  * ATSSimTableLogData is used by the ATSSimTableLogSupport class to store
  * information required by each table to log.
  */

#include <iostream>
#include <stdexcept>

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableLogData.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTable.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableLogSupport.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"

#include "core/utilities/src/DebugUtil.h"

// using namespace TA_IRS_App;
using namespace TA_IRS_App::ATS_Sim;

/* ----------------------------------------------------------------------------
 * Construction/Destruction
 * --------------------------------------------------------------------------*/
ATSSimTableLogData::ATSSimTableLogData( UserQuery * query, ATSSimTableLogSupport * parent, ATSSimTable * t )
: m_table(t)
, m_parent(parent)
, m_userQuery(0)
{
    m_userQuery = new UserQuery( Show, query->getOutputFormat() );
    m_userQuery->setTableAddress( query->getTableAddress() );
    m_userQuery->setRecords( query->getRecords() );
    while ( query->moreArgs() )
    {
        m_userQuery->pushArg( query->popArg() );
    }
}

ATSSimTableLogData::~ATSSimTableLogData()
{
    delete m_userQuery;
}


void ATSSimTableLogData::addRecordRange( const RecordRange & records )
{
	m_userQuery->setRecords( records );
}


void ATSSimTableLogData::removeRecordRange( const RecordRange & records )
{
    //
    // if the table being logged hasn't had any records specified, add all
    // possible so that removing some works as expected
    if ( m_userQuery->getRecords().empty() )
    {
        m_userQuery->setRecords( "1-", m_table->getNumberOfRecords());
    }

    m_userQuery->removeRecords( records );
}


void ATSSimTableLogData::doLog()
{
    if ( m_userQuery->getTableAddress() == TableAddressRegister::INVALID_ADDRESS)
    {
        std::cerr << "What?!?! An invalid table address !?!?" << std::endl;
        return;
        throw std::logic_error("Bad request to log INVALID_ADDRESS");
    }

    UserResponse * r = m_table->processUserRead(m_userQuery);
    if ( r != 0 )
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugNorm,
            "Processed User Read for table %d",m_userQuery->getTableAddress());

        LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugNorm, r->m_responseData );

        delete r;
    }
}


const RecordRange & ATSSimTableLogData::getRecordRange( ) const
{
    return m_userQuery->getRecords();
}

void ATSSimTableLogData::setOutputFormat(TableOutputFormat f)
{
    m_userQuery->setOutputFormat( f );
}

TableOutputFormat ATSSimTableLogData::getOutputFormat() const
{
    return m_userQuery->getOutputFormat();
}

