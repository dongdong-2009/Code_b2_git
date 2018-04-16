/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimulatorModuleCreator.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#if !defined(ATSSimulatorModuleCreator_H)
#define ATSSimulatorModuleCreator_H

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
namespace ATS_Sim
{

    class ATSSimulatorModuleCreator : public TA_Base_Bus::GssAbstractModuleCreator
    {

    public:

        ATSSimulatorModuleCreator(const std::string & type);

        virtual ~ATSSimulatorModuleCreator();

        virtual TA_Base_Bus::ISimulationModule * createSimulationModule(const std::string & name, const std::string & parameters);

    private:
        void usage(std::ostream & os);
    }; // class ATSSimulatorModuleCreator


} // namespace ATS_Sim
} // namespace TA_IRS_Bus

#endif // !defined(ATSSimulatorModuleCreator_H)
