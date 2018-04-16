#pragma once
namespace TA_IRS_App
{
//TVSS Method define
#define METHOD_QUAD_SCREEN               0x01
#define METHOD_CLEAR_IMAGE               0x02
#define METHOD_ACKNOWLEDGE_EVENT         0x03
#define METHOD_SINGLE_SCREEN             0x04
#define METHOD_SEQUENCE_SCREEN           0x05


//TVSS EVENT define
#define EVENT_COMMAND_RESPONSE          0x06
#define EVENT_TRAINBORNE_EVENT          0x07
#define METHOD_EQUIPMENT_ALARM_QUERY    0x08
#define EVENT_EQUIPMENT_ALARM_RESPONSE  0x0A
#define METHOD_TRAINBORNE_HEARTBEAT     0x40


// Information
#define STATUS_OK                       0
#define SHORTSTRING_LENGTH              32 

#define TRAINID_START_POSITION			4


typedef unsigned char MonitorID ;
};