#include "ParamConvertor.h"
#include "Serialization.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/entity_access/src//EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataBaseHelper.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DataPointConfigAccess.h"
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>


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

static const std::string BOOLEAN_DATA_POINT = "0";
static const std::string NUMERIC_DATA_POINT = "1";
static const std::string COMBINED_DATA_POINT = "2";
static const std::string TEXT_DATA_POINT = "3";
static const std::string UNDEFINED_DATA_POINT = "4";

std::string getDataPointTypeStringByScadaType( const std::string& scadaType )
{
    std::string type;

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
        type = COMBINED_DATA_POINT;
    }
    return type;
}

unsigned long getNodeId( const std::string& path )
{
    unsigned long node = 0;
    bool foundNode = false;

    unsigned long parent = 1;
    std::string nodeName = "";

    TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Plans_Cd, TA_Base_Core::Read );

    const std::string delimiter = "/";

    std::string::size_type beginPos = 0;

    //Remove the terminating "/" if any

    beginPos = path.find_last_of( delimiter );

    std::string newPath;

    if( beginPos == ( path.size() - 1 ) )
    {
        newPath = path.substr( 0, beginPos );
    }
    else
    {
        newPath = path;
    }

    beginPos = 0;
    while( beginPos < newPath.size() )
    {
        beginPos = newPath.find_first_of( delimiter, beginPos );
        if( beginPos == std::string::npos )
        {
            break;
        }

        std::string::size_type endPos = newPath.find_first_of( delimiter, beginPos + 1 );

        if( endPos == std::string::npos )
        {
            nodeName = newPath.substr( beginPos + 1 );
        }
        else
        {
            nodeName = newPath.substr( beginPos + 1, endPos - 1 - beginPos );
        }

        std::ostringstream sql;
        sql << "select p_key from node_tree where parent_node = " << parent << " and name = '" << database->escapeQueryString( nodeName ) << "'";
        TA_Base_Core::SQLStatement strSql;
        strSql.nSQLID = 0;
        strSql.strCommon = sql.str();

        std::vector<std::string> columns;
        columns.push_back( "p_key" );

        std::auto_ptr<TA_Base_Core::IData> data( database->executeQuery( strSql, columns ) );
        TA_Base_Bus::PlanDataBaseHelper dataBaseHelper( database );

        if( ( NULL == data.get() ) || ( 1 != data->getNumRows() ) )
        {
            break;
        }

        node = data->getUnsignedLongData( 0, columns[0] );

        if( endPos == std::string::npos )
        {
            foundNode = true;
            break;
        }

        parent = node;

        // set up for next loop
        beginPos = endPos;
    }

    if( foundNode )
    {
        return node;
    }
    return 0;

    /*
    std::string message = "The node '";
    message += path;
    message += "' does not exist!";

    TA_THROW( TA_Base_Core::TransactiveException( message.c_str() ) );
    */
}

unsigned long getNewPkey()
{
    std::string strSql = "select plans_seq.nextval Next_Step_Id from dual";

    TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Plans_Cd, TA_Base_Core::Read );

    TA_Base_Core::SQLStatement sql;
    sql.nSQLID = 0;
    sql.strCommon = strSql;

    std::vector<std::string> valueColumns;
    valueColumns.push_back( "Next_Step_Id" );

    unsigned long pkey = 0u;

    TA_Base_Core::IData* data = database->executeQuery( sql, valueColumns );
    do
    {
        std::auto_ptr<TA_Base_Core::IData> dataPtr( data );
        for( unsigned long i = 0, sz = dataPtr->getNumRows(); i < sz; ++i )
        {
            pkey = data->getUnsignedLongData( i, "Next_Step_Id" );
        }
        dataPtr.reset();
        data = NULL;
    }
    while( database->moreData( data ) );

    return pkey;
}
// Structure for serialize
// Launch gui app
struct WindowPosition
{
    unsigned long top;
    unsigned long left;
    unsigned long width;
    unsigned long height;
    WindowPosition() : top( 0 ), left( 0 ), width( 0 ), height( 0 )
    {
    }
};
TA_SERIALIZATION( WindowPosition, ( top )( left )( width )( height ) )

// Tis
struct TISMessage
{
    unsigned long libraryVersion;
    int librarySection;
    unsigned short messageId;
    TISMessage() : libraryVersion( 0 ), librarySection( 0 ), messageId( 0 )
    {
    }
};
TA_SERIALIZATION( TISMessage, ( libraryVersion )( librarySection )( messageId ) )

// Pa
struct PAMessage
{
    unsigned short libVersion;
    unsigned short libSection;
    unsigned long messageId;
};
TA_SERIALIZATION( PAMessage, ( libVersion )( libSection )( messageId ) )

}

ParamConvertor::ParamConvertor( const std::map<unsigned long, std::string>& updatedParams ) : m_mapUpdatedParams( updatedParams )
{
}

ParamConvertor::~ParamConvertor( void )
{
}

void ParamConvertor::generateSql( std::ostream& outPut )
{
    if( m_newParameterDatas.empty() )
    {
        if( m_oldParameterDatas.empty() )
        {
            loadData();
        }
        convert();
    }
    outPut << "SET DEFINE OFF;\n";
    unsigned long fkStepDetails = 0;
    unsigned long fkNodeTree = 0;
    for( std::vector<std::pair<ParameterData, bool>>::iterator it = m_newParameterDatas.begin(); it != m_newParameterDatas.end(); ++it )
    {
        ParameterData& data = ( *it ).first;
        if( data.getFkNodeTree() != fkNodeTree )
        {
            fkNodeTree = data.getFkNodeTree();
            outPut << "--Plan " << fkNodeTree << " ********************************************************************************" << "\n";
        }
        if( data.getFkStepDetails() != fkStepDetails )
        {
            fkStepDetails = data.getFkStepDetails();
            outPut << "--" << data.getStepTypeName() << " " << fkStepDetails << "\n";
        }
        if( !( *it ).second ) // Not insert
        {
            outPut << "UPDATE PLAN_STEP_PARAMETERS " << "SET VALUE = " << "'" << data.getValue() << "', "
                   << "POSITION = " << data.getPostion() << " WHERE P_KEY = " << data.getPkey() << ";\n";
        }
        else
        {
            outPut << "INSERT INTO PLAN_STEP_PARAMETERS (P_KEY, FK_STEP_DETAILS, POSITION, VALUE) VALUES ("
                   << getNewPkey() << ", "
                   << data.getFkStepDetails() << ", " << data.getPostion() << ", " << "'" << data.getValue() << "');\n";
        }
    }
}

void ParamConvertor::convert()
{
    m_newParameterDatas.clear();
    for( std::vector<ParameterData>::iterator it = m_oldParameterDatas.begin(); it != m_oldParameterDatas.end(); ++it )
    {
        ParameterData& oldData = *it;
        std::string value = oldData.getValue();
        if( value.size() > 1 && value[0] == '{' && value[1] == '\"' ) // new format, not process
        {
            continue;
        }

        unsigned long pkey = oldData.getPkey();

        if( m_mapUpdatedParams.count( pkey ) != 0 )
        {
            value = m_mapUpdatedParams[pkey];
        }

        unsigned long fkStepDetails = oldData.getFkStepDetails();
        int position = oldData.getPostion();
        unsigned long fkStepType = oldData.getFkStepType();
        unsigned long fkNodeTree = oldData.getFkNodeTree();
        std::string stepTypeName = oldData.getStepTypeName();
        int paramStartIndex = getParamStartIndexByStepType( stepTypeName );

        //TA_ASSERT(paramStartIndex + position < sizeof(paramNames) / sizeof(paramNames[0]), "");

        try
        {
            switch( static_cast<ParamNameIndex>( paramStartIndex ) )
            {
                case ASSIGN_CAMERA_TO_MONITOR:
                case ASSIGN_CAMERA_TO_PRESET:
                case EXECUTE_SCRIPT:
                case FAILURE_SUMMARY:
                case LAUNCH_SCHEMATIC:
                case LOG_STEP:
                case PLAN_SET_TIMER:
                case PROMPT:
                case SHUTDOWN_SYSTEM:
                    m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                  Serialization::serialize( value, paramNames[paramStartIndex + position - 1] ),
                                                                                  fkStepType, stepTypeName ), false ) );
                    break;
                case RUN_PLAN:
                case TERMINATE_PLAN:
                    if( 1 == position )
                    {
                        unsigned long nodeId = getNodeId( value );
                        value = boost::lexical_cast<std::string>( nodeId );
                    }
                    m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                  Serialization::serialize( value, paramNames[paramStartIndex + position - 1] ),
                                                                                  fkStepType, stepTypeName ), false ) );
                    break;
                case DATA_POINT_CHECK:
                case DATA_POINT_SET:
                    {
                        static std::string dataPointName;
                        if( 1 == position )
                        {
                            dataPointName = value;
                            m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                          Serialization::serialize( value, paramNames[paramStartIndex + position - 1] ),
                                                                                          fkStepType, stepTypeName ), false ) );
                        }
                        else if( 2 == position )
                        {
                            size_t pos = value.find_first_of( ',' );
                            //std::string valueDataPointType = value.substr( 0, pos );
                            std::string valueDataPointValue = value.substr( pos + 1 );
                            std::string valueDataPointType = UNDEFINED_DATA_POINT;

                            std::auto_ptr<TA_Base_Core::DataPointEntityData> ptrData(
                                dynamic_cast<TA_Base_Core::DataPointEntityData*>(
                                    TA_Base_Core::EntityAccessFactory::getInstance().getEntity( dataPointName ) ) );
                            if( NULL != ptrData.get() )
                            {
                                valueDataPointType = getDataPointTypeStringByScadaType( ptrData->getDataType() );
                                if( BOOLEAN_DATA_POINT == valueDataPointType )
                                {
                                    valueDataPointValue = ( ( "1" == valueDataPointValue ) ? ptrData->getTrueLabel() : ptrData->getFalseLabel() );
                                }
                                else if( COMBINED_DATA_POINT == valueDataPointType )
                                {
                                    using namespace TA_Base_App;
                                    int state = boost::lexical_cast<int>( valueDataPointValue.substr( 0, valueDataPointValue.find_first_of( ',' ) ) );
                                    DataPointConfigAccess::StateValueList states = DataPointConfigAccess::getDataPointStates( dataPointName );
									for(DataPointConfigAccess::StateValueList::iterator it = states.begin(); it != states.end(); ++it)
									{
										const DataPointConfigAccess::StateValue& tmp = *it;
										if(tmp.state == state)
										{
											valueDataPointValue = tmp.description;
											break;
										}
									}
                                }
                                else if( NUMERIC_DATA_POINT == valueDataPointType )
                                {
                                    valueDataPointValue = valueDataPointValue.substr( 0, valueDataPointValue.find_first_of( ',' ) );
                                }
                            }

                            m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                          Serialization::serialize( valueDataPointType, paramNames[paramStartIndex + position - 1] ),
                                                                                          fkStepType, stepTypeName ), false ) );

                            m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position + 1,
                                                                                          Serialization::serialize( valueDataPointValue, paramNames[paramStartIndex + position] ),
                                                                                          fkStepType, stepTypeName ), true ) );
                        }
                        else if( 3 == position )
                        {
                            m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position + 1,
                                                                                          Serialization::serialize( value, paramNames[paramStartIndex + position] ),
                                                                                          fkStepType, stepTypeName ), false ) );
                        }
                        else
                        {
                            if( !value.empty() && value[0] == '#' )
                            {
                                value.erase( 0, 1 ); // remove step number prefix '#'
                            }
                            m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position + 1,
                                                                                          Serialization::serialize( value, paramNames[paramStartIndex + position] ),
                                                                                          fkStepType, stepTypeName ), false ) );
                        }
                    }
                    break;
                case DATA_POINT_TIME_CHECK:
                    if( position >= 3 && !value.empty() && value[0] == '#' )
                    {
                        value.erase( 0, 1 ); // remove step number prefix '#'
                    }
                    m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                  Serialization::serialize( value, paramNames[paramStartIndex + position - 1] ),
                                                                                  fkStepType, stepTypeName ), false ) );
                    break;
                case DECISION:
                    if( ( position == 3 || position == 4 ) && !value.empty() && value[0] == '#' )
                    {
                        value.erase( 0, 1 ); // remove step number prefix '#'
                    }
                    m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                  Serialization::serialize( value, paramNames[paramStartIndex + position - 1] ),
                                                                                  fkStepType, stepTypeName ), false ) );
                    break;
                case EVALUATE_EXPRESSION:
                case PLAN_CHECK_TIMER:
                    if( position >= 2 && !value.empty() && value[0] == '#' )
                    {
                        value.erase( 0, 1 ); // remove step number prefix '#'
                    }
                    m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                  Serialization::serialize( value, paramNames[paramStartIndex + position - 1] ),
                                                                                  fkStepType, stepTypeName ), false ) );
                    break;
                case PLAN_CHECK:
                    if( 1 == position )
                    {
                        unsigned long nodeId = getNodeId( value );
                        value = boost::lexical_cast<std::string>( nodeId );
                    }
                    if( position >= 2 && !value.empty() && value[0] == '#' )
                    {
                        value.erase( 0, 1 ); // remove step number prefix '#'
                    }
                    m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                  Serialization::serialize( value, paramNames[paramStartIndex + position - 1] ),
                                                                                  fkStepType, stepTypeName ), false ) );
                    break;
                case JUMP:
                    if( !value.empty() && value[0] == '#' )
                    {
                        value.erase( 0, 1 ); // remove step number prefix '#'
                    }
                    m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                  Serialization::serialize( value, paramNames[paramStartIndex + position - 1] ),
                                                                                  fkStepType, stepTypeName ), false ) );
                    break;
                case LAUNCH_GUI_APP:
                    if( 3 == position )
                    {
                        boost::char_separator<char> sep( "," );
                        boost::tokenizer<boost::char_separator<char> > tokens( value, sep );
                        std::vector<std::string> values( tokens.begin(), tokens.end() );
                        WindowPosition wndPos;
                        if( 4 == values.size() )
                        {
                            wndPos.top = boost::lexical_cast<unsigned long>( values[0] );
                            wndPos.left = boost::lexical_cast<unsigned long>( values[1] );
                            wndPos.width = boost::lexical_cast<unsigned long>( values[2] );
                            wndPos.height = boost::lexical_cast<unsigned long>( values[3] );
                        }
                        value = Serialization::structSerialize( wndPos, paramNames[paramStartIndex + position - 1] );
                    }
                    else
                    {
                        value = Serialization::serialize( value, paramNames[paramStartIndex + position - 1] );
                    }
                    m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position, value,
                                                                                  fkStepType, stepTypeName ), false ) );
                    break;
                case POPUP_MFT_MESSAGE:
                    if( ( ( 2 <= position && 4 >= position ) || ( 7 <= position && 11 >= position ) ) && !value.empty() && value[0] == '#' )
                    {
                        value.erase( 0, 1 ); // remove step number prefix '#'
                    }
                    m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                  Serialization::serialize( value, paramNames[paramStartIndex + position - 1] ),
                                                                                  fkStepType, stepTypeName ), false ) );
                    break;
                case STATION_TIS:
                    if( 1 == position )
                    {
                        size_t pos = value.find_first_of( ',' );
                        std::string valueStation = value.substr( 0, pos );
                        std::string valueAllStation = value.substr( pos + 1 );

                        m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                      Serialization::serialize( valueStation, paramNames[paramStartIndex + position - 1] ),
                                                                                      fkStepType, stepTypeName ), false ) );

                        m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position + 1,
                                                                                      Serialization::serialize( valueAllStation, paramNames[paramStartIndex + position] ),
                                                                                      fkStepType, stepTypeName ), true ) );
                    }
                    else if( 2 == position ) // not process level
                    {
                        break;
                    }
                    else
                    {
                        if( 3 == position )
                        {
                            boost::char_separator<char> sep( "," );
                            boost::tokenizer<boost::char_separator<char> > tokens( value, sep );
                            std::vector<std::string> values( tokens.begin(), tokens.end() );
                            TISMessage tisMsg;
                            if( 3 == values.size() )
                            {
                                tisMsg.libraryVersion = boost::lexical_cast<unsigned long>( values[0] );
                                tisMsg.librarySection = boost::lexical_cast<int>( values[1] );
                                tisMsg.messageId = boost::lexical_cast<unsigned short>( values[2] );
                            }
                            value = Serialization::structSerialize( tisMsg, paramNames[paramStartIndex + position - 1] );
                        }
                        else if( 6 == position ) // pid list, pid list may use multiple parameter entry, current only process one
                        {
                            boost::char_separator<char> sep( "," );
                            boost::tokenizer<boost::char_separator<char> > tokens( value, sep );
                            std::vector<std::string> pids( tokens.begin(), tokens.end() );
                            value = Serialization::serialize( pids, paramNames[paramStartIndex + position - 1] );
                        }
                        else if( position > 6 )
                        {
                            break;
                        }
                        else
                        {
                            value = Serialization::serialize( value, paramNames[paramStartIndex + position - 1] );
                        }
                        m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position, value,
                                                                                      fkStepType, stepTypeName ), false ) );
                    }
                    break;
                case TRAIN_TIS:
                    if( 1 == position )
                    {
                        size_t pos = value.find_first_of( ',' );
                        std::string valueTrainId = value.substr( 0, pos );
                        std::string valueTrainGroup = value.substr( pos + 1 );

                        m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                      Serialization::serialize( valueTrainId, paramNames[paramStartIndex + position - 1] ),
                                                                                      fkStepType, stepTypeName ), false ) );

                        m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position + 1,
                                                                                      Serialization::serialize( valueTrainGroup, paramNames[paramStartIndex + position] ),
                                                                                      fkStepType, stepTypeName ), true ) );
                    }
                    else
                    {
                        if( 2 == position )
                        {
                            boost::char_separator<char> sep( "," );
                            boost::tokenizer<boost::char_separator<char> > tokens( value, sep );
                            std::vector<std::string> values( tokens.begin(), tokens.end() );
                            TISMessage tisMsg;
                            if( 3 == values.size() )
                            {
                                tisMsg.libraryVersion = boost::lexical_cast<unsigned long>( values[0] );
                                tisMsg.librarySection = boost::lexical_cast<int>( values[1] );
                                tisMsg.messageId = boost::lexical_cast<unsigned short>( values[2] );
                            }
                            value = Serialization::structSerialize( tisMsg, paramNames[paramStartIndex + position] );
                        }
                        else
                        {
                            value = Serialization::serialize( value, paramNames[paramStartIndex + position] );
                        }
                        m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position + 1, value,
                                                                                      fkStepType, stepTypeName ), false ) );
                    }
                    break;
                case STATION_PA:
                    if( 1 == position )
                    {
                        size_t pos = value.find_first_of( ',' );
                        std::string valueStation = value.substr( 0, pos );
                        std::string valueAllStation = value.substr( pos + 1 );

                        m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                      Serialization::serialize( valueStation, paramNames[paramStartIndex + position - 1] ),
                                                                                      fkStepType, stepTypeName ), false ) );

                        m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position + 1,
                                                                                      Serialization::serialize( valueAllStation, paramNames[paramStartIndex + position] ),
                                                                                      fkStepType, stepTypeName ), true ) );
                    }
                    else
                    {
                        if( 2 == position ) // Pa message
                        {
                            boost::char_separator<char> sep( "," );
                            boost::tokenizer<boost::char_separator<char> > tokens( value, sep );
                            std::vector<std::string> values( tokens.begin(), tokens.end() );
                            PAMessage paMsg;
                            if( 3 == values.size() )
                            {
                                paMsg.libVersion = boost::lexical_cast<unsigned short>( values[0] );
                                paMsg.libSection = boost::lexical_cast<unsigned short>( values[1] );
                                paMsg.messageId = boost::lexical_cast<unsigned long>( values[2] );
                            }
                            value = Serialization::structSerialize( paMsg, paramNames[paramStartIndex + position] );
                        }
                        else if( 8 == position ) // zones, zones may use multiple parameter entry, current only process one
                        {
                            boost::char_separator<char> sep( "," );
                            boost::tokenizer<boost::char_separator<char> > tokens( value, sep );
                            std::vector<unsigned long> zones;
                            for( boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin(); it != tokens.end(); ++it )
                            {
                                zones.push_back( boost::lexical_cast<unsigned long>( *it ) );
                            }
                            value = Serialization::serialize( zones, paramNames[paramStartIndex + position] );
                        }
                        else if( position > 8 )
                        {
                            break;
                        }
                        else
                        {
                            value = Serialization::serialize( value, paramNames[paramStartIndex + position] );
                        }
                        m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position + 1, value,
                                                                                      fkStepType, stepTypeName ), false ) );
                    }
                    break;
                case TRAIN_PA:
                    if( 1 == position )
                    {
                        size_t pos = value.find_first_of( ',' );
                        std::string valueTrainId = value.substr( 0, pos );
                        std::string valueTrainGroup = value.substr( pos + 1 );

                        m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position,
                                                                                      Serialization::serialize( valueTrainId, paramNames[paramStartIndex + position - 1] ),
                                                                                      fkStepType, stepTypeName ), false ) );

                        m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position + 1,
                                                                                      Serialization::serialize( valueTrainGroup, paramNames[paramStartIndex + position] ),
                                                                                      fkStepType, stepTypeName ), true ) );
                    }
                    else
                    {
                        m_newParameterDatas.push_back( std::make_pair( ParameterData( pkey, fkStepDetails, fkNodeTree, position + 1,
                                                                                      Serialization::serialize( value, paramNames[paramStartIndex + position] ),
                                                                                      fkStepType, stepTypeName ), false ) );
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
    m_oldParameterDatas.clear();
    TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Plans_Cd, TA_Base_Core::Read );

    std::stringstream ss;
    ss << "SELECT P.P_KEY, P.FK_STEP_DETAILS, STEPDETAILS.FK_NODE_TREE, P.POSITION, P.VALUE, STEPTYPE.P_KEY AS FK_STEP_TYPE, STEPTYPE.TYPE_NAME FROM STEP_PARAMETERS P, STEP_DETAILS STEPDETAILS, STEP_TYPE STEPTYPE WHERE P.FK_STEP_DETAILS = STEPDETAILS.P_KEY AND STEPTYPE.P_KEY = STEPDETAILS.FK_STEP_TYPE ORDER BY STEPDETAILS.FK_NODE_TREE, STEPDETAILS.POSITION, P.POSITION";

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
            m_oldParameterDatas.push_back( ParameterData( pkey, fkStepDetails, fkNodeTree, position, value, fkStepType, stepTypeName ) );
        }
        dataPtr.reset();
        data = NULL;
    }
    while( database->moreData( data ) );
}