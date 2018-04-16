#
# $Source: /cvs/1TS-9999-T/code/tools/BuildSystem/UpdateVersions.pl,v $
# $Revision: 1.1.2.3 $
# $Date: 2003/08/06 02:53:57 $
# $Author: andrewdc $
#
# This program automatically creates each application and library's
# VersionGen.cpp file which contains information about the version of the
# application / library.
#
# It receives its data from the specified XML definitions file.
#
# History
#
# $Log: UpdateVersions.pl,v $
# Revision 1.1.2.3  2003/08/06 02:53:57  andrewdc
# removed second level namespace
#
# Revision 1.1.2.2  2003/06/12 02:29:55  andrewdc
# Updated to print out not only version, but build number aswell
# prints this information to a file aswell for its and waston scripts to use
#
# Revision 1.1.2.1  2003/05/23 03:59:12  adamr
# added versioning to build system
#
# Revision 1.1.2.1  2002/09/20 04:14:15  itsbuild
# added from Adelaide R5-1_Integration_Branch for manila
#
# Revision 1.1.2.6.4.1  2002/08/23 07:58:36  itsbuild
# Fixed Paths for locations without src dirs
#
# Revision 1.1.2.6  2002/08/05 01:44:45  geoffg
# Added support for allowing the "Other" field to be automatically
# generated from the XML file.  It uses the "OTHER" tag to locate the
# information, and splits it using the new line character.
#
# Revision 1.1.2.5  2002/07/31 03:52:47  geoffg
# Clarified the inclusion of application patches against project patch
# releases.
#
# Revision 1.1.2.4  2002/07/31 02:53:51  geoffg
# Changed date / time format so that it is a C preprocessor macro that
# is expanded at compile time.
#
# Revision 1.1.2.3  2002/07/31 02:40:01  geoffg
# Corrected code so that generated code doesn't include CVS information.
#
# Revision 1.1.2.2  2002/07/31 02:35:24  geoffg
# Corrected minor typo.
#
# Revision 1.1.2.1  2002/07/31 02:31:01  geoffg
# Added
#
#
use XML::Simple;
use Data::Dumper;
require Time::Local;
# version file for DrWatson.pl and its.sh scripts to use
open( VERSIONINFO, ">./versions" ) or die "Can't open ./versions\n";
sub GenerateArrayData
{
    my ( $fileHandle, $arrayName, @arrayData ) = @_;

    if( -1 == $#arrayData )
    {
        printf $fileHandle "\t\tint %sSize = 1;\n", $arrayName;
        print $fileHandle "\t\tchar * $arrayName\[\] =\n";
        print $fileHandle "\t\t{\n\t\t\t\" \"\n";
        print $fileHandle "\t\t};\n";
    }
    else
    {
        printf $fileHandle "\t\tint %sSize = %d;\n", $arrayName, $#arrayData + 1;
        print $fileHandle "\t\tchar * $arrayName\[\] =\n";
        print $fileHandle "\t\t{\n";
        for $value ( @arrayData )
        {
            print $fileHandle "\t\t\t\"$value\"";
            if( $value ne $arrayData[ $#arrayData ] )
            {
                print $fileHandle ",";
            }
            print $fileHandle "\n"
        }
        print $fileHandle "\t\t};\n";
    }
}

sub GenerateVersionFile
{
    my ( $versionFile,
         $componentName,
	 $buildNumber,
         $buildVersion,
         $patchLevel,
         $libraries,
         $headers,
         $other ) = @_;

    my $fileHandle;
    if (open( $fileHandle, ">$versionFile" ))
	{
		print "Generating version file for component '$componentName'\n";
		
		print $fileHandle "/*\n";
		printf $fileHandle " * \$%s:\$\n", "Source";
		printf $fileHandle " * \$%s:\$\n", "Revision";
		printf $fileHandle " * \$%s:\$\n *\n", "Date";
		print $fileHandle " * DO NOT MANUALLY EDIT THIS FILE.  THIS FILE IS\n";
		print $fileHandle " * CREATED AUTOMATICALLY BY THE BUILD SYSTEM\n";
		print $fileHandle " */\n";
	
		# insert namespace
		
		print $fileHandle "namespace TA_Core\n";
		print $fileHandle "{\n";
		
		print $fileHandle "\tchar componentName[] = \"$componentName\";\n";
		# RY : removed keyword substitution for cvs tag as was not working correctly
		# printf $fileHandle "char buildVersion[] = \"$buildVersion TAG: \$%s:\$\";\n", "Name";
		printf $fileHandle "\tchar buildVersion[] = \"$buildVersion\";\n";
		print $fileHandle "\tchar componentVersion[] = \"$buildVersion";
		if ( $patchLevel ne "0" )
		{
			print $fileHandle " - Application Patch $patchLevel";
		}
		print $fileHandle "\";\n";
	
		print $fileHandle "\tchar buildDate[] = \"$buildNumber\";\n";
	
		print $fileHandle "\tchar identString[] = \"\@(\#)Uncontrolled\";\n";
	
		GenerateArrayData( $fileHandle, "libraryArray", @$libraries );
		GenerateArrayData( $fileHandle, "headerArray", @{ $headers } );
		GenerateArrayData( $fileHandle, "otherArray", @{ $other } );
		
		# end namespace
		print $fileHandle "}\n";
		
		close($fileHandle);
	}
	else
	{
		print "Cant write version file for component '$componentName'\n";
	}
}

#
# Make sure that there are sufficient arguments.
#
if( $#ARGV != 0 )
{
    die "usage: ./UpdateVersions.pl <versiondata>\n";
}

#
# NOTE: suppressempty must be set to 1, otherwise XMS::Simple returns empty
# attributes as empty hashes and this confuses the application.
#
my $xmlParser = new XML::Simple( suppressempty => 1 );
my $xmlDefinition = $xmlParser->XMLin( $ARGV[0] );

#
# Extract the project specific values.
#
my $projectTitle = $xmlDefinition->{DETAILS}->{TITLE};
my $projectVersion = $xmlDefinition->{DETAILS}->{VERSION};
print VERSIONINFO "Version:$projectVersion\n";

#
# Generate the VersionGen.cxx file for each component.
#
for $component ( keys %{ $xmlDefinition->{COMPONENTS} } )
{
    my @componentLibraries = ( split / /, $xmlDefinition->{COMPONENTS}->{$component}->{LIBS} );
    my @componentHeaders = ( $projectTitle );
    my @componentOther = ( split /\n/, $xmlDefinition->{COMPONENTS}->{$component}->{OTHER} );
    my $componentPatchLevel = $xmlDefinition->{COMPONENTS}->{$component}->{PATCH_LEVEL};
    my $componentName = $xmlDefinition->{COMPONENTS}->{$component}->{NAME};
    my $componentBuildNumber = $xmlDefinition->{COMPONENTS}->{$component}->{BUILDNUMBER};
    my $componentLocation = $xmlDefinition->{COMPONENTS}->{$component}->{LOCATION};

    print VERSIONINFO "$componentName:$componentBuildNumber\n";
    #
    # Now generate the file.
    #
    GenerateVersionFile( $componentLocation . "/VersionGen.cpp",
                         $componentName,
			 $componentBuildNumber,
                         $projectVersion,
                         $componentPatchLevel,
                         \@componentLibraries,
                         \@componentHeaders,
                         \@componentOther );
}


