/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/GenericBroadcastModel.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * GenericBroadcastModel contains shared functionality between broadcast models
  *
  */

#include "app/trains/TrainBorne_PA/src/stdafx.h"

#include "core/utilities/src/DebugUtil.h"

#include "app/trains/TrainBorne_PA/src/GenericBroadcastModel.h"
#include "app/trains/TrainBorne_PA/src/IGenericBroadcastPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GenericBroadcastModel::GenericBroadcastModel(IGenericBroadcastPage& page)
:
m_broadcastPage(page)
{
    FUNCTION_ENTRY( "GenericBroadcastModel" );
    FUNCTION_EXIT;
}


GenericBroadcastModel::~GenericBroadcastModel()
{
    FUNCTION_ENTRY( "~GenericBroadcastModel" );
    FUNCTION_EXIT;
}

