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
    CUR_DIR=$(dirname "${CUR_DIR}")
}

get_cur_dir

SRC_DIR=$CUR_DIR
DST_DIR=$CUR_DIR/gen
DST_DIR_CPP=$(dirname "${SRC_DIR}")/v2

echo SRC_DIR: $CUR_DIR
echo DST_DIR: $CUR_DIR/gen
echo DST_DIR_CPP: $DST_DIR_CPP

#C++
mkdir -p $DST_DIR_CPP
protoc -I=$SRC_DIR --cpp_out=$DST_DIR_CPP $SRC_DIR/*.proto

##JAVA
#mkdir -p $DST_DIR/java
#protoc -I=$SRC_DIR --java_out=$DST_DIR/java/ $SRC_DIR/*.proto
#
##PYTHON
#mkdir -p $DST_DIR/python
#protoc -I=$SRC_DIR --python_out=$DST_DIR/python/ $SRC_DIR/*.proto

##oc
#mkdir -p $DST_DIR/objc
#protoc -I=$SRC_DIR --objc_out=$DST_DIR/objc/ $SRC_DIR/*.proto

#js
#mkdir -p $DST_DIR/js

#$SRC_DIR/../bin/minipbjs  --keep-case --path $SRC_DIR/ PTP.Common.proto PTP.Buddy.proto PTP.Switch.proto PTP.Group.proto PTP.Auth.proto PTP.Msg.proto PTP.File.proto PTP.Other.proto PTP.Server.proto  --out $SRC_DIR/../../client/bd-apps/src/app/protobuf --name protobuf
