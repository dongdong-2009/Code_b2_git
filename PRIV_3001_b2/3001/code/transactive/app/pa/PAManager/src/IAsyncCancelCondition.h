/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/IAsyncCancelCondition.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Provides basic interface to test if a cancel (termination) condition has been met
  *
  */

#pragma once

interface IAsyncCancelCondition
{
public:
    virtual bool shouldCancel() = 0;
};
