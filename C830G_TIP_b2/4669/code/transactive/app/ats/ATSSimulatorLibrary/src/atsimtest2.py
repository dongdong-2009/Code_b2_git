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

table_size_OCC= {
                    6:0,
                    1:5200,
                    2:9200,
                    5:80,
                    3:1,
                    4:2,
                    1001:2600,
                    1002:600,
                    1003:600,
                    1004:2800,
                    1006:2
                }

table_size_MSS= {
                    6:0,
                    1:810,
                    2:1380,
                    5:1,
                    3:1,
                    4:2,
                    1001:130,
                    1002:60,
                    1003:30,
                    1004:140,
                    1006:2
                }

table_size_DPT= {
                    6:0,
                    1:1,
                    2:9200,
                    5:1,
                    3:8,
                    4:2,
                    1001:2600,
                    1004:2800,
                    1005:22,
                    1006:2
                }


table_sizes_ALL={
                    'OCC':table_size_OCC,
                    'MSS':table_size_MSS,
                    'DPT':table_size_DPT
                }

gTableSizesDict=None


exception_codes={  0:'No_Error',
                1:'Function_Not_Defined',
                2:'Modbus_Table_Address_Not_Recognized',
                3:'Illegal_Value_For_Address'
             }

#
# Set of printable characters. Not used.
# 


##################################################################
def validateDataLength(tNum,received):
    global gTableSizesDict
    expected=gTableSizesDict.get(tNum,0)
    global gVerbosity
    if (expected!=received):
        if (gVerbosity>=1):
            print "--->ERROR--->--->--->ERROR--->--->--->"
            print "--->ERROR: received size for table %d incorrect. Expected:%d Received:%d" % (tNum,expected,received)
            print "--->ERROR--->--->--->ERROR--->--->--->"


##################################################################
def processReadResponse(tNum,data,exception=0):
    global gVerbosity

    datalen=len(data)
    if (gVerbosity>=1):
        if (datalen>512):
            if (gVerbosity>=5):
                print "Processing Read Response for table %d: %d bytes (%s...)" % (tNum,datalen,binascii.b2a_hex(data[0:16]))
            else:
                print "Processing Read Response for table %d: %d bytes" % (tNum,datalen)
        elif (exception):
            # Checking the datalen here is for silly buggers. Tcp will easily
            # read in more than the 6 bytes in an exception response in one
            # bite, so the buffer often contains more than just the currently
            # awaited response.
            # if (datalen!=1):
            #     print "ERROR: Read Exception Response data too long: %d bytes (%s)" % (datalen,binascii.b2a_hex(data))
            exceptioncode=int((struct.unpack('B',data[0]))[0])
            print "Processing Read EXCEPTION Response for table %d: %d (%s)" % (tNum,exceptioncode,exception_codes.get(exceptioncode,"KeyNotFound"))
            # don't validate the data length if an exception occurred, we
            # already know something went wrong. Also, the data len here should
            # be 1, when we'll be expecting 0.
            return
        else:
            print "Processing Read Response for table",tNum,"data",datalen,"bytes:",binascii.b2a_hex(data)

    validateDataLength(tNum,datalen)


##################################################################
def processWriteResponse(tNum,wdata):
    global gVerbosity
    global exception_codes
    # (responsevalue)=struct.unpack('B',wdata)
    # responsevalue=int(wdata)
    responsevalue=int((struct.unpack('B',wdata))[0])
    if (gVerbosity>=1):
        print "Processing Write Response for table %d. Response=%d (%s)" % (tNum,responsevalue,exception_codes.get(responsevalue,"KeyNotFound"))


##################################################################
def makeReadQuery(tableAddress):
    hexstr="04%0.4x" % tableAddress
    return hexstr


##################################################################
def makeWriteQuery(tNum):
    global gTableSizesDict
    length=gTableSizesDict.get(tNum,0)
    if (length==0):
        return None
    hexstr='10%0.4x%0.4x' % (tNum,length)
    charidx=0x20
    # deal with table 1006 (IscsWatchdog) specially
    if (tNum==1006):
        global gIscsWatchdog
        gIscsWatchdog+=1
        hexstr+='%0.4x' % gIscsWatchdog
    elif (tNum==1005): # IscsWash
        for x in range(length):
            hexstr+='00'
    else:
        for x in range(length):
            if charidx>0x7e:
                charidx=0x20
            hexstr+='%0.2x' % charidx
            charidx+=1
    return hexstr


##################################################################
def sendReadQuery(tNum,thesock):
    r=makeReadQuery(tNum)
    global gVerbosity
    if (gVerbosity>=1):
        print 'sending table',tNum,'read request:',r
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
                if (arg=='OCC') or (arg=='MSS') or (arg=='DPT'):
                    gLocation=arg
                else:
                    print "WARNING: location option '%s' not recognised, defaulting to %s" % (arg,gLocation)
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
def doReceive(thesock):
    global gReceivedBuffer
    longness=0
    must_continue=1
    while (must_continue):
        try:
            sys.stdout.flush()
            log('###################################################')
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
            print ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
            print "Socket Exception:",e
            print ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
            sys.exit(0)

        # print '-------------------------------'

        # 
        # calculate some introductory statistics about the packet
        responseType=(struct.unpack('B',gReceivedBuffer[0]))[0]
        tableAddress=socket.ntohs((struct.unpack('H',gReceivedBuffer[1:3]))[0])
        longness=len(gReceivedBuffer)

        if (gVerbosity>=6):
            print "@responseType=%x" % responseType
            print "@tableAddress=%d" % tableAddress
        ######################
        #
        #   WRITE RESPONSE
        #
        if (responseType==0x10) or (responseType==0x90):
            if (gVerbosity>=6):
                print "=>responseType=%x" % responseType
                print "=>tableAddress=%d" % tableAddress
            #
            # process a write reply packet.  These are always short
            # rdata=gReceivedBuffer[5:]
            processWriteResponse(tableAddress,gReceivedBuffer[3:])
            # trim the gReceivedBuffer data buffer
            # gReceivedBuffer=''
            trimmed=gReceivedBuffer[6:]
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
            # requested (plus 5 bytes)
            # 
            tableSize=socket.ntohs((struct.unpack('H',gReceivedBuffer[3:5]))[0])
            requiredDataLength=tableSize+5
            if (gVerbosity>=4):
                print "         <table:%d tableSize:%d expect:%d gReceivedBuffer:%d>" % (tableAddress,tableSize,requiredDataLength,longness)
            if (longness<requiredDataLength):
                # we don't yet have all the reply
                if (gVerbosity>=5):
                    print "   (Still receiving table %d, expecting:%d gReceivedBuffer:%d)" % (tableAddress,requiredDataLength,longness)
                continue
            elif (longness>requiredDataLength):
                # we have more than one reply
                if (gVerbosity>=5):
                    print "   (Received too much [required:%d, gReceivedBuffer:%d])" % (requiredDataLength,longness)
                enoughResponse=gReceivedBuffer[5:requiredDataLength]
                processReadResponse(tableAddress,enoughResponse)
                # trim the gReceivedBuffer data buffer
                # gReceivedBuffer=gReceivedBuffer[requiredDataLength:]
                # gReceivedBuffer=trimReceived(gReceivedBuffer,requiredDataLength)
                trimmed=gReceivedBuffer[requiredDataLength:]
                log('READ RESPONSE (EXCESS)')
                log('RECEIVED(%d):' % len(gReceivedBuffer)+binascii.b2a_hex(gReceivedBuffer))
                log('TRIMMED(%d):' % len(trimmed)+binascii.b2a_hex(trimmed))
                gReceivedBuffer=trimmed
                continue

            # else:
            if (gVerbosity>=6):
                print "=>responseType=%x" % responseType
                print "=>tableAddress=%d" % tableAddress
                print "=>tableSize=%d" % tableSize
                # print "Received table",tableAddress,"READ reply"
            rdata=gReceivedBuffer[5:]
            processReadResponse(tableAddress,rdata)
            # trim the gReceivedBuffer data buffer
            # gReceivedBuffer=gReceivedBuffer[requiredDataLength:]
            # gReceivedBuffer=trimReceived(gReceivedBuffer,requiredDataLength)
            trimmed=gReceivedBuffer[requiredDataLength:]
            log('READ RESPONSE')
            log('RECEIVED(%d):' % len(gReceivedBuffer)+binascii.b2a_hex(gReceivedBuffer))
            log('TRIMMED(%d):' % len(trimmed)+binascii.b2a_hex(trimmed))
            gReceivedBuffer=trimmed

        ######################
        #
        #   READ EXCEPTION
        #
        elif ( responseType == 0x84 ):
            tableSize=socket.ntohs((struct.unpack('H',gReceivedBuffer[3:5]))[0])
            requiredDataLength=tableSize+5
            if (gVerbosity>=4):
                print " READ EXCEPTION:"
            rdata=gReceivedBuffer[5:]
            processReadResponse(tableAddress,rdata,exception=1)
            # trim the gReceivedBuffer data buffer
            # gReceivedBuffer=gReceivedBuffer[requiredDataLength:]
            # gReceivedBuffer=trimReceived(gReceivedBuffer,requiredDataLength)
            trimmed=gReceivedBuffer[requiredDataLength:]
            log('READ RESPONSE')
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
    if not gWriteOnly:
        global gTableSizesDict
        for thekey in gTableSizesDict.keys():
            # only read Ats tables, ie. table "address" <1000
            if thekey<1000:
                sendReadQuery(thekey,thesock)
                doReceive(thesock)

##################################################################
def doWriteIscsTables(thesock):
    if not gReadOnly:
        global gTableSizesDict
        for thekey in gTableSizesDict.keys():
            # only write to Iscs tables, ie. table "address" >1000
            if thekey>1000:
                sendWriteQuery(thekey,thesock)
                doReceive(thesock)


##################################################################
def mainLoop(thesock):
    while (1):
        try:
            doWriteIscsTables(thesock)
            time.sleep(2)
            doReadAtsTables(thesock)
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
gTableSizesDict=table_sizes_ALL.get(gLocation,table_size_OCC)


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
