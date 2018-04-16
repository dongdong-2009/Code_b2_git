/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/IRestartApplicationCallback.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Basic interface to receive application restart requests
  *
  */

#pragma once

interface IRestartApplicationCallback
{
    public:
        /**
         * Called to request the application restarts.  If this is not possible the application
         *  should display an error to the operator.
         *
         * @param leftPos the x co-ordinate (absolute) of the left side of screen we want
         *          application to be launched on
         *
         */
        virtual void restartApplication(int leftPos) = 0;
};