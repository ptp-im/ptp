#!/usr/bin/env bash
#start or stop the server
apps="business file push route msfs msg login"
#apps="business file push route msfs ws msg login"
apps1="business|file|push|route|msfs|ws|msg|login"

CUR_DIR=
PTP_DIR=
VERSION=$2
OS=`uname -s`
if [ $OS" " ==  "Darwin"" " ];then
  SYSTEM="mac"
else
  SYSTEM="linux"
fi

# Get the fully qualified path to the script
case $0 in
    /*)
        SCRIPT="$0"
        ;;
    *)
        PWD_DIR=$(pwd);
        SCRIPT="${PWD_DIR}/$0"
        ;;
esac
# Resolve the true real path without any sym links.
CHANGED=true
while [ "X$CHANGED" != "X" ]
do
    # Change spaces to ":" so the tokens can be parsed.
    SAFESCRIPT=`echo $SCRIPT | sed -e 's; ;:;g'`
    # Get the real path to this script, resolving any symbolic links
    TOKENS=`echo $SAFESCRIPT | sed -e 's;/; ;g'`
    REALPATH=
    for C in $TOKENS; do
        # Change any ":" in the token back to a space.
        C=`echo $C | sed -e 's;:; ;g'`
        REALPATH="$REALPATH/$C"
        # If REALPATH is a sym link, resolve it.  Loop for nested links.
        while [ -h "$REALPATH" ] ; do
            LS="`ls -ld "$REALPATH"`"
            LINK="`expr "$LS" : '.*-> \(.*\)$'`"
            if expr "$LINK" : '/.*' > /dev/null; then
                # LINK is absolute.
                REALPATH="$LINK"
            else
                # LINK is relative.
                REALPATH="`dirname "$REALPATH"`""/$LINK"
            fi
        done
    done

    if [ "$REALPATH" = "$SCRIPT" ]
    then
        CHANGED=""
    else
        SCRIPT="$REALPATH"
    fi
done
# Change the current directory to the location of the script
CUR_DIR=$(dirname "${REALPATH}")
if [ -e $CUR_DIR/build ]; then
  PTP_DIR=$CUR_DIR/build
else
  PTP_DIR=$CUR_DIR
fi

BIN_DIR=$PTP_DIR/bin
echo $0
echo CUR_DIR: $CUR_DIR
echo PTP_DIR: $PTP_DIR
echo BIN_DIR: $BIN_DIR
echo SYSTEM: $SYSTEM
echo "========================================"

function run() {
    name="ptp_server_$1"
    if [ -e $BIN_DIR/$name  ]; then
      echo start... $name
      cd $BIN_DIR/$name
      killall $name
      if [ -e server.pid  ]; then
          pid=`cat server.pid`
          echo "kill pid=$pid"
          kill -9 $pid
          ./$name
      else
          ./$name
      fi
      sleep 1
    else
      echo " ===>  $name not exists"
    fi
}


function daeml_server() {
    name="ptp_server_$1"
    if [ -e $BIN_DIR/$name  ]; then
      echo start... $name
      cd $BIN_DIR/$name
      killall $name
      if [ -e server.pid  ]; then
          pid=`cat server.pid`
          echo "kill pid=$pid"
          kill -9 $pid
          ../daeml ./$name
      else
          ../daeml ./$name
      fi
    else
      echo " ===>  $name not exists"
    fi

}

function stop_server() {
    name="ptp_server_$1"
    if [ -e $BIN_DIR/$name  ]; then
      echo stop... $name
      cd $BIN_DIR/$name
      killall $name
      if [ -e server.pid  ]; then
          pid=`cat server.pid`
          echo "kill pid=$pid"
          kill -9 $pid
          rm -rf server.pid
      fi
    else
      echo " ===>  $name not exists"
    fi

}

function look_server() {
    name="ptp_server_$1"
    if [ -e $BIN_DIR/$name  ]; then
      cd $BIN_DIR/$name
      pid="     "
      if [ -e server.pid  ]; then
          pid=`cat server.pid`
          process_count=`ps aux | grep $name |grep $pid | wc -l`
      else
        process_count=`ps aux | grep $name | wc -l`
      fi
      echo "pid=$pid, process_cnt=$process_count $name"
      look_server $1
    else
      echo " ===>  $name not exists"
    fi

}
function daeml() {
  servers_str=$@
  if [ "$servers_str" == "daeml" ]; then
    servers_str=$apps
  fi
  servers=$(echo $servers_str | tr " " "\n")
  for server in $servers
  do
      if [ "$server" != "daeml" ]; then
        daeml_server $server
      fi
  done
}

function look() {
  servers_str=$@
  if [ "$servers_str" == "look" ]; then
    servers_str=$apps
  fi
  servers=$(echo $servers_str | tr " " "\n")
  for server in $servers
  do
      if [ "$server" != "look" ]; then
        name="ptp_server_${server}"
        if [ -e $BIN_DIR/$name  ]; then
          cd $BIN_DIR/$name
          pid="     "
          if [ -e server.pid  ]; then
              pid=`cat server.pid`
              process_count=`ps aux | grep $name |grep $pid | wc -l`
          else
            process_count=`ps aux | grep $name | wc -l`
          fi
          echo "pid=$pid, process_cnt=$process_count $name"
        else
          echo " ===>  $name not exists"
        fi
      fi
  done
  echo "===================="
  sleep 2
  look $@
}

function stop() {
  servers_str=$@
  if [ "$servers_str" == "stop" ]; then
    servers_str=$apps
  fi
  servers=$(echo $servers_str | tr " " "\n")
  for server in $servers
  do
      if [ "$server" != "stop" ]; then
        stop_server $server
      fi
  done
}

case $1 in
	run)
		run $2
  ;;
	daeml)
		daeml $@
  ;;
	stop)
		stop $@
  ;;
	log)
		log $@
  ;;
	look)
		look $@
  ;;
	look_server)
		look_server $2
  ;;
	log_clean)
		log_clean $@
  ;;
	*)
		echo "Usage: "
		echo "  $0 run msg"
		echo "  $0 daeml msg"
		echo "  $0 stop msg"
		echo "  $0 look msg"
		;;
esac
