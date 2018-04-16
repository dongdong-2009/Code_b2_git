#include "ParameterData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include <sstream>

unsigned long ParameterData::INVALID_PKEY = -1;

ParameterData::ParameterData(void):
    m_pkey(INVALID_PKEY),
    m_fkStepDetails(INVALID_PKEY),
    m_fkNodeTree(INVALID_PKEY),
    m_position(0),
    m_fkStepType(INVALID_PKEY),
    m_dataLoaded(false)
{
}

ParameterData::ParameterData(const unsigned long pkey,
                             const unsigned long fkStepDetails,
                             const unsigned long fkNodeTree,
                             const int position,
                             const std::string& value,
                             const unsigned long fkStepType,
                             const std::string& stepTypeName):
    m_pkey(pkey),
    m_fkStepDetails(fkStepDetails),
    m_fkNodeTree(fkNodeTree),
    m_position(position),
    m_value(value),
    m_fkStepType(fkStepType),
    m_stepTypeName(stepTypeName),
    m_dataLoaded(true)
{

}

ParameterData::ParameterData(unsigned long pkey):
    m_pkey(pkey),
    m_fkStepDetails(INVALID_PKEY),
    m_fkNodeTree(INVALID_PKEY),
    m_position(0),
    m_fkStepType(INVALID_PKEY),
    m_dataLoaded(false)
{
}

ParameterData::~ParameterData(void)
{
}

int ParameterData::getPostion()
{
    if(!m_dataLoaded)
    {
        loadData();
    }
    return m_position;
}

unsigned long ParameterData::getPkey()
{
    if(!m_dataLoaded)
    {
        loadData();
    }
    return m_pkey;
}

unsigned long ParameterData::getFkStepDetails()
{
    if(!m_dataLoaded)
    {
        loadData();
    }
    return m_fkStepDetails;
}

std::string ParameterData::getValue()
{
    if(!m_dataLoaded)
    {
        loadData();
    }
    return m_value;
}

unsigned long ParameterData::getFkStepType()
{
    if(!m_dataLoaded)
    {
        loadData();
    }
    return m_fkStepType;
}

std::string ParameterData::getStepTypeName()
{
    if(!m_dataLoaded)
    {
        loadData();
    }
    return m_stepTypeName;
}

void ParameterData::loadData()
{
    TA_ASSERT(m_pkey != INVALID_PKEY, "");
    TA_Base_Core::IDatabase* database = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Plans_Cd, TA_Base_Core::Read);

    std::stringstream ss;
    ss << "SELECT P.P_KEY, P.FK_STEP_DETAILS, STEPDETAILS.FK_NODE_TREE, P.POSITION, P.VALUE, STEPTYPE.P_KEY AS FK_STEP_TYPE, STEPTYPE.TYPE_NAME FROM STEP_PARAMETERS P, STEP_DETAILS STEPDETAILS, STEP_TYPE STEPTYPE WHERE P.P_KEY = ";
    ss << m_pkey;
    ss << " P.FK_STEP_DETAILS = STEPDETAILS.P_KEY AND STEPTYPE.P_KEY = STEPDETAILS.FK_STEP_TYPE";

    TA_Base_Core::SQLStatement sql;
    sql.nSQLID = 0;
    sql.strCommon = ss.str();

    std::vector<std::string> valueColumns;
    valueColumns.push_back("FK_STEP_DETAILS");
    valueColumns.push_back("FK_NODE_TREE");
    valueColumns.push_back("POSITION");
    valueColumns.push_back("VALUE");
    valueColumns.push_back("FK_STEP_TYPE");
    valueColumns.push_back("TYPE_NAME");

    TA_Base_Core::IData* data = database->executeQuery(sql, valueColumns);

    do
    {
        std::auto_ptr<TA_Base_Core::IData> dataPtr(data);
        if(data->getNumRows() != 1)
        {
            throw TA_Base_Core::DataException("", data->getNumRows() == 0 ? TA_Base_Core::DataException::NO_VALUE : TA_Base_Core::DataException::NOT_UNIQUE, "");
        }

        m_fkStepDetails = data->getUnsignedLongData(0, "FK_STEP_DETAILS");
        m_fkNodeTree = data->getUnsignedLongData(0, "FK_NODE_TREE");
        m_position = data->getIntegerData(0, "POSITION");
        m_value = data->getStringData(0, "VALUE");
        m_fkStepType = data->getUnsignedLongData(0, "FK_STEP_TYPE");
        m_stepTypeName = data->getStringData(0, "TYPE_NAME");

        dataPtr.reset();
        data = NULL;
    }
    while(database->moreData(data));

    m_dataLoaded = true;
}

unsigned long ParameterData::getFkNodeTree()
{
    if(!m_dataLoaded)
    {
        loadData();
    }
    return m_fkNodeTree;
}
