#pragma once
#include <string>

class StationMode
{
public:
	StationMode(void);
public:
	void setStationName ( std::string stationName );
	void setStationId ( unsigned long stationId );
	unsigned long getStationId () ;
	std::string getStationName();
public:
	~StationMode(void);
private:
	std::string m_stationName;
	unsigned long m_stationId;
};
