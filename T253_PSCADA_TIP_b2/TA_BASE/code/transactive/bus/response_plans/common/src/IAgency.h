#ifndef IAGENCY_H__INCLUDED_
#define IAGENCY_H__INCLUDED_

namespace TA_Base_Bus
{

class IAgency
{
public:
    virtual void handleAck( const unsigned int ulToLocation, const unsigned int unId ) = 0;
    virtual void handleReq( const unsigned int ulToLocation, const unsigned int unId, const int eType, 
        const unsigned int ulPlanLocation, const unsigned int ulPlanId, const unsigned int ulInstanceNum, 
        const unsigned int ulStepNum, const unsigned int ulHandler, const std::vector<std::string>& vecParameters ) = 0;
};

}

#endif