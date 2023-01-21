#!/usr/bin/env bash
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

function run_backend() {
  app=$2
  if [ "$app" == "" ]; then
      cd $BIN_DIR/
      apps=$(ls ./ | tr " " "\n")
      for app in $apps
      do
        if [[ $app == *"."* ]]; then
          continue
        fi
        if [[ $app == "daeml" ]]; then
          continue
        fi
        echo $0 "run_backend $app"
      done
      cd $PTP_DIR
  else
    echo start... $app
    cd $BIN_DIR
    killall $app
    if [ -e "/tmp/$app.pid"  ]; then
        pid=`cat /tmp/$app.pid`
        echo "kill pid=$pid"
        kill -9 $pid
        ./daeml ./$app
    else
        ./daeml ./$app
    fi
  fi
}

function run() {
  app=$2
  if [ "$app" == "" ]; then
      cd $BIN_DIR/
      apps=$(ls ./ | tr " " "\n")
      for app in $apps
      do
        if [[ $app == *"."* ]]; then
          continue
        fi
        if [[ $app == "daeml" ]]; then
          continue
        fi

        if [[ $app == "conf" ]]; then
          continue
        fi
        if [[ $app == "log" ]]; then
          continue
        fi
        echo $0 "run $app"
      done
      cd $PTP_DIR
  else
    echo start... $app
    cd $BIN_DIR
    killall $app
    if [ -e "/tmp/$app.pid"  ]; then
        pid=`cat /tmp/$app.pid`
        echo "kill pid=$pid"
        kill -9 $pid
        ./$app
    else
        ./$app
    fi
  fi
}

function stop_server() {
  app=$2
  if [ "$app" == "" ]; then
    cd $BIN_DIR/
    apps=$(ls ./ | tr " " "\n")
    for app in $apps
    do
      if [[ $app == *"."* ]]; then
        continue
      fi
      if [[ $app == "daeml" ]]; then
        continue
      fi
      echo $0 "stop $app"
    done
    cd $PTP_DIR
  else
    echo stop... $app
    cd $BIN_DIR
    killall $app
    if [ -e "/tmp/$app.pid"  ]; then
      pid=`cat /tmp/$app.pid`
      echo "kill pid=$pid"
      kill -9 $pid
    fi
  fi
}

function look() {
  app=$2
  if [ "$app" == "" ]; then
      cd $BIN_DIR/
      apps=$(ls ./ | tr " " "\n")
      for app in $apps
      do
        if [[ $app == *"."* ]]; then
          continue
        fi
        if [[ $app == "daeml" ]]; then
          continue
        fi
        echo $0 "look $app"
      done
      cd $PTP_DIR
  else
    cd $BIN_DIR
    pid="     "
    if [ -e /tmp/$app.pid  ]; then
        pid=`cat /tmp/$app.pid`
        process_count=`ps aux | grep $app |grep $pid | wc -l`
    else
      process_count=`ps aux | grep $app | wc -l`
    fi
    echo "pid=$pid, process_cnt=$process_count $app"
    sleep 1
    look $@
  fi
}

case $1 in
	run)
		run $@
  ;;
	run_backend)
		run_backend $@
  ;;
	stop)
		stop_server $@
  ;;
	look)
		look $@
  ;;
	*)
		echo "Usage: "
		echo "  $0 run"
		echo "  $0 run_backend"
		echo "  $0 stop"
		echo "  $0 look"
		;;
esac
