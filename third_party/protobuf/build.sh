SRC_DIR=
BUILD_DIR=
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
PROTOBUF=protobuf-3.6.1
CUR_DIR=$(dirname "${REALPATH}")
cd $CUR_DIR
cd ../../
PTP_DIR=$PWD

echo CUR_DIR: $CUR_DIR
echo PTP_DIR: $PTP_DIR
echo SYSTEM: $SYSTEM
echo "========"

echo "=================================================="
echo "start installing the development library [protobuf]"
echo "=================================================="
cd $CUR_DIR
rm -rf build
mkdir build build/protobuf
tar -xvf $CUR_DIR/$PROTOBUF.tar.gz -C build
cd build/$PROTOBUF
./configure --prefix=$CUR_DIR/build/protobuf
make
make install

cp $CUR_DIR/build/protobuf/lib/libprotobuf-lite.a $PTP_DIR/build/lib
cp -r $CUR_DIR/build/protobuf/include/* $PTP_DIR/include/protobuf

if [ $? -eq 0 ]; then
  echo "==================================================="
  echo "development library installation complete.[protobuf]"
  echo "==================================================="
  sleep 1s
else
    echo "make [protobuf] failed";
    exit;
fi
