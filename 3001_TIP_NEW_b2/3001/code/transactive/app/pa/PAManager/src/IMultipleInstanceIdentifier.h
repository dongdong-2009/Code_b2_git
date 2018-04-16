/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/IMultipleInstanceIdentifier.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Interface to provide functions which allow generation of
  *     window titles that are unique amongst instances.  The
  *     window title is required to distinguish instances from
  *     each other.  Could use EnumWindows in order to make
  *     the distinction without using window titles, but this would
  *     be less lightweight.
  *
  */

#ifndef _IMultipleInstanceIdentifier_HEADER_
#define _IMultipleInstanceIdentifier_HEADER_

class IMultipleInstanceIdentifier
{
public:
    virtual const std::string getBasicWindowName() const = 0;

    virtual const std::string getExactName(const std::string& stationName, 
                                           const std::string& screenIdentifier) = 0;

    virtual std::string getExactName(const PaManagerTarget& targetInstance) = 0;
};

