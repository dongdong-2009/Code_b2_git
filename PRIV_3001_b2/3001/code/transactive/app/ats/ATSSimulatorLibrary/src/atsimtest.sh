#!/bin/sh
HOSTNAME=`hostname`

if [ $# -lt 3 ] ; then
    BASENAME=`basename $0`
    echo ""
    echo "Usage: $BASENAME <location> <remote port> <local port>"
    echo ""
    echo "$BASENAME is a shortcut for"
    echo ""
    echo "python atsimtest.py --location=<location> --remote-addr=$HOSTNAME --remote-port=<remote port> --local-addr=$HOSTNAME --local-port=<local port> \$*"
    echo ""
    exit 1
fi


# shifting all our known parameters off allows us
# to pass the remaining args ('$*') in our call
LOCATION=$1 ; shift
REMOTE_PORT=$1 ; shift
LOCAL_PORT=$1 ; shift

# echo "python atsimtest.py --location=$1 --remote-addr=192.168.70.147 --remote-port=$2 --local-addr=192.168.70.147 --local-port=$3 --do-log"
# python atsimtest.py --location=$1 --remote-addr=192.168.70.147 --remote-port=$2 --local-addr=192.168.70.147 --local-port=$3 --do-log

echo "python atsimtest.py --location=$LOCATION --remote-addr=$HOSTNAME --remote-port=$REMOTE_PORT --local-addr=$HOSTNAME --local-port=$LOCAL_PORT $*"
python atsimtest.py --location=$LOCATION --remote-addr=$HOSTNAME --remote-port=$REMOTE_PORT --local-addr=$HOSTNAME --local-port=$LOCAL_PORT $*

