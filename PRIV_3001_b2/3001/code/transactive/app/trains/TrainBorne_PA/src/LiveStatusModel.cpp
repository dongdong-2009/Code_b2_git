/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/LiveStatusModel.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * LiveStatusModel controls the shared business logic for live broadcast status dialog screens
  */

#include "app/trains/TrainBorne_PA/src/StdAfx.h"
#include "app/trains/TrainBorne_PA/src/LiveStatusModel.h"
#include "core/exceptions/src/PasExceptions.h"

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

