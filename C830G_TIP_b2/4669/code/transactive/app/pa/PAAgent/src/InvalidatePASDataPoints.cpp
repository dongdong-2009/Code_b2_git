/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/InvalidatePASDataPoints.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/pa/PAAgent/src/InvalidatePASDataPoints.h"
#include "app/pa/PAAgent/src/PasTableManager.h"

namespace TA_IRS_App
{

InvalidatePASDataPoints::InvalidatePASDataPoints( TA_IRS_Bus::Scheduler& processScheduler,
                                                  PasTableManager& pasTableManager )
    : m_processScheduler( processScheduler )
    , m_pasTableManager( pasTableManager )
{
}

InvalidatePASDataPoints::~InvalidatePASDataPoints()
{
}

void InvalidatePASDataPoints::consume()
{
    this->m_processScheduler.post( new InvalidateTable550( this->m_pasTableManager.getTable550() ) );
    this->m_processScheduler.post( new InvalidateTable570( this->m_pasTableManager.getTable570() ) );
}


InvalidateStationPASDataPoints::InvalidateStationPASDataPoints( TA_IRS_Bus::Scheduler& processScheduler,
                                                                PasTableManager& pasTableManager )
    : InvalidatePASDataPoints( processScheduler, pasTableManager )
{
}

InvalidateStationPASDataPoints::~InvalidateStationPASDataPoints()
{
}

void InvalidateStationPASDataPoints::consume()
{
    InvalidatePASDataPoints::consume();

    this->m_processScheduler.post( new InvalidateTable200( this->m_pasTableManager.getTable200() ) );

    delete this;
}

void InvalidateStationPASDataPoints::cancel()
{
    delete this;
}

InvalidateMasterPASDataPoints::InvalidateMasterPASDataPoints( TA_IRS_Bus::Scheduler& processScheduler,
                                                                PasTableManager& pasTableManager )
    : InvalidatePASDataPoints( processScheduler, pasTableManager )
{
}

InvalidateMasterPASDataPoints::~InvalidateMasterPASDataPoints()
{
}

void InvalidateMasterPASDataPoints::consume()
{
    InvalidatePASDataPoints::consume();

    this->m_processScheduler.post( new InvalidateTable1200( this->m_pasTableManager.getTable1200() ) );

    delete this;
}

void InvalidateMasterPASDataPoints::cancel()
{
    delete this;
}

} // namespace TA_IRS_App

