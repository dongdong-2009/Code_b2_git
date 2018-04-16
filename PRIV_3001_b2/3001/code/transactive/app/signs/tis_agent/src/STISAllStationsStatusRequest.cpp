#include "core/utilities/src/DebugUtil.h"
#include "StatusMonitor.h"
#include "STISAllStationsStatusRequest.h"
#include <sstream>
#include <iterator>

using TA_Base_Core::DebugUtil;
using std::vector;
using std::string;
using std::stringstream;
using std::copy;
using std::istreambuf_iterator;
using std::back_inserter;
using std::distance;

namespace TA_IRS_App
{
    STISAllStationsStatusRequest::STISAllStationsStatusRequest(void)
    {
        m_messageIDString = "M30";
    }

    STISAllStationsStatusRequest::~STISAllStationsStatusRequest(void)
    {
    }

    vector<unsigned char> STISAllStationsStatusRequest::createProtocolMessage()
    {
        vector<unsigned char> messageVector;
        stringstream messageStream;
        // Add the sequence number
        messageStream << getSequenceNumberAsString();
        // Add the timestamp to the message
        messageStream << m_timestamp;
        // Now the message ID
        messageStream << m_messageIDString;
        // Now the data length - in this case it will always be 3 byte
        messageStream << "0003";
        // Now the actual data
        messageStream << "OCC";
        messageVector.push_back(0x02);
        copy(istreambuf_iterator<char>(messageStream),
             istreambuf_iterator<char>(),
             back_inserter(messageVector));
        messageVector.push_back(0x03);
        return messageVector;
    }

    void STISAllStationsStatusRequest::processResponse()
    {
        const int DATALENTHBEGINPOS = 22;
        const int STATIONREPORTBEGINPOS = 26;
        const int DATALENGTHLEN = 4;
        const int REPORTSTATIONLEN = 6;
        const int CONNECTIONLINKALARMLEN = 1;
        const int LIBRARYVERSIONLEN = 3;
        const int PIDNUMBERLEN = 2;
        vector<unsigned char>::iterator it = m_responseMessage.begin();
        string dataLengthStr(it + DATALENTHBEGINPOS, it + DATALENTHBEGINPOS + DATALENGTHLEN);
        if(dataLengthStr.find_first_not_of("0123456789") != string::npos)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Response message format is not valid");
            return;
        }
        const int dataLength = atoi(dataLengthStr.c_str());
        vector<unsigned char>::iterator dataBegin = m_responseMessage.begin() + STATIONREPORTBEGINPOS;
        vector<unsigned char>::iterator dataEnd = dataBegin + dataLength + 1;
        while(dataBegin != dataEnd)
        {
            // Report station
            if(distance(dataBegin, dataEnd) < REPORTSTATIONLEN) break;
            string reportStation(dataBegin, dataBegin + REPORTSTATIONLEN);
            dataBegin += REPORTSTATIONLEN;
            reportStation.erase(reportStation.find_first_of(' '));
            // ConnectionLinkAlarm
            if(distance(dataBegin, dataEnd) < CONNECTIONLINKALARMLEN) break;
            bool connectionLinkAlarm = (*dataBegin != '0');
            dataBegin += CONNECTIONLINKALARMLEN;
            // Current library version
            if(distance(dataBegin, dataEnd) < LIBRARYVERSIONLEN) break;
            string currentLibraryVersionStr(dataBegin, dataBegin + LIBRARYVERSIONLEN);
            dataBegin += LIBRARYVERSIONLEN;
            // Next library version
            if(distance(dataBegin, dataEnd) < LIBRARYVERSIONLEN) break;
            string nextLibraryVersionStr(dataBegin, dataBegin + LIBRARYVERSIONLEN);
            dataBegin += LIBRARYVERSIONLEN;
            int currentLibraryVersion = atoi(currentLibraryVersionStr.c_str());
            int nextLibraryVersion = atoi(nextLibraryVersionStr.c_str());
            updateCurrentLibraryVersion(reportStation, currentLibraryVersion);
            updateNextLibraryVersion(reportStation, nextLibraryVersion);
            // Update station connection link alarm status
            updateStationConnectionLinkStatus(reportStation, connectionLinkAlarm);
            // Pid number
            if(distance(dataBegin, dataEnd) < PIDNUMBERLEN) break;
            string pidNumberStr(dataBegin, dataBegin + PIDNUMBERLEN);
            dataBegin += PIDNUMBERLEN;
            int pidNumber = atoi(pidNumberStr.c_str());
            if(pidNumber > 0)
            {
                if(distance(dataBegin, dataEnd) < pidNumber) break;
                string pidStatus(dataBegin, dataBegin + pidNumber);
                dataBegin += pidNumber;
                updateStationPidStatus(reportStation, pidStatus);
            }
        }
    }

    void STISAllStationsStatusRequest::updateStationConnectionLinkStatus(const string& reportSatation, bool status)
    {
		string dataPointFirstLevelName = "OCC_" + reportSatation;
		StatusMonitor::getInstance()->updateConnectionLinkAlarmDataPointState(status, dataPointFirstLevelName);
    }

    void STISAllStationsStatusRequest::updateStationPidStatus(const string& reportSatation, const string& pidStatus)
    {
		string dataPointFirstLevelName = "OCC_" + reportSatation;
		for(int i = 0; i < pidStatus.size(); ++i)
		{
			int status = pidStatus[i] - '0';
			StatusMonitor::getInstance()->updatePidStatus(i+1, status, dataPointFirstLevelName);
		}
    }

    void STISAllStationsStatusRequest::updateCurrentLibraryVersion(const string& reportSatation, int version)
    {
		string dataPointFirstLevelName = "OCC_" + reportSatation;
		StatusMonitor::getInstance()->setCurrentSTISStationLibraryVersion(version, dataPointFirstLevelName);
    }

    void STISAllStationsStatusRequest::updateNextLibraryVersion(const string& reportSatation, int version)
    {
		string dataPointFirstLevelName = "OCC_" + reportSatation;
		StatusMonitor::getInstance()->setNextSTISStationLibraryVersion(version, dataPointFirstLevelName);
    }

    void STISAllStationsStatusRequest::log( bool sendSuccess )
    {
    }
}
