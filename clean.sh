#!/bin/bash


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

CUR_DIR=$(dirname "${REALPATH}")
echo $CUR_DIR;
clean_dir() {
  CMAKE_DIR=$1
  echo "clean $CMAKE_DIR"
  cd $CMAKE_DIR
  rm -rf CMakeCache.txt CMakeFiles CTestTestfile.cmake cmake_install.cmake Testing Makefile cmake-build-debug
  rm -rf lib build include
}

rm -rf CMakeCache.txt CMakeFiles CTestTestfile.cmake cmake_install.cmake Testing Makefile cmake-build-debug

clean_dir $CUR_DIR/src/ptp_crypto
clean_dir $CUR_DIR/src/ptp_global
clean_dir $CUR_DIR/src/ptp_net
clean_dir $CUR_DIR/src/ptp_protobuf
clean_dir $CUR_DIR/src/ptp_server
clean_dir $CUR_DIR/src/ptp_server_business
clean_dir $CUR_DIR/src/ptp_server_login
clean_dir $CUR_DIR/src/ptp_server_msfs
clean_dir $CUR_DIR/src/ptp_server_msg
clean_dir $CUR_DIR/src/ptp_server_push
clean_dir $CUR_DIR/src/ptp_server_route
clean_dir $CUR_DIR/src/ptp_server_ws
clean_dir $CUR_DIR/src/ptp_toolbox
clean_dir $CUR_DIR/src/ptp_wallet
clean_dir $CUR_DIR/src/tests
clean_dir $CUR_DIR/src/tools
clean_dir $CUR_DIR/third_party/slog

cd $CUR_DIR/third_party/protobuf
rm -rf build
cd $CUR_DIR/third_party/redis
rm -rf build
