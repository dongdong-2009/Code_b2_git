/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/StationLiveStatusModel.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
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

    void removeAnnouncement();

protected:

private:

};

#endif // !defined(StationLiveStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
