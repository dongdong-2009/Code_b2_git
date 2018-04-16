/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/LiveStatusModel.cpp $
  * @author Jade Lee
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2015/11/27 17:46:47 $
  * Last modified by: $Author: qi.huang $
  * 
  * LiveStatusModel controls the shared business logic for live broadcast status dialog screens
  */

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/LiveStatusModel.h"
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

