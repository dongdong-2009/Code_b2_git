#include "StationMode.h"

StationMode::StationMode(void)
{
}

StationMode::~StationMode(void)
{
}

void StationMode::setStationName ( std::string stationName ) 
{
	m_stationName = stationName;
}
void StationMode::setStationId ( unsigned long stationId ) 
{
	m_stationId = stationId;
}
unsigned long StationMode::getStationId ()  
{
	return m_stationId;
}
std::string StationMode::getStationName() 
{
	return m_stationName;
}
