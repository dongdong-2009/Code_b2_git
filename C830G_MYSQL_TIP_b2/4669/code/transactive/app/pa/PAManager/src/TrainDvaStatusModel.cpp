/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/pa/PAManager/src/TrainDvaStatusModel.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * TrainDvaStatusModel controls additional layer of business logic for DVA
  * specific broadcast dialog
  *
  */

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/TrainDvaStatusModel.h"
#include "app/pa/PAManager/src/StationBroadcastModel.h"
#include "core/exceptions/src/PasExceptions.h"
#include "app/pa/PAManager/src/PaManagerModel.h"

#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TrainDvaStatusModel::TrainDvaStatusModel(GenericStatusPage& page, 
                                         const std::string& broadcastId)
:
DvaStatusModel(page, broadcastId, GenericStatusModel::TRAIN),
m_hasOverrideOption(false)
{
}


TrainDvaStatusModel::~TrainDvaStatusModel()
{
}


bool TrainDvaStatusModel::getHasOverrideOption()
{
    return m_hasOverrideOption;
}