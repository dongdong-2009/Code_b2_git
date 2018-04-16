#ifndef IBROADCASTMANAGER_H
#define IBROADCASTMANAGER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/IBroadcastManager.h $
  * @author  A. Parker
  * @version $Revision: #2 $
  * Last modification: $Date: 2012/02/06 $
  * Last modified by:  $Author: haijun.li $
  *
  *
  */

#include <string>

namespace TA_IRS_App
{

class IBroadcastManager
{
public:
    virtual void delBroadcast( const std::string& strBroadcastId ) = 0;
};

} // namespace TA_IRS_App

#endif // IBROADCASTMANAGER_H

