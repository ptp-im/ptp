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
HIREDIS=hiredis-master
CUR_DIR=$(dirname "${REALPATH}")
cd $CUR_DIR
cd ../../
PTP_DIR=$PWD

echo CUR_DIR: $CUR_DIR
echo PTP_DIR: $PTP_DIR
echo SYSTEM: $SYSTEM
echo "========"

echo "=================================================="
echo "start installing the development library [redis]"
echo "=================================================="
cd $CUR_DIR
rm -rf build
mkdir -p build $PTP_DIR/build/lib $PTP_DIR/include/redis
unzip $CUR_DIR/$HIREDIS.zip -d build
cd build/$HIREDIS
make

cp -a libhiredis.a $PTP_DIR/build/lib
cp -a hiredis.h async.h read.h sds.h adapters $PTP_DIR/include/redis

if [ $? -eq 0 ]; then
  echo "==================================================="
  echo "development library installation complete.[redis]"
  echo "==================================================="
  sleep 1s
else
    echo "make [redis] failed";
    exit;
fi
