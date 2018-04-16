#pragma once
#include <string>

class HusParam
{
public:
	HusParam(void);
	~HusParam(void);
	void setSiteIP(const std::string& siteIP);
	void setSiteID(const std::string& siteID);
	void setUserName(const std::string& userName);
	void setPassword(const std::string& passWord);
	void setClientIP(const std::string& clientIP);
	void setClientPort(const std::string& clientPort);

	std::string getSiteIP();
	std::string getSiteID();
	std::string getUserName();
	std::string getPassword();
	std::string getClientIP();
	std::string getClientPort();
private:
	std::string m_siteIP;
	std::string m_siteID;
	std::string m_userName;
	std::string m_password;
	std::string m_clientIP;
	std::string m_clientPort;
};
