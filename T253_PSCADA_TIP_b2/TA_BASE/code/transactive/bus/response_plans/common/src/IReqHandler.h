#ifndef IREQHANDLER_H__INCLUDED_
#define IREQHANDLER_H__INCLUDED_

#include <vector>
#include <string>

namespace TA_Base_Bus
{

const unsigned int ANY_HANDLER = 0u;

class IReqHandler
{
public:
    virtual void handle( const unsigned int unId, const int eType, const unsigned int ulPlanLocation, 
        const unsigned int ulPlanId, const unsigned int ulInstanceNum, const unsigned int ulStepNum, 
        const std::vector<std::string>& vecParameters ) = 0;
};

}

#endif