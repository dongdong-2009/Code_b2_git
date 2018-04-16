/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimulatorModuleCreator.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  */

#if !defined(PASimulatorModuleCreator_H)
#define PASimulatorModuleCreator_H

#ifdef WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4284)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/GssAbstractModuleCreator.h"

namespace TA_IRS_Bus
{
    class ISimulationModule;
}

namespace TA_IRS_App
{
namespace PA_Sim
{

    class PASimulatorModuleCreator : public TA_Base_Bus::GssAbstractModuleCreator
    {

    public:

        PASimulatorModuleCreator(const std::string & type);

        virtual ~PASimulatorModuleCreator();

        virtual TA_Base_Bus::ISimulationModule * createSimulationModule(const std::string & name, const std::string & parameters);

    private:
        void usage(std::ostream & os);
    }; // class PASimulatorModuleCreator


} // namespace PA_Sim
} // namespace TA_IRS_Bus

#endif // !defined(PASimulatorModuleCreator_H)
