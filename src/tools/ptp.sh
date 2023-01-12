#/bin/sh
#start or stop the server
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

function build_app() {
  APP_NAME=$1
  APP_PATH=$PTP_DIR/src/$APP_NAME
  cd $PTP_DIR
  cmake .
  cd $APP_PATH
  make
  echo " ===>  build $APP_PATH "
}

function print_usage() {
  echo "Usage: "
  		echo "  $0 build ptp_protobuf"
  		echo "  $0 build ptp_crypto"
  		echo "  $0 build ptp_global"
  		echo "  $0 build ptp_wallet"
  		echo "  $0 build ptp_toolbox"
  		echo "  $0 build ptp_net"
  		echo "  $0 build ptp_server"
  		echo "  $0 build tests"
  		echo "  $0 build ptp_server_business"
  		echo "  $0 build ptp_server_msg"
  		echo "  $0 build ptp_server_file ptp_server_route ptp_server_login ptp_server_push ptp_server_ws ptp_server_msfs"
  		echo "  $0 cmake"
  		echo "  $0 build_all"
  		echo "  $0 build_server"
  		echo "  $0 build_tools"
  		echo "  $0 clean_ptp"
  		echo "  $0 build_tests"
  		echo "  $0 run_tests"
  		echo "  $0 run_test"
  		echo "  $0 clean_tests"
}

function run_tests() {
  cd $PTP_DIR
  cmake .
  cd $PTP_DIR/src/tests
  make
  cd $PTP_DIR/build/bin/tests
  tests=$(ls *.run | tr " " "\n")
  for test in $tests
  do
    chmod +x $test
    ./$test
  done

}

function build() {
  servers_str=$@
  if [ "$servers_str" == "build" ]; then
    print_usage
  fi
  servers=$(echo $servers_str | tr " " "\n")
  for server in $servers
  do
    if [ "$server" != "build" ]; then
      name="${server}"
      build_app $name
    fi
  done
}

function run_test() {
  testName=$2
  if [ "$2" == "" ]; then
    ls -al $PTP_DIR/build/bin/tests/
    echo "[WARN] enter test name!!"
  else
    echo "run $testName"
    cd $PTP_DIR/build/bin/tests/
    ./$testName
  fi
}


# Change the current directory to the location of the script
CUR_DIR=$(dirname "${REALPATH}")
cd $CUR_DIR
cd ../../
PTP_DIR=$PWD
echo CUR_DIR: $CUR_DIR
echo PTP_DIR: $PTP_DIR
echo SYSTEM: $SYSTEM
echo "========"

case $1 in
	build)
		build $@
  ;;
	clean_ptp)
    cd $PTP_DIR
    sh clean.sh
  ;;
	build_all)
		cd $PTP_DIR
		cmake .
		make
  ;;
	build_server)
		build ptp_server
  ;;
	cmake)
		cd $PTP_DIR
		cmake .
  ;;
	build_tests)
		build_app tests
  ;;
	run_tests)
		run_tests
  ;;
	run_test)
		run_test $@
  ;;
	clean_tests)
		rm -rf $PTP_DIR/build/bin/tests
  ;;
	build_tools)
		build tools
  ;;
	*)
		print_usage
		;;
esac
