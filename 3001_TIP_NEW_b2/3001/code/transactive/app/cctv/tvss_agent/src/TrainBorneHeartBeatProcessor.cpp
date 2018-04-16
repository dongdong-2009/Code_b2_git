#include "app/cctv/tvss_agent/src/TrainBorneHeartBeatProcessor.h"
#include "app/cctv/tvss_agent/src/TVSSServant.h"

namespace TA_IRS_App
{

TrainBorneHeartBeatProcessor::TrainBorneHeartBeatProcessor( TVSSServant* servant, unsigned long heartbitInterval /*= 5000*/ )
    : m_tvssServant( servant ),
      m_heartbitInterval( heartbitInterval ),
      m_terminate( true )
{
}

TrainBorneHeartBeatProcessor::~TrainBorneHeartBeatProcessor()
{
    terminateAndWait();
}

void TrainBorneHeartBeatProcessor::setControlMode()
{
    m_terminate = false;
    start();
}

void TrainBorneHeartBeatProcessor::setMonitorMode()
{
    terminateAndWait();
}

void TrainBorneHeartBeatProcessor::run()
{
    while ( !m_terminate )
    {
        //servant process alarm status
        m_tvssServant->tvssTrainborneHeartbeat();
        sleep( m_heartbitInterval );
    }
}

void TrainBorneHeartBeatProcessor::terminate()
{
    m_terminate = true;
}

}