#!/usr/bin/python

import sys
import binascii
import os
import socket
import struct
import time

BYTEMASK=0xAA

# there are 4 dictionaries of values. read and write requests, and for each of
# those a hex and bin.
requests_w_hex={}

gRemoteAddr='localhost'
gRemotePort=25500
gLocalAddr='localhost'
gLocalPort=25499
gLocation='OCC'
gReadOnly=0
gWriteOnly=0
gVerbosity=1
gLogFile=None
gLogging=0
gIscsWatchdog=0

# Header lengths for Read and Write Queries and Responses
gRQHeaderLen=6
gWQHeaderLen=7
gRRHeaderLen=3
gWRHeaderLen=6
gERHeaderLen=2

# Footer lengths for Read and Write Queries and Responses
# This is currently the redundant (fixed to zero) CRC
gRQFooterLen=2
gWQFooterLen=2
gRRFooterLen=2
gWRFooterLen=2
gERFooterLen=2

gAtsBaseAddr=0
gIscsBaseAddr=10000


table_size = {
    'OCC': {
            'Ats'  :{ 0:2 , 2:9504 , 9602:5400 , 15002:90 , 5080:80 },
            'Iscs' :{ 0:2 , 2:2772 , 3202:1000 , 3802:2574 }
           },

    'MSS': {
            'Ats'  :{ 0:2 , 2:1920 , 1922:1160 , 3082:80 },
            'Iscs' :{ 0:2 , 2:280 , 282:60 , 342:60 , 402:260 }
           },

    'SS1': {
            'Ats'  :{ 0:2 , 2:1920 , 1922:1160 , 3082:80 },
            'Iscs' :{ 1000:2 , 1002:280 , 1282:60 , 1342:60 , 1402:260 }
           },

    'SS2': {
            'Ats'  :{ 0:2 , 2:1920 , 1922:1160 , 3082:80 },
            'Iscs' :{ 2000:2 , 2002:280 , 2282:60 , 2342:60 , 2402:260 }
           },

    'DPT': {
            'Ats'  :{ 0:2 , 2:9504 , 9602:80 , 9802:8 },
            'Iscs' :{ 0:2 , 2:2772 , 2802:32 , 2824:2574 }
           }
    }

gAtsTableSizes=None
gIscsTableSizes=None

exception_codes={
                    0:'No_Error',
                    1:'Function_Not_Defined',
                    2:'Modbus_Table_Address_Not_Recognized',
                    3:'Illegal_Value_For_Address'
                 }

#
# Set of printable characters. Not used.
# 

##################################################################
def validateDataLength(tNum,received):
    global gAtsTableSizes
    global gVerbosity
    bytesReceived=received
    expected=gAtsTableSizes.get(tNum,0)
    if (expected!=bytesReceived):
        if (gVerbosity>=1):
            print "-----ERROR-----ERROR-----ERROR-----ERROR-----"
            print "-----ERROR: received size for table %d incorrect" % tNum
            print "-----ERROR: Expected:%d Received:%d" % (expected,bytesReceived)
            print "-----ERROR-----ERROR-----ERROR-----ERROR-----"


##################################################################
def processReadResponse(tNum,header,data,footer):
    global gVerbosity

    if (gVerbosity>=4):
        print "   processReadResponse(%d,%s,%s)" % (tNum,binascii.b2a_hex(header),binascii.b2a_hex(footer))
    deviceAddress=int((struct.unpack('B',header[0]))[0])
    responsevalue=int((struct.unpack('B',header[1]))[0])
    byteCount=int((struct.unpack('B',header[2]))[0])
    datalen=len(data)
    crc=socket.ntohs((struct.unpack('H',footer[0:2]))[0])
    
    if (gVerbosity>=4):
        if (datalen>64):
            print "Processing Read Response: %d bytes (%s...)" % (datalen,binascii.b2a_hex(data[0:64]))
        else:
            print "Processing Read Response:",datalen,"bytes:",binascii.b2a_hex(data)

        print "Device Address :",deviceAddress,"(fixed to 0)"
        print " Response Type : %0.2x" % responsevalue
        print "    Byte Count :",byteCount,"(fixed to 0)"
        print "           CRC :",crc,"(fixed to 0)"
        print " - - - - - - - - - - - - - - - - - - "

    if (gVerbosity>=3):
        if (tNum==3082) or (tNum==5080) or (tNum==9602): # Dss Tables
            print " YYYY mm dd HH MM SS Ev In "
            for x in range(0,datalen,16):
                x_year=socket.ntohs((struct.unpack('H',data[x:x+2]))[0])
                x_month=socket.ntohs((struct.unpack('H',data[x+2:x+4]))[0])
                x_day=socket.ntohs((struct.unpack('H',data[x+4:x+6]))[0])
                x_hour=socket.ntohs((struct.unpack('H',data[x+6:x+8]))[0])
                x_minute=socket.ntohs((struct.unpack('H',data[x+8:x+10]))[0])
                x_second=socket.ntohs((struct.unpack('H',data[x+10:x+12]))[0])
                x_event=socket.ntohs((struct.unpack('H',data[x+12:x+14]))[0])
                x_input=socket.ntohs((struct.unpack('H',data[x+14:x+16]))[0])
                print ' %0.4d %0.2d %0.2d %0.2d %0.2d %0.2d %0.2d %0.2d ' % (x_year,x_month,x_day,x_hour,x_minute,x_second,x_event,x_input)
        elif (tNum==15002): # Occ Wild Table
            print "Data Length is",datalen
            print " YYYY mm dd HH MM SS PT# AC# HCD"
            for x in range(0,datalen,18):
                x_year=socket.ntohs((struct.unpack('H',data[x:x+2]))[0])
                x_month=socket.ntohs((struct.unpack('H',data[x+2:x+4]))[0])
                x_day=socket.ntohs((struct.unpack('H',data[x+4:x+6]))[0])
                x_hour=socket.ntohs((struct.unpack('H',data[x+6:x+8]))[0])
                x_minute=socket.ntohs((struct.unpack('H',data[x+8:x+10]))[0])
                x_second=socket.ntohs((struct.unpack('H',data[x+10:x+12]))[0])
                x_phystrain=socket.ntohs((struct.unpack('H',data[x+12:x+14]))[0])
                x_activecab=socket.ntohs((struct.unpack('H',data[x+14:x+16]))[0])
                x_headcabdir=socket.ntohs((struct.unpack('H',data[x+16:x+18]))[0])
                print ' %0.4d %0.2d %0.2d %0.2d %0.2d %0.2d %0.3d %0.3d %0.3d' % (x_year,x_month,x_day,x_hour,x_minute,x_second,x_phystrain,x_activecab,x_headcabdir)
        elif (tNum==1922): # Station Platform Table
            print " Platform Doors: "
            # print " XX XX 01 02 03 04 05 06 XX XX 07 08 09 10 11 12"
            for x in range(0,datalen,58):
                x_platformdoor=socket.ntohs((struct.unpack('H',data[x+56:x+58]))[0])
                print " %0.4x" % x_platformdoor
            # no, not a bitmap of doors.  This assists in mapping from
            # door number to bit index:
            # Door   X  X  1  2  3  4  5  6  X  X  7  8  9 10 11 12
            # Bit   15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
#             doorBitIndexMap=[13,12,11,10,9,8,5,4,3,2,1,0]
#             x_door=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
#             x_bitDoorIndex=[
#             for x in range(0,datalen,58):
#                 x_platformdoor=socket.ntohs((struct.unpack('H',data[x+56:x+58]))[0])
#                 for bitnum in range(0,16):
#                 x_door[]=
#                 print '  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d' % ()
        elif (tNum==0): # watchdog
            print "Watchdog =",socket.ntohs((struct.unpack('H',data[0:2]))[0])

    validateDataLength(tNum,datalen)


##################################################################
def processWriteResponse(tNum,header,footer):
    global gVerbosity
    tNum += gIscsBaseAddr
    # (responsevalue)=struct.unpack('B',wdata)
    # responsevalue=int(wdata)
    if (gVerbosity>=4):
        print "   processWriteResponse(%d,%s,%s)" % (tNum,binascii.b2a_hex(header),binascii.b2a_hex(footer))
    deviceAddress=int((struct.unpack('B',header[0]))[0])
    responsevalue=int((struct.unpack('B',header[1]))[0])
    startAddress=socket.ntohs((struct.unpack('H',header[2:4]))[0])
    numRegisters=socket.ntohs((struct.unpack('H',header[4:6]))[0])

    crc=socket.ntohs((struct.unpack('H',footer[0:]))[0])

    if (gVerbosity>=1) and (gVerbosity<4):
        print "Processed Write Response for table:%d registers written:%d" % (tNum,numRegisters)

    if (gVerbosity>=4):
        print "Processed Write Response for table %d" % (tNum)
        print "Device Address :",deviceAddress,"(fixed to 0)"
        print " Response Type : %0.2x" % responsevalue
        print " Start Address :",startAddress
        print " Num Registers :",numRegisters
        print "           CRC :",crc,"(fixed to 0)"
        print " - - - - - - - - - - - - - - - - - - "
    
    if (startAddress != tNum):
        print "ERROR: Response received for wrong table. Expected:%d Received:%d" % (tNum,startAddress)


##################################################################
def processExceptionResponse(tNum,header,data,footer):
    global gVerbosity
    global exception_codes

    print "   processExceptionResponse(%d,%s,%s)" % (tNum,binascii.b2a_hex(header),binascii.b2a_hex(footer))
    deviceAddress=int((struct.unpack('B',header[0]))[0])
    responsevalue=int((struct.unpack('B',header[1]))[0])
    exceptionValue=int((struct.unpack('B',data[0]))[0])

    crc=socket.ntohs((struct.unpack('H',footer[0:]))[0])

    if (gVerbosity>=1):
        print "Processing Exception Response for table %d. Response=%d (%s)" % (tNum,exceptionValue,exception_codes.get(exceptionValue,"KeyNotFound"))

    if (gVerbosity>=4):
        print "Device Address :",deviceAddress
        print "Response Value : %0.2x" % responsevalue
        print "Exception Valu :",exceptionValue
        print "           CRC :",crc
        print " - - - - - - - - - - - - - - - - - - "
    

##################################################################
def makeReadQuery(tableAddress):
    global gAtsTableSizes
    tableByteSize=gAtsTableSizes.get(tableAddress,0)
    # da  fc  addr  size  crc_
    # 00  04  XXXX  YYYY  0000
    hexstr="0004%0.4x%0.4x0000" % (tableAddress,tableByteSize/2)
    return hexstr


##################################################################
def makeWriteQuery(tNum):
    global gIscsTableSizes
    global gIscsBaseAddr
    byteLength=gIscsTableSizes.get(tNum,0)
    if (byteLength==0):
        print "EEK. Couldn't find table number",tNum
        sys.exit(1)
        return None
    length=byteLength/2
    # da  fc  addr  size  bc
    # 00  04  XXXX  YYYY  00
    hexstr='0010%0.4x%0.4x00' % (tNum+gIscsBaseAddr,length)
    charidx=0x20
    # deal with the three IscsWatchdog tables specially
    if (tNum==0):
        global gIscsWatchdog
        gIscsWatchdog=(time.time() % 65535)
        hexstr+='%0.4x' % gIscsWatchdog
        print ' #### gIscsWatchdog=%d hexstr=%s' % (gIscsWatchdog, hexstr)
    elif (gLocation=='DPT') and (tNum==2802):
        # IscsWash
        print
        for x in range(byteLength):
            hexstr+='00'
        print ' #### IscsWash tAddr=%d hexstr=%s' % (tNum, hexstr)
    elif ((gLocation=='MSS') and (tNum==282)) or \
         ((gLocation=='SS1') and (tNum==1282)) or \
         ((gLocation=='SS2') and (tNum==2282)):
        # IscsPlatform
        for x in range(0,byteLength,6):
            # platform id
            hexstr+='%0.4x' % (x/6)
            if (gLocation=='OCC'):
                # physical train number
                hexstr+='%0.4x' % x
            elif (gLocation=='MSS') or (gLocation=='SS1') or (gLocation=='SS2'):
                # close/open request
                hexstr+='0000'
            else:
                hexstr+='%0.4x' % (x+1)
            # psd door status
            if (x==0):
                hexstr+='FFFF'
            elif (x==6):
                hexstr+='AAAA'
            elif (x==12):
                hexstr+='5555'
            elif (x==18):
                hexstr+='8888'
            elif (x==24):
                hexstr+='1111'
            else:
                hexstr+='0000'
    else:
        for x in range(byteLength):
            if charidx>0x7e:
                charidx=0x20
            hexstr+='%0.2x' % charidx
            charidx+=1
    # crc
    hexstr+='0000'
    return hexstr


##################################################################
def sendReadQuery(tNum,thesock):
    r=makeReadQuery(tNum)
    global gVerbosity
    if (gVerbosity>=1):
        print 'sending table',tNum,'read request:',r
    log('>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>')
    log('READ QUERY (table=%d,query_len=%d):' % (tNum,len(r)/2) + r)
    thesock.send(binascii.a2b_hex(r))
    sys.stdout.flush()


##################################################################
def sendWriteQuery(tNum,thesock):
    request=makeWriteQuery(tNum)
    if (request==None):
        print 'ERROR: No table with number:',tNum
        return
    query=binascii.a2b_hex(request)
    qleng=len(query)
    global gVerbosity
    if (gVerbosity>=1):
        print 'sending table',tNum,'write request:',qleng,'bytes'
        sys.stdout.flush()
    log('>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>')
    log('WRITE QUERY (table=%d,query_len=%d):' % (tNum,len(query)) + request)
    thesock.send(query)

##################################################################
def trimReceived(r,num):
    r=r[num:]
    return r


##################################################################
def openLogFile():
    timestamp=time.strftime('%Y%m%d%H%M%S',time.localtime())
    filename='atsim.'+timestamp+'.log'
    file=open(filename,'w')
    return file

##################################################################
def log(s):
    global gLogging
    global gLogFile
    if (gLogging):
        print >>gLogFile, s
        gLogFile.flush()


##########################################################
def usage():
    from os.path import basename
    global gRemoteAddr
    global gRemotePort
    global gLocalAddr
    global gLocalPort
    global gLocation
    print """
    Usage: %s OPTIONS
    
         where OPTIONS are one or more of:
         --remote-addr=x.x.x.x  ip address of the machine hosting PASimulator
                                (default:%s)
         --remote-port=x        port of the remote PASimulator
                                (default:%d)
         --local-addr=x.x.x.x   ip address that the socket of this script will bind to
                                (default:%s)
         --local-port=x         port that the socket of this script will bind to
                                (default:%d)
         --location=XXX         location is one of OCC,MSS,DPT
                                (default:%s)
         --readonly             only send read queries to ATSSimulator
         --writeonly            only send write queries to ATSSimulator
         --do-log               logs received bytes to "atsim.YYYYMMDDhhmmss.log"
         -h|--help              print this message

         NOTE:  if both --readonly and --writeonly options are specified, both
                read and write queries will be sent.
""" % (basename(sys.argv[0]),gRemoteAddr,gRemotePort,gLocalAddr,gLocalPort,gLocation)

###########################################################
def usage_extra():
    global gVerbosity
    print """
        
        EXTRA OPTIONS are one or more of:
         --verbosity=n          set verbosity to level n, range 0-9 (default:%d)
         --verbose              set verbosity to level 9
""" % (gVerbosity)

##################################################################
def parseCmdLineOptions():
    import getopt
    try:
        global gRemoteAddr
        global gRemotePort
        global gLocalAddr
        global gLocalPort
        global gLocation
        global gReadOnly
        global gWriteOnly
        global gVerbosity
        global gLogging
        cmdoptions= ['remote-addr=','remote-port=','local-addr=','local-port=','location=']
        cmdoptions+=['readonly','writeonly','do-log']
        cmdoptions+=['help','help-extra','verbosity=','verbose']
        opts, pargs = getopt.getopt(sys.argv[1:], 'h',cmdoptions)
        for opt,arg in opts:
            if (opt == '--remote-addr'):
                gRemoteAddr=arg
            elif (opt == '--remote-port'):
                gRemotePort=int(arg)
            elif (opt == '--local-addr'):
                gLocalAddr=arg
            elif (opt == '--local-port'):
                gLocalPort=int(arg)
            elif (opt == '--location'):
                global table_size
                checkTableSizes=table_size.get(gLocation,None)
                if (checkTableSizes!=None):
                    gLocation=arg
                else:
                    print "ERROR: location option '%s' not recognised" % (arg)
                    sys.exit(1)
            elif (opt == '--readonly'):
                gReadOnly=1
            elif (opt == '--verbosity'):
                gVerbosity=int(arg)
            elif (opt == '--verbose'):
                gVerbosity=9
            elif (opt == '--do-log'):
                gLogging=1
            elif (opt == '--writeonly'):
                gWriteOnly=1
            elif (opt == '--help-extra'):
                usage()
                usage_extra()
                sys.exit(0)
            elif (opt == '-h') or (opt == '--help'):
                usage()
                sys.exit(0)
        if (gReadOnly) and (gWriteOnly):
            print "WARNING: Both --readonly and --writeonly options specified (Will do both)"
            gReadOnly=0
            gWriteOnly=0
    except getopt.GetoptError,e:
        print "ERROR: unrecognised option or argument (%s): %s" % (e.opt,e.msg)
        usage()
        sys.exit(1)



gReceivedBuffer=''
##################################################################
#
# Main Receiving-Processing Loop
#
def doReceive(thesock,tableNum):
    global gReceivedBuffer
    global gCrcLength
    global gRRHeaderLen
    global gWRHeaderLen
    global gERHeaderLen
    global gRRFooterLen
    global gWRFooterLen
    global gERFooterLen
    global gAtsTableSizes
    global gIscsTableSizes
    longness=0
    must_continue=1
    while (must_continue):
        try:
            sys.stdout.flush()
            log('<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<')
            buffer=''
            buffer = thesock.recv(16384)
            bufferlen=len(buffer)
            # log(time.strftime("%H:%M:%S",time.localtime()))
            log('BUFFER(%d):' % (bufferlen) + binascii.b2a_hex(buffer))
            if (gVerbosity>=2):
                print "Buffered %d Bytes" % (bufferlen)
            gReceivedBuffer += buffer
            log('RECEIVED(%d):' % (len(gReceivedBuffer))+binascii.b2a_hex(gReceivedBuffer[:2000]))
            # print "logged RECEIVED"
            # gReceivedBuffer.append(buffer)
            # gReceivedBuffer=gReceivedBuffer+buffer
        except socket.error,e:
            print
            print "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
            print "Socket Exception:",e
            print "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
            sys.exit(0)
        except KeyboardInterrupt,ki:
            print
            print "!!!!!!!!!!!!! Keyboard Interrupt !!!!!!!!!!!!!"
            print
            sys.exit(0)

        # print '-------------------------------'

        # 
        # calculate some introductory statistics about the packet
        responseType=(struct.unpack('B',gReceivedBuffer[1]))[0]
        longness=len(gReceivedBuffer)

        if (gVerbosity>=6):
            print "@responseType=%x" % responseType
            print "@bufferLength=%d" % longness
        ######################
        #
        #   WRITE RESPONSE
        #
        if (responseType==0x10):
            if (gVerbosity>=6):
                print " -- Write Response -- "
            #
            # process a write reply packet.  These are always short, so we
            # don't worry about receiving multiple tcp packets for each
            w_header=gReceivedBuffer[:gWRHeaderLen]
            # w_footer=gReceivedBuffer[gWRHeaderLen+1:gWRHeaderLen+gWRFooterLen+1]
            w_footer=gReceivedBuffer[gWRHeaderLen:gWRHeaderLen+gWRFooterLen]
            processWriteResponse(tableNum,w_header,w_footer)

            # trim the gReceivedBuffer data buffer
            trimmed=gReceivedBuffer[gWRHeaderLen+gWRFooterLen+1:]
            log('WRITE RESPONSE')
            log('RECEIVED(%d):' % len(gReceivedBuffer)+binascii.b2a_hex(gReceivedBuffer))
            log('TRIMMED(%d):' % len(trimmed)+binascii.b2a_hex(trimmed))
            gReceivedBuffer=trimmed

        ######################
        #
        #   READ RESPONSE
        #
        elif ( responseType == 0x04 ):
            #
            # process a read reply packet.  These are as long as the table they
            # requested (plus Header + Footer)
            # 
            tableSize=(gAtsTableSizes.get(tableNum,0))
            requiredDataLength=tableSize+gRRHeaderLen+gRRFooterLen

            if (gVerbosity>=4):
                print "         <table:%d size:%d expected:%d received:%d>" % (tableNum,tableSize,requiredDataLength,longness)
            if (longness<requiredDataLength):
                # we don't yet have all the reply
                if (gVerbosity>=5):
                    print "   (Still receiving table %d, expecting:%d received:%d)" % (tableNum,requiredDataLength,longness)
                continue
            elif (longness>requiredDataLength):
                # we have more than one reply
                if (gVerbosity>=5):
                    print "   (Received too much [required:%d, received:%d])" % (requiredDataLength,longness)
                r_header=gReceivedBuffer[:gRRHeaderLen]
                r_data=gReceivedBuffer[gRRHeaderLen:gRRHeaderLen+tableSize]
                r_footer=gReceivedBuffer[gRRHeaderLen+tableSize:gRRHeaderLen+tableSize+gRRFooterLen]
                processReadResponse(tableNum,r_header,r_data,r_footer)
                # trim the gReceivedBuffer data buffer
                # gReceivedBuffer=gReceivedBuffer[requiredDataLength:]
                # gReceivedBuffer=trimReceived(gReceivedBuffer,requiredDataLength)
                trimmed=gReceivedBuffer[gRRHeaderLen+tableSize+gRRFooterLen:]
                log('READ RESPONSE (EXCESS)')
                log('RECEIVED(%d):' % len(gReceivedBuffer)+binascii.b2a_hex(gReceivedBuffer))
                log('TRIMMED(%d):' % len(trimmed)+binascii.b2a_hex(trimmed))
                gReceivedBuffer=trimmed
                continue

            # else:
            if (gVerbosity>=6):
                print "=>responseType=%0.2x" % responseType
                print "=>tableSize=%d" % tableSize
            r_header=gReceivedBuffer[:gRRHeaderLen]
            r_data=gReceivedBuffer[gRRHeaderLen:gRRHeaderLen+tableSize]
            r_footer=gReceivedBuffer[gRRHeaderLen+tableSize:gRRHeaderLen+tableSize+gRRFooterLen]
            processReadResponse(tableNum,r_header,r_data,r_footer)
            # trim the gReceivedBuffer data buffer
            trimmed=gReceivedBuffer[gRRHeaderLen+requiredDataLength+gRRFooterLen:]

            log('READ RESPONSE')
            log('RECEIVED(%d):' % len(gReceivedBuffer)+binascii.b2a_hex(gReceivedBuffer))
            log('TRIMMED(%d):' % len(trimmed)+binascii.b2a_hex(trimmed))
            gReceivedBuffer=trimmed

        ######################
        #
        #   EXCEPTION RESPONSE
        #
        elif ( responseType == 0x84 ) or (responseType==0x90):
            tableSize=socket.ntohs((struct.unpack('H',gReceivedBuffer[3:5]))[0])
            requiredDataLength=tableSize+5
            if (gVerbosity>=4):
                print " EXCEPTION RESPONSE:"
            e_header=gReceivedBuffer[:gERHeaderLen]
            e_data=gReceivedBuffer[gERHeaderLen]
            e_footer=gReceivedBuffer[gERHeaderLen+1:gERHeaderLen+1+gERFooterLen]
            processExceptionResponse(tableNum,e_header,e_data,e_footer)
            # trim the gReceivedBuffer data buffer
            # gReceivedBuffer=gReceivedBuffer[requiredDataLength:]
            # gReceivedBuffer=trimReceived(gReceivedBuffer,requiredDataLength)
            trimmed=gReceivedBuffer[gERHeaderLen+1+gERFooterLen:]
            log('EXCEPTION RESPONSE')
            log('RECEIVED(%d):' % len(gReceivedBuffer)+binascii.b2a_hex(gReceivedBuffer))
            log('TRIMMED(%d):' % len(trimmed)+binascii.b2a_hex(trimmed))
            gReceivedBuffer=trimmed


        ######################
        #
        #   UNSOLICITED (Out Of The Blue) RESPONSE
        #
        else:
            if (gVerbosity>=1):
                print "RECEIVED %d Bytes OUT OF THE BLUE: %s..." % (longness,binascii.b2a_hex(gReceivedBuffer[:6]))

        #
        # some terminals don't do this automatically (like cygwin's xterm !!)
        sys.stdout.flush()
        #
        must_continue=0

##################################################################
def doReadAtsTables(thesock):
    global gWriteOnly
    if not gWriteOnly:
        global gAtsTableSizes
        for thekey in gAtsTableSizes.keys():
            sendReadQuery(thekey,thesock)
            doReceive(thesock,thekey)

##################################################################
def doWriteIscsTables(thesock):
    global gReadOnly
    if not gReadOnly:
        global gIscsTableSizes
        for thekey in gIscsTableSizes.keys():
            sendWriteQuery(thekey,thesock)
            doReceive(thesock,thekey)


##################################################################
def mainLoop(thesock):
    while (1):
        try:
            doWriteIscsTables(thesock)
            print "Sent tables"
            time.sleep(2)
            doReadAtsTables(thesock)
            print "Read tables"
            time.sleep(2)
        except:
            print "Unexpected error:", sys.exc_info()[0]
            sys.exit(1)
            # raise

##################################################################
#
# The main business
#
#
##################################################################

parseCmdLineOptions()

# defaults to OCC, especially if the command-line parameter was faulty
localTableSizes=table_size.get(gLocation,None)

if (localTableSizes==None):
    print 'ERROR: Could not get table sizes for location "%s"' % gLocation
    sys.exit(2)


# print 'localTableSizes:',localTableSizes
gAtsTableSizes=localTableSizes.get('Ats',None)
# print 'gAtsTableSizes:',gAtsTableSizes
gIscsTableSizes=localTableSizes.get('Iscs',None)
# print 'gIscsTableSizes:',gIscsTableSizes

if (gVerbosity>=2):
    print "Remote = %s:%d" % (gRemoteAddr,gRemotePort)
    print "Local  = %s:%d" % (gLocalAddr,gLocalPort)
if (gVerbosity>=8):
    print "ExceptionCodes=",exception_codes

sys.stdout.flush()


#
#
thesock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

thesock.bind((gLocalAddr,gLocalPort))
tryport=gRemotePort
# lastport=tryport+4
lastport=tryport
good=0
while not good:
    try:
        if (gVerbosity>=1):
            print "Attempting connect %s:%d" % (gRemoteAddr,tryport)
            sys.stdout.flush()
        thesock.connect((gRemoteAddr,tryport))
        good=1
    except socket.error,e:
        good=0
        if (gVerbosity>=1):
            print "Port Error:",tryport,e
            sys.stdout.flush()
        tryport+=1
        if (tryport>=lastport):
            sys.exit(1)

if (gLogging):
    gLogFile=openLogFile()

mainLoop(thesock)

# vim:tabstop=4:expandtab:shiftwidth=4
