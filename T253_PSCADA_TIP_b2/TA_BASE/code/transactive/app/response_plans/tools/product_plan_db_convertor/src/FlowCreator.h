#pragma once

#include <sstream>
#include <vector>
#include <string>
#include "PlanStep.h"

class FlowCreator
{
public:
    FlowCreator();
    ~FlowCreator();

    void generateSql( std::ostream& outPut );
    void getSteps( const int eType, const unsigned long ulStepKey, std::vector<unsigned int>& vecSteps );
    void attachFlowSql( const std::vector<std::string>& vecFlows );

private:
    void create();
    void loadData();
    unsigned int getStepId( const std::string& strStep );

private:
    std::vector<PlanStep> m_vecSteps;
    std::vector<std::string> m_vecFlowSqls;
};

