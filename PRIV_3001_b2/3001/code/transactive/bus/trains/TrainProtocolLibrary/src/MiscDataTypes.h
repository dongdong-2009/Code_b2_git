/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(ETrainArea_H)
#define ETrainArea_H

namespace TA_IRS_Bus
{

    /**
     * The area for the change area command/event
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:07 PM
     */
    enum ETrainArea
    {
        MainlineArea = 0,
        DepotArea = 1
    };


    /**
     * Represents normal or degraded mode for a train
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:07 PM
     */
    enum ETrainMode
    {
        TrainDegraded = 1,
        TrainNormal = 0
    };

} // TA_IRS_Bus

#endif // !defined(ETrainArea_H)
