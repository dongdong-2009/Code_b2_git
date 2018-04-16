#include "ParamConvertor.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointAccessFactory.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataBaseHelper.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <iterator>

namespace
{

std::string paramNames[] =
{
    // ASSIGN_CAMERA_TO_MONITOR
    // ParamCamera,  ParamMonitor,
    "Camera", "Monitor",

    // ASSIGN_CAMERA_TO_PRESET
    // ParamCamera,  ParamPreset,
    "Camera", "Preset",

    // DATA_POINT_CHECK
    // ParamDataPoint,  ParamValueType,  ParamValue,  ParamTestType,  ParamSuccessStep,  ParamFailureStep,
    "Data Point Name", "Value Type", "Value", "Test Type", "Success Step", "Failure Step",

    // DATA_POINT_TIME_CHECK
    // ParamDataPoint,  ParamTimeSpan,  ParamSuccessStep,  ParamFailureStep,
    "Data Point Name", "Timespan", "Success Step", "Failure Step",

    // DATA_POINT_SET
    // ParamDataPoint,  ParamNewValueType,  ParamNewValue,
    "Data Point Name", "New Value Type", "New Value",

    // DECISION
    // ParamDecisionMessage,  ParamResponseTimeout,  ParamYesStep,  ParamNoStep,  ParamYesButtonLabel,  ParamNoButtonLabel,  ParamPauseButtonLabel,  ParamNoButtonEnabled, ParamPauseButtonEnabled,
    "Decision Message", "Response Timeout", "Yes Step", "No Step", "Yes Button Label", "No Button Label", "Pause Button Label", "No Button Enabled", "Pause Button Enabled",

    // EVALUATE_EXPRESSION
    // ParamExpression,  ParamSuccessStep,  ParamFailureStep,
    "Expression", "SuccessStep", "FailureStep",

    // EXECUTE_SCRIPT
    // ParamCommands,
    "Commands",

    // FAILURE_SUMMARY
    // ParamFailureCategory,
    "Failure Category",

    // JUMP
    // ParamStepId,
    "StepId",

    // LAUNCH_GUI_APP
    // ParamApp,  ParamArgs,  ParamPosition,
    "Application", "Command Line", "Position",

    // LAUNCH_SCHEMATIC
    // ParamSchematic,  ParamScreen,  ParamCommands,
    "Schematic", "Screen", "Command Line",

    // LOG
    // ParamLogMessage ,  ParamLogMandatory ,  ParamLogTimeout ,
    "Prompt message", "Mandatory log entry", "Response timeout (sec)",

    // PLAN_CHECK
    // ParamPlan, ParamYesStep, ParamNoStep
    "Plan", "Yes Step", "No Step",

    // PLAN_CHECK_TIMER
    // ParamTimeSpan,  ParamSuccessStep,  ParamFailureStep,
    "Timespan", "Success Step", "Failure Step",

    // PLAN_SET_TIMER
    // ParamTime,
    "Timespan",

    // POPUP_MFT_MESSAGE
    // ParamDecisionMessage,  ParamYesStep,  ParamNoStep,  ParamPendingStep,  ParamOperatorProfileNumber,  ParamMessageBoxType,  ParamAbortStep,  ParamIgnoreStep,  ParamRetryStep, , ParamOkStep, , ParamCancelStep, , ParamTimeSpan,
    "Decision Message", "Yes Step", "No Step", "Pending Step", "Operator Profile", "MessageBox Style", "Abort Step", "Ignore Step", "Retry Step", "Ok Step", "Cancel Step", "Timespan",

    // PROMPT
    // ParamMessage,  ParamDismissTimeout,
    "Prompt Message", "Dismiss Timeout",

    // SHUTDOWN_SYSTEM
    // ParamNotifyPeer,
    "Notify Peer",

    // STATION_TIS
    // ParamStation,  ParamAllStations,   ParamMessage,  ParamPriority,  ParamDuration,  ParamStationPids,
    "Station", "All Stations", "Message", "Priority", "Duration", "PID List",

    // TRAIN_PA
    // ParamTrainId,  ParamTrainGroup,  ParamMessage,  ParamCyclic,  ParamDuration,
    "Train Id", "Train Group", "Message", "Cyclic", "Duration",

    // TRAIN_TIS
    // ParamTrainId,  ParamTrainGroup,  ParamMessage,  ParamPriority,  ParamDuration,
    "TrainId", "TrainGroup", "Message", "Priority", "Duration",

    // STATION_PA
    // ParamStation,  ParamAllStations,  ParamMessage,  ParamChime,  ParamCyclic,  ParamTISSynch,  ParamDuration,  ParamInterval,  ParamZones,
    "PA Station", "All Stations", "Message", "Chime", "Cyclic", "TIS Synchronised", "Duration", "Interval (sec)", "Zones",

    // RUN_PLAN
    // ParamPlan,  ParamManualLaunch,  ParamWaitForCompletion,
    "Plan", "Manual Launch", "Wait For Completion",

    // TERMINATE_PLAN
    // ParamPlan,
    "Plan",

    // PARALLEL_GATEWAY
    // ParamPreviousSteps, ParamSubsequentSteps,
    "Incoming Steps", "Outgoing Steps",
};

enum ParamNameIndex
{
    ASSIGN_CAMERA_TO_MONITOR = 0,
    ASSIGN_CAMERA_TO_PRESET  = ASSIGN_CAMERA_TO_MONITOR + 2,
    DATA_POINT_CHECK         = ASSIGN_CAMERA_TO_PRESET + 2,
    DATA_POINT_TIME_CHECK    = DATA_POINT_CHECK + 6,
    DATA_POINT_SET           = DATA_POINT_TIME_CHECK + 4,
    DECISION                 = DATA_POINT_SET + 3,
    EVALUATE_EXPRESSION      = DECISION + 9,
    EXECUTE_SCRIPT           = EVALUATE_EXPRESSION + 3,
    FAILURE_SUMMARY          = EXECUTE_SCRIPT + 1,
    JUMP                     = FAILURE_SUMMARY + 1,
    LAUNCH_GUI_APP           = JUMP + 1,
    LAUNCH_SCHEMATIC         = LAUNCH_GUI_APP + 3,
    LOG_STEP                 = LAUNCH_SCHEMATIC + 3,
    PLAN_CHECK               = LOG_STEP + 3,
    PLAN_CHECK_TIMER         = PLAN_CHECK + 3,
    PLAN_SET_TIMER           = PLAN_CHECK_TIMER + 3,
    POPUP_MFT_MESSAGE        = PLAN_SET_TIMER + 1,
    PROMPT                   = POPUP_MFT_MESSAGE + 12,
    SHUTDOWN_SYSTEM          = PROMPT + 2,
    STATION_TIS              = SHUTDOWN_SYSTEM + 1,
    TRAIN_PA                 = STATION_TIS + 6,
    TRAIN_TIS                = TRAIN_PA + 5,
    STATION_PA               = TRAIN_TIS + 5,
    RUN_PLAN                 = STATION_PA + 9,
    TERMINATE_PLAN           = RUN_PLAN + 3,
    PARALLEL_GATEWAY         = TERMINATE_PLAN + 1
};

struct StepTypeParamNameIndexInfo
{
    std::string typeName;
    ParamNameIndex index;
};

StepTypeParamNameIndexInfo stepTypeParamIndexInfos[] =
{
    {"Assign Camera To Monitor"     , ASSIGN_CAMERA_TO_MONITOR},
    {"Activate Camera Preset"       , ASSIGN_CAMERA_TO_PRESET },
    {"Data Point Check"             , DATA_POINT_CHECK        },
    {"Data Point Time Check"        , DATA_POINT_TIME_CHECK   },
    {"Data Point Set"               , DATA_POINT_SET          },
    {"Decision"                     , DECISION                },
    {"Evaluate Expression"          , EVALUATE_EXPRESSION     },
    {"Run Script"                   , EXECUTE_SCRIPT          },
    {"Failure Summary"              , FAILURE_SUMMARY         },
    {"Jump"                         , JUMP                    },
    {"Launch Gui App"               , LAUNCH_GUI_APP          },
    {"Launch Schematic"             , LAUNCH_SCHEMATIC        },
    {"Log"                          , LOG_STEP                },
    {"Active Plan Check"            , PLAN_CHECK              },
    {"Plan Check Timer"             , PLAN_CHECK_TIMER        },
    {"Plan Set Timer"               , PLAN_SET_TIMER          },
    {"Popup MFT Message"            , POPUP_MFT_MESSAGE       },
    {"Prompt"                       , PROMPT                  },
    {"Shutdown System"              , SHUTDOWN_SYSTEM         },
    {"Station Traveller Info System", STATION_TIS             },
    {"Train Public Address"         , TRAIN_PA                },
    {"Train Traveller Info System"  , TRAIN_TIS               },
    {"Station Public Address"       , STATION_PA              },
    {"Run Plan"                     , RUN_PLAN                },
    {"Terminate Plan"               , TERMINATE_PLAN          },
    {"Parallel Gateway"             , PARALLEL_GATEWAY        },
};

int getParamStartIndexByStepType( const std::string& stepType )
{
    for( size_t i = 0; i < sizeof( stepTypeParamIndexInfos ) / sizeof( stepTypeParamIndexInfos[0] ); ++i )
    {
        if( stepType == stepTypeParamIndexInfos[i].typeName )
        {
            return stepTypeParamIndexInfos[i].index;
        }
    }
    return -1;
}


}

ParamConvertor::ParamConvertor()
{
}

ParamConvertor::~ParamConvertor( void )
{
}

void ParamConvertor::generateSql( std::ostream& outPut )
{
    loadData();
    initDataPointParams();
    outPut << "SET DEFINE OFF;\n";
    for( std::map<unsigned long, DataPointParamInfo>::iterator it = m_mapDataPointParams.begin(); it != m_mapDataPointParams.end(); ++it )
    {
        const DataPointParamInfo& tmp = ( *it ).second;
        if( m_validDataPoint.needChange( tmp.m_name ) )
        {
            std::string newDataPointName;
            if( tmp.m_name.find( "DBG" ) != std::string::npos )
            {
                newDataPointName = m_validDataPoint.getValidDataPoint( tmp.m_nodeTree, tmp.m_valueType, ValidDataPoint::getTypeByDataPointName( tmp.m_name ), "DT17" );
            }
            else
            {
                newDataPointName = m_validDataPoint.getValidDataPoint( tmp.m_nodeTree, tmp.m_valueType, ValidDataPoint::getTypeByDataPointName( tmp.m_name ), "DT18" );
            }

            if( newDataPointName.empty() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Adjust datapoint name failed, step key:%lu", tmp.m_stepKey );
            }
            else
            {
                outPut << "UPDATE STEP_PARAMETERS " << "SET VALUE = " << "'" << newDataPointName << "' WHERE P_KEY = " << tmp.m_pkey << ";\n";
            }
        }
    }
}

void ParamConvertor::initDataPointParams()
{
    for( std::vector<ParameterData>::iterator it = m_parameterDatas.begin(); it != m_parameterDatas.end(); ++it )
    {
        ParameterData& paramData = *it;
        std::string value = paramData.m_value;
        if( value.size() > 1 && value[0] == '{' && value[1] == '\"' ) // new format, not process
        {
            continue;
        }

        unsigned long pkey = paramData.m_pkey;
        unsigned long fkStepDetails = paramData.m_fkStepDetails;
        int position = paramData.m_position;
        unsigned long fkStepType = paramData.m_fkStepType;
        unsigned long fkNodeTree = paramData.m_fkNodeTree;
        std::string stepTypeName = paramData.m_stepTypeName;
        int paramStartIndex = getParamStartIndexByStepType( stepTypeName );

        //TA_ASSERT(paramStartIndex + position < sizeof(paramNames) / sizeof(paramNames[0]), "");

        try
        {
            switch( static_cast<ParamNameIndex>( paramStartIndex ) )
            {
                case DATA_POINT_CHECK:
                case DATA_POINT_SET:
                    if( 1 == position )
                    {
                        DataPointParamInfo param;
                        param.m_nodeTree = fkNodeTree;
                        param.m_pkey = pkey;
                        param.m_stepKey = fkStepDetails;
                        param.m_stepParamIndex = paramStartIndex;
                        param.m_name = value;
                        param.m_valueType = UNDEFINED_DATA_POINT;
                        param.m_position = position;
                        m_mapDataPointParams.insert( std::make_pair( fkStepDetails, param ) );
                    }
                    else if( 2 == position )
                    {
                        size_t pos = value.find_first_of( ',' );
                        std::string valueDataPointType = value.substr( 0, pos );

                        if( m_mapDataPointParams.count( fkStepDetails ) != 0 )
                        {
                            m_mapDataPointParams[fkStepDetails].m_valueType = boost::lexical_cast<int>( valueDataPointType );
                        }
                    }
                    break;
                case DATA_POINT_TIME_CHECK:
                    if( position == 1 )
                    {
                        DataPointParamInfo param;
                        param.m_nodeTree = fkNodeTree;
                        param.m_pkey = pkey;
                        param.m_stepKey = fkStepDetails;
                        param.m_stepParamIndex = paramStartIndex;
                        param.m_name = value;
                        param.m_valueType = UNDEFINED_DATA_POINT;
                        param.m_position = position;
                        m_mapDataPointParams.insert( std::make_pair( fkStepDetails, param ) );
                    }
                    break;
                default:
                    break;
            }
        }
        catch( ... )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Convert step parameter %lu failed, step type: %s, value: %s", pkey, stepTypeName.c_str(), value.c_str() );
        }

    }
}

void ParamConvertor::loadData()
{
    m_validDataPoint.init();
    m_parameterDatas.clear();
    TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Plans_Cd, TA_Base_Core::Read );

    std::stringstream ss;
    ss << "SELECT P.P_KEY, P.FK_STEP_DETAILS, STEPDETAILS.FK_NODE_TREE, P.POSITION, P.VALUE, STEPTYPE.P_KEY AS FK_STEP_TYPE, STEPTYPE.TYPE_NAME FROM STEP_PARAMETERS P, STEP_DETAILS STEPDETAILS, STEP_TYPE STEPTYPE WHERE P.FK_STEP_DETAILS = STEPDETAILS.P_KEY AND STEPTYPE.P_KEY = STEPDETAILS.FK_STEP_TYPE AND (STEPTYPE.TYPE_NAME = \'Data Point Set\' OR STEPTYPE.TYPE_NAME = \'Data Point Check\' OR STEPTYPE.TYPE_NAME = \'Data Point Time Check\') ORDER BY STEPDETAILS.FK_NODE_TREE, STEPDETAILS.POSITION, P.POSITION";

    TA_Base_Core::SQLStatement sql;
    sql.nSQLID = 0;
    sql.strCommon = ss.str();

    std::vector<std::string> valueColumns;
    valueColumns.push_back( "P_KEY" );
    valueColumns.push_back( "FK_STEP_DETAILS" );
    valueColumns.push_back( "FK_NODE_TREE" );
    valueColumns.push_back( "POSITION" );
    valueColumns.push_back( "VALUE" );
    valueColumns.push_back( "FK_STEP_TYPE" );
    valueColumns.push_back( "TYPE_NAME" );

    unsigned long pkey;
    unsigned long fkStepDetails;
    unsigned long fkNodeTree;
    int position;
    std::string value;
    unsigned long fkStepType;
    std::string stepTypeName;

    TA_Base_Core::IData* data = database->executeQuery( sql, valueColumns );
    do
    {
        std::auto_ptr<TA_Base_Core::IData> dataPtr( data );
        for( unsigned long i = 0, sz = dataPtr->getNumRows(); i < sz; ++i )
        {
            pkey = data->getUnsignedLongData( i, "P_KEY" );
            fkStepDetails = data->getUnsignedLongData( i, "FK_STEP_DETAILS" );
            fkNodeTree = data->getUnsignedLongData( i, "FK_NODE_TREE" );
            position = data->getIntegerData( i, "POSITION" );
            value = data->getStringData( i, "VALUE" );
            fkStepType = data->getUnsignedLongData( i, "FK_STEP_TYPE" );
            stepTypeName = data->getStringData( i, "TYPE_NAME" );
            m_parameterDatas.push_back( ParameterData( pkey, fkStepDetails, fkNodeTree, position, value, fkStepType, stepTypeName ) );
        }
        dataPtr.reset();
        data = NULL;
    }
    while( database->moreData( data ) );
}

void ParamConvertor::ValidDataPoint::init()
{
    std::vector<TA_Base_Core::DataPointEntityData*> dataPoints;
    std::vector<TA_Base_Core::DataPointEntityData*> dataPointsDt17;
    std::vector<TA_Base_Core::DataPointEntityData*> dataPointsDt18;
    TA_Base_Core::DataPointAccessFactory::getAllDataPointEntitiesWithNameLikeToken( "DT17%", dataPointsDt17 );
    TA_Base_Core::DataPointAccessFactory::getAllDataPointEntitiesWithNameLikeToken( "DT18%", dataPointsDt18 );
    std::copy( dataPointsDt17.begin(), dataPointsDt17.end(), std::back_inserter( dataPoints ) );
    std::copy( dataPointsDt18.begin(), dataPointsDt18.end(), std::back_inserter( dataPoints ) );

    for( std::vector<TA_Base_Core::DataPointEntityData*>::iterator it = dataPoints.begin(); it != dataPoints.end(); ++it )
    {
        TA_Base_Core::DataPointEntityData& tmp = *( *it );
        m_setAllDataPoints.insert( tmp.getName() );
        if( tmp.getDataType().empty() )
        {
            tmp.invalidate();
        }
        if( tmp.getAddress().find_first_not_of( "0123456789." ) == std::string::npos
            || tmp.getDataPointTypeShortName() == "DDO"
            || tmp.getDataPointTypeShortName() == "DDI" )
        {
            DataPoint t = {tmp.getName(), getValueTypeByScadaType( tmp.getDataType() ), tmp.getDataPointTypeShortName(), tmp.getLocationName()};
            m_allValidDataPoints.insert( t );
        }
        delete *it;
    }
}

bool ParamConvertor::ValidDataPoint::needChange( const std::string& name )
{
    return m_setAllDataPoints.find( name ) == m_setAllDataPoints.end();
}

std::string ParamConvertor::ValidDataPoint::getValidDataPoint( unsigned long planId, int valueType, const std::string& type, const std::string& location )
{
    if( planId != m_currentPlan )
    {
        m_currentPlan = planId;
        m_usedDataPoints.clear();
    }

    std::pair<Container::nth_index<1>::type::iterator, Container::nth_index<1>::type::iterator> tmp;
    if( valueType != UNDEFINED_DATA_POINT )
    {
        tmp = m_allValidDataPoints.get<1>().equal_range( boost::make_tuple( location, type, valueType ) );
    }
    else
    {
        tmp = m_allValidDataPoints.get<1>().equal_range( boost::make_tuple( location, type ) );
    }
    while( tmp.first != tmp.second )
    {
        const DataPoint& dataPoint = *( tmp.first );
        if( m_usedDataPoints.count( dataPoint.m_name ) == 0 )
        {
            m_usedDataPoints.insert( dataPoint.m_name );
            return dataPoint.m_name;
        }
        ++tmp.first;
    }
    return "";
}

std::string ParamConvertor::ValidDataPoint::getTypeByDataPointName( const std::string& name )
{
    if( name.find( ".aii" ) != std::string::npos ) return "AI";
    if( name.find( ".aio" ) != std::string::npos ) return "AO";
    if( name.find( ".dao" ) != std::string::npos ) return "DAO";
    if( name.find( ".ddi" ) != std::string::npos ) return "DDI";
    if( name.find( ".ddo" ) != std::string::npos ) return "DDO";
    if( name.find( ".dii" ) != std::string::npos ) return "DI";
    if( name.find( ".dio" ) != std::string::npos ) return "DO";
    if( name.find( ".eii" ) != std::string::npos ) return "EI";
    if( name.find( ".met" ) != std::string::npos ) return "MET";
    if( name.find( ".mii" ) != std::string::npos ) return "MI";
    if( name.find( ".tao" ) != std::string::npos ) return "TAO";
    if( name.find( ".tdi" ) != std::string::npos ) return "TDI";
    if( name.find( ".tdo" ) != std::string::npos ) return "TDO";
    if( name.find( ".tii" ) != std::string::npos ) return "TI";
    return "";
}

int ParamConvertor::ValidDataPoint::getValueTypeByScadaType( const std::string& scadaType )
{
    EValueType type = UNDEFINED_DATA_POINT;

    if( scadaType == TA_Base_Bus::DPT_CFG_UINT16 )
    {
        type = NUMERIC_DATA_POINT;
    }
    else if( scadaType == TA_Base_Bus::DPT_CFG_INT16 )
    {
        type = NUMERIC_DATA_POINT;
    }
    else if( scadaType == TA_Base_Bus::DPT_CFG_INT16S )
    {
        type = NUMERIC_DATA_POINT;
    }
    else if( scadaType == TA_Base_Bus::DPT_CFG_UINT32 )
    {
        type = NUMERIC_DATA_POINT;
    }
    else if( scadaType == TA_Base_Bus::DPT_CFG_INT32 )
    {
        type = NUMERIC_DATA_POINT;
    }
    else if( scadaType == TA_Base_Bus::DPT_CFG_INT32S )
    {
        type = NUMERIC_DATA_POINT;
    }
    else if( scadaType == TA_Base_Bus::DPT_CFG_IEEE32 )
    {
        type = NUMERIC_DATA_POINT;
    }
    else if( scadaType == TA_Base_Bus::DPT_CFG_BOOL )
    {
        type = BOOLEAN_DATA_POINT;
    }
    else if( scadaType == TA_Base_Bus::DPT_CFG_TEXT )
    {
        type = TEXT_DATA_POINT;
    }
    else if( scadaType == TA_Base_Bus::DPT_CFG_DERIVED )
    {
        type = NUMERIC_DATA_POINT;
    }

    return static_cast<int>( type );
}
