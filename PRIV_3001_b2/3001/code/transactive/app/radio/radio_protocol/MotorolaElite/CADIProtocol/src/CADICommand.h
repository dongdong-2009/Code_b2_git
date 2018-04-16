
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADICommand.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#ifndef CADICOMMAND_H
#define CADICOMMAND_H



#include "ace/Method_Request.h"

#include <string>
#include <vector>


namespace TA_IRS_App 
{
	typedef	std::vector<std::string> TokenList;

	// Function
    class CADIFunction
    {
    public:
        CADIFunction();
        virtual ~CADIFunction();

        virtual int call(TokenList &args) = 0;

    private:
        // disabled methods
        CADIFunction(const CADIFunction&);
        void operator=(const CADIFunction&);
    };
	

	// Handshake
    class HandshakeCommand : public CADIFunction
    {
    public:
        HandshakeCommand();
        virtual ~HandshakeCommand();

        virtual int call(TokenList &args);
        
    private:
        // disable methods
        HandshakeCommand(const HandshakeCommand&);
        void operator=(const HandshakeCommand&);
    };


	// Login 
    class LoginCommand : public CADIFunction  
    {
    public:
        LoginCommand();
        virtual ~LoginCommand();
        
        virtual int call(TokenList &args);

    private:
        // disabled methods
        LoginCommand(const LoginCommand&);
        void operator=(const LoginCommand&);
    };


	// Logout
    class LogoutCommand : public CADIFunction  
    {
    public:
        LogoutCommand();
        virtual ~LogoutCommand();
        
        virtual int call(TokenList & args);

    private:
        // disable methods
        LogoutCommand(const LogoutCommand&);
        void operator=(const LogoutCommand&);
    };


	// Regroup
    class RegroupCommand : public CADIFunction  
    {
    public:
        RegroupCommand();
        virtual ~RegroupCommand();
        
        virtual int call(TokenList & args);

    private:
        // disabled methods
        RegroupCommand(const RegroupCommand&);
        void operator=(const RegroupCommand&);
    };


	// Cancel Regroup
    class CancelRegroupCommand : public CADIFunction  
    {
    public:
        CancelRegroupCommand();
        virtual ~CancelRegroupCommand();
            
        virtual int call(TokenList & args);

    private:
        // disabled methods
        CancelRegroupCommand(const CancelRegroupCommand&);
        void operator=(const CancelRegroupCommand&);
    };


	// XRegroupWGid
    class XRegroupWGidCommand : public CADIFunction
    {
    public:
        XRegroupWGidCommand();
        virtual ~XRegroupWGidCommand();
        
        virtual int call(TokenList & args);

    private:
        // disabled methods
        XRegroupWGidCommand(const XRegroupWGidCommand&);
        void operator=(const XRegroupWGidCommand&);
    };

};

#endif
