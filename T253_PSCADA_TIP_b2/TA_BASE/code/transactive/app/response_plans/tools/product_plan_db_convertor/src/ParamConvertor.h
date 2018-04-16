#pragma once
#include <vector>
#include <ostream>
#include <map>
#include "ParameterData.h"

class ParamConvertor
{
public:
    ParamConvertor( const std::map<unsigned long, std::string>& updatedParams );
    ~ParamConvertor( void );
    void generateSql( std::ostream& outPut );
private:
    void convert();
    void loadData();
    std::vector<ParameterData> m_oldParameterDatas;
    std::vector<std::pair<ParameterData, bool>> m_newParameterDatas;
    std::map<unsigned long, std::string> m_mapUpdatedParams;
};
