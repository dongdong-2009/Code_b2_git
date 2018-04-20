/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/pa/PAManager/src/IRunParamChangedListener.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * RunParamListener listens out for general RunParam changes, 
  *     and queue changes (to ensure non are missed)
  *
  */

#if !defined(IRunParamChangedListener_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define IRunParamChangedListener_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>

class IRunParamChangedListener
{
public:
    /**
     * processRunParamChange
     *
     * Performs processing of run parameter changes
     *
     * @param value the string identifying the cahnged value
     *
     * @param name the name of the parameter value
     *
     * @exception InvalidPasConfigurationException if expected input parameters 
     *      (contained in 'value' were not found - meaning bad format)
     *
     */
    virtual void processRunParamChange(const std::string& name, const std::string& value) = 0;
};


#endif // IRunParamChangedListener_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_