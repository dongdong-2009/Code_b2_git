/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File$
  * @author Jade Lee
  * @version $Revision$
  * Last modification: $DateTime$
  * Last modified by: $Author$
  * 
  * RunParamListener listens out for general RunParam changes, 
  *     and queue changes (to ensure non are missed)
  *
  */

#ifndef IRUNPARAMCHANGEDLISTENER__INCLUDED_
#define IRUNPARAMCHANGEDLISTENER__INCLUDED_

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

#endif // IRUNPARAMCHANGEDLISTENER__INCLUDED_