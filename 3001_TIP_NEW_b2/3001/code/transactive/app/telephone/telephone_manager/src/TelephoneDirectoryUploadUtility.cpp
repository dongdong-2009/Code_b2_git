/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_manager/src/TelephoneDirectoryUploadUtility.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This class will perform the upload of Telephone Directory data from an external
  * source (typically a file) into the database. This class will utilise the
  * TelephoneDirectoryLibrary class to perform the database manipulation functions.
  */

#include "app/telephone/telephone_manager/src/stdafx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"
#include "app/telephone/telephone_manager/src/TelephoneDirectoryUploadUtility.h"
#include "app/telephone/telephone_manager/src/TelephoneDirectoryUploadUtilityException.h"

#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibrary.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibraryException.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryData.h"
#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryEntryData.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>

using namespace std;

const unsigned short TelephoneDirectoryUploadUtility::MAX_BUFFER_SIZE = 200;
// TD 16047
const unsigned short TelephoneDirectoryUploadUtility::LINES_TO_SKIP = 2;
const unsigned short TelephoneDirectoryUploadUtility::LENGTH_OF_LOCATIONNAME = 3;
const unsigned short TelephoneDirectoryUploadUtility::LENGTH_OF_DIRECTORYNAME = 3;
const std::string TelephoneDirectoryUploadUtility::START_PROCESS_CHAR    ("EPAX");
// TD 16047

TelephoneDirectoryUploadUtility::TelephoneDirectoryUploadUtility()
    :m_SessionId("0")
{
    FUNCTION_ENTRY( "TelephoneDirectoryUploadUtility" );
    FUNCTION_EXIT;
}


TelephoneDirectoryUploadUtility::TelephoneDirectoryUploadUtility(const std::string& sessionId)
    :m_SessionId(sessionId)
{
    FUNCTION_ENTRY( "TelephoneDirectoryUploadUtility" );
    FUNCTION_EXIT;
}


TelephoneDirectoryUploadUtility::~TelephoneDirectoryUploadUtility()
{
    FUNCTION_ENTRY( "~TelephoneDirectoryUploadUtility" );

    // Need to close file handle
    // And clean up internal memory allocation
    if(m_directories.empty() == false || m_entries.empty() == false)
    {
        cleanUp();
    }

    FUNCTION_EXIT;
}


TelephoneDirectoryUploadUtility::TelephoneDirectoryUploadUtility(string& uploadFileName, const string& sessionId)
    : m_SessionId(sessionId)
{
    FUNCTION_ENTRY( "TelephoneDirectoryUploadUtility" );

    m_uploadFileName = uploadFileName;

    CFileException e;
    if( !m_FileHandle.Open( uploadFileName.c_str(), CFile::modeRead, &e ) )
    {
        string errMsg = "Error opening file ";
        errMsg += m_uploadFileName.c_str();

        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CFileException", "%s", e.m_cause);
        TA_THROW(TelephoneDirectoryUploadUtilityException(errMsg));
    }

    FUNCTION_EXIT;
}


void TelephoneDirectoryUploadUtility::setUploadFile(std::string& uploadFileName)
{
    FUNCTION_ENTRY( "setUploadFile" );

    m_uploadFileName = uploadFileName;
    openUploadFile();

    FUNCTION_EXIT;
}


void TelephoneDirectoryUploadUtility::openUploadFile()
{
    FUNCTION_ENTRY( "openUploadFile" );

    CFileException e;
    if( !m_FileHandle.Open( m_uploadFileName.c_str(), CFile::modeRead, &e ) )
    {
        string errMsg = "Error opening file ";
        errMsg += m_uploadFileName.c_str();

        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CFileException", "%s", e.m_cause);
        TA_THROW(TelephoneDirectoryUploadUtilityException(errMsg));
    }

}


bool TelephoneDirectoryUploadUtility::processUploadFromFile()
{
    FUNCTION_ENTRY( "processUploadFromFile" );

    if(parseUploadFile() == true)
    {
        FUNCTION_EXIT;
        return uploadDataToDatabase();
    }

    else
    {
        FUNCTION_EXIT;
        return false;
    }
}


bool TelephoneDirectoryUploadUtility::parseUploadFile()
{
    FUNCTION_ENTRY( "parseUploadFile" );

    int fileLength = m_FileHandle.GetLength();
    char pBuffer[MAX_BUFFER_SIZE];

    unsigned int index = 0;
    int ibufferRead = 0;
    bool bState = true;
    // TD 16047
    unsigned int lineNumber = 0;
    // TD 16047

    string tmpBuffer;
    string buffer;
    string recBuffer;
    // TD 16047
    string substituteBuffer;
    // TD 16047

    while(fileLength > 0)
    {
        ibufferRead = m_FileHandle.Read(&pBuffer, MAX_BUFFER_SIZE - buffer.size());
        if(ibufferRead == 0)
        {
            FUNCTION_EXIT;
            return false;
        }


        tmpBuffer = pBuffer;
        // TD 16047
        buffer = buffer + substituteBuffer + tmpBuffer.substr(0, ibufferRead);
        substituteBuffer = "";
        // TD 16047
        while(buffer.empty() == false)
        {
            index = buffer.find('\n');
            if(index != string::npos)
            {
                // TD 16047
                // skip LINES_TO_SKIP lines, now is set to 2, since the example file has 2 lines
                // unuseful information at the beginning
                if (lineNumber < LINES_TO_SKIP)
                {
                    lineNumber++;
                    buffer = buffer.substr(index+1, ibufferRead);
                    continue;
                }
                // since the example file generated by EPAX has too much information, and we only
                // want the information before the first blank line
                if (1 == index || 0 == index)
                {
                    m_FileHandle.Close();

                    FUNCTION_EXIT;
                    return true;

                }
                // TD 16047

                recBuffer = buffer.substr(0, index);

                bState = processRecord(recBuffer);
                if(bState == false)
                {
                    break;
                }


                // TD 16047
                lineNumber++;
                // TD 16047
                buffer = buffer.substr(index+1, ibufferRead);
            }
            else
            {
                // TD 16047
                // save the buffer
                substituteBuffer = "";
                substituteBuffer = buffer;
                buffer = "";
                // TD 16047
                break;
            }
        }
        fileLength -= ibufferRead;
    }

    // TD 16047
    if (buffer.empty() == true && substituteBuffer.empty() == false)
    {
        buffer = substituteBuffer;
    }

    // TD 16047

    // Clean up remaining stuff in the buffer
    if(buffer.empty() == false && bState == true)
    {
        while(buffer.empty() != true)
        {
            index = buffer.find('\n');
            if(index == string::npos || index == 0)
            {
                recBuffer = buffer;
            }

            else if(index < buffer.length())
                recBuffer = buffer.substr(0, index);

            bState = processRecord(recBuffer);
            if(bState == false)
            {
                break;
            }


            if(index == string::npos || index == 0)
            {
                buffer="";
            }

            else
                buffer = buffer.substr(index+1, ibufferRead);
        }
    }

    m_FileHandle.Close();

    if(bState == false)
    {
        cleanUp();

        FUNCTION_EXIT;
        return false;
    }

    FUNCTION_EXIT;
    return true;
}


bool TelephoneDirectoryUploadUtility::uploadDataToDatabase()
{
    FUNCTION_ENTRY( "uploadDataToDatabase" );

    try
    {
        if( TelephoneDirectoryLibrary::uploadDirectory(m_directories, m_entries) == false)
        // TD14164 ++
        {
            /*AfxMessageBox("Error loading content to database ");*/
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_620019);
        }
        // ++ TD14164
        else
        {
            // clean up the internal structures
            FUNCTION_EXIT;
            return true;
        }

    }
    catch(TelephoneDirectoryLibraryException& e)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TelephoneDirectoryLibraryException", e.what());
    }

    FUNCTION_EXIT;
    return false;
}


bool TelephoneDirectoryUploadUtility::processRecord(const string& input)
{
    FUNCTION_ENTRY( "processRecord" );

    // TD 16047
//    // Format is <number, last_name, first_name, directory_name, location>
//    TelephoneDirectoryEntryData* entryData = new TelephoneDirectoryEntryData();
//
//    unsigned int sIndex = 0;
//    unsigned int index = 0;
//    int tIndex = 0;
//
//    index = input.find('\t', sIndex);
//    if(index == string::npos)
//    {
//        delete entryData;
//        return false;
//    }
//    entryData->number = input.substr(0, index);
//    sIndex = index + 1;
//
//    string name;
//    index = input.find('\t', sIndex);
//    if(index == string::npos)
//    {
//        delete entryData;
//        return false;
//    }
//    name = input.substr(sIndex, index - sIndex);
//    sIndex = index + 1;
//
//    index = input.find('\t', sIndex);
//    if(index == string::npos)
//    {
//        delete entryData;
//        return false;
//    }
//    entryData->fullName = input.substr(sIndex, index - sIndex);
//    entryData->fullName += " " + name;
//    sIndex = index + 1;
//
//    index = input.find('\t', sIndex);
//    if(index == string::npos)
//    {
//        delete entryData;
//        return false;
//    }
//    entryData->directoryDataName = input.substr(sIndex, index - sIndex);
//    sIndex = index + 1;
//
//    index = input.find('\n', sIndex);
//    if(index == string::npos && sIndex > input.length())
//    {
//        delete entryData;
//        return false;
//    }
//
//    entryData->location = input.substr(sIndex, input.length() - sIndex - 1);
//
//    bool isDirExist = false;
//    for(unsigned int i = 0; i < m_directories.size(); i++)
//    {
//        if(m_directories[i]->name == entryData->directoryDataName)
//        {
//            isDirExist = true;
//            break;
//        }
//    }
//
//    if(false == isDirExist)
//    {
//        TelephoneDirectoryData* dirData = new TelephoneDirectoryData();
//        dirData->name = entryData->directoryDataName;
//        m_directories.push_back(dirData);
//    }
//
//    m_entries.push_back(entryData);


    // File Format <Node, Directory Number, Directory name, Directory First Name, ...>
    // entryData Format <number, last_name, first_name, directory_name, location>
    // So the relationship of File format and entryData format is:
    // 
    // entryData.number = file.Directory Number
    // entryData.last_name = file.Directory First Name
    // entryData.first_name = file.Directory name(second part). For example "KCD TCO1" is file.Directory name, and entryData.first_name is "TCO1"
    // entryData.directory_name = file.Directory name(first part). From above example, it is "KCD".
    // entryData.location = file.Node(some part). And the value should be same as entryData.directory_name.
    // entryData.fullname = file.Directory name + file.Directory First Name = entryData.directory_name + entryData.first_name + entryData.last_name

    TelephoneDirectoryEntryData* entryData = new TelephoneDirectoryEntryData();

    unsigned int sIndex = 0;
    unsigned int index = 0;
    int tIndex = 0;

    index = input.find('\t', sIndex);
    if(index == string::npos)
    {
        delete entryData;

        FUNCTION_EXIT;
        return false;
    }
    sIndex = index + 1;

    index = input.find('\t', sIndex);
    if (index == string::npos)
    {
        delete entryData;

        FUNCTION_EXIT;
        return false;
    }
    entryData->location = input.substr(index - LENGTH_OF_LOCATIONNAME , LENGTH_OF_LOCATIONNAME);
    sIndex = index + 1;

    index =input.find('\t', sIndex);
    if (index == string::npos)
    {
        delete entryData;

        FUNCTION_EXIT;
        return false;
    }
    entryData->number = input.substr(sIndex, index - sIndex);
    sIndex = index + 1;

    string name;
    index = input.find('\t', sIndex);
    if(index == string::npos)
    {
        delete entryData;

        FUNCTION_EXIT;
        return false;
    }
    name = input.substr(sIndex, index - sIndex);
    entryData->directoryDataName = input.substr(sIndex, LENGTH_OF_DIRECTORYNAME);
    sIndex = index + 1;

    string lastName;
    index = input.find('\t', sIndex);
    if(index == string::npos)
    {
        delete entryData;

        FUNCTION_EXIT;
        return false;
    }
    lastName = input.substr(sIndex, index - sIndex);
    entryData->fullName = name + " " + lastName;
    sIndex = index + 1;

    bool isDirExist = false;
    for(unsigned int i = 0; i < m_directories.size(); i++)
    {
        if(m_directories[i]->name == entryData->directoryDataName)
        {
            isDirExist = true;
            break;
        }
    }

    if(false == isDirExist)
    {
        TelephoneDirectoryData* dirData = new TelephoneDirectoryData();
        dirData->name = entryData->directoryDataName;
        m_directories.push_back(dirData);
    }

    m_entries.push_back(entryData);

    FUNCTION_EXIT;
    return true;
    // TD 16047
}


void TelephoneDirectoryUploadUtility::cleanUp()
{
    FUNCTION_ENTRY( "cleanUp" );

    unsigned int i;
    for(i = 0; i < m_directories.size(); i++)
    {
        delete(m_directories[i]);
    }
    m_directories.clear();

    for(i = 0; i < m_entries.size(); i++)
    {
        delete(m_entries[i]);
    }
    m_entries.clear();

    FUNCTION_EXIT;
}


