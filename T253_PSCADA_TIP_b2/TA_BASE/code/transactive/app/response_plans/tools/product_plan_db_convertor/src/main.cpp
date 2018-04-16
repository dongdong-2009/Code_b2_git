#include <iostream>
#include <fstream>
#include <string>
#include "ParamConvertor.h"
#include "FlowCreator.h"
#include "ParallelCreator.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtilInit.h"

void printHelp()
{
    std::cout << std::endl << "usage: Copy PlanDbToolConfig.ini with this application" << std::endl;
}

int main( int argc, char** argv )
{
    //if(argc < 2)
    //{
    //    printHelp();
    //    return 0;
    //}
    std::string runParam = "--run-param-file=PlanDbToolConfig.ini --debug-file=PlanDbConvertLog.log --debug-level=INFO";
    //runParam += argv[1];
    TA_Base_Core::RunParams::getInstance().parseCmdLine( runParam.c_str() );
    TA_Base_Core::gSetDebugUtilFromRunParams();

    if( !TA_Base_Core::RunParams::getInstance().isSet( RPARAM_DBCONNECTIONFILE ) )
    {
        printHelp();
        return 0;
    }

    try
    {
        //if(argc > 2)
        //{

        ParallelCreator oParallelCreator;
        std::ofstream ofParallel( "3.2.CreateParallelSteps.sql", std::ios::out | std::ios::trunc );
        oParallelCreator.generateSql( ofParallel );
        ofParallel.close();

        ParamConvertor oParamConvertor( oParallelCreator.getUpdatedParams() );
        std::ofstream ofParam( "3.4.ConvertParameters.sql", std::ios::out | std::ios::trunc );
        oParamConvertor.generateSql( ofParam );
        ofParam.close();

        FlowCreator oFlowCreator;
        std::ofstream ofFlow( "3.3.CreateFlows.sql", std::ios::out | std::ios::trunc );
        oFlowCreator.attachFlowSql( oParallelCreator.flowSqls() );
        oFlowCreator.generateSql( ofFlow );
        ofFlow.close();
        //}
        //else
        //{
        //    convertor.generateSql(std::cout);
        //}
    }
    catch( ... )
    {
    }
    return 0;
}
