/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_manager/src/SchedulingManagerStructs.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#ifndef SCHEDULING_MANAGER_STRUCT_H
#define SCHEDULING_MANAGER_STRUCT_H

#include <string>
#include <vector>
#include <map>

namespace TA_Base_Core
{
    class IProfile;
    class IOperator;
    class ILocation;
}

struct NameValuePair
{
    std::string name;
    std::string value;
};

typedef std::map<unsigned long, TA_Base_Core::ILocation*> LocationMap;
typedef std::map<unsigned long, TA_Base_Core::IProfile*> ProfileMap;
typedef std::vector<TA_Base_Core::IProfile*> ProfileVector;


#endif // SCHEDULING_MANAGER_STRUCT_H