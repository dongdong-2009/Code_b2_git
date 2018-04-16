#ifndef TRAINBORNEHEARTBEATPROCESSOR_H
#define TRAINBORNEHEARTBEATPROCESSOR_H

#include "core/threads/src/Thread.h"

namespace TA_IRS_App
{

class TVSSServant;

class TrainBorneHeartBeatProcessor : public TA_Base_Core::Thread
{
public:
    TrainBorneHeartBeatProcessor(TVSSServant* servant, unsigned long heartbitInterval = 5000);
    virtual ~TrainBorneHeartBeatProcessor();

	void setControlMode();
	void setMonitorMode();

    virtual void run();
    virtual void terminate();
private:
    TVSSServant* m_tvssServant;
    unsigned long m_heartbitInterval;
    bool m_terminate;
};

};

#endif