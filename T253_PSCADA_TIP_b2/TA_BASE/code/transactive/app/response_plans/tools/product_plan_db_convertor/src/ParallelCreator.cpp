#include <set>

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DataConversion.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "ParallelCreator.h"
#include "Serialization.h"

enum StepTypeKey
{
    eEnd = 1,
    eActivePlanCheck = 3,
    eDataPointCheck = 5,
    eDataPointSet = 6,
    eDecision = 7,
    eJump = 8,
    ePlanCheckTimer = 26,
    eDataPointTimeCheck = 29,
    ePopupMFTMessage = 30
};

const int ParallelStepTypeId = 8;

ParallelCreator::ParallelCreator()
{
}


ParallelCreator::~ParallelCreator()
{
}

void ParallelCreator::generateSql( std::ostream& outPut )
{
    loadData();
    create();

    for ( std::vector<std::string>::iterator itLoop = m_vecParallelSqls.begin(); m_vecParallelSqls.end() != itLoop; ++itLoop )
    {
        outPut << *itLoop << std::endl;
    }
}

void ParallelCreator::create()
{
    m_vecParallelSqls.clear();

    unsigned long sfPkey = 1;
    unsigned long newSteps = 0u;
    unsigned long plan = 0;
    unsigned long newInParallelKey = 0u;
    unsigned long newOutParallelKey = 0u;
    unsigned int inParallelPos = 0u;
    unsigned int outParallelPos = 0u;
    unsigned int id = 1u;
    bool found = false;
    std::vector<unsigned long> vecParallelIds;
    std::stringstream stmSqlGen;
    std::stringstream stmFlowSql;

    std::multimap<unsigned int, unsigned int> mapFlowNextSteps;
    std::map<unsigned long, unsigned int> mapNextSteps;
    std::map<unsigned int, unsigned int> mapOldPosToNewPos;

    for ( std::vector<NoWaitStep>::iterator itLoop = m_vecSteps.begin(); m_vecSteps.end() != itLoop; ++itLoop )
    {
        if ( plan != itLoop->plan )
        {
            if ( !mapOldPosToNewPos.empty() && !mapNextSteps.empty() )
            {
                updateStepParam( stmSqlGen, mapNextSteps, mapOldPosToNewPos );
            }
            if ( !mapFlowNextSteps.empty() )
            {
                createFlowForNextSteps( stmSqlGen, plan, mapFlowNextSteps, mapOldPosToNewPos, sfPkey, id );
            }

            stmSqlGen.str( "" );
            stmSqlGen << "-- Plan " << itLoop->plan;
            m_vecParallelSqls.push_back( stmSqlGen.str() );

            mapFlowNextSteps.clear( );
            mapOldPosToNewPos.clear();
            mapNextSteps.clear();
            newSteps = 0u;
            id = 1u;
            plan = itLoop->plan;
            found = false;
        }

        if ( found && !itLoop->nowait )
        {
            inParallelPos = itLoop->position + newSteps;
            newInParallelKey = getNewPkey();
            stmSqlGen.str( "" );
            stmSqlGen << "insert into plan_step_details (P_KEY, FK_STEP_TYPE, FK_NODE_TREE, POSITION, DESCRIPTION, SKIP, SKIPPABLE, DELAY, IGNORE_FAILURE, NAME) values ( "
                << newInParallelKey << ", " << ParallelStepTypeId << ", " << itLoop->plan << ", " << inParallelPos << ", "
                << "''" << ", " << 0 << ", " << 0 << ", " << 0u << ", " << 0 << ", " << "'Incoming Parallel Step - " << newSteps + 1 << "' );";
            m_vecParallelSqls.push_back( stmSqlGen.str() );

            // Insert sequence flow
            pushFlowSql( stmFlowSql, itLoop->plan, inParallelPos, inParallelPos + 1, sfPkey, id );

            unsigned int unParamPos = 1u;

            {
                std::vector<unsigned int> vecSteps;
                if ( outParallelPos != 1u ) // First step
                {
                    vecSteps.push_back( outParallelPos - 1u );
                }

                stmSqlGen.str( "" );
                stmSqlGen << "insert into plan_step_parameters (P_KEY, FK_STEP_DETAILS, POSITION, VALUE) values ( "
                    << getNewPkey() << ", " << newOutParallelKey << ", " << unParamPos++ << ", " << "'" << Serialization::serialize( vecSteps, "Incoming Steps" ) << "' );";
                m_vecParallelSqls.push_back( stmSqlGen.str() );
            }

            stmSqlGen.str( "" );
            stmSqlGen << "insert into plan_step_parameters (P_KEY, FK_STEP_DETAILS, POSITION, VALUE) values ( "
                << getNewPkey() << ", " << newOutParallelKey << ", " << unParamPos++ << ", " << "'" << Serialization::serialize( vecParallelIds, "Outgoing Steps" ) << "' );";
            m_vecParallelSqls.push_back( stmSqlGen.str() );

            for ( std::vector<unsigned long>::iterator itParam = vecParallelIds.begin(); vecParallelIds.end() != itParam; ++itParam )
            {
                // Insert sequence flow
                pushFlowSql( stmFlowSql, itLoop->plan, outParallelPos, *itParam, sfPkey, id );
            }

            unParamPos = 1u;
            // [ToDo] Update parameter format to jason
            stmSqlGen.str( "" );
            stmSqlGen << "insert into plan_step_parameters (P_KEY, FK_STEP_DETAILS, POSITION, VALUE) values ( "
                << getNewPkey() << ", " << newInParallelKey << ", " << unParamPos++ << ", " << "'" << Serialization::serialize( vecParallelIds, "Incoming Steps" ) << "' );";
            m_vecParallelSqls.push_back( stmSqlGen.str() );

            for ( std::vector<unsigned long>::iterator itParam = vecParallelIds.begin(); vecParallelIds.end() != itParam; ++itParam )
            {
                // Insert sequence flow
                pushFlowSql( stmFlowSql, itLoop->plan, *itParam, inParallelPos, sfPkey, id );
            }

            {
                std::vector<unsigned int> vecSteps;
                vecSteps.push_back( inParallelPos + 1u );

                stmSqlGen.str( "" );
                stmSqlGen << "insert into plan_step_parameters (P_KEY, FK_STEP_DETAILS, POSITION, VALUE) values ( "
                    << getNewPkey() << ", " << newInParallelKey << ", " << unParamPos++ << ", " << "'" << Serialization::serialize( vecSteps, "Outgoing Steps" ) << "' );";
                m_vecParallelSqls.push_back( stmSqlGen.str() );
            }

            vecParallelIds.clear();
            found = false;
            ++newSteps;
        }
        else if ( !found && itLoop->nowait )
        {
            outParallelPos = itLoop->position + newSteps;
            newOutParallelKey = getNewPkey();

            stmSqlGen.str( "" );
            stmSqlGen << "insert into plan_step_details (P_KEY, FK_STEP_TYPE, FK_NODE_TREE, POSITION, DESCRIPTION, SKIP, SKIPPABLE, DELAY, IGNORE_FAILURE, NAME) values ( "
                << newOutParallelKey << ", " << ParallelStepTypeId << ", " << itLoop->plan << ", " << outParallelPos << ", "
                << "''" << ", " << 0 << ", " << 0 << ", " << 0u << ", " << 0 << ", " << "'Outgoing Parallel Step - " << newSteps + 1 << "' );";
            m_vecParallelSqls.push_back( stmSqlGen.str() );

            found = true;
            ++newSteps;
        }

        if ( !itLoop->nowait )
        {
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
                    getStepPostionsForFlow( itLoop->type, itLoop->pkey, itLoop->position, mapFlowNextSteps );
                    break;
                }
            case eEnd:
                {
                    break;
                }
            default:
                {
                    pushFlowSql( stmFlowSql, itLoop->plan, itLoop->position + newSteps, itLoop->position + newSteps + 1, sfPkey, id );
                    break;
                }
            }
        }

        if ( itLoop->nowait )
        {
            vecParallelIds.push_back( itLoop->position + newSteps );
        }

        if ( newSteps > 0u )
        {
            stmSqlGen.str( "" );
            stmSqlGen << "update plan_step_details set position = " << itLoop->position + newSteps << " where p_key = " << itLoop->pkey << ";";
            m_vecParallelSqls.push_back( stmSqlGen.str() );

            mapOldPosToNewPos.insert( std::make_pair( itLoop->position, itLoop->position + newSteps ) );
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
                getStepPostions( itLoop->type, itLoop->pkey, mapNextSteps );
                break;
            }
        case eEnd:
            {
                break;
            }
        default:
            {
                break;
            }
        }
    }
}

void ParallelCreator::loadData()
{
    std::string strSql = "select P_KEY, FK_STEP_TYPE, FK_NODE_TREE, POSITION, NO_WAIT from step_details where "
        "fk_node_tree in (select distinct ( fk_node_tree ) from step_details where no_wait <> 0) "
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
    valueColumns.push_back( "NO_WAIT" );

    unsigned long pkey = 0u;
    unsigned long fkStepType = 0u;
    unsigned long fkPlan = 0u;
    int position = 0;
    bool nowait = false;

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
            nowait = data->getBooleanData( i, "NO_WAIT" );

            NoWaitStep stStep( pkey, fkStepType, fkPlan, position, nowait );
            m_vecSteps.push_back( stStep );
        }
        dataPtr.reset();
        data = NULL;
    } while ( database->moreData( data ) );
}

unsigned int ParallelCreator::getStepPos( const std::string& strStep, const bool bByKey )
{
    unsigned int unId = 0;

    if ( 0u == strStep.find( '#' ))
    {
        std::stringstream stmConvertor(strStep.substr(1));
        stmConvertor >> unId;
    }
    else if ( bByKey && ( std::string::npos == strStep.find_first_not_of( "0123456789" )))
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
        } while ( database->moreData( data ) );
    }

    return unId;
}

void ParallelCreator::getStepPostionsForFlow( const int eType, const unsigned long ulStepKey, const unsigned int unInPos, std::multimap<unsigned int, unsigned int>& mapSteps )
{
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
    strSql += TA_Base_Core::DataConversion::toString( ulStepKey );

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
                unsigned int unId = getStepPos( strValue, true );

                if ( 0u != unId )
                {
                    mapSteps.insert( std::make_pair( unInPos, unId ) );

                    if ( mapSteps.size() == setPos.size() )
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

void ParallelCreator::getStepPostions( const int eType, const unsigned long ulStepKey, std::map<unsigned long, unsigned int>& vecSteps )
{
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

    std::string strSql = "select P_KEY, POSITION, VALUE from step_parameters where fk_step_details = ";
    strSql += TA_Base_Core::DataConversion::toString( ulStepKey );
    strSql += " and value like '#%'";

    TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Plans_Cd, TA_Base_Core::Read );

    TA_Base_Core::SQLStatement sql;
    sql.nSQLID = 0;
    sql.strCommon = strSql;

    std::vector<std::string> valueColumns;
    valueColumns.push_back( "P_KEY" );
    valueColumns.push_back( "POSITION" );
    valueColumns.push_back( "VALUE" );

    unsigned long ulPkey = 0u;
    unsigned int unPosition = 0u;
    std::string strValue = "";
    TA_Base_Core::IData* data = database->executeQuery( sql, valueColumns );
    do
    {
        std::auto_ptr<TA_Base_Core::IData> dataPtr( data );
        for ( unsigned long i = 0, sz = dataPtr->getNumRows(); i < sz; ++i )
        {
            ulPkey = data->getUnsignedLongData( i, "P_KEY" );
            unPosition = data->getIntegerData( i, "POSITION" );
            strValue = data->getStringData( i, "VALUE" );

            if ( setPos.find( unPosition ) != setPos.end() )
            {
                unsigned int unId = getStepPos( strValue, false );

                if ( 0u != unId )
                {
                    vecSteps.insert( std::make_pair( ulPkey, unId ));

                    if ( vecSteps.size() == setPos.size() )
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
        }

        dataPtr.reset();
        data = NULL;
    } while ( database->moreData( data ) );
}

void ParallelCreator::createFlowForNextSteps( std::stringstream& stmFlowSql, const unsigned long ulPlan, const std::multimap<unsigned int, unsigned int>& mapNextSteps, const std::map<unsigned int, unsigned int>& mapOldPosToNewPos, unsigned long& sfPkey, unsigned int& id )
{
    for ( std::multimap<unsigned int, unsigned int>::const_iterator itNext = mapNextSteps.begin( ); mapNextSteps.end( ) != itNext; ++itNext )
    {
        unsigned int unNewIncomingStep = itNext->first;
        std::map<unsigned int, unsigned int>::const_iterator itFound = mapOldPosToNewPos.find( unNewIncomingStep );
        if ( mapOldPosToNewPos.end() != itFound )
        {
            unNewIncomingStep = itFound->second;
        }

        unsigned int unNewOutgoingStep = itNext->second;
        itFound = mapOldPosToNewPos.find( unNewOutgoingStep );
        if ( mapOldPosToNewPos.end() != itFound )
        {
            unNewOutgoingStep = itFound->second;
        }

        pushFlowSql( stmFlowSql, ulPlan, unNewIncomingStep, unNewOutgoingStep, sfPkey, id );
    }
}

void ParallelCreator::updateStepParam( std::stringstream& stmSqlGen, const std::map<unsigned long, unsigned int>& mapNextSteps, const std::map<unsigned int, unsigned int>& mapOldPosToNewPos )
{
    for ( std::map<unsigned long, unsigned int>::const_iterator itNext = mapNextSteps.begin(); mapNextSteps.end() != itNext; ++itNext )
    {
        unsigned int unNewStepPos = 0u;
        std::map<unsigned int, unsigned int>::const_iterator itFound = mapOldPosToNewPos.find( itNext->second );
        if ( mapOldPosToNewPos.end() == itFound )
        {
            continue;
        }

		std::stringstream newValue;
		newValue << "#" << itFound->second;

		m_mapUpdatedParams.insert( std::make_pair( itNext->first, newValue.str() ) );
    }
}

unsigned long ParallelCreator::getNewPkey()
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
        for ( unsigned long i = 0, sz = dataPtr->getNumRows(); i < sz; ++i )
        {
            pkey = data->getUnsignedLongData( i, "Next_Step_Id" );
        }
        dataPtr.reset();
        data = NULL;
    } while ( database->moreData( data ) );

    return pkey;
}

std::vector<std::string>& ParallelCreator::flowSqls()
{
    return m_vecFlowSqls;
}

void ParallelCreator::pushFlowSql( std::stringstream& stmFlowSql, const unsigned long ulPlan, const unsigned int unIncoming, const unsigned int unOutgoing, unsigned long& sfPkey, unsigned int& id )
{
    stmFlowSql.str( "" );

    static unsigned long s_plan = 0u;
    if ( ulPlan != s_plan )
    {
        s_plan = ulPlan;
        stmFlowSql << "-- Plan " << ulPlan << std::endl;
    }

    stmFlowSql << "insert into plan_sequence_flow (P_KEY, INCOMING_STEP, OUTGOING_STEP, CONDITION, GRAPH, PLAN_PKEY, ID) values ("
        << sfPkey++ << ", " << unIncoming << ", " << unOutgoing << ", " << "''" << ", " << "''" << ", " << ulPlan << ", " << id++ << ");";
    m_vecFlowSqls.push_back( stmFlowSql.str().c_str() );
}
