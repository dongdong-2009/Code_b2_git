#!/usr/bin/sh

chown transactive:scada ./*
chmod 0755 ./Trace.prg
chmod 755 ./*
chmod u+s MmsAgent

chown root:other ./MmsAgent_BEA
chown root:other ./RadioTetraAgent
chown root:other ./SystemController
chown root:other ./SystemStatusAgent
chmod +s ./SystemStatusAgent
chmod +s ./SystemController
chmod +s ./RadioTetraAgent
