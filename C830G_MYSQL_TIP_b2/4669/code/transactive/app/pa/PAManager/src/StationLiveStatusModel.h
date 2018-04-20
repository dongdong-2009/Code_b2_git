/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/pa/PAManager/src/StationLiveStatusModel.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * StationLiveStatusModel controls additional layer of business logic for Live
  * broadcast dialog
  *
  */

#if !defined(StationLiveStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define StationLiveStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#include "app/pa/PAManager/src/LiveStatusPage.h"
#include "app/pa/PAManager/src/LiveStatusModel.h"
	

class StationLiveStatusModel
: public LiveStatusModel
{
public:

    /**
     * StationLiveStatusModel (constructor)
     *
     * @param page the view associated with this model
     *
     * @param broadcastId the broadcastId to be associated with this status model
     *
     */
    StationLiveStatusModel(GenericStatusPage& page, const std::string& broadcastId);
       
    virtual ~StationLiveStatusModel();

protected:

private:

};

#endif // !defined(StationLiveStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
