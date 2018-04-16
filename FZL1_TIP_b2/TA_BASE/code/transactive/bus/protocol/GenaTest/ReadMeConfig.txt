This file describes the how to configure the Test Application

1. Using Test Application with Database configuration.
	To set up database configuration see file "GENA Database Configuration.docx"
	See GenaSampleDbData on SampleConfig folder
	
	Run the test application using this command
	1.1 Server
		run GenaServerTest.exe --entity-name=<entityName> --db-connection=<connection string> --DataBase_C955=DataBaseGood --db-connection-file=OCCConnectionStrings.csv
		
	1.2 Bridge
		run GenaBridgeTest.exe --entity-name=<entityName> --db-connection=<connection string> --DataBase_C955=DataBaseGood --db-connection-file=OCCConnectionStrings.csv
	
	1.3 Client
		run GenaClientTest.exe --entity-name=<entityName> --db-connection=<connection string> --DataBase_C955=DataBaseGood --db-connection-file=OCCConnectionStrings.csv
		
	Entity name should be the parent entity of the configured entity from the database.
		
2. Using Xml File Configuration, See SampleConfig Folder
	Run the test application using this command
	1.1 Server
		run GenaServerTest.exe --config-file=GenaServerConfig.xml
		
	1.2 Bridge
		run GenaBridgeTest.exe --config-file=GenaBridgeConfig.xml
		
	1.3 Client
		run GenaClientTest.exe --config-file=GenaClientConfig.xml
		
3. Running with Third Party Simulators
	Third party simulators are located on GENA Third Party Simulators folder together with its configuration and sample data.
	To run the simulator just execute the application.
	
	