#ifndef MESSAGELIBRARYPARSER_H_INCLUDED
#define MESSAGELIBRARYPARSER_H_INCLUDED

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/data_access_interface/tis_agent_4669/src/IConfigPredefinedMessageLibrary.h"
#include "core/utilities/src/DebugUtil.h"
#include <string>
#include "boost/regex.hpp"

namespace TA_IRS_App
{
    enum EMessageLibraryType
    {
        STATION_LIBRARY,
        TRAIN_LIBRARY
    };

	// Cache the string & wstring tag
	class StringHolder 
	{
	public:
		StringHolder(const std::string& str, const std::wstring& wstr)
			:m_str(str), m_wstr(wstr)
		{

		}
		operator std::string() const {return m_str;}
		operator std::wstring() const {return m_wstr;}
		/*
		operator const char*() const {return m_str.c_str();}
		operator const wchar_t*() const {return m_wstr.c_str();} 
		*/
	private:
		std::string m_str;
		std::wstring m_wstr;
	};

    class MessageLibraryParser
    {
        public:
            class DBOperator
            {
                public:
                    // DBOperator constructor, set conntype to Tis_OccCd can write occ local db
                    DBOperator( TA_Base_Core::EDataTypes conntype = TA_Base_Core::Tis_Cd );
                    // Delete the old library content from db, include the default adhoc attributes
                    void deleteLibrary( unsigned long version, const std::string& libraryType );
                    std::auto_ptr<TA_Base_Core::IConfigPredefinedMessageLibrary>
                    createNewPredefinedMessageLibrary( unsigned long version,
                                                       const std::string& libraryType,
                                                       std::vector<unsigned char>& libraryFileContent );
                private:
                    TA_Base_Core::EDataTypes m_connType;
            };
            MessageLibraryParser( const std::vector<unsigned char>& fileContent,
                                  EMessageLibraryType libraryType,
                                  TA_Base_Core::EDataTypes conntype = TA_Base_Core::Tis_Cd );
            virtual ~MessageLibraryParser() {}
            virtual unsigned long getLibraryVersion() = 0;
            virtual void parseLibrary() = 0;
			// Tags for search usage
			static const StringHolder DATABASE_LENGTH_TAG;
			static const StringHolder VERSION_TAG;
			static const StringHolder CREATED_BY_TAG;
			static const StringHolder CREATE_DATE_TAG;
			static const StringHolder DESCRIPTION_TAG;
			static const StringHolder MESSAGE_TYPE_TAG;
			static const StringHolder NUMBER_MESSAGES_TAG;
			static const StringHolder DEFAULT_ATTRIBUTE_TAG;
			static const StringHolder CHECKSUM_TAG;
			static const StringHolder NEW_LINE;
			static const StringHolder EQUALS;
			static const StringHolder DELIMITER;
			static const StringHolder WINDOWS_NEW_LINE;
			static const StringHolder EMPTY_STRING;
			static const StringHolder TRIM_REGX;
        protected:

            // read a "tag = value" line
            // returns an empty string on failure
			template<class T> static std::basic_string<T> getValue( const std::basic_string<T>& file,
				                                                    const StringHolder& tagString,
				                                                    const typename std::basic_string<T>::size_type findFrom = 0)
			{
				typedef std::basic_string<T> string_type;
				string_type value;
				string_type tag = tagString;
		        // find the tag
		        typename string_type::size_type start = file.find( tag, findFrom );
		        // if it was found
		        if ( start != string_type::npos )
		        {
		            // find the equals
		            start = file.find( EQUALS, start+1 );
		            start += 1;
		            // trim the leading and trailing whitespace (if any)
		            value = trim( getcurrentLine( file, start ) );
		        }
		        return value;
			}
            // get a number from a string
            // returns -1 on failure
            template<class T> static int getNumber( const std::basic_string<T>& numberStr )
            {
                std::basic_stringstream<T> numberStream;
                numberStream << numberStr;
                int number = -1;
                numberStream >> number;
                return number;
            }

            template<class T> static int getHexNumber( const std::basic_string<T>& numberStr )
            {
                std::basic_stringstream<T> numberStream;
                numberStream << numberStr;
                int number = -1;
                numberStream >> std::hex >> number;
                return number;
            }

			// Get current line from startPos
			template<class T> static std::basic_string<T> getcurrentLine( const std::basic_string<T>& s, 
				                                                          const typename std::basic_string<T>::size_type& startPos)
			{
				typedef std::basic_string<T> string_type;
				typename string_type::size_type endPos;
				string_type tmpS = EMPTY_STRING;
				if((endPos = s.find(WINDOWS_NEW_LINE, startPos)) != string_type::npos)
				{
					tmpS = s.substr(startPos, endPos - startPos);
				}
				else if((endPos = s.find(NEW_LINE, startPos))!=string_type::npos)
				{
					tmpS = s.substr(startPos, endPos - startPos);
				}
				else
				{
					tmpS = s.substr(startPos);
				}
				return tmpS;
			}

			// Trim s leading and trailing whitespace
			template<class T> static std::basic_string<T> trim( const std::basic_string<T>& s)
			{
				typedef std::basic_string<T> string_type;
				string_type tmpS = EMPTY_STRING;
				string_type regexString = TRIM_REGX;
				boost::basic_regex<T> tmpRegex(regexString.c_str());
				return boost::regex_replace(s, tmpRegex, tmpS.c_str());
			}

            DBOperator m_dbOperator;
            std::vector<unsigned char> m_fileContent;
            EMessageLibraryType m_libraryType;
            unsigned long m_libraryVersion;
    };

    class StisLibraryParser : public MessageLibraryParser
    {
        public:
            StisLibraryParser( const std::vector<unsigned char>& fileContent,                              
                               TA_Base_Core::EDataTypes conntype = TA_Base_Core::Tis_Cd );
            virtual unsigned long getLibraryVersion();
            virtual ~StisLibraryParser() {}
            // Parse stis library and save the library content to database, include the default adhoc attributes
            virtual void parseLibrary();
        private:
            void parseSTISAttributeString( const std::string& attributeString,
                                           TA_Base_Core::DisplayAttributes& stisAttributes,
                                           TA_Base_Core::LCDAttributesSTIS& stisLcdAttributes,
                                           TA_Base_Core::LEDAttributes& stisLedAttributes );
            TA_Base_Core::PredefinedMessage* parseSTISMessage( const std::wstring& stisMessageStr,
                                                               TA_Base_Core::ELibrarySection librarySection );
			std::wstring m_stisFile; // Be parsed stis library content
    };

    class TtisLibraryParser : public MessageLibraryParser
    {
        public:
            TtisLibraryParser( const std::vector<unsigned char>& fileContent,                               
                               TA_Base_Core::EDataTypes conntype = TA_Base_Core::Tis_Cd );
            void parseTTISMessageLibrary();
            virtual unsigned long getLibraryVersion();
            virtual ~TtisLibraryParser() {}
            virtual void parseLibrary();
			static std::string getComprisedFileSearchMark();
        private:
            TA_Base_Core::PredefinedMessage* parseTTISMessage( const std::string& ttisMessageStr,
                                                               TA_Base_Core::ELibrarySection librarySection );
			std::string m_ttisFile; // Be parsed ttis library content
		    static const std::string     TTIS_MESSAGE_MARK;
		    static const int             MAX_MESSAGE_LENGTH;
			static const std::string m_comprisedFileSearchMark;
    };   

    class ParserCreateFactory
    {
        public:
            static MessageLibraryParser* CreateParser( const std::vector<unsigned char>& fileContent,
                                                       EMessageLibraryType type,
                                                       TA_Base_Core::EDataTypes conntype = TA_Base_Core::Tis_Cd )
            {
				if( STATION_LIBRARY == type )
				{
                    return new StisLibraryParser( fileContent, conntype );
				}
				else
				{
					return new TtisLibraryParser(fileContent, conntype);
				}
            }          
    };
}
#endif