<%@ page import="java.util.*" %>

<%
  // Extract the version summary information.
  Hashtable[] versions = (Hashtable[])request.getAttribute( "versions" );

  // The styles used for colouring rows.
  String[] colours={ "colour1", "colour2" };
  String[] coloursWithPointers={ "colour1pointer", "colour2pointer" };
%>

<HTML>
<HEAD>
  <TITLE>Summary of All Tested Versions</TITLE>
  <LINK rel="stylesheet" href="/TestResultReporter/css/ReportStyles.css" type="text/css" media="all">
</HEAD>

<BODY>
<CENTER>
<H2>Summary of All Tested Versions</H2>
<TABLE>

<TR class="headings">
<TD>Version</TD>
<TD>Code Tag</TD>
<TD>Config Tag</TD>
<TD>Total Modules</TD>
<TD>Failed Modules</TD>
</TR>
<%
for ( int i=0; i<versions.length; i++ )
{
  // Check if this row should be 'clickable'.
  if( 0 != ((String)versions[i].get( "numfailed" )).compareTo( "0" ) )
  {
    out.print( "<TR class=\""+coloursWithPointers[i%coloursWithPointers.length]+"\" onClick=\"parent.location='?version="+(String)versions[i].get( "pkey" )+"'\">" );
  }
  else
  {
    out.print( "<TR class=\""+colours[i%colours.length]+"\">" );
  }
%>
<TD><%= (String)versions[i].get( "version" ) %></TD>
<TD><%= (String)versions[i].get( "codetag" ) %></TD>
<TD><%= (String)versions[i].get( "configtag" ) %></TD>
<TD><%= (String)versions[i].get( "numtotal" ) %></TD>
<TD><%= (String)versions[i].get( "numfailed" ) %></TD>
</TR>
<%
}
%>
</TABLE>
<BR>
<I>Click on rows with a non-zero failure count to obtain more detailed information relating to that row</I>
</CENTER>

</BODY>
</HTML>
