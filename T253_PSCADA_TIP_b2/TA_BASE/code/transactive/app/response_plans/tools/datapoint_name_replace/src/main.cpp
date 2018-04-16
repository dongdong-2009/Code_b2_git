#include <iostream>
#include <fstream>
#include <string>
#include "ParamConvertor.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtilInit.h"

void printHelp()
{
    std::cout << std::endl << "usage: Copy PlanDbToolConfig.ini with this application" << std::endl;
}

int main( int argc, char** argv )
{
    std::string runParam = "--run-param-file=PlanDbToolConfig.ini --debug-file=DataPointNameReplace.log --debug-level=INFO";
    TA_Base_Core::RunParams::getInstance().parseCmdLine( runParam.c_str() );
    TA_Base_Core::gSetDebugUtilFromRunParams();

    if( !TA_Base_Core::RunParams::getInstance().isSet( RPARAM_DBCONNECTIONFILE ) )
    {
        printHelp();
        return 0;
    }

    try
    {
        ParamConvertor oParamConvertor;
        std::ofstream ofParam( "DataPointNameReplace.sql", std::ios::out | std::ios::trunc );
        oParamConvertor.generateSql( ofParam );
        ofParam.close();
    }
    catch( ... )
    {
    }
    return 0;
}
