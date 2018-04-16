#pragma once
#include <string>

class ParameterData
{
public:
    ParameterData(void);
    ParameterData(const unsigned long pkey);
    ParameterData(const unsigned long pkey,
                  const unsigned long fkStepDetails,
                  const unsigned long fkNodeTree,
                  const int position,
                  const std::string& value,
                  const unsigned long fkStepType,
                  const std::string& stepTypeName);
    ~ParameterData(void);
    int getPostion();
    unsigned long getPkey();
    unsigned long getFkStepDetails();
    unsigned long getFkNodeTree();
    std::string getValue();
    unsigned long getFkStepType();
    std::string getStepTypeName();
private:
    static unsigned long INVALID_PKEY;
    void loadData();
    unsigned long m_pkey;
    unsigned long m_fkStepDetails;
    unsigned long m_fkNodeTree;
    int m_position;
    std::string m_value;
    unsigned long m_fkStepType;
    std::string m_stepTypeName;
    bool m_dataLoaded;
};
