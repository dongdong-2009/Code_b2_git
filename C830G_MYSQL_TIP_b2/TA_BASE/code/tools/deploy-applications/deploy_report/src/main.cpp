// start_monitor_agent.cpp : Defines the entry point for the console application.
//

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_iterator.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>

#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>


static void getFileString( const std::string& file_name, std::string& str )
{
    std::ifstream ifs( file_name.c_str() );
    
    if ( !ifs )
    {
        std::cout << "Error: can not open file: " << file_name << std::endl;
        return;
    }
    
    // get length of file:
    ifs.seekg ( 0, std::ios::end );
    long length = ifs.tellg();
    ifs.seekg ( 0, std::ios::beg );
    
    char *buffer = new char[length];
    
    ifs.read( buffer, length );
    
    str = buffer; // copy, TODO: can remove this copy ?
    
    // delete temporary buffer
    delete [] buffer;
    
    ifs.close();
}


class AgentVersion
{
public:

    AgentVersion()
    {
    }

    AgentVersion( const std::string& component )
        : m_component( component ),
          m_release_number( 11, '#' ),
          m_build_time( 20, '#' ),
          m_component_version( 11, '#' ),
          m_permission( 10, '#' ),
          m_user( 11, '#' ),
          m_group( 5, '#' )
    {
    }

    AgentVersion( const std::string& release_number, const std::string& build_time, const std::string& component, const std::string& component_version )
        : m_release_number( release_number ),
          m_build_time( build_time ),
          m_component( component ),
          m_component_version( component_version )
    {
    }

    bool operator == ( const AgentVersion& rhs ) const
    {
        return m_release_number == rhs.m_release_number &&
               m_build_time == rhs.m_build_time &&
               m_component == rhs.m_component &&
               m_component_version == rhs.m_component_version &&
               m_permission == rhs.m_permission &&
               m_user == rhs.m_user &&
               m_group == rhs.m_group;
   }

    bool operator != ( const AgentVersion& rhs ) const
    {
        return !( *this == rhs );
    }

    void setDifference( const AgentVersion& rhs )
    {
        if ( m_release_number != rhs.m_release_number )
        {
            m_release_number += "*";
        }

        if ( m_build_time != rhs.m_build_time )
        {
            m_build_time += "*";
        }

        if ( m_component_version != rhs.m_component_version )
        {
            m_component_version += "*";
        }

        if ( m_permission != rhs.m_permission )
        {
            m_permission += "*";
        }

        if ( m_user != rhs.m_user )
        {
            m_user += "*";
        }

        if ( m_group != rhs.m_group )
        {
            m_group += "*";
        }
    }

    void output( std::ostream& os, size_t max_length ) const
    {
        os << std::setw( max_length ) << m_component;
        os << std::setw( max_length ) << m_release_number;
        os << std::setw( max_length ) << m_build_time;
        os << std::setw( max_length ) << m_component_version;
        os << std::setw( max_length ) << m_permission;
        os << std::setw( max_length ) << m_user;
        os << std::setw( max_length ) << m_group;
        os << std::endl;
    }

    size_t getMaxLength() const
    {
        size_t max_length = m_component.size();
        max_length = std::max( max_length, m_release_number.size() );
        max_length = std::max( max_length, m_build_time.size() );
        max_length = std::max( max_length, m_component_version.size() );
        max_length = std::max( max_length, m_permission.size() );
        max_length = std::max( max_length, m_user.size() );
        max_length = std::max( max_length, m_group.size() );
        return max_length;
    }

public:
    
    std::string m_release_number;
    std::string m_build_time;
    std::string m_component;
    std::string m_component_version;

    // permission
    std::string m_permission;
    std::string m_user;
    std::string m_group;
};

typedef std::map<std::string, AgentVersion> AgentVersionMap; // <component, AgentVersion>


class StationVersion;
typedef boost::shared_ptr<StationVersion> StationVersionPtr;
typedef std::vector<StationVersionPtr> StationVersionPtrList;

class StationVersion
{
public:

    StationVersion(){}

    StationVersion( const std::string& file )
        : m_file( file )
    {
        parse();

        getStationFromFileName();
    }

    void getStationFromFileName()
    {
        std::transform( m_file.begin(), m_file.end(), m_file.begin(), std::ptr_fun( ::tolower ) );

        size_t pos1 = m_file.find( "agents-" );
        size_t pos2 = m_file.find( ".csv" );

        if ( pos1 == std::string::npos || pos2 == std::string::npos )
        {
            if ( std::string::npos == m_file.find( "master") )
            {
                m_station = "unknown";
                std::cout << "Error: can not parse station name from file name: " << m_file << std::endl;
            }
            else
            {
                m_station = "master";
            }
        }
        else
        {
            m_station = m_file.substr( pos1 + 7, pos2 - pos1 - 7 ); // 7 = "agents-"
        }
    }

    void parse()
    {
        std::string str;
        getFileString( m_file, str );

        // Release Number:         01.27.13.80
        // Build Time:             Feb 22 2010 15:23:27
        // Component:              ECS Agent
        // Component Version:      01.27.13.80
        static boost::regex e
            (
                "\\s*"
                "Release Number:\\s*(.*?)\\s*"        // match 1: Release Number
                "Build Time:\\s*(.*?)\\s*"            // match 2: Build Time
                "Component:\\s*(.*?)\\s*"             // match 3: Component
                "Component Version:\\s*(.*?)\\s*"     // match 4: Component Version
                "Component Library Information:\\s*(.*?)\\s*"
                "Other Component Information:\\s*(.*?)\\s*"
            );

        boost::smatch m;
        
        boost::sregex_iterator it( str.begin(), str.end(), e );
        boost::sregex_iterator end;

        for ( ; it != end; ++it )
        {
            const std::string& release_number = it->str(1);
            const std::string& build_time = it->str(2);
            const std::string& component = it->str(3);
            const std::string& component_version = it->str(4);
            
            addAgentVersion( AgentVersion( release_number, build_time, component, component_version ) );
        }

        {
            // lrwxrwxrwx   1 root     other         41 Mar  2 19:32 MmsAgent -> /u02/deployment/1400/bin/MmsAgent_Stubbed
            static boost::regex e
                (
                    "([\\l\\-]{10})\\s+"                                        // match 1: permission
                    "[\\d]+\\s+"                                                // 1
                    "([^\\s]+)\\s+"                                             // match 2: user
                    "([^\\s]+)\\s+"                                             // match 3: group
                    "[\\d]+\\s+"                                                // size
                    "(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)\\s+"     // match 4: month
                    "[\\d]{1,2}\\s+"                                            // day
                    "([\\d]{4}|[\\d]{2}:[\\d]{2})\\s+"                          // match 5: time or year
                    "([^\\s]+)\\s+"                                             // match 6: component name
                    "(->[^\\n]+)?"                                              // link
                );
        
            boost::smatch m;
        
            boost::sregex_iterator it( str.begin(), str.end(), e );
            boost::sregex_iterator end;
        
            for ( ; it != end; ++it )
            {
                const std::string& permission = it->str(1);
                const std::string& user = it->str(2);
                const std::string& group = it->str(3);
                const std::string& component = it->str(6);

                addPermission( component, permission, user, group );
            }
        }
    }

    void addAgentVersion( const AgentVersion& agentVersion )
    {
        if ( m_agentVersions.find( agentVersion.m_component ) == m_agentVersions.end() )
        {
            std::string key = agentVersion.m_component;
            key.erase( std::remove( key.begin(), key.end(), ' ' ), key.end() ); // erase spaces
            std::transform( key.begin(), key.end(), key.begin(), std::ptr_fun( ::tolower ) ); // to lower
            m_agentVersions[ key ] = agentVersion;
        }
    }

    void addPermission( const std::string& compment, const std::string& permission, const std::string& user, const std::string& group )
    {
        std::string key = compment;
        std::transform( key.begin(), key.end(), key.begin(), std::ptr_fun( ::tolower ) );

        AgentVersionMap::iterator findIter = m_agentVersions.find( key );

        if ( findIter != m_agentVersions.end() )
        {
            findIter->second.m_permission = permission;
            findIter->second.m_user = user;
            findIter->second.m_group = group;
        }
        else
        {
            // ignore not managed agent
        }
    }

    void compare( StationVersionPtr& rhs, StationVersionPtrList& different )
    {
        StationVersionPtr newStation( new StationVersion() );

        newStation->setStation( rhs->getStation() );

        for ( AgentVersionMap::iterator iter = m_agentVersions.begin(); iter != m_agentVersions.end(); ++iter )
        {
            AgentVersionMap& rhsAgentVersion = rhs->getAgentVersions();
            
            AgentVersionMap::iterator findIter = rhsAgentVersion.find( iter->first );
            
            if ( findIter != rhsAgentVersion.end() )
            {
                if ( findIter->second != iter->second )
                {
                    findIter->second.setDifference( iter->second );

                    newStation->addAgentVersion( findIter->second );
                }
            }
            else
            {
                AgentVersion empty( iter->first );

                newStation->addAgentVersion( empty );
            }
        }

        if ( false == newStation->getAgentVersions().empty() )
        {
            different.push_back( newStation );
        }
    }

    AgentVersionMap& getAgentVersions()
    {
        return m_agentVersions;
    }

    const std::string& getStation() const
    {
        return m_station;
    }

    void setStation( const std::string& station )
    {
        m_station = station;
    }

    const std::string& getFileName() const
    {
        return m_file;
    }

    void output( std::ostream& os, size_t max_length ) const
    {
        for ( AgentVersionMap::const_iterator it = m_agentVersions.begin(); it != m_agentVersions.end(); ++it )
        {
            os << std::setw( max_length ) << m_station;

            it->second.output( os, max_length );
        }
    }

    size_t getMaxLength() const
    {
        size_t max_length = 0;

        for ( AgentVersionMap::const_iterator it = m_agentVersions.begin(); it != m_agentVersions.end(); ++it )
        {
            max_length = std::max( max_length, it->second.getMaxLength() );
        }

        return max_length;
    }

public:

    AgentVersionMap m_agentVersions;
    std::string m_station;
    std::string m_file;
};

typedef std::vector< StationVersion > StationVersionList;


class CompareRunner
{
public:

    void compare()
    {
        if ( !m_master || !m_other )
        {
            std::cout << "Error: pointer is empty." << std::endl;
        }

        m_master->compare( m_other, m_differents );
    }

    void setMaster( const StationVersionPtr& master )
    {
        m_master = master;
    }

    void setOther( const StationVersionPtr& other )
    {
        m_other = other;
    }

    StationVersionPtrList& getDifferences()
    {
        return m_differents;
    }

public:

    StationVersionPtrList m_differents;
    StationVersionPtr m_other;
    StationVersionPtr m_master;
};

std::ostream& operator << ( std::ostream& os, const StationVersionPtrList& versionList )
{
    // output to ostream
    size_t max_length = 17; // "Component Version";

    {
        for ( StationVersionPtrList::const_iterator it = versionList.begin(); it != versionList.end(); ++it )
        {
            max_length = std::max( max_length, (*it)->getMaxLength() );
        }
    }

    max_length += 2;

    os.setf( std::ios_base::left );
    os << std::setw( max_length ) << "Station";
    os << std::setw( max_length ) << "Component";
    os << std::setw( max_length ) << "Release Number";
    os << std::setw( max_length ) << "Build Time";
    os << std::setw( max_length ) << "Component Version";
    os << std::setw( max_length ) << "Permission";
    os << std::setw( max_length ) << "User";
    os << std::setw( max_length ) << "Group";
    
    os << std::endl;
    os << std::string( max_length * 8, '-' );
    os << std::endl;

    for ( StationVersionPtrList::const_iterator it = versionList.begin(); it != versionList.end(); ++it )
    {
        (*it)->output( os, max_length );
        
        os << std::endl;
    }

    return os;
}


int main( int argc, char* argv[] )
{
    const std::string master_name = "master.csv";

    char currentDirectory[ 512 ];
    GetCurrentDirectory( 512, currentDirectory );
    std::string current_folder( currentDirectory );

    const std::string report_folder = std::string( currentDirectory ) + "\\report\\";

    std::string str;
    getFileString( current_folder + "\\dest.conf", str );

    std::ofstream ofs( std::string( report_folder + "report.csv" ).c_str() );

    static boost::regex e
        (
            "([^\\s]+)\\s+"   // match 1: host name: occa, sms-dbg
            "([^\\s]+)\\s+"   // match 2: /u02/deployment
            "([^\\s]+)\\n?"   // match 3: station name: OCC, KCD, DBG
        );
    
    boost::smatch m;
    
    boost::sregex_iterator it( str.begin(), str.end(), e );
    boost::sregex_iterator end;
    
    std::vector<std::string> host_names;

    for ( ; it != end; ++it )
    {
        const std::string& host_name = it->str(1);
        host_names.push_back( host_name );
    }

    CompareRunner runner;
    
    StationVersionPtr master( new StationVersion( report_folder + master_name ) );

    if ( "master" != master->getStation() )
    {
        ofs << "Error: can not open master file." << std::endl;

        system( "pause" );
        return 0;
    }
    
    runner.setMaster( master );
    
    for ( std::vector<std::string>::iterator hostIt = host_names.begin(); hostIt != host_names.end(); ++hostIt )
    {
        StationVersionPtr other( new StationVersion( report_folder + "Agents-" + *hostIt + ".csv" ) );

        if ( "unknown" == other->getStation() )
        {
            ofs << "Error: can not open file: " << other->getFileName() << std::endl;
        }
        
        runner.setOther( other );
        
        runner.compare();
    }


    StationVersionPtrList& differences = runner.getDifferences();

    if ( false == differences.empty() )
    {
        ofs << differences;

        std::cout << differences;
    }
    else
    {
        ofs << "OK" << std::endl;

        std::cout << "OK" << std::endl;
    }

    system( "pause" );

    return 0;
}
