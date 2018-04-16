#include "WorkflowException.h"


WorkflowException::WorkflowException( const std::string& strWhat ) : std::exception(),
m_strWhat( strWhat.c_str() )
{
}


WorkflowException::~WorkflowException()
{
}

const char* WorkflowException::what() const
{
    return m_strWhat.c_str();
}
