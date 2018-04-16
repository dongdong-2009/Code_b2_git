/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTableLogData.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * \class PASimTableLogData
  * Implementation file for the PASimTableLogData class.
  *
  * PASimTableLogData is used by the PASimTableLogSupport class to store
  * information required by each table to log.
  */

#include <iostream>
#include <stdexcept>

#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableLogData.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTable.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableLogSupport.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableAddressRegister.h"

#include "core/utilities/src/DebugUtil.h"

// using namespace TA_IRS_App;
using namespace TA_IRS_App::PA_Sim;

/* ----------------------------------------------------------------------------
 * Construction/Destruction
 * --------------------------------------------------------------------------*/
PASimTableLogData::PASimTableLogData( UserQuery * query, PASimTableLogSupport * parent, PASimTable * t )
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

PASimTableLogData::~PASimTableLogData()
{
    delete m_userQuery;
}


void PASimTableLogData::addRecordRange( const RecordRange & records )
{
	m_userQuery->setRecords( records );
}


void PASimTableLogData::removeRecordRange( const RecordRange & records )
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


void PASimTableLogData::doLog()
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


const RecordRange & PASimTableLogData::getRecordRange( ) const
{
    return m_userQuery->getRecords();
}

void PASimTableLogData::setOutputFormat(TableOutputFormat f)
{
    m_userQuery->setOutputFormat( f );
}

TableOutputFormat PASimTableLogData::getOutputFormat() const
{
    return m_userQuery->getOutputFormat();
}

