#ifndef UTILITIES_H_INCLUDE
#define UTILITIES_H_INCLUDE

#ifdef _MSC_VER
    #pragma warning(disable:4996)
#endif

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/utilities/src/RunParams.h"
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <set>
#include <sstream>

using namespace TA_Base_Core;

namespace TA_Base_App
{
    struct StriCmp
    {
        bool operator()(const std::string& lhs, const std::string& rhs) const
        {
            return boost::ilexicographical_compare(lhs, rhs);
        }
    };

    struct Utilities
    {
        static unsigned long getMgrPort(const std::string& hostname)
        {
            return TA_Base_Core::getRunParamValue(getParameterName(hostname) + RPARAM_MGRPORT, TA_Base_Core::DEFAULT_MGR_PORT);
        }

        static std::string getParameterName(const std::string& name)
        {
            std::vector<std::string> vs;

            if (boost::starts_with(name, "--"))
            {
                std::string temp = name.substr(2);
                boost::split(vs, temp, boost::is_any_of("-"));
            }
            else
            {
                boost::split(vs, name, boost::is_any_of("-"));
            }

            std::stringstream strm;

            BOOST_FOREACH(std::string& s, vs)
            {
                if (s.size())
                {
                    s[0] = ::toupper(s[0]);
                    strm << s;
                }
            }

            return strm.str();
        }

        static std::string mergeCsvStrings(const std::string& s1, const std::string& s2)
        {
            if (s1.empty() || s2.empty())
            {
                return s1 + s2;
            }

            std::string result;
            std::set<std::string> vs;
            std::string merge = s1 + "," + s2;
            boost::split(vs, merge, boost::is_any_of(","));

            BOOST_FOREACH(const std::string& s, vs)
            {
                if (result.empty())
                {
                    result = s;
                }
                else
                {
                    if (s.size())
                    {
                        result.append(",").append(s);
                    }
                }
            }

            return result;
        }

        static IEntityDataPtr getEntityFromList(IEntityDataList entities, unsigned long key)
        {
            BOOST_FOREACH(IEntityData* entity, entities)
            {
                if (entity->getKey() == key)
                {
                    return IEntityDataPtr(entity);
                }
            }

            return IEntityDataPtr();
        }
    };
}

#endif
