/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/pa/PAManager/src/LiveStatusModel.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
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
