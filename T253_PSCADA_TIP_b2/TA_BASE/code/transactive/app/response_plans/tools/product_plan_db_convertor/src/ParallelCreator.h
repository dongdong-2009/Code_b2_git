#pragma once

#include <sstream>
#include <vector>
#include <map>
#include <string>
#include "NoWaitStep.h"

class ParallelCreator
{
public:
    ParallelCreator();
    ~ParallelCreator();

    void generateSql( std::ostream& outPut );
    std::vector<std::string>& flowSqls();
	std::map<unsigned long, std::string> getUpdatedParams() { return m_mapUpdatedParams; }

private:
    void create();
    void loadData();
    void getStepPostions( const int eType, const unsigned long ulStepKey, std::map<unsigned long, unsigned int>& vecSteps );
    void getStepPostionsForFlow( const int eType, const unsigned long ulStepKey, const unsigned int unInPos, std::multimap<unsigned int, unsigned int>& mapSteps );
    unsigned int getStepPos( const std::string& strStep, const bool bByKey = false );
    unsigned long getNewPkey();
    void createFlowForNextSteps( std::stringstream& stmFlowSql, const unsigned long ulPlan, const std::multimap<unsigned int, unsigned int>& mapNextSteps, const std::map<unsigned int, unsigned int>& mapOldPosToNewPos, unsigned long& sfPkey, unsigned int& id );
    void updateStepParam( std::stringstream& stmSqlGen, const std::map<unsigned long, unsigned int>& mapNextSteps, const std::map<unsigned int, unsigned int>& mapOldPosToNewPos );
    void pushFlowSql( std::stringstream& stmFlowSql, const unsigned long ulPlan, const unsigned int unIncoming, const unsigned int unOutgoing, unsigned long& sfPkey, unsigned int& id );

private:
    std::vector<NoWaitStep> m_vecSteps;
    std::vector<std::string> m_vecParallelSqls;
    std::vector<std::string> m_vecFlowSqls;
	std::map<unsigned long, std::string> m_mapUpdatedParams;
};

