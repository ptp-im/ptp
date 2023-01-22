#!/bin/sh

CUR_DIR=
get_cur_dir() {
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
    SRC_DIR=$(dirname "${CUR_DIR}")
}

get_cur_dir
PTP_DIR=$SRC_DIR/../../../../
DST_DIR=$SRC_DIR/gen
DST_CPP_ACTIONS_DIR=$PTP_DIR/src/ptp_server/actions
DST_CPP_ACTIONS_CMD_DIR=$PTP_DIR/src/base/ptp_protobuf
DST_CPP_TEST_DIR=$PTP_DIR/tests/test_server/actions
DST_DIR_CPP=$(dirname "${SRC_DIR}")

PBJS_PATH=minipbjs

echo PTP_DIR: $PTP_DIR
echo SRC_DIR: $SRC_DIR
echo DST_DIR: $DST_DIR
echo DST_DIR_CPP: $DST_DIR_CPP
echo DST_CPP_TEST_DIR: $DST_CPP_TEST_DIR
echo DST_CPP_ACTIONS_DIR: $DST_CPP_ACTIONS_DIR
echo DST_CPP_ACTIONS_CMD_DIR: $DST_CPP_ACTIONS_CMD_DIR

#C++
protoc -I=$SRC_DIR --cpp_out=$DST_DIR_CPP $SRC_DIR/*.proto

##JAVA
mkdir -p $DST_DIR/java
protoc -I=$SRC_DIR --java_out=$DST_DIR/java/ $SRC_DIR/*.proto

#js
mkdir -p $DST_DIR/js
$PBJS_PATH  --keep-case --path $SRC_DIR/  --out $DST_DIR/js \
  --out-cpp-command-dir $DST_CPP_ACTIONS_CMD_DIR \
  --write-action-if-exists off --name protobuf --out-cpp $DST_CPP_ACTIONS_DIR --out-cpp-test $DST_CPP_TEST_DIR --gen-ts on --pb-dir $SRC_DIR
