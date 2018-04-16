mkdir log
START "Radio Epn" ..\RadioSimulator.exe --debug-file=log\EpnRadioSimulator.log --debug-level=INFO --startup-script=EpnFallback.txt
