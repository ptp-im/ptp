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
PTP_DIR=$CUR_DIR
echo $0
echo PTP_DIR: $PTP_DIR
echo SYSTEM: $SYSTEM
echo "========================================"

function build_app() {
  APP_NAME=$1
  APP_PATH=$PTP_DIR/src/$APP_NAME
  cd $PTP_DIR
  cmake .
  cd $APP_PATH
  make
  echo " ===>  build $APP_PATH "
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
      cd $PTP_DIR/build/bin/tests
      tests=$(ls *.run | tr " " "\n")
      for test in $tests
      do
        echo $0 "run_test $test"
      done
      cd $PTP_DIR
  else
    echo "run $testName"
    cd $PTP_DIR/build/bin/tests/
    ./$testName
  fi
}

function build_test() {
  testName=$2
  if [ "$2" == "" ]; then
      cd $PTP_DIR/tests/
      tests=$(ls ./ | grep test_ | tr " " "\n")
      for test in $tests
      do
        echo $0 "build_test $test"
      done
      cd $PTP_DIR
  else
    echo "build $testName"
      cd $PTP_DIR
      cmake .
      cd $PTP_DIR/tests/$testName
      make
      cd $PTP_DIR/build/bin/tests/
      ./$testName.run
  fi
}

clean_dir() {
  CMAKE_DIR=$1
  echo "clean $CMAKE_DIR"
  cd $CMAKE_DIR
  rm -rf CMakeCache.txt CMakeFiles CTestTestfile.cmake cmake_install.cmake Testing Makefile cmake-build-debug
  rm -rf build
}


function run_server() {
  name=$2
  if [ "$2" == "" ]; then
      echo "$0 run msg"
      echo "$0 run business"
      cd $PTP_DIR
  else
    $PTP_DIR/ptp-server.sh run $name
  fi
}

function clean_ptp() {
  cd $CUR_DIR
      rm -rf CMakeCache.txt CMakeFiles CTestTestfile.cmake cmake_install.cmake Testing Makefile cmake-build-debug

      clean_dir $CUR_DIR/tests
      dirs=$(ls $CUR_DIR/tests | tr " " "\n")
      for dir in $dirs
      do
        if [ -d $CUR_DIR/tests/$dir ]; then
              clean_dir $CUR_DIR/tests/$dir
        fi
      done
      dirs=$(ls $CUR_DIR/src | tr " " "\n")
      for dir in $dirs
      do
        if [ -d $CUR_DIR/src/$dir ]; then
              clean_dir $CUR_DIR/src/$dir
        fi
      done
      clean_dir $CUR_DIR/third_party/gtest
      clean_dir $CUR_DIR/third_party/gtest/googlemock
      clean_dir $CUR_DIR/third_party/gtest/googletest
      cd $CUR_DIR/third_party/protobuf
      rm -rf build
      cd $CUR_DIR/third_party/redis
      rm -rf build
      cd $CUR_DIR/third_party/log4cxx
      rm -rf build
      cd $CUR_DIR
      rm -rf build
}

function run_redis_dev_server() {
  cd /opt && sudo redis-server --appendonly yes --daemonize yes --requirepass s9mE_s3cUrE_prsS
  cd $CUR_DIR
  ps aux |  grep redis-server
}
function print_usage() {
  echo "Usage: "
  		echo "  $0 build_docker"
  		echo "  $0 run_docker"
  		echo "  $0 clean_tests"
  		echo "  $0 clean_ptp"
  		echo "  $0 cmake"
  		echo "  $0 run_redis_dev_server"
  		echo "  $0 init_dev"
  		echo "  $0 build_all"
  		echo "  $0 build tools"
  		echo "  $0 build_tests"
  		echo "  $0 run_tests"
  		echo "  $0 build ptp_protobuf"
  		echo "  $0 build ptp_crypto"
  		echo "  $0 build ptp_global"
  		echo "  $0 build ptp_wallet"
  		echo "  $0 build ptp_server"
  		echo "  $0 build ptp_server_msg"
  		echo "  $0 build_test"
  		echo "  $0 run_test"
  		echo "  $0 run"
  		echo "  $0 run msg"
}


case $1 in
	build)
		# shellcheck disable=SC2068
		build $@
  ;;
	clean_ptp)
    clean_ptp
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
	build_test)
		# shellcheck disable=SC2068
		build_test $@
  ;;
	run_tests)
		run_tests
  ;;
	run_test)
		# shellcheck disable=SC2068
		run_test $@
  ;;
	run)
		# shellcheck disable=SC2068
		run_server $@
  ;;
	clean_tests)
		rm -rf $PTP_DIR/build/bin/tests
  ;;
	build_tools)
		build tools
  ;;
	init_dev)
    if [ $SYSTEM"" ==  "linux" ];then
      sudo cp $CUR_DIR/third_party/libs/linux/* /usr/local/lib
      run_redis_dev_server >> /tmp/init_redis.log
    fi
  ;;
	run_redis_dev_server)
    run_redis_dev_server
  ;;
	build_docker)
		docker build -t ptp-cpp:latest -f Dockerfile ./
		docker run -it ptp-cpp:latest bash
  ;;
	run_docker)
		docker run -it ptp-cpp:latest bash
  ;;
	*)
		print_usage
		;;
esac
