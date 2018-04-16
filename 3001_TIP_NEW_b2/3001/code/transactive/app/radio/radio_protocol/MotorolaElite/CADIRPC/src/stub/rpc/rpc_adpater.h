#ifndef CADI_RPC_ADAPTER_H
#define CADI_RPC_ADAPTER_H

#define MAX_USERNAME_LEN 9
#define MAX_PASSWORD_LEN 31


namespace
{
	static const int CADI_PDUTYPE_INDEX = 1;

	enum CADI_PDUType
	{
		CADI_LOGIN_REQ                  = 0x00,
		CADI_LOGIN_RESP					= 0x01,
		CADI_LOGOUT_REQ		            = 0x02,
		CADI_LOGOUT_RESP	            = 0x03,
		CADI_REGROUP_REQ                = 0x04,
		CADI_REGROUP_RESP				= 0x05,
		CADI_XREGROUP_REQ				= 0x06,
		CADI_XREGROUP_RESP				= 0x07
	};
}

struct CADI_REGROUP_DATA {
	unsigned long target_radio_id;
	unsigned long target_group_id;
};


//for login, logout doesn't use parameter
struct CADI_LOGIN_DATA {
	char user_name[MAX_USERNAME_LEN];
	char password[MAX_PASSWORD_LEN];
	int is_password_encrypted;
};


struct CADI_MSG
{
	char length;
	enum CADI_MSG_TYPE { CADI_LOGIN, CADI_LOGOUT, CADI_REGROUP, CADI_XREGROUP} iMsgType;
	union {
		CADI_REGROUP_DATA	regroup;
		CADI_LOGIN_DATA		login;
	} data;
};

//adatpter interface.

extern "C"
{
	void CADIConnect(char* host);
	void CADIRegroup(char * pData);
	void CADICancelRegroup(char * pData);
	bool CADILogin(char * pData);
	void CADILogout();
}

#endif