mkdir log
START "Occ Radio Simulator" ..\RadioSimulator.exe --debug-file=log\OccRadioSimulatorLoadOnly.log --debug-level=NORM --debug-file-max-size=1000000 --startup-script=OccFallbackLoadOnly.txt --parser-verbosity=ON
