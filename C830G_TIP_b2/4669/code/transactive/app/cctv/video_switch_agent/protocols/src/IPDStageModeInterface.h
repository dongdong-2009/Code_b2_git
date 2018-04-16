/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  oliverk
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(IPDStageModeInterface_H)
#define IPDStageModeInterface_H

namespace TA_IRS_App
{
    /**
     * @author oliverk
     * @version 1.0
     * @created 05-Feb-2008 2:43:31 PM
     */
    class IPDStageModeInterface
    {

    public:

        virtual ~IPDStageModeInterface() {};


        /**
         * 
         * @param originatingStage
         * @param destinationStage
         * @param allowSwitch
         */
        virtual void setStageSwitchMode(unsigned long originatingStage, unsigned long destinationStage, bool allowSwitch) =0;

    };

}
#endif // !defined(IPDStageModeInterface_H)
