/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Huang Qi
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifndef SCHEDULEBROADCAST__INCLUDED_
#define SCHEDULEBROADCAST__INCLUDED_

struct TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig;

namespace TA_IRS_App
{

class ScheduleBroadcast
{
public:
	ScheduleBroadcast( const TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig& refConfig );
	virtual ~ScheduleBroadcast();

    void executeBroadcast() const;

    time_t getStartTime() const;
    unsigned long getPkey() const;
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig& getConfig() const;

    void setToEnable();
    void setToDisable();
    bool isEnable();

private:
    ScheduleBroadcast();

private:
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* m_pConfig;
    bool m_bEnable;
};

}

#endif // ifndef (SCHEDULEBROADCAST__INCLUDED_)
