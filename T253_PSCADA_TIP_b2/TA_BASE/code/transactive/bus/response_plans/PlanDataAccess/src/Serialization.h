/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/common/src/Serialization.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/21 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Serialization commonly used by active and editor steps.
*/


#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <list>
#include <vector>
#include <string>
#include <boost/type_traits/is_same.hpp>
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

namespace TA_Base_Bus
{
	using boost::property_tree::ptree;
	using boost::core::is_same;

#define TA_SERIALIZATION( TypeName, members ) namespace ObjectSerialization \
    { \
        template<> inline void serialize( TypeName& refTarget, bool bInverse, boost::property_tree::ptree& pt, const std::string& strName ) \
        { \
            BOOST_PP_SEQ_FOR_EACH(DEF_EACH_SERIALIZE,, members) \
        } \
    }

	namespace ObjectSerialization
	{

#define _A(str) (#str)

#define DEF_EACH_SERIALIZE( r, data, elem ) serialize( refTarget.elem, bInverse, pt, _A(elem) );

		template<typename TypeName>
		void serialize(TypeName& refTarget, bool bInverse, boost::property_tree::ptree& pt, const std::string& strName)
		{
			if (bInverse)
			{
				pt.put(strName, refTarget);
			}
			else
			{
				refTarget = pt.get<TypeName>(strName);
			}
		}
	}


	class Serialization
	{
	public:
		typedef std::vector<std::string> StringVector;

		Serialization();
		Serialization(const StringVector& strSource);
		StringVector stringVector();
		bool isSerialize();
		ptree& getPtree();
		bool isPtreeEmpty();

		template<typename T>
		void serialize(T& oSrc, std::string nameStr);

		template<typename T>
		void serialize(std::list<T>& oSrc, std::string nameStr);

		template<typename T>
		void serialize(std::vector<T>& oSrc, std::string nameStr);

		template<typename T>
		void serialize(std::vector< std::vector<T> >& matrix, std::string nameStr);

		template<typename T>
		void structSerialize(T& oSrc, std::string nameStr);

		template<typename Object>
		void serializeMLPos(Object& objectType);

	private:
		bool m_bInverse;
		StringVector m_tmpResult;
		ptree m_ptResult;
	};


	template<typename T>
	void Serialization::serialize(T& oSrc, std::string nameStr)
	{
		if (m_bInverse)
		{
			m_ptResult.put(nameStr, oSrc);
			std::ostringstream buf;
			boost::property_tree::json_parser::write_json(buf, m_ptResult, false);
			m_tmpResult.push_back(buf.str());
			m_ptResult.clear();
		}
		else
		{
			if (!m_tmpResult.empty())
			{
				getPtree();
				oSrc = m_ptResult.get<T>(nameStr);
				m_tmpResult.erase(m_tmpResult.begin());
				m_ptResult.clear();
			}
		}
	}


	template<typename T>
	void Serialization::serialize(std::list<T>& oSrc, std::string nameStr)
	{
		static const std::string EmptyChildName = "";

		if (m_bInverse)
		{
			boost::property_tree::ptree children;
			for (typename std::list<T>::iterator it = oSrc.begin(); it != oSrc.end(); ++it)
			{
				boost::property_tree::ptree child;
				ObjectSerialization::serialize(*it, m_bInverse, child, EmptyChildName);
				children.push_back(std::make_pair(EmptyChildName, child));
			}
			m_ptResult.add_child(nameStr, children);
			std::ostringstream buf;
			boost::property_tree::json_parser::write_json(buf, m_ptResult, false);
			m_tmpResult.push_back(buf.str());
			m_ptResult.clear();
		}
		else
		{
			if (!m_tmpResult.empty())
			{
				getPtree();				
				BOOST_FOREACH(boost::property_tree::ptree::value_type &v, m_ptResult.get_child(nameStr))
				{
					boost::property_tree::ptree& childparse = v.second;
					T t;
					ObjectSerialization::serialize(t, m_bInverse, childparse, EmptyChildName);
					oSrc.push_back(t);
				}
				m_tmpResult.erase(m_tmpResult.begin());
				m_ptResult.clear();
			}
		}
	}


	template<typename T>
	void Serialization::serialize(std::vector<T>& oSrc, std::string nameStr)
	{
		static const std::string EmptyChildName = "";

		if (m_bInverse)
		{
			boost::property_tree::ptree children;
			for (typename std::vector<T>::iterator it = oSrc.begin(); it != oSrc.end(); ++it)
			{
				boost::property_tree::ptree child;
				ObjectSerialization::serialize(*it, m_bInverse, child, EmptyChildName);
				children.push_back(std::make_pair(EmptyChildName, child));
			}
			m_ptResult.add_child(nameStr, children);
			std::ostringstream buf;
			boost::property_tree::json_parser::write_json(buf, m_ptResult, false);
			m_tmpResult.push_back(buf.str());
			m_ptResult.clear();
		}
		else
		{
			if (!m_tmpResult.empty())
			{
				getPtree();
				BOOST_FOREACH(boost::property_tree::ptree::value_type &v, m_ptResult.get_child(nameStr))
				{
					T t;
					ObjectSerialization::serialize(t, m_bInverse, v.second, EmptyChildName);
					oSrc.push_back(t);
				}
				m_tmpResult.erase(m_tmpResult.begin());
				m_ptResult.clear();
			}
		}
	}


	template<typename T>
	void Serialization::serialize(std::vector< std::vector<T> >& matrix, std::string nameStr)
	{
		static const std::string EmptyChildName = "";

		if (m_bInverse)
		{
			boost::property_tree::ptree ptCol;
			for (typename std::vector< std::vector<T> >::iterator itCol = matrix.begin(); itCol != matrix.end(); ++itCol)
			{
				boost::property_tree::ptree ptRow;
				std::vector<T> row = *itCol;
				for (typename std::vector<T>::iterator itCell= row.begin(); itCell != row.end(); ++itCell)
				{
					boost::property_tree::ptree cell;
					if (is_same<T, double>::value || is_same<T, float>::value)
					{						
						T t = *itCell;
						std::stringstream ss;
						ss << std::fixed << std::setprecision(3) << t;
						cell.put(EmptyChildName, ss.str());
					}
					else
					{
						ObjectSerialization::serialize(*itCell, m_bInverse, cell, EmptyChildName);
					}
					ptRow.push_back(std::make_pair(EmptyChildName, cell));
				}
				ptCol.push_back(std::make_pair(EmptyChildName, ptRow));
			}
			m_ptResult.add_child(nameStr, ptCol);
			std::ostringstream buf;
			boost::property_tree::json_parser::write_json(buf, m_ptResult, false);
			m_tmpResult.push_back(buf.str());
			m_ptResult.clear();
		}
		else
		{
			if (!m_tmpResult.empty())
			{
				getPtree();
				BOOST_FOREACH(boost::property_tree::ptree::value_type &ptCol, m_ptResult.get_child(nameStr))
				{
					std::vector<T> row;
					BOOST_FOREACH(boost::property_tree::ptree::value_type &ptRow, ptCol.second.get_child(EmptyChildName))
					{
						T t;
						ObjectSerialization::serialize(t, m_bInverse, ptRow.second, EmptyChildName);
						row.push_back(t);
					}
					matrix.push_back(row);
				}
				m_tmpResult.erase(m_tmpResult.begin());
				m_ptResult.clear();
			}
		}
	}


	template<typename T>
	void Serialization::structSerialize(T& oSrc, std::string nameStr)
	{
		static const std::string EmptyChildName = "";

		if (m_bInverse)
		{
			boost::property_tree::ptree child;
			ObjectSerialization::serialize(oSrc, m_bInverse, child, EmptyChildName);
			m_ptResult.add_child(nameStr, child);
			std::ostringstream buf;
			boost::property_tree::json_parser::write_json(buf, m_ptResult, false);
			m_tmpResult.push_back(buf.str());
			m_ptResult.clear();
		}
		else
		{
			if (!m_tmpResult.empty())
			{
				getPtree();
				boost::property_tree::ptree child = m_ptResult.get_child(nameStr);
				ObjectSerialization::serialize(oSrc, m_bInverse, child, EmptyChildName);
				m_tmpResult.erase(m_tmpResult.begin());
				m_ptResult.clear();
			}
		}
	}

	template<typename Object>
	void Serialization::serializeMLPos(Object& objectType)
	{
		static const std::string EmptyChildName = "";

		if (m_bInverse)
		{
			ObjectSerialization::serialize(objectType, m_bInverse, m_ptResult, EmptyChildName);
		}
		else
		{
			getPtree();
			ObjectSerialization::serialize(objectType, m_bInverse, getPtree(), EmptyChildName);
		}
	}
} // TA_Base_App

#endif // SERIALIZATION_H
