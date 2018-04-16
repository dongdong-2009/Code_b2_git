#pragma once
namespace TA_IRS_App
{
//TVSS Method define
#define METHOD_CLEAR_IMAGE               0x02
#define METHOD_QUAD_SCREEN               0x03
#define METHOD_SINGLE_SCREEN             0x04
#define METHOD_SEQUENCE_SCREEN           0x05
#define METHOD_EQUIPMENT_ALARM_QUERY     0x08

//TVSS EVENT define
#define EVENT_COMMAND_RESPONSE          0x06
#define EVENT_TRAINBORNE_EVENT          0x07
#define EVENT_EQUIPMENT_ALARM_RESPONSE  0x09


// Information
#define STATUS_OK                       0
#define SHORTSTRING_LENGTH              32 
typedef unsigned char MonitorID ;
};