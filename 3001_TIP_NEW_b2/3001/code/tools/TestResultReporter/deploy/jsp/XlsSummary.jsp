<%@ page import="java.util.*" %>
<%@ page contentType="application/vnd.ms-excel" %>
<%
  // Extract the version.
  String version = (String)request.getAttribute( "versionid" );

  // Extract summary information.
  Hashtable[] results = (Hashtable[])request.getAttribute( "allresults" );

  // Output version information.
  out.println( "Version:\t"+version );

  // Output column headings
  out.println( "Module\tPath\tOS\tTest Type\tTest Tool\tFile\tClass\tFunction\tLine\tMessage" );

  // Output the columns
  for ( int i=0; i<results.length; i++ )
  {
    out.print( (String)results[i].get( "module" )+"\t" );
    out.print( (String)results[i].get( "path" )+"\t" );
    out.print( (String)results[i].get( "os" )+"\t" );
    out.print( (String)results[i].get( "testtype" )+"\t" );
    out.print( (String)results[i].get( "testtool" )+"\t" );
    out.print( (String)results[i].get( "file" )+"\t" );
    out.print( (String)results[i].get( "class" )+"\t" );
    out.print( (String)results[i].get( "function" )+"\t" );
    out.print( (String)results[i].get( "line" )+"\t" );
    out.println( (String)results[i].get( "message" ) ); 
  }
%>
