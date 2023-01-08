#!/bin/bash
# author: barry
# date: 04/11/2021
LOG4CXX=apache-log4cxx-0.10.0
PROTOBUF=protobuf-3.6.1
CRYPTOPP=cryptopp850
HIREDIS=hiredis-master
SRC_DIR=
OS=`uname -s`

if [ $OS" " ==  "Darwin"" " ];then
  SYSTEM="mac"
else
  SYSTEM="linux"
fi

check_user() {
    if [ $(id -u) != "0" ]; then
        if [ $SYSTEM != "mac" ]; then
          echo "Error: You must be root to run this script, please use root to install BangDeTalk 1.0"
          exit 1
        fi
    fi
}

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
    PACKAGE_DIR=$(dirname "${SRC_DIR}")/package
    echo PACKAGE_DIR: $PACKAGE_DIR
    BUILD_DIR=$(dirname "${SRC_DIR}")/build
    echo BUILD_DIR: $BUILD_DIR
    LIBS_DIR=$(dirname "${SRC_DIR}")/libs/$SYSTEM
    echo LIBS_DIR: $LIBS_DIR

}

check_dev_lib(){
  echo "start installing the development library.."
  echo "=========================================="
  sleep 1s
  cmake --version
  echo "==========================================="
  sleep 1s
}

check_sys_var(){
    echo "============================"
    echo "Welcome to BangDeTalk 1.0"
    echo "============================"
    sleep 1s
    if [ $SYSTEM != "mac" ]; then
      CENTOS_VERSION=$(less /etc/redhat-release)
      echo "start checking the run environment...."
      sleep 1s
      if [[ $CENTOS_VERSION =~ "7." ]]; then
          echo "environmental inspection passed.";
          sleep 1s
          echo "==================================";
          echo "start installing BangDeTalk 1.0...";
          echo "==================================";
          sleep 1s
          check_dev_lib;
      else
          echo "centos version must >=7.0 !!!";
          exit 1;
      fi
    fi
}

build_log4cxx(){
    echo "=================================================="
    echo "start installing the development library [log4cxx]"
    echo "=================================================="
    sleep 3s
    rm -rf $BUILD_DIR/log4cxx $BUILD_DIR/$LOG4CXX
    mkdir -p $BUILD_DIR/log4cxx
    tar -xf $PACKAGE_DIR/$LOG4CXX.tar.gz -C $BUILD_DIR
    cd $BUILD_DIR/$LOG4CXX

    if [ $SYSTEM ==  "mac" ];then
      ./configure --prefix=$BUILD_DIR/log4cxx --with-apr=/usr/local/opt/apr/bin/apr-1-config --with-apr-util=/usr/local/opt/apr-util/bin/apu-1-config --with-charset=utf-8 --with-logchar=utf-8
      cp $SRC_DIR/log4cxx/simpledateformat.h ./src/main/include/log4cxx/helpers
    else
      ./configure --prefix=$BUILD_DIR/log4cxx --with-apr=/usr --with-apr-util=/usr --with-charset=utf-8 --with-logchar=utf-8
    fi

    cp $SRC_DIR/log4cxx/inputstreamreader.cpp     ./src/main/cpp/
    cp $SRC_DIR/log4cxx/socketoutputstream.cpp    ./src/main/cpp/
    cp $SRC_DIR/log4cxx/console.cpp               ./src/examples/cpp/
    make
    make install

    mkdir -p $LIBS_DIR/slog/lib/
    cp -rf $BUILD_DIR/log4cxx/include             $LIBS_DIR/slog/

    if [ $SYSTEM ==  "mac" ];then
      cp -f $BUILD_DIR/log4cxx/lib/liblog4cxx.*   $LIBS_DIR/slog/lib/
    else
      cp -f $BUILD_DIR/log4cxx/lib/liblog4cxx.so* $LIBS_DIR/slog/lib/
    fi

    if [ $? -eq 0 ]; then
      echo "==================================================="
      echo "development library installation complete.[log4cxx]"
      echo "==================================================="
      sleep 1s
    else
        echo "make [log4cxx] failed";
        exit;
    fi
}

build_protobuf(){
    echo "=================================================="
    echo "start installing the development library [protobuf]"
    echo "=================================================="
    sleep 1s
    rm -rf $BUILD_DIR/protobuf $BUILD_DIR/$PROTOBUF
    mkdir -p $BUILD_DIR/protobuf
    tar -xvf $PACKAGE_DIR/$PROTOBUF.tar.gz -C $BUILD_DIR
    cd $BUILD_DIR/$PROTOBUF
    make clean
    ./configure --prefix=$BUILD_DIR/protobuf
    make
    make install

    mkdir -p $LIBS_DIR/base/pb/lib/$SYSTEM/
    cp $BUILD_DIR/protobuf/lib/libprotobuf-lite.a $LIBS_DIR/base/pb/lib/$SYSTEM/
    cp  -r $BUILD_DIR/protobuf/include/* $LIBS_DIR/base/pb/
    if [ $? -eq 0 ]; then
      echo "==================================================="
      echo "development library installation complete.[protobuf]"
      echo "==================================================="
      sleep 1s
    else
        echo "make [protobuf] failed";
        exit;
    fi
    sleep 1s
}

build_hiredis(){
    echo "=================================================="
    echo "start installing the development library [hiredis]"
    echo "=================================================="
    sleep 1s
    rm -rf $ $BUILD_DIR/$HIREDIS
    mkdir -p $LIBS_DIR/hiredis
    unzip $PACKAGE_DIR/$HIREDIS.zip -d $BUILD_DIR
    cd $BUILD_DIR/$HIREDIS
    make
    cp -a libhiredis.a $LIBS_DIR/hiredis
    cp -a hiredis.h async.h read.h sds.h adapters $LIBS_DIR/hiredis
    if [ $? -eq 0 ]; then
      echo "==================================================="
      echo "development library installation complete.[hiredis]"
      echo "==================================================="
      sleep 1s
    else
        echo "make [hiredis] failed";
        exit;
    fi
    sleep 1s
}

make_clean(){
  rm -rf CMakeFiles
  rm -rf Makefile
  rm -rf CMakeCache.txt
  rm -rf cmake_install.cmake
}

build_security(){
    echo "=================================================="
    echo "start installing the development library [security]"
    echo "=================================================="
    sleep 1s
    mkdir -p $LIBS_DIR/base/security/lib/$SYSTEM
    rm -rf $BUILD_DIR/libsecurity
    cp -a $PACKAGE_DIR/libsecurity $BUILD_DIR/libsecurity
    cd $BUILD_DIR/libsecurity/unix
    make_clean
    rm -rf libsecurity.a
    cmake ../src
    make
    cp libsecurity.a $LIBS_DIR/base/security/lib/$SYSTEM

    if [ $? -eq 0 ]; then
      echo "==================================================="
      echo "development library installation complete.[security]"
      echo "==================================================="
      sleep 1s
    else
        echo "make [security] failed";
        exit;
    fi
    sleep 1s
}

build_cryptopp(){
    echo "=================================================="
    echo "start installing the development library [cryptopp]"
    echo "=================================================="
    sleep 1s
    rm -rf $ $BUILD_DIR/$CRYPTOPP
    mkdir -p $LIBS_DIR/cryptopp/lib
    mkdir -p $LIBS_DIR/cryptopp/cryptopp
    unzip $PACKAGE_DIR/$CRYPTOPP.zip -d $BUILD_DIR/$CRYPTOPP
    cd $BUILD_DIR/$CRYPTOPP
    make clean
    make libcryptopp.a
    make test

    cp -a libcryptopp.a $LIBS_DIR/cryptopp/lib
    cp *.h $LIBS_DIR/cryptopp/cryptopp

    if [ $? -eq 0 ]; then
      echo "==================================================="
      echo "development library installation complete.[cryptopp]"
      echo "==================================================="
      sleep 1s
    else
        echo "make [cryptopp] failed";
        exit;
    fi
    sleep 1s
}
print_help() {
  echo "BangDeTalk 1.0"
	echo "Usage: "
	echo "  1. sh $0 build log4cxx  --- build spec lib: cryptopp | security | log4cxx | protobuf | hiredis"
	echo "  2. sh $0 build all      --- build all lib"
}

case $1 in
	build)
    if [ $# != 2 ]; then
			print_help
			exit
		fi
		check_user
    get_cur_dir
    check_sys_var
	  echo "start compiling BangDeTalk 1.0..."
    echo "=================================="
		echo $2

    if [ $2 ==  "log4cxx" ];then
      build_log4cxx
    fi
    if [ $2 ==  "cryptopp" ];then
      build_cryptopp
    fi
    if [ $2 ==  "protobuf" ];then
      build_protobuf
    fi
    if [ $2 ==  "hiredis" ];then
      build_hiredis
    fi
    if [ $2 ==  "security" ];then
      build_security
    fi

    if [ $2 ==  "all" ];then
      build_log4cxx
      build_cryptopp
      build_hiredis
      build_protobuf
      build_security
    fi
		;;
	*)
		print_help
		;;
esac
