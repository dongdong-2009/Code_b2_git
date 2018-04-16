#include "HusParam.h"

HusParam::HusParam(void)
{
}

HusParam::~HusParam(void)
{
}

void HusParam::setSiteIP( const std::string& siteIP )
{
	m_siteIP = siteIP;
}

void HusParam::setSiteID( const std::string& siteID )
{
	m_siteID = siteID;
}

void HusParam::setUserName( const std::string& userName )
{
	m_userName = userName;
}

void HusParam::setPassword( const std::string& password )
{
	m_password = password;
}

void HusParam::setClientIP( const std::string& clientIP )
{
	m_clientIP = clientIP;
}

void HusParam::setClientPort( const std::string& clientPort )
{
	m_clientPort = clientPort;
}

std::string HusParam::getSiteIP()
{
	return m_siteIP;
}

std::string HusParam::getSiteID()
{
	return m_siteID;
}

std::string HusParam::getUserName()
{
	return m_userName;
}

std::string HusParam::getPassword()
{
	return m_password;
}

std::string HusParam::getClientIP()
{
	return m_clientIP;
}

std::string HusParam::getClientPort()
{
	return m_clientPort;
}
