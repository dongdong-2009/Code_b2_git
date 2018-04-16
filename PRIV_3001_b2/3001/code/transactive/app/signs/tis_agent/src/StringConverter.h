#ifndef STRINGCONVERTER_H_INCLUDE
#define STRINGCONVERTER_H_INCLUDE
#include <memory.h>
#include <string>
#include <vector>
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/signs_4669/TisManagerCorbaDef/src/ISTISManagerCorbaDef.h"

namespace TA_IRS_App
{
    class RawString
    {
        public:
            inline RawString()
            {
                m_data = 0;
                m_length = 0;
            }

            inline RawString( const char* p, int length )
            {
                if ( ( 0 != p ) && ( 0 != length ) )
                {
                    m_data = dup( p, length );
                    m_length = length;
                }
                else
                {
                    m_data = 0;
                    m_length = 0;
                }
            }

            inline RawString( const RawString& s )
            {
				m_length = 0;
                m_data = dup( s );
                if ( 0 != m_data )
                {
                    m_length = s.m_length;
                }
            }

            inline ~RawString()
            {
                free( this );
            }

            inline RawString& operator=( const RawString& s )
            {
                if ( (&s) != this )
                {
                    free( this );
                    m_data = 0;
                    m_length = 0;
                    m_data = dup( s );
                    if ( m_data )
                    {
                        m_length = s.m_length;
                    }
                }
                return *this;
            }
			
			inline char& operator[](const unsigned long index)
			{
				TA_ASSERT(index < m_length, "Exceed to array boundary");
				return m_data[index];
			}

            inline char* getData() const
            {
                return dup( m_data, m_length );
            }

            inline unsigned long getLength() const
            {
                return m_length;
            }

            static inline char* dup( const RawString& s )
            {
                if (  0 == s.m_length || 0 == s.m_data )
                {
                    return 0;
                }
                char* r = new char[s.m_length];
				memset(r, 0, s.m_length);
                memcpy( r, s.m_data, s.m_length );
                return r;
            }

            static inline char* dup( const char* l, unsigned long length )
            {
				if ( 0 == l || 0 == length ) 
                {
                    return 0;
                }
                char* r = new char[length];
				memset(r, 0, length);
                memcpy( r, l, length );
                return r;
            }
            static inline void free( RawString* r )
            {
                if ( ( 0 != r->m_data) && (0 != r->m_length ) )
                {
                    delete[] ( r->m_data );
					r->m_data = 0;
					r->m_length = 0;
                }
            }

        private:
            char* m_data;
            unsigned long m_length;
    };


    class StringConverter
    {
        public:
            static std::wstring convertUtf16LeToUtf32( const RawString& rawString );
			static RawString convertUtf32ToUtf16Le(const std::wstring& ws);
            static std::string convertUtf32ToUtf8( std::wstring& ws );
            static std::wstring vectorCharToWstring( const std::vector<unsigned char>& charVector );
	        static std::string vectorCharToString(const std::vector<unsigned char>& charVector);
			static std::string convertUtf16leToUtf8( const std::wstring& src ); 
			// Only convert ascii character string
            static std::string wstringToString( const std::wstring& ws );
			// Save wstring(if utf32 first convert it to utf16) to vector
			static std::vector<unsigned char> stringToVectorChar(const std::wstring& ws);
			static std::vector<unsigned char> stringToVectorChar(const std::string& s);			
			
			static TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType vectorCharToCorbaCharSequence(const std::vector<unsigned char>& charVector);
			static std::vector<unsigned char> corbaCharSequenceToVectorChar(const TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType& charSequence);
			
    };
}
#endif