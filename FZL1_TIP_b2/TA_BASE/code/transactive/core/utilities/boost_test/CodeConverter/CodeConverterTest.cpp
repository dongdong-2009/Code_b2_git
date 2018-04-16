// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include "CodeConverter.h"
#include "TAAssert.h"
#include <openssl/sha.h>

using namespace TA_Base_Core;

BOOST_AUTO_TEST_SUITE(CodeConverTerTest)

BOOST_AUTO_TEST_CASE(test_CodeConverTer)
{
    std::wstring str = L"Hello, World!\n你好，世界！";
    std::string utf8 = CodeConverter::UnicodeToUTF8(str);
    std::wstring uncode = CodeConverter::UTF8ToUnicode(utf8);
    TA_Assert(str == uncode);
    std::string ansi = CodeConverter::UnicodeToANSI(uncode);
    std::string utf8_2 = CodeConverter::ANSIToUTF8(ansi);
    TA_Assert(utf8 == utf8_2);
    std::cout << ansi << std::endl;
}

BOOST_AUTO_TEST_CASE(test_SHA1)
{
    struct Local
    {
        static void GetHash(const std::string& data, std::string& results)
        {
            //unsigned char *origin = new unsigned char[data.length()];
            //strcpy((char*)origin,data.data());
            //SHA1(origin, strlen(data.data()), obuf);
            unsigned char obuf[SHA_DIGEST_LENGTH] = { 0 };
            std::string dataUTF8 = CodeConverter::toUTF8(data);
            unsigned char* origin = (unsigned char*) dataUTF8.c_str();

            SHA1(origin, dataUTF8.size(), obuf);

            std::stringstream stmCon;
            stmCon << std::hex;
            for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
            {
                stmCon << std::setw(2) <<std::setfill('0') << (int) (obuf[i]);
            }
            std::string outputString = stmCon.str();
            for(unsigned int i=0;i<outputString.length();i++)
            {
                outputString[i] = toupper(outputString[i]);
            }

            results = outputString;
        }
    };

    std::string str = "操作员/transactive";
    std::string sha;
    Local::GetHash(str, sha);
    std::cout << str << std::endl << sha << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
