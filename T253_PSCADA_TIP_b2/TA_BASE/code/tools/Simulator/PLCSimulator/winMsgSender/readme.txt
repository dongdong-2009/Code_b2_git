command line:
myExeName sendMsgProcessName winTitileIncludeKeyWord msgConten

test case:
winMsgSender.exe PLCSimulator.exe DT18 SetDataString 123=22;1234=65535;20.0=1;20.1=0;
winMsgSender.exe PLCSimulator.exe DT18-PLCSimulator SetDataString 123=22;1234=65535;20.0=1;20.1=0;

winMsgSender.exe PLCSimulator.exe DT18 StressPlcOnOffOp plc-on=1 stress-on=0;
winMsgSender.exe PLCSimulator.exe DT18 Kill