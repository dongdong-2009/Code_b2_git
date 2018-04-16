#ifndef ATS_SIM_USER_PROTOCOL_H
#define ATS_SIM_USER_PROTOCOL_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Declaration file for classes used to convey user requests and responses
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommon.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include <string>
#include <list>
#include <queue>
#include <map>
// required for std::pair when not #including <map>
#include <utility>

namespace TA_IRS_App
{
namespace ATS_Sim
{
    ///
    /// Enumerated type definition for types of messages that users might
    /// invoke
    typedef enum MessageType
    {
        Show    = 'R',  /// Code for 'show table' command messages
        Set     = 'W',  /// Code for 'set table' command messages
        Dump    = 'D',  /// Code for 'dumb table' command messages
        Mode    = 'M',  /// Code for 'show mode' messages
        LogStart= 'L',  /// Code for 'log start' command messages
        LogStop = 'l',  /// Code for 'log stop' command messages
        LogMode = 'm',  /// Code for 'log mode' messages
        Uninitialised = 'U' /// Invaluable invalid value
    };


    /** Base class for UserQuery and UserResponse
      *
      */
    class UserProtocol
    {
    public:
        /// Constructor with values that should be known at construction time
        UserProtocol(MessageType type);

        /// Destructor
        virtual ~UserProtocol();

        /// accessor for type of this message
        /// @return value of m_type data member
        MessageType getType() const;

        /// accessor for the address of the table of the focus of this message
        /// @return value of m_tableAddress data member
        unsigned short getTableAddress() const;

        ///
        void setTableAddress(unsigned short tAddr);

        /// with some user queries, the type changes if later parameters dictate so
		/// eg. "show mode" changes the type from 'R' to 'M'
        void setType(MessageType type);

    private:
        /// type of this user protocol message
        MessageType         m_type;
        /// address of the table in focus
        unsigned short      m_tableAddress;
    }; // class UserProtocol


    /**
      * A class formed by the ATSSimulator::execXXXX methods from user
      * querys given by ATSSimulatorCommandProcessor, sent to
      * ATSTableQueryProcessor.
      */
    class UserQuery : public UserProtocol
    {
    public:
        /// Constructor with values that ought to be known at construction time
        UserQuery(MessageType type, TableOutputFormat format);

        /// Destructor
        ~UserQuery();
        
        ///
        TableOutputFormat getOutputFormat() const;
        
        ///
        void setOutputFormat(TableOutputFormat format);

        ///
        int setRecord( unsigned long recordNumber );

        ///
        int setRecords( const RecordRange & records );

        ///
        int removeRecords( const RecordRange & records );

        ///
        /// @param recordstr as per ATSSimUtility::expandRange 'r' parameter
        /// @param max as per ATSSimUtility::expandRange 'max' parameter
        int setRecords( const std::string & recordstr, unsigned long max );

        ///
        typedef Word FieldValueType;

        /// type definition of a pair of numbers, representing a field and
        /// the value it is to be set to.
        typedef std::pair< unsigned long, FieldValueType > FieldValuePair;

        /// Type definition of a list of arguments for Subcommand
        /// implementations to pass stuff to us
        typedef std::list< FieldValuePair > ArgumentList;

        /// Type definition of a queue of arguments for
        /// ATSSimTableAtsXXXX::processUserWrite implementations to deal with
        typedef std::queue< FieldValuePair, std::list< FieldValuePair> > ArgumentQueue;

        ///
        void pushArg( const FieldValuePair & fieldvalue );

        ///
        void pushArg( unsigned long field, const FieldValueType & value );

        ///
        void copyArgs( const ArgumentList & arglist );

        /** Breaks the list of token strings "num=value","num=value", ... into
          * a list of FieldValuePairs for giving to copyArgs().
          * 
          * @param args the list of tokens passed in
          * @param err the ostream to output error strings to
          * @return the number of args successfully parsed
          */
        int parseArgs(const TokenList & args, std::ostream & err);

        ///
        FieldValuePair popArg();

        /// @return true if there are more arguments to process
        bool moreArgs();
        
        ///
        const RecordRange & getRecords() const;

        /** encapsulates ::strtoul with suitable error processing
          */
        unsigned long str2ul(const char * s, std::ostream & err, bool & ok);

        /** encapsulates ::strtoul with suitable error processing
          */
        FieldValueType str2FieldValueType(const char * s, std::ostream & err, bool & ok);

    private:
        /// format requested
        TableOutputFormat   m_outputFormat; // not valid for W messages

        /// Queue of arguments
        ArgumentQueue       m_arguments;

        /// Range of records
        RecordRange         m_records;
    }; // UserQuery

    const unsigned long MaskFieldIndicator          = 0x4D41534B; // 'MASK' in hex
    const unsigned long ToggleMaskFieldIndicator    = 0x544F474C; // 'TOGL'
    const unsigned long LeaveMaskFieldIndicator     = 0x4C454156; // 'LEAV'
    const unsigned long MaskEndFieldIndicator       = 0x4D454E44; // 'MEND'
    const unsigned long InvalidFieldIndicator       = 0xFFFFFFFF;

    /**
      * A class used to respond to user querys received from the
      * ATSSimulator/CommandProcessor
      */
    class UserResponse : public UserProtocol
    {
    public:
        /// The response to the query. Output to the console.
        std::string         m_responseData;

        /// only construct in response to a query
        UserResponse(UserQuery * query);
    };


    /// Code for 'show' queries or replies
    const unsigned char ATSSim_User_Show                 = 'R'; // Read
    /// Code for 'set' queries or replies
    const unsigned char ATSSim_User_Set                  = 'W'; // Write
    /// Code for 'dump' queries or replies
    const unsigned char ATSSim_User_Dump                 = 'D'; // Dump
    /// Code for 'log' queries or replies
    const unsigned char ATSSim_User_Log                  = 'L'; // Log
    /// Code for 'show mode' queries or replies
    const unsigned char ATSSim_User_Mode                 = 'M'; // Mode

} // namespace ATS_Sim
} // namespace TA_IRS_App
#endif // #ifndef ATS_SIM_USER_PROTOCOL_H
