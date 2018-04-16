/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/pa/PAManager/src/IRestartApplicationCallback.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
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