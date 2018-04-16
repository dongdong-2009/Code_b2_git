#pragma once
#include "ParameterData.h"
#include <vector>
#include <ostream>
#include <map>
#include <set>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

class ParamConvertor
{
public:
    ParamConvertor();
    ~ParamConvertor( void );
    void generateSql( std::ostream& outPut );
private:
    void initDataPointParams();
    void loadData();
    std::vector<ParameterData> m_parameterDatas;

    struct DataPointParamInfo
    {
        unsigned long m_nodeTree;
        int m_stepParamIndex;
        unsigned long m_stepKey;
        unsigned long m_pkey;
        std::string m_name;
        int m_valueType;
        int m_position;
    };

    std::map<unsigned long, DataPointParamInfo> m_mapDataPointParams;

    enum EValueType
    {
        BOOLEAN_DATA_POINT,
        NUMERIC_DATA_POINT,
        TEXT_DATA_POINT,
        UNDEFINED_DATA_POINT
    };

    class ValidDataPoint
    {
    public:
        void init();
        bool needChange( const std::string& name );
        std::string getValidDataPoint( unsigned long planId, int valueType, const std::string& type, const std::string& location );
        static std::string getTypeByDataPointName( const std::string& name );
        static int getValueTypeByScadaType( const std::string& scadaType );
    private:
        struct DataPoint
        {
            std::string m_name;
            int m_valueType;
            std::string m_type;
            std::string m_location;
        };

        unsigned long m_currentPlan;
        std::set<std::string> m_usedDataPoints;

        typedef boost::multi_index::member<DataPoint, std::string, &DataPoint::m_name> IndexByName;
        typedef boost::multi_index::composite_key <
        DataPoint,
        boost::multi_index::member<DataPoint, std::string, &DataPoint::m_location>,
        boost::multi_index::member<DataPoint, std::string, &DataPoint::m_type>,
        boost::multi_index::member<DataPoint, int, &DataPoint::m_valueType>
        > IndexByType;

        typedef boost::multi_index::multi_index_container <
        DataPoint,
        boost::multi_index::indexed_by <
        boost::multi_index::hashed_unique<IndexByName>,
        boost::multi_index::ordered_non_unique <IndexByType>
        >
        > Container;

        Container m_allValidDataPoints;

        std::set<std::string> m_setAllDataPoints;
    };

    ValidDataPoint m_validDataPoint;

};
