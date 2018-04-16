#pragma once

struct Utilities
{
    template <typename KeyT, typename ValueT>
    static ValueT find(const KeyT& key, const std::map<KeyT, ValueT>& map, const ValueT defaultValue = ValueT())
    {
        std::map<KeyT, ValueT>::const_iterator it = map.find(key);

        if (it != map.end())
        {
            return it->second;
        }

        return defaultValue;
    }

    struct StriCmp
    {
        bool operator()(const std::string& lhs, const std::string& rhs) const
        {
            return boost::ilexicographical_compare(lhs, rhs);
        }
    };

    struct IsEmptyString
    {
        bool operator()(const std::string& str) const
        {
            return str.empty();
        }
    };

    static std::string getLocationName();
    static std::string getLocationName(unsigned long locationKey, const std::string& defaultName = "");
    static std::string makeRunParameterName(const std::string& name);
    static std::string getDebugLogDir();
    static void checkConsoleManagerInstance(int port);
    static void changeProfile(const std::string& profileIdParam, const std::string& locationIdParam, const std::string& passwordParam);
    static bool retrieveSessionId();
};
