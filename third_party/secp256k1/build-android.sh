#!/bin/bash
export
set -e

if [ -z "$ANDROID_NDK" ]; then
   echo "export the ANDROID_NDK environment variable"
   exit 1
fi

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

# Only choose one of these, depending on your build machine...
export TOOLCHAIN=$ANDROID_NDK/toolchains/llvm/prebuilt/darwin-x86_64
#export TOOLCHAIN=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64

# create links to some toolchains binaries (https://github.com/android/ndk/issues/1324)
cd $TOOLCHAIN/bin/
for source in arm-linux-androideabi-*
do
    dest=${source/arm/armv7a}
    ln -sf $source $dest
done
cd -

# Set this to your minSdkVersion.
export API=21

pwd=`pwd`

buildit()
{
    abi=$1
    target=$2

    echo ""
    echo "-------------------------------------------------------------------------------"
    echo " Compiling for $abi"
    echo "-------------------------------------------------------------------------------"

    export TARGET=$target

    # Configure and build.
    export AR=$TOOLCHAIN/bin/$TARGET-ar
    export AS=$TOOLCHAIN/bin/$TARGET-as
    export CC=$TOOLCHAIN/bin/$TARGET$API-clang
    export CXX=$TOOLCHAIN/bin/$TARGET$API-clang++
    export LD=$TOOLCHAIN/bin/$TARGET-ld
    export RANLIB=$TOOLCHAIN/bin/$TARGET-ranlib
    export STRIP=$TOOLCHAIN/bin/$TARGET-strip
    mkdir -p ${PTP_DIR}/third_party/libs/android
    ./autogen.sh
    ./configure --prefix="${PTP_DIR}/third_party/libs/android/$abi" --enable-static=no --enable-tests=no --enable-module-ecdh --enable-module-recovery --enable-module-extrakeys --enable-module-schnorrsig --host=$TARGET --disable-benchmark
    make clean
    make
    make install
}

cd $CUR_DIR
rm -rf build
mkdir -p build
unzip $CUR_DIR/secp256k1.zip -d build
cd build/secp256k1

buildit armeabi-v7a armv7a-linux-androideabi
buildit arm64-v8a   aarch64-linux-android
buildit x86         i686-linux-android
buildit x86_64      x86_64-linux-android

make clean

echo "done. the files are in the android folder"