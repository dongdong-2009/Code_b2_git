This Readme file describes how to use the macros contained in COMMENTS.DSM.

COMMENTS.DSM contains two Visual Studio macros, called AddFunctionDescription and AddHeaderFileDescription, that can be used to (partly) automate the process of creating a comment block for a header file or function. Gaining access to the macros in Visual Studio is very easy: click on the Tools menu, Macro item, Options button, Loaded Files button, Browse. Then navigate to the COMMENTS.DSM file and click Open. Then Click Close. To assign the macros to a toolbar, click on the Tools menu and Macro item again, then click on Options, then Toolbars. Now drag the macro names to a toolbar, and choose an icon for them or a text description.

To use the AddHeaderFileDescription macro, move to the very top of your header file, and then activate the macro. Your header file should then contain a complete comment block (except for the description) that conforms to the programming guidelines.

To use the AddFunctionDescription macro, select the line or lines containing a function prototype in a header file. Make sure the entire line or lines containing the prototype is/are selected. Then activate the macro. A comment block should be inserted above the function prototype that contains a template of the documentation required by the programming guidelines.

Known bugs: the AddFunctionDescription macro may not work perfectly for template member functions, or functions that take a function pointer as a parameter.

