<%@ page import="java.util.*" %>

<%
  // Extract the version.
  String version = (String)request.getAttribute( "versionid" );

  // Extract module summary information.
  Hashtable[] modules = (Hashtable[])request.getAttribute( "modules" );

  // The styles used for colouring rows.
  String[] colours={ "colour1", "colour2" };
  String[] coloursWithPointers={ "colour1pointer", "colour2pointer" };
%>

<HTML>
<HEAD>
  <TITLE>Summary of All Modules in Version <%= version %></TITLE>
  <LINK rel="stylesheet" href="/TestResultReporter/css/ReportStyles.css" type="text/css" media="all">
  <META http-equiv="Pragma" content="no-cache">

  <SCRIPT language="JavaScript">
    var isSubmitting = false;
    var updatedDefects = new Array();

    function submitChanges()
    {
      if ( updatedDefects.length > 0 )
      {
        var updateRequest = "&defects=";
        for ( var defect in updatedDefects )
        {
          updateRequest = updateRequest+defect+":"+updatedDefects[defect]+",";
        }
        isSubmitting = true;
        parent.location = "?version=<%= request.getParameter( "version" ) %>"+updateRequest;
      }
      else
      {
        alert( "No defect numbers have changed." );
      }
    }

    function stopUnload()
    {
      if ( ( updatedDefects.length > 0 ) && ( ! isSubmitting ) )
      {
        var conf = confirm( "You have unsaved modifications on this page. Do you want to save them now?" );
        if ( conf )
        {
          submitChanges();
        }
      }
    }

    function updateDefect( textbox, pkey )
    {
      // Strip any non-numeric characters from the textbox
      var i=0, validStr="";
      for (i = 0; i < textbox.value.length; i++)
      {   
        // Check that current character is number.
        var c = textbox.value.charAt(i);
        if ( (c >= "0") && (c <= "9") )
        {
          validStr=validStr+c;
        }
      }
      textbox.value = validStr;

      textbox.style.backgroundColor = 'pink';
      updatedDefects[pkey] = textbox.value;
    }

    // copyright 1999 Idocs, Inc. http://www.idocs.com
    // Distribute this script freely but keep this notice in place
    function validateKey(myfield, e, dec)
    {
      var key;
      var keychar;

      if (window.event)
        key = window.event.keyCode;
      else if (e)
        key = e.which;
      else
        return true;
      keychar = String.fromCharCode(key);

      // control keys
      if ((key==null) || (key==0) || (key==8) || (key==9) || (key==13) || (key==27) )
        return true;

      // numbers
      else if ((("0123456789").indexOf(keychar) > -1))
        return true;

      // decimal point jump
      else if (dec && (keychar == "."))
      {
        myfield.form.elements[dec].focus();
        return false;
      }
      else
        return false;
    }

  </SCRIPT>
</HEAD>

<BODY onLoad="document.modulesForm.reset()" onUnload="JavaScript:stopUnload()">
<CENTER>
<H2>Summary of All Modules in Version <%= version %></H2>

<FORM name="modulesForm">
<TABLE>

<TR class="headings">
<TD>Module</TD>
<TD>Path</TD>
<TD>OS</TD>
<TD>Test Type</TD>
<TD>Test Tool</TD>
<TD>Run Date</TD>
<TD>Number of Tests</TD>
<TD>Number of Failures</TD>
<TD>Defect Number</TD>
</TR>
<%
for ( int i=0; i<modules.length; i++ )
{
  // Check if this row should be 'clickable'.
  if ( 0 != ((String)modules[i].get( "numfailed" )).compareTo( "0" ) )
  {
    out.print( "<TR class=\""+coloursWithPointers[i%coloursWithPointers.length]+"\" onClick=\"parent.location='?module="+(String)modules[i].get( "pkey" )+"'\">" );
  }
  else
  {
    out.print( "<TR class=\""+colours[i%colours.length]+"\">" );
  }
%>
<TD><%= (String)modules[i].get( "module" ) %></TD>
<TD><%= (String)modules[i].get( "path" ) %></TD>
<TD><%= (String)modules[i].get( "os" ) %></TD>
<TD><%= (String)modules[i].get( "testtype" ) %></TD>
<TD><%= (String)modules[i].get( "testtool" ) %></TD>
<TD><%= (String)modules[i].get( "rundate" ) %></TD>
<TD><%= (String)modules[i].get( "numtotal" ) %></TD>
<TD><%= (String)modules[i].get( "numfailed" ) %></TD>
<TD><INPUT type="text" onKeyPress="return validateKey(this, event)" onChange="JavaScript:updateDefect(this,<%= (String)modules[i].get( "pkey" ) %>)" onClick="event.cancelBubble=true;" value="<%= (String)modules[i].get( "defectnumber" ) %>"></TD>
</TR>
<%
} // for ( int i=0; i<modules.length; i++ )
%>
</TABLE>
<INPUT type="button" class="button" name="submit" value="Submit Defect Changes" onClick="JavaScript:submitChanges()">
</FORM>
<BR>
<I>Click on rows with a non-zero failure count to obtain more detailed information relating to that row</I>
</CENTER>
<BR>
<P align="right">
<A href="?version=<%= request.getParameter( "version" ) %>&format=xls">XLS</A>
</P>

</BODY>
</HTML>
