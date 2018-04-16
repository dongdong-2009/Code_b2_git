#pragma once;

#include <list>
#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>
// Avoid Windows build error
#define _WINSOCKAPI_
#define _WINIOCTL_
#include <boost/property_tree/json_parser.hpp>
#undef _WINIOCTL_
#undef _WINSOCKAPI_
#include <boost/foreach.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_product.hpp>
#include <boost/algorithm/string.hpp>

#define _A(str) _TMP(str)
#define _TMP(str) #str

#define DEF_JSON_SERIALIZE( r, data, elem ) pt->put(_A(elem), o.elem);

template<typename T>
void jsonSerialize(const T& o, boost::property_tree::ptree* pt)
{
    if(pt != NULL)
    {
        pt->put("", o);
    }
}

#define TA_SERIALIZATION( f, members ) void jsonSerialize(const f & o, boost::property_tree::ptree *pt) \
    {\
        if (pt != NULL){ BOOST_PP_SEQ_FOR_EACH( DEF_JSON_SERIALIZE,, members )}\
    }


class Serialization
{
public:
    Serialization() {}

    template<typename T>
    static std::string serialize(const T& oSrc, const std::string& nameStr);

    template<typename T>
    static std::string serialize(const std::list<T>& oSrc, const std::string& nameStr);

    template<typename T>
    static std::string serialize(const std::vector<T>& oSrc, const std::string& nameStr);

    template<typename T>
    static std::string structSerialize(const T& oSrc, const std::string& nameStr);
};

template<typename T>
std::string Serialization::serialize(const T& oSrc, const std::string& nameStr)
{
    boost::property_tree::ptree pt;
    std::stringstream ss;
    ss << oSrc;
    pt.put(nameStr, ss.str());
    std::ostringstream buf;
    boost::property_tree::json_parser::write_json(buf, pt, false);
    std::string s = buf.str();
    boost::erase_all(s, "\n");
    return s;
}

template<typename T>
std::string Serialization::serialize(const std::list<T>& oSrc, const std::string& nameStr)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ptree children;
    for(std::list<T>::const_iterator it = oSrc.begin();
        it != oSrc.end(); ++it)
    {
        boost::property_tree::ptree child;
        jsonSerialize(*it, &child);
        children.push_back(std::make_pair("", child));
    }
    pt.add_child(nameStr, children);
    std::ostringstream buf;
    boost::property_tree::json_parser::write_json(buf, pt, false);
    std::string s = buf.str();
    boost::erase_all(s, "\n");
    return s;
}

template<typename T>
std::string Serialization::serialize(const std::vector<T>& oSrc, const std::string& nameStr)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ptree children;
    for(std::vector<T>::const_iterator it = oSrc.begin();
        it != oSrc.end(); ++it)
    {
        boost::property_tree::ptree child;
        jsonSerialize(*it, &child);
        children.push_back(std::make_pair("", child));
    }
    pt.add_child(nameStr, children);
    std::ostringstream buf;
    boost::property_tree::json_parser::write_json(buf, pt, false);
    std::string s = buf.str();
    boost::erase_all(s, "\n");
    return s;
}

template<typename T>
std::string Serialization::structSerialize(const T& oSrc, const std::string& nameStr)
{
    boost::property_tree::ptree pt;
    boost::property_tree::ptree child;
    jsonSerialize(oSrc, &child);
    pt.add_child(nameStr, child);
    std::ostringstream buf;
    boost::property_tree::json_parser::write_json(buf, pt, false);
    std::string s = buf.str();
    boost::erase_all(s, "\n");
    return s;
}