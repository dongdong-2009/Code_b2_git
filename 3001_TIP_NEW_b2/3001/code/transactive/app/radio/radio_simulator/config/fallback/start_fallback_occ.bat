mkdir log
START "Occ Radio Simulator" ..\RadioSimulator.exe --debug-file=log\OccRadioSimulator.log --debug-level=INFO --debug-file-max-size=1000000 --startup-script=OccFallback.txt
