/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/pa/PAManager/src/LiveStatusModel.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * LiveStatusModel controls the shared business logic for live broadcast status dialog screens
  */

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/LiveStatusModel.h"
#include "app/pa/PAManager/src/StationBroadcastModel.h"
#include "core/exceptions/src/PasExceptions.h"
#include "app/pa/PAManager/src/PaManagerModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LiveStatusModel::LiveStatusModel(GenericStatusPage& page, 
                                 const std::string& broadcastId,
                                 const EModelSubType& subType)
:
GenericStatusModel(page, broadcastId, subType)
{

}

