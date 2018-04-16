/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/InvalidatePASDataPoints.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
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

    delete this;
}

void InvalidateMasterPASDataPoints::cancel()
{
    delete this;
}

} // namespace TA_IRS_App

