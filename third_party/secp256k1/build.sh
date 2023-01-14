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
CUR_DIR=$(dirname "${REALPATH}")
cd $CUR_DIR
cd ../../
PTP_DIR=$PWD

echo CUR_DIR: $CUR_DIR
echo PTP_DIR: $PTP_DIR
echo SYSTEM: $SYSTEM
echo "========"

echo "=================================================="
echo "start installing the development library [secp256k1]"
echo "=================================================="
cd $CUR_DIR
rm -rf build
mkdir -p build
unzip $CUR_DIR/secp256k1.zip -d build
cd build/secp256k1
./autogen.sh
./configure --prefix=/usr/local --enable-static=yes --enable-tests=no --enable-module-ecdh --enable-module-recovery --enable-module-extrakeys --enable-module-schnorrsig --disable-benchmark
make
make install
make clean
rm -rf $CUR_DIR/build

if [ $? -eq 0 ]; then
  echo "==================================================="
  echo "development library installation complete.[secp256k1]"
  echo "==================================================="
  sleep 1s
else
    echo "make [secp256k1] failed";
    exit;
fi
