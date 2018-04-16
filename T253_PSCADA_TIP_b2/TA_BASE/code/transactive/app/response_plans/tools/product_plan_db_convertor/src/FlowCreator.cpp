#include <set>

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DataConversion.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "FlowCreator.h"

enum StepTypeKey
{
    eEnd = 1,
    eActivePlanCheck = 3,
    eDataPointCheck = 5,
    eDecision = 7,
    eJump = 8,
    ePlanCheckTimer = 26,
    eDataPointTimeCheck = 29,
    ePopupMFTMessage = 30
};


FlowCreator::FlowCreator()
{
}

FlowCreator::~FlowCreator()
{
}

void FlowCreator::generateSql( std::ostream& outPut )
{
    loadData();
    create();

    for ( std::vector<std::string>::iterator itLoop = m_vecFlowSqls.begin(); m_vecFlowSqls.end() != itLoop; ++itLoop )
    {
        outPut << *itLoop << std::endl;
    }
}

void FlowCreator::create()
{
    unsigned long pkey = m_vecFlowSqls.size() + 1;
    int id = 1;
    unsigned long plan = 0;
    std::stringstream stmSqlGen;

    for ( std::vector<PlanStep>::iterator itLoop = m_vecSteps.begin(); m_vecSteps.end() != itLoop; ++itLoop )
    {
        if ( plan != itLoop->plan )
        {
            id = 1;
            plan = itLoop->plan;
            stmSqlGen.str( "" );
            stmSqlGen << "-- Plan " << itLoop->plan;
            m_vecFlowSqls.push_back( stmSqlGen.str() );

        }

        switch ( itLoop->type )
        {
        case eActivePlanCheck:
        case eDataPointCheck:
        case eDecision:
        case eJump:
        case ePlanCheckTimer:
        case eDataPointTimeCheck:
        case ePopupMFTMessage:
            {
                std::vector<unsigned int> vecNextSteps;
                getSteps( itLoop->type, itLoop->pkey, vecNextSteps );
                for ( std::vector<unsigned int>::iterator itNext = vecNextSteps.begin(); vecNextSteps.end() != itNext; ++itNext )
                {
                    stmSqlGen.str( "" );
                    stmSqlGen << "insert into plan_sequence_flow (P_KEY, INCOMING_STEP, OUTGOING_STEP, CONDITION, GRAPH, PLAN_PKEY, ID) values ("
                        << pkey++ << ", " << itLoop->position /*Could be use as id here*/ << ", " << *itNext << ", "
                        << "''" << ", " << "''" << ", " << itLoop->plan << ", " << id++ << ");";
                    m_vecFlowSqls.push_back( stmSqlGen.str() );
                }
                break;
            }
        case eEnd:
            break;
        default:
            {
                stmSqlGen.str( "" );
                stmSqlGen << "insert into plan_sequence_flow (P_KEY, INCOMING_STEP, OUTGOING_STEP, CONDITION, GRAPH, PLAN_PKEY, ID) values ("
                    << pkey++ << ", " << itLoop->position /*Could be use as id here*/ << ", " << itLoop->position + 1 /*Could be use as id here*/ << ", "
                    << "''" << ", " << "''" << ", " << itLoop->plan << ", " << id++ << ");";
                m_vecFlowSqls.push_back( stmSqlGen.str() );
                break;
            }
        }
    }
}

void FlowCreator::loadData()
{
    std::string strSql = "select P_KEY, FK_STEP_TYPE, FK_NODE_TREE, POSITION from step_details where "
        "fk_node_tree in (select distinct ( fk_node_tree ) from step_details where fk_step_type in (select p_key from step_type where type_name in ('Active Plan Check', 'Data Point Check', 'Decision', 'Jump', 'Plan Check Timer', 'Popup MFT Message', 'Data Point Time Check'  ))) "
        "and fk_node_tree not in (select distinct ( fk_node_tree ) from step_details where no_wait <> 0) "
        "order by FK_NODE_TREE, position";

    TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Plans_Cd, TA_Base_Core::Read );

    TA_Base_Core::SQLStatement sql;
    sql.nSQLID = 0;
    sql.strCommon = strSql;

    std::vector<std::string> valueColumns;
    valueColumns.push_back( "P_KEY" );
    valueColumns.push_back( "FK_STEP_TYPE" );
    valueColumns.push_back( "FK_NODE_TREE" );
    valueColumns.push_back( "POSITION" );

    unsigned long pkey = 0u;
    unsigned long fkStepType = 0u;
    unsigned long fkPlan = 0u;
    int position = 0;

    TA_Base_Core::IData* data = database->executeQuery( sql, valueColumns );
    do
    {
        std::auto_ptr<TA_Base_Core::IData> dataPtr( data );
        for ( unsigned long i = 0, sz = dataPtr->getNumRows(); i < sz; ++i )
        {
            pkey = data->getUnsignedLongData( i, "P_KEY" );
            fkPlan = data->getUnsignedLongData( i, "FK_NODE_TREE" );
            fkStepType = data->getUnsignedLongData( i, "FK_STEP_TYPE" );
            position = data->getIntegerData( i, "POSITION" );

            PlanStep stStep( pkey, fkStepType, fkPlan, position );
            m_vecSteps.push_back( stStep );
        }
        dataPtr.reset();
        data = NULL;
    } while ( database->moreData( data ) );
}

unsigned int FlowCreator::getStepId( const std::string& strStep )
{
    unsigned int unId = 0;

    if ( 0u == strStep.find( '#' ))
    {
        std::stringstream stmConvertor(strStep.substr(1));
        stmConvertor >> unId;
    }
    else if ( std::string::npos == strStep.find_first_not_of( "0123456789" ))
    {
        std::string strSql = "select position from step_details where p_key = ";
        strSql += strStep;

        TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Plans_Cd, TA_Base_Core::Read );

        TA_Base_Core::SQLStatement sql;
        sql.nSQLID = 0;
        sql.strCommon = strSql;

        std::vector<std::string> valueColumns;
        valueColumns.push_back( "POSITION" );

        TA_Base_Core::IData* data = database->executeQuery( sql, valueColumns );
        do
        {
            std::auto_ptr<TA_Base_Core::IData> dataPtr( data );
            unId = data->getIntegerData( 0, "POSITION" );

            dataPtr.reset();
            data = NULL;
        } while ( database->moreData( data ));
    }

    return unId;
}

void FlowCreator::getSteps( const int eType, const unsigned long ulStepKey, std::vector<unsigned int>& vecSteps )
{
    vecSteps.clear();

    std::set<unsigned int> setPos;
    switch ( eType )
    {
    case eActivePlanCheck:
        setPos.insert( 2u );
        setPos.insert( 3u );
        break;
    case eDataPointCheck:
        setPos.insert( 4u );
        setPos.insert( 5u );
        break;
    case eDecision:
        setPos.insert( 3u );
        setPos.insert( 4u );
        break;
    case eJump:
        setPos.insert( 1u );
        break;
    case ePlanCheckTimer:
        setPos.insert( 2u );
        setPos.insert( 3u );
        break;
    case eDataPointTimeCheck:
        setPos.insert( 3u );
        setPos.insert( 4u );
        break;
    case ePopupMFTMessage:
        setPos.insert( 2u );
        setPos.insert( 3u );
        setPos.insert( 4u );
        break;
    default:
        break;
    }

    std::string strSql = "select POSITION, VALUE from step_parameters where fk_step_details = ";
    strSql += TA_Base_Core::DataConversion::toString(ulStepKey);

    TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Plans_Cd, TA_Base_Core::Read );

    TA_Base_Core::SQLStatement sql;
    sql.nSQLID = 0;
    sql.strCommon = strSql;

    std::vector<std::string> valueColumns;
    valueColumns.push_back( "POSITION" );
    valueColumns.push_back( "VALUE" );

    unsigned int unPosition = 0u;
    std::string strValue = "";
    TA_Base_Core::IData* data = database->executeQuery( sql, valueColumns );
    do
    {
        std::auto_ptr<TA_Base_Core::IData> dataPtr( data );
        for ( unsigned long i = 0, sz = dataPtr->getNumRows(); i < sz; ++i )
        {
            unPosition = data->getIntegerData( i, "POSITION" );
            strValue = data->getStringData( i, "VALUE" );

            if ( setPos.find( unPosition ) != setPos.end() )
            {
                unsigned int unId = getStepId( strValue );

                if ( 0u != unId )
                {
                    vecSteps.push_back( unId );

                    if ( vecSteps.size() == setPos.size() )
                    {
                        break;
                    }
                }
            }
        }

        dataPtr.reset();
        data = NULL;
    } while ( database->moreData( data ) );
}

void FlowCreator::attachFlowSql( const std::vector<std::string>& vecFlows )
{
    m_vecFlowSqls.resize( vecFlows.size() );
    std::copy( vecFlows.begin(), vecFlows.end(), m_vecFlowSqls.begin() );
}
