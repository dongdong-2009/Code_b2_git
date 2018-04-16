//
//

#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_iterator.hpp>

#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <stdio.h>


static bool getFileString( const std::string& file_name, std::string& str )
{
    std::ifstream ifs( file_name.c_str() );
    
    if ( !ifs )
    {
        std::cout << "Error: can not open file: " << file_name << std::endl;
        return false;
    }
    
    // get length of file:
    ifs.seekg ( 0, std::ios::end );
    long length = ifs.tellg();
    ifs.seekg ( 0, std::ios::beg );
    
    char *buffer = new char[length];
    
    ifs.read( buffer, length );
    
    str = buffer; // copy, TODO: can remove this copy ?
    
    // delete temporary buffer
    delete[] buffer;
    
    ifs.close();

    return true;
}


// should be named AbstructFolder, but IFolder is a shorter name
class IFolder;
typedef boost::shared_ptr<IFolder> IFolderPtr;
typedef std::vector<IFolderPtr> IFolderPtrList;
typedef std::map<std::string, IFolderPtr> IFolderPtrMap;

class IFolder
{
public:

    class Item
    {
    public:

        Item( const std::string& details )
            : m_details( details )
        {
            //2 drwxr-xr-x  10 root     sys          512 Feb 27  2007 kernel
            //0 -r--------   1 transactive scada          0 Feb 18 02:33 gwindows
            static boost::regex e
                (
                    "(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)\\s*"  // match 1: month
                    "[\\d]{1,2}\\s*"                                         // day
                    "([\\d]{4}|[\\d]{2}:[\\d]{2})\\s*"                       // match 2: time or year
                    "([^\\n]+)\\s*$"                                         // match 3: name
                );

            boost::smatch m;

            if ( boost::regex_search( details, m, e ) )
            {
                m_name = m[3];
            }

            // link name
            size_t pos = m_name.find( " -> " );

            if ( pos != std::string::npos )
            {
                m_name = m_name.substr( 0, pos );
            }
        }

        const std::string& getDetails() const
        {
            return m_details;
        }

        const std::string& getName() const
        {
            return m_name;
        }

        bool isHidden() const
        {
            if ( m_name.empty() )
            {
                std::cout << "Error: empty item." << std::endl;
                return true;
            }

            return '.' == m_name[ 0 ];
        }

    private:

        std::string m_details;
        std::string m_name;
    };

    typedef boost::shared_ptr<Item> ItemPtr;
    typedef std::map< std::string, ItemPtr > ItemPtrMap;

public:

    IFolder( const std::string& full_name )
        : m_full_name( full_name )
    {
        m_name = getNameByFullName( m_full_name );
    }

    IFolder( const std::string& full_name, const std::string& items )
        : m_full_name( full_name )
    {
        m_name = getNameByFullName( m_full_name );

        if ( false == items.empty() )
        {
            static boost::regex e
                (
                    "(.*?)\\n"
                );

            boost::sregex_iterator it( items.begin(), items.end(), e );
            static boost::sregex_iterator end;

            for ( ; it != end; ++it )
            {
                const std::string& str = it->str( 1 );
                ItemPtr item( new Item( str ) );

                if ( false == item->isHidden() )
                {
                    m_items.insert( ItemPtrMap::value_type( item->getName(), item ) );
                }
            }
        }
        else
        {
            std::cout << "Error: there isn't items in folder: " << m_full_name << std::endl;
        }
    }

    virtual ~IFolder() {};

public:

    void addItem( const ItemPtr& other )
    {
        ItemPtrMap::iterator findIter = m_items.find( other->getName() );

        if ( findIter == m_items.end() )
        {
            m_items.insert( ItemPtrMap::value_type( other->getName(), other ) );
        }
    }

    const ItemPtrMap& getItems() const
    {
        return m_items;
    }

    ItemPtrMap& getItems()
    {
        return m_items;
    }

    const std::string& getName() const
    {
        return m_name;
    }

    const std::string& getFullName() const
    {
        return m_full_name;
    }

    virtual void output( std::ostream& os ) const = 0;
    virtual void compare( const IFolderPtr& rhs, IFolderPtrList& result ) const = 0;

private:

    std::string getNameByFullName( const std::string& full_name )
    {
        size_t pos = m_full_name.find_last_of( "/" );

        if ( pos != std::string::npos )
        {
            return m_full_name.substr( pos + 1 );
        }
        else
        {
            return m_full_name;
        }
    }

private:

    std::string m_name;
    std::string m_full_name;
    ItemPtrMap m_items;
};


std::ostream& operator << ( std::ostream& os, const IFolderPtr& rhs )
{
    rhs->output( os );
    return os;
}

std::ostream& operator << ( std::ostream& os, const IFolderPtrList& rhs )
{
    for ( IFolderPtrList::const_iterator it = rhs.begin(); it != rhs.end(); ++it )
    {
        os << ( *it );
    }

    return os;
}


class TreeFolder;
typedef boost::shared_ptr<TreeFolder> TreeFolderPtr;
typedef std::map<std::string, TreeFolderPtr> TreeFolderPtrMap;

class TreeFolder : public IFolder
{
public:

    TreeFolder( const std::string& full_name )
        : IFolder( full_name )
    {
    }

    TreeFolder( const std::string& full_name, const std::string& items )
        : IFolder( full_name, items )
    {
    }

    void addSubFolder( TreeFolderPtr sub )
    {
        m_sub_folders.insert( TreeFolderPtrMap::value_type( sub->getName(), sub ) );
    }

    TreeFolder* getParentFolder( const std::string& full_name )
    {
        if ( std::string::npos != full_name.find( "/." ) )
        {
            return NULL;
        }

        size_t pos = full_name.find( "/" );

        if ( pos != std::string::npos )
        {
            size_t pos2 = full_name.find( "/", pos + 1 );

            if ( pos2 != std::string::npos )
            {
                std::string topFolder = full_name.substr( pos + 1, pos2 - pos -1 );

                TreeFolderPtrMap::iterator findIter = m_sub_folders.find( topFolder );

                if ( findIter != m_sub_folders.end() )
                {
                    std::string tailFolder = full_name.substr( pos + 1 );

                    return findIter->second->getParentFolder( tailFolder );
                }
                else
                {
                    std::cout << "Error: can not find out folder: " << full_name << std::endl;
                    return NULL;
                }
            }
            else
            {
                return this;
            }
        }
        else
        {
            std::cout << "Error: this is root folder, no parent folder." << std::endl;
            return NULL;
        }
    }

    virtual void compare( const IFolderPtr& rhs, IFolderPtrList& result ) const
    {
        // compare items

        IFolderPtr newFolder;

        const ItemPtrMap& rhsItems = rhs->getItems();

        for ( ItemPtrMap::const_iterator itemIt = getItems().begin(); itemIt != getItems().end(); ++itemIt )
        {
            ItemPtrMap::const_iterator findIter = rhsItems.find( itemIt->first );

            if ( findIter == rhsItems.end() )
            {
                if ( !newFolder )
                {
                    newFolder.reset( new TreeFolder( getFullName() ) );
                }

                newFolder->addItem( itemIt->second );
            }
        }

        if ( newFolder && false == newFolder->getItems().empty() )
        {
            result.push_back( newFolder );
        }

        // compare sub folders

        TreeFolderPtr rhsFolder = boost::dynamic_pointer_cast<TreeFolder>( rhs );

        if ( !rhsFolder )
        {
            std::cout << "Error: failed to type cast rhs folder." << std::endl;
            return;
        }

        const TreeFolderPtrMap& rhsSubFolders = rhsFolder->getSubFolders();

        for ( TreeFolderPtrMap::const_iterator subIt = m_sub_folders.begin(); subIt != m_sub_folders.end(); ++subIt )
        {
            TreeFolderPtrMap::const_iterator findIter = rhsSubFolders.find( subIt->first );

            if ( findIter != rhsSubFolders.end() )
            {
                subIt->second->compare( findIter->second, result );
            }
            else
            {
                IFolderPtr newFolder( new TreeFolder( subIt->second->getFullName() ) );

                result.push_back( newFolder );
            }
        }
    }

    virtual void output( std::ostream& os ) const
    {
        {   // output current folder
            os << getFullName() << ":" << std::endl;

            for ( ItemPtrMap::const_iterator it = getItems().begin(); it != getItems().end(); ++it )
            {
                os << it->second->getDetails() << std::endl;
            }

            os << std::endl;
        }

        // output sub folders
        for ( TreeFolderPtrMap::const_iterator it = m_sub_folders.begin(); it != m_sub_folders.end(); ++it )
        {
            it->second->output( os );
        }
    }

    TreeFolderPtrMap& getSubFolders()
    {
        return m_sub_folders;
    }

    const TreeFolderPtrMap& getSubFolders() const
    {
        return m_sub_folders;
    }

private:

    TreeFolderPtrMap m_sub_folders;
};


class SimpleFolder : public IFolder
{
public:

    SimpleFolder( const std::string& full_name )
        : IFolder( full_name )
    {
    }

    SimpleFolder( const std::string& full_name, const std::string& items )
        : IFolder( full_name, items )
    {
    }

    virtual void compare( const IFolderPtr& rhs, IFolderPtrList& result ) const
    {
        IFolderPtr newFolder;

        ItemPtrMap& rhsItems = rhs->getItems();

        for ( ItemPtrMap::const_iterator itemIt = getItems().begin(); itemIt != getItems().end(); ++itemIt )
        {
            ItemPtrMap::const_iterator findIter = rhsItems.find( itemIt->first );

            if ( findIter == rhsItems.end() )
            {
                if ( !newFolder )
                {
                    newFolder.reset( new SimpleFolder( getFullName() ) );
                }

                newFolder->addItem( itemIt->second );
            }
        }

        if ( newFolder && false == newFolder->getItems().empty() )
        {
            result.push_back( newFolder );
        }
    }

    virtual void output( std::ostream& os ) const
    {
        const ItemPtrMap& items = getItems();

        os << getFullName() << ":" << std::endl;
        os << "total " << items.size() << std::endl;

        for ( ItemPtrMap::const_iterator it = items.begin(); it != items.end(); ++it )
        {
            os << it->second->getDetails() << std::endl;
        }

        os << std::endl;
    }
};

typedef boost::shared_ptr<SimpleFolder> SimpleFolderPtr;
typedef std::map<std::string, SimpleFolderPtr> SimpleFolderPtrMap;


class IStationFolder
{
public:

    IStationFolder( const std::string& input_folder, const std::string& output_folder, const std::string& file_name )
        : m_file_name( file_name ),
          m_input_folder( input_folder ),
          m_output_folder( output_folder )
    {
    }

    const std::string& getFileName() const
    {
        return m_file_name;
    }

    const std::string& getInputFolder() const
    {
        return m_input_folder;
    }

    const std::string& getOutputFolder() const
    {
        return m_output_folder;
    }

    virtual ~IStationFolder() {};

    virtual void output( std::ostream& os ) const = 0;

public:

    std::string m_file_name;
    std::string m_input_folder;
    std::string m_output_folder;
};

typedef boost::shared_ptr<IStationFolder> IStationFolderPtr;


class StationTreeFolder: public IStationFolder
{
public:

    StationTreeFolder( const std::string& input_folder, const std::string& output_folder, const std::string& file_name )
        : IStationFolder( input_folder, output_folder, file_name )
    {
        parse();
    }

    void parse()
    {
        std::string str;

        if ( false == getFileString( m_input_folder + "/" + m_file_name, str ) )
        {
            return;
        }

        static boost::regex e
            (
                "^\\s*(\\.[^\\n]*):\\s*"    // match 1: folder name
                "total\\s*[\\d]+\\n"        //
                "((\\s*[\\d]+.*?\\n)+)"     // match 2: items
            );

        boost::sregex_iterator it( str.begin(), str.end(), e );
        boost::sregex_iterator end;

        if ( it != end )
        {
            m_root.reset( new TreeFolder( it->str( 1 ), it->str( 2 ) ) );

            ++it;
        }

        if ( !m_root )
        {
            std::cout << "Error: can not create root folder." << std::endl;
            return;
        }

        for ( ; it != end; ++it )
        {
            const std::string& folderName = it->str( 1 );

            // filter hidden folder
            if ( std::string::npos == folderName.find( "/.") )
            {
                TreeFolderPtr newFolder( new TreeFolder( it->str( 1 ), it->str( 2 ) ) );

                TreeFolder* parent = m_root->getParentFolder( it->str( 1 ) );

                if ( parent != NULL )
                {
                    parent->addSubFolder( newFolder );
                }
                else
                {
                    // the parent folder is a hidden folder, ignore it
                }
            }
        }

        std::cout << "Parse complete: " << m_input_folder << "/" << m_file_name << std::endl;
    }

    TreeFolderPtr getRoot() const
    {
        return m_root;
    }

    virtual void output( std::ostream& os ) const
    {
        m_root->output( os );
    }

private:

    TreeFolderPtr m_root;
};

typedef boost::shared_ptr<StationTreeFolder> StationTreeFolderPtr;


class StationSimpleFolder : public IStationFolder
{
public:

    StationSimpleFolder( const std::string& input_folder, const std::string& output_folder, const std::string& file_name )
        : IStationFolder( input_folder, output_folder, file_name )
    {
        parse();
    }

    void parse()
    {
        std::string str;

        if ( false == getFileString( m_input_folder + "/" + m_file_name, str ) )
        {
            return;
        }

        static boost::regex e
            (
                "^\\s*(\\.[^\\n]*):\\s*"    // match 1: folder name
                "total\\s*[\\d]+\\n"        //
                "((\\s*[\\d]+.*?\\n)+)"     // match 2: items
            );

        boost::sregex_iterator it( str.begin(), str.end(), e );
        boost::sregex_iterator end;

        for ( ; it != end; ++it )
        {
            const std::string& folderName = it->str( 1 );

            // filter hidden folder
            if ( std::string::npos == folderName.find( "/.") )
            {
                SimpleFolderPtr newFolder( new SimpleFolder( it->str( 1 ), it->str( 2 ) ) );

                m_folders.insert( SimpleFolderPtrMap::value_type( newFolder->getFullName(), newFolder ) );
            }
        }

        std::cout << "Parse complete: " << m_input_folder << "/" << m_file_name << std::endl;
    }

    SimpleFolderPtrMap& getFolders()
    {
        return m_folders;
    }

    SimpleFolderPtr getFolderByName( const std::string& name ) const
    {
        SimpleFolderPtrMap::const_iterator findIter = m_folders.find( name );

        if ( findIter != m_folders.end() )
        {
            return findIter->second;
        }
        else
        {
            return SimpleFolderPtr();
        }
    }

    virtual void output( std::ostream& os ) const
    {
        for ( SimpleFolderPtrMap::const_iterator it = m_folders.begin(); it != m_folders.end(); ++it )
        {
            it->second->output( os );
        }
    }

private:

    SimpleFolderPtrMap m_folders;
};

typedef boost::shared_ptr<StationSimpleFolder> StationSimpleFolderPtr;


class ICompareRunner
{
public:

    virtual ~ICompareRunner() {};

    void setMaster( IStationFolderPtr master )
    {
        m_master = master;
    }

    void setOther( IStationFolderPtr other  )
    {
        m_other = other;
    }

    void output( const std::string& file_name, IFolderPtrList folder_list )
    {
        std::ofstream ofs( file_name.c_str() );

        if ( !ofs )
        {
            std::cout << "Error: can not create file: " << file_name << std::endl;
        }
        else
        {
            ofs << folder_list;
        }
    }

    virtual void compare() = 0;

public:

    IStationFolderPtr m_other;
    IStationFolderPtr m_master;
};

typedef boost::shared_ptr<ICompareRunner> ICompareRunnerPtr;


class StationTreeFolderCompareRunner : public ICompareRunner
{
    virtual void compare()
    {
        IFolderPtrList to_be_added;
        IFolderPtrList to_be_deleted;

        if ( !m_master || !m_other )
        {
            std::cout << "Error: empty pointer." << std::endl;
            return;
        }

        StationTreeFolderPtr master = boost::dynamic_pointer_cast<StationTreeFolder>( m_master );
        if ( !master )
        {
            std::cout << "Error: master folder pointer type cast failed." << std::endl;
            return;
        }

        StationTreeFolderPtr other = boost::dynamic_pointer_cast<StationTreeFolder>( m_other );
        if ( !other )
        {
            std::cout << "Error: other folder pointer type cast failed." << std::endl;
            return;
        }

        master->getRoot()->compare( other->getRoot(), to_be_added );
        other->getRoot()->compare( master->getRoot(), to_be_deleted );

        output( m_other->getOutputFolder() + "/" + m_other->getFileName() + ".to_be_added", to_be_added );
        output( m_other->getOutputFolder() + "/" + m_other->getFileName() + ".to_be_deleted", to_be_deleted );

        std::cout << "to be deleted: " << to_be_deleted.size() << ", " << "to be added: " << to_be_added.size() << std::endl << std::endl;
    }
};


class StationSimpleFolderCompareRunner : public ICompareRunner
{
public:

    virtual void compare()
    {
        IFolderPtrList to_be_added;
        IFolderPtrList to_be_deleted;

        if ( !m_master || !m_other )
        {
            std::cout << "Error: empty pointer." << std::endl;
            return;
        }

        StationSimpleFolderPtr master = boost::dynamic_pointer_cast<StationSimpleFolder>( m_master );
        if ( !master )
        {
            std::cout << "Error: master folder pointer type cast failed." << std::endl;
            return;
        }

        StationSimpleFolderPtr other = boost::dynamic_pointer_cast<StationSimpleFolder>( m_other );
        if ( !other )
        {
            std::cout << "Error: other folder pointer type cast failed." << std::endl;
            return;
        }

        SimpleFolderPtrMap& masterFolders = master->getFolders();

        for ( SimpleFolderPtrMap::const_iterator it = masterFolders.begin(); it != masterFolders.end(); ++it )
        {
            SimpleFolderPtr masterFolder = it->second;
            SimpleFolderPtr otherFolder = other->getFolderByName( it->first );

            if ( !otherFolder )
            {
                to_be_added.push_back( masterFolder );
            }
            else
            {
                masterFolder->compare( otherFolder, to_be_added );
                otherFolder->compare( masterFolder, to_be_deleted );
            }
        }

        output( m_other->getOutputFolder() + "/" + m_other->getFileName() + ".to_be_added", to_be_added );
        output( m_other->getOutputFolder() + "/" + m_other->getFileName() + ".to_be_deleted", to_be_deleted );

        std::cout << "to be deleted: " << to_be_deleted.size() << ", " << "to be added: " << to_be_added.size() << std::endl << std::endl;
    }
};

typedef boost::shared_ptr<StationSimpleFolderCompareRunner> StationSimpleFolderCompareRunnerPtr;


class IFactory
{
public:

    virtual ICompareRunnerPtr createICompareRunnerPtr() = 0;
    virtual IStationFolderPtr createIStationFolderPtr( const std::string& input_folder, const std::string& output_folder, const std::string& file_name ) = 0;
};

typedef boost::shared_ptr<IFactory> IFactoryPtr;


class SimpleFactory : public IFactory
{
public:

    ICompareRunnerPtr createICompareRunnerPtr()
    {
        return ICompareRunnerPtr( new StationSimpleFolderCompareRunner() );
    }

    IStationFolderPtr createIStationFolderPtr( const std::string& input_folder, const std::string& output_folder, const std::string& file_name )
    {
        return IStationFolderPtr( new StationSimpleFolder( input_folder, output_folder, file_name ) );
    }
};


class TreeFactory : public IFactory
{
public:

    ICompareRunnerPtr createICompareRunnerPtr()
    {
        return ICompareRunnerPtr( new StationTreeFolderCompareRunner() );
    }

    IStationFolderPtr createIStationFolderPtr( const std::string& input_folder, const std::string& output_folder, const std::string& file_name )
    {
        return IStationFolderPtr( new StationTreeFolder( input_folder, output_folder, file_name ) );
    }
};


bool getConfigurations( const std::string& file_name,
                        std::string& input_directory,
                        std::string& output_directory,
                        std::string& master_file,
                        std::string& file_names,
                        std::string& compare_by_tree )
{
    // compare_directory.ini
    // --input-directory=
    // --output-directory=
    // --master-file=
    // --file-names=
    // --compare-by-tree=

    std::string str;

    if ( false == getFileString( file_name, str ) )
    {
        return false;
    }
    
    static boost::regex e
        (
            "\\s*(--[^\\s=]+)\\s*="    // match 1: name
            "\\s*([^\\s]+)\\s*"        // match 2: value
        );
    
    boost::sregex_iterator it( str.begin(), str.end(), e );
    static boost::sregex_iterator end;

    for ( ; it != end; ++it )
    {
        const std::string& name = it->str( 1 );
        const std::string& value = it->str( 2 );
        
        if ( "--input-directory" == name )
        {
            input_directory = value;
        }
        else if ( "--output-directory" == name )
        {
            output_directory = value;
        }
        else if ( "--master-file" == name )
        {
            master_file = value;
        }
        else if ( "--file-names" == name )
        {
            file_names = value;
        }
        else if ( "--compare-by-tree" == name )
        {
            compare_by_tree = value;
        }
        else
        {
            std::cout << "Error: unknown parameter: " << name << ", " << value << std::endl;
        }
    }

    if ( true == input_directory.empty() )
    {
        std::cout << "Error: input_directory is empty" << std::endl;
        return false;
    }
    
    if ( true == output_directory.empty() )
    {
        std::cout << "Error: output_directory is empty" << std::endl;
        return false;
    }
    
    if ( true == input_directory.empty() )
    {
        std::cout << "Error: input_directory is empty" << std::endl;
        return false;
    }
    
    if ( true == master_file.empty() )
    {
        std::cout << "Error: master_file is empty" << std::endl;
        return false;
    }
    
    if ( true == file_names.empty() )
    {
        std::cout << "Error: file_names is empty" << std::endl;
        return false;
    }
    
    if ( true == compare_by_tree.empty() )
    {
        std::cout << "Error: compare_by_tree is empty" << std::endl;
        return false;
    }
    
    return true;
}


int main(int argc, char* argv[])
{
    std::string input_directory;
    std::string output_directory;
    std::string master_file;
    std::string file_names;
    std::string compare_by_tree;

    if ( false == getConfigurations( "compare_directory.ini", input_directory, output_directory, master_file, file_names, compare_by_tree ) )
    {
        std::cout << "Error: configuration error." << std::endl;
        system( "pause" );
        return 0;
    }
    
    std::vector<std::string> files;

    typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep( ", " );

    tokenizer tokens( file_names, sep );

    for( tokenizer::iterator iter = tokens.begin(); iter != tokens.end(); ++iter )
    {
        files.push_back( *iter );
    }

    IFactoryPtr factory;

    if ( "yes" == compare_by_tree || "Yes" == compare_by_tree || "YES" == compare_by_tree ||
         "true" == compare_by_tree ||"True" == compare_by_tree || "TRUE" == compare_by_tree ||
         "1" == compare_by_tree )
    {
        factory.reset( new TreeFactory() );
    }
    else
    {
        factory.reset( new SimpleFactory() );
    }

    ICompareRunnerPtr runner = factory->createICompareRunnerPtr();

    IStationFolderPtr master = factory->createIStationFolderPtr( input_directory, output_directory, master_file );

    std::cout << std::endl;

    runner->setMaster( master );

    for ( std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it )
    {
        IStationFolderPtr other = factory->createIStationFolderPtr( input_directory, output_directory, *it );

        runner->setOther( other );

        runner->compare();
    }

    system( "pause" );

    return 0;
}
