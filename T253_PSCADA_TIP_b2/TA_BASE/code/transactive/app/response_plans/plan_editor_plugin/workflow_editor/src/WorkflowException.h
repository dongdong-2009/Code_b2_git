#ifndef WORKFLOWEXCEPTION_INCLUDE
#define WORKFLOWEXCEPTION_INCLUDE

#include <exception>
#include <string>

class WorkflowException : std::exception
{
public:
    WorkflowException( const std::string& strWhat );
    ~WorkflowException();

    virtual const char * what() const;

private:
    std::string m_strWhat;
};

#endif
