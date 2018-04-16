/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent/src/FutureResult.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef FUTURE_RESULT_H_INCLUDED
#define FUTURE_RESULT_H_INCLUDED

enum CommandID
{
	REGISTER_AP = 1,
	DEREGISTER_AP,
	RETRIEVE_RCDB_GRANT,
	ASSIGN_RESOURCE,
	DEASSIGN_RESOURCE,
	SELECT_RESOURCE,
	DESELECT_RESOURCE,
	CHANGE_VOLUME,
	GET_FREE_PATCHGRP,
	ADD_PATCH_MEMBER,
	DELETE_PATCH_MEMBER,			
	GET_FREE_MSEL,
	OPEN_MSEL,
	CLOSE_MSEL,
	ADD_MSEL_MEMBER,
	DELETE_MSEL_MEMBER,
	BEGIN_PRIVATE_CALL,
	ANSWER_PRIVATE_CALL,
	END_PRIVATE_CALL,
	BEGIN_AMBIENCE_LISTENING,
	END_AMBIENCE_LISTENING,
	BEGIN_EMERGENCY_CALL,
	END_EMERGENCY_CALL,
	BEGIN_GENERAL_XMIT,
	END_GENERAL_XMIT,
	PHONE_SELECT,
	DIAL_PHONE,
	END_PHONE_CALL,
	BROADCAST_TRAIN
};


template <class T>
class FutureResult_T
{

	public:

		virtual ~FutureResult_T() {};

	public:

		enum TransactionState
		{
			NO_STATE,
			NEW,
			COMPLETED,
			ABORTED,
			TIMEDOUT
		};

		virtual void set(const T& data) = 0;
		virtual T get() = 0;

	
	protected:

		T m_data;

	public:

		TransactionState m_exceptionIndicator;

		long	m_param;

        unsigned long m_resourceId; // to associate the future with correct resource
        unsigned short m_resourceType;
        unsigned long m_channel;

		unsigned short m_state; 
};


template <class T>
class MConsoleResult_T : public FutureResult_T<T>
{

	public:

		MConsoleResult_T()
		{
			m_data  = 0;
			m_param = 0;
			m_state = 0;
            m_resourceId = 0;
            m_resourceType = 0;
            m_channel = 0;
			m_exceptionIndicator = NO_STATE;
		};

		virtual ~MConsoleResult_T() {};


	public:


		virtual void set(const T& data)
		{
			m_data = data;

			m_exceptionIndicator = NEW;
		};

		virtual T get() { return m_data; };
};


#endif 
