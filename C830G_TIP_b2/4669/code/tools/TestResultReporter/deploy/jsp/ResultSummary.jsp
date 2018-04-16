<%@ page import="java.util.*" %>

<%
  // Extract the module.
  String module = (String)request.getAttribute( "moduleid" );

  // Extract result summary information.
  Hashtable[] results = (Hashtable[])request.getAttribute( "results" );

  // The styles used for colouring rows.
  String[] styles={ "colour1", "colour2" };
%>

<HTML>
<HEAD>
  <TITLE>Summary of All Failed Results in Module <%= module %></TITLE>
  <LINK rel="stylesheet" href="/TestResultReporter/css/ReportStyles.css" type="text/css" media="all">
</HEAD>

<BODY>
<CENTER>
<H2>Summary of All Failed Results in Module <%= module %></H2>

<TABLE>
<TR class="headings">
<TD>Class</TD>
<TD>Function</TD>
<TD>Line</TD>
<TD>Message</TD>
</TR>
<%
for ( int i=0; i<results.length; i++ )
{
%>
<TR class="<%= styles[i%styles.length] %>">
<TD><%= results[i].get( "class" ) %></TD>
<TD><%= results[i].get( "function" ) %></TD>
<TD><%= results[i].get( "line" ) %></TD>
<TD><%= results[i].get( "message" ) %></TD>
</TR>
<%
}
%>
</TABLE>
</BODY>
</HTML>
