#pragma once
#include <string>

struct ParameterData
{
public:
    ParameterData( const unsigned long pkey,
                   const unsigned long fkStepDetails,
                   const unsigned long fkNodeTree,
                   const int position,
                   const std::string& value,
                   const unsigned long fkStepType,
                   const std::string& stepTypeName ):
        m_pkey( pkey ),
        m_fkStepDetails( fkStepDetails ),
        m_fkNodeTree( fkNodeTree ),
        m_position( position ),
        m_value( value ),
        m_fkStepType( fkStepType ),
        m_stepTypeName( stepTypeName ),
        m_dataLoaded( true )
    {
    }

    unsigned long m_pkey;
    unsigned long m_fkStepDetails;
    unsigned long m_fkNodeTree;
    int m_position;
    std::string m_value;
    unsigned long m_fkStepType;
    std::string m_stepTypeName;
    bool m_dataLoaded;
};
