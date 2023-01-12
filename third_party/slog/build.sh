SRC_DIR=
BUILD_DIR=
LOG4CXX=apache-log4cxx-0.10.0
LOG4CXX=apache-log4cxx-1.0.0
OS=`uname -s`

if [ $OS" " ==  "Darwin"" " ];then
  SYSTEM="mac"
else
  SYSTEM="linux"
fi

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

    SRC_DIR=$(dirname "${REALPATH}")
    echo SRC_DIR: $SRC_DIR
    BUILD_DIR=$SRC_DIR/build
    echo BUILD_DIR: $BUILD_DIR
}


echo "=================================================="
echo "start installing the development library [log4cxx]"
echo "=================================================="

get_cur_dir

rm -rf $BUILD_DIR/log4cxx $BUILD_DIR/$LOG4CXX
mkdir -p $BUILD_DIR/log4cxx $SRC_DIR/../../include $SRC_DIR/../../build/lib
tar -xf $SRC_DIR/$LOG4CXX.tar.gz -C $BUILD_DIR

cd $BUILD_DIR/$LOG4CXX

#if [ $SYSTEM ==  "mac" ];then
  #./configure --prefix=$BUILD_DIR/log4cxx --with-apr=/usr/local/opt/apr/bin/apr-1-config --with-apr-util=/usr/local/opt/apr-util/bin/apu-1-config --with-charset=utf-8 --with-logchar=utf-8
  #cp $SRC_DIR/log4cxx-fix/simpledateformat.h ./src/main/include/log4cxx/helpers
#else
#  ./configure --prefix=$BUILD_DIR/log4cxx --with-apr=/usr --with-apr-util=/usr --with-charset=utf-8 --with-logchar=utf-8
#fi

#cp $SRC_DIR/log4cxx-fix/inputstreamreader.cpp     ./src/main/cpp/
#cp $SRC_DIR/log4cxx-fix/socketoutputstream.cpp    ./src/main/cpp/
#cp $SRC_DIR/log4cxx-fix/console.cpp               ./src/examples/cpp/
#make
#make install
cmake .
make
sudo make install
#
#if [ $SYSTEM ==  "mac" ];then
#  cp -fa $BUILD_DIR/log4cxx/lib/liblog4cxx.*   $SRC_DIR/../../build/lib
#else
#  cp -fa $BUILD_DIR/log4cxx/lib/liblog4cxx.so* $SRC_DIR/../../build/lib
#fi
#rm -rf $SRC_DIR/../../include/log4cxx
#mkdir $SRC_DIR/../../include
#cp -a $BUILD_DIR/log4cxx/include/log4cxx $SRC_DIR/../../include
#
#cd $SRC_DIR
#cmake .
#make

#rm -rf $BUILD_DIR

if [ $? -eq 0 ]; then
  echo "==================================================="
  echo "development library installation complete.[log4cxx]"
  echo "==================================================="
  sleep 1s
else
    echo "make [log4cxx] failed";
    exit;
fi