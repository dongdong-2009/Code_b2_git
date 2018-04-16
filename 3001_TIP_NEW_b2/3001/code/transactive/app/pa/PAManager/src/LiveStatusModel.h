/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/LiveStatusModel.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * LiveStatusModel controls the shared business logic for live broadcast status dialog screens
  */

#if !defined(LiveStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define LiveStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#include "app/pa/PAManager/src/GenericStatusModel.h"

class LiveStatusModel : public GenericStatusModel
{
public:

    /**
     * LiveStatusModel(constructor)
     *
     */
    LiveStatusModel(GenericStatusPage& page, 
                    const std::string& broadcastId,
                    const EModelSubType& subType);
    
protected:


private:

};

#endif // !defined(LiveStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
