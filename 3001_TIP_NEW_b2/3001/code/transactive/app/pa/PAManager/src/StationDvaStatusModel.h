/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/StationDvaStatusModel.h $
  * @author Jade Lee
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2014/05/16 18:00:29 $
  * Last modified by: $Author: qi.huang $
  * 
  * StationDvaStatusModel controls additional layer of business logic for DVA
  * specific broadcast dialog
  *
  */

#if !defined(StationDvaStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define StationDvaStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#include "app/pa/PAManager/src/DvaStatusPage.h"
#include "app/pa/PAManager/src/DvaStatusModel.h"
	
class StationDvaStatusModel
: public DvaStatusModel
{
public:

    /**
     * StationDvaStatusModel (constructor)
     *
     * @param page the view associated with this model
     *
     * @param broadcastId the broadcastId to be associated with this status model
     *
     */
    StationDvaStatusModel(GenericStatusPage& page, const std::string& broadcastId);
       
    virtual ~StationDvaStatusModel();

    void removeAnnouncement();

protected:

private:

};

#endif // !defined(StationDvaStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
