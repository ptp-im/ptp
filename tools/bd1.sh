#!/bin/bash
SRC_DIR=
VERSION=
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
    echo REALPATH: $REALPATH
    SRC_DIR=$(dirname "${REALPATH}")
    echo SRC_DIR: $SRC_DIR
    LIBS_DIR=$(dirname "${SRC_DIR}")/libs/$SYSTEM
    echo LIBS_DIR: $LIBS_DIR
    RUN_DIR=$(dirname "${SRC_DIR}")/run/$SYSTEM/$VERSION
    echo RUN_DIR: $RUN_DIR
}

make_clean(){
  rm -rf CMakeFiles
  rm -rf Makefile
  rm -rf CMakeCache.txt
  rm -rf cmake_install.cmake
  rm -rf cmake-build-debug
  rm -rf *.cbp
  make clean
}


make_lib(){
  cd $SRC_DIR/$1
  make_clean
  cmake .
	make
	if [ $? -eq 0 ]; then
      echo "make [$1] successed";
  else
      echo "make [$1] failed"
      exit;
  fi
}

compile_project(){
  MODULE="bd_$1"
  MODULE_CONF="bd_${1}.conf"
  cd $SRC_DIR/$MODULE
  make_clean
  cmake .
  make
  if [ $? -eq 0 ]; then
      echo "make $MODULE successed";
  else
      echo "make $MODULE failed";
      exit;
  fi

  mkdir -p $RUN_DIR/$MODULE
  chmod +x $MODULE
	mv $MODULE $RUN_DIR/$MODULE/
	cp $MODULE_CONF $RUN_DIR/$MODULE/
	cd $SRC_DIR
}

copy_slog(){
  MODULE="bd_$1"
  cp -a $RUN_DIR/lib/log4cxx.properties     $RUN_DIR/$MODULE
  cp -a $RUN_DIR/lib/liblog4cxx*            $RUN_DIR/$MODULE/
  if [ $SYSTEM" " ==  "mac"" " ];
  then
    cp $RUN_DIR/lib/libslog.dylib           $RUN_DIR/$MODULE
  else
    cp $RUN_DIR/lib/libslog.so              $RUN_DIR/$MODULE
  fi
}

copy(){
  MODULE=$1
  if [ $MODULE == "all" ]; then
    copy_slog route_server
    copy_slog msg_server
    copy_slog http_msg_server
    copy_slog push_server
    copy_slog db_proxy_server
  else
    copy_slog $MODULE
  fi
}


build_lib() {
	echo "#ifndef __VERSION_H__" > base/version.h
	echo "#define __VERSION_H__" >> base/version.h
	echo "#define VERSION \"$VERSION\"" >> base/version.h
	echo "#endif" >> base/version.h
  mkdir -p $SRC_DIR/lib $RUN_DIR
  echo $VERSION > $RUN_DIR/version.txt
  echo "process [libsecurity]"
  if [ $SYSTEM" " ==  "mac"" " ];
  then
      rm -rf $SRC_DIR/libsecurity/lib/$SYSTEM
      mkdir -p $SRC_DIR/libsecurity/lib/$SYSTEM
      cp $LIBS_DIR/base/security/lib/$SYSTEM/libsecurity.a $SRC_DIR/libsecurity/lib/$SYSTEM
  else
      rm -rf $SRC_DIR/libsecurity/lib/$SYSTEM/unix
      rm -rf $SRC_DIR/base/security/lib/$SYSTEM
      mkdir -p $SRC_DIR/libsecurity/lib/$SYSTEM/unix
      mkdir -p $SRC_DIR/base/security/lib/$SYSTEM
      cp $LIBS_DIR/base/security/lib/$SYSTEM/libsecurity.a $SRC_DIR/libsecurity/lib/unix
      cp $LIBS_DIR/base/security/lib/$SYSTEM/libsecurity.a $SRC_DIR/base/security/lib/$SYSTEM
  fi
  cp $SRC_DIR/../package/libsecurity/include/security.h $SRC_DIR/base/security/

  if [ $? -eq 0 ]; then
    echo "==================================================="
    echo "process complete.[libsecurity]"
    echo "==================================================="
    sleep 1s
  else
      echo "make [libsecurity] failed";
      exit;
  fi

  echo "process [cryptopp]"
  rm -rf $SRC_DIR/base/cryptopp
  cp -a $LIBS_DIR/cryptopp $SRC_DIR/base/cryptopp

  echo "process [hiredis]"
  cp -a $LIBS_DIR/hiredis/* $SRC_DIR/bd_db_proxy_server
  cp -a $LIBS_DIR/hiredis/* $SRC_DIR/bd_http_msg_server

  if [ $? -eq 0 ]; then
    echo "==================================================="
    echo "process complete.[libsecurity]"
    echo "==================================================="
    sleep 1s
  else
      echo "process [hiredis] failed";
      exit;
  fi

  echo "process [pb]"
  rm -rf $SRC_DIR/base/pb/google
  rm -rf $SRC_DIR/base/pb/lib/$SYSTEM
  mkdir -p $SRC_DIR/base/pb/google
  mkdir -p $SRC_DIR/base/pb/lib/$SYSTEM
  cp -a $LIBS_DIR/base/pb/* $SRC_DIR/base/pb/
  cp -a $LIBS_DIR/base/pb/lib/$SYSTEM/* $SRC_DIR/base/pb/lib/$SYSTEM/

  if [ $? -eq 0 ]; then
    echo "==================================================="
    echo "process complete.[pb]"
    echo "==================================================="
    sleep 1s
  else
      echo "make [pb] failed";
      exit;
  fi

  echo "process [slog]"
  rm -rf $SRC_DIR/slog/lib
  rm -rf $SRC_DIR/slog/include

  cp -af  $LIBS_DIR/slog/lib $SRC_DIR/slog/
  cp -af  $LIBS_DIR/slog/include $SRC_DIR/slog/

  echo "build [slog]"
	make_lib slog
  mkdir -p $SRC_DIR/base/slog/lib
  cp $SRC_DIR/slog/slog_api.h         $SRC_DIR/base/slog/

  mkdir -p $RUN_DIR/lib/
  cp -a $SRC_DIR/slog/lib/liblog4cxx.so*        $RUN_DIR/lib/
  cp -a $SRC_DIR/slog/lib/liblog4cxx*           $RUN_DIR/lib/
  cp -a $SRC_DIR/slog/lib/liblog4cxx*           $SRC_DIR/base/slog/lib/
  cp -a $SRC_DIR/slog/log4cxx.properties.conf   $SRC_DIR/slog/log4cxx.properties

  if [ $SYSTEM ==  "mac" ];then
    cp -f $SRC_DIR/slog/libslog.dylib           $RUN_DIR/lib/
    cp -f $SRC_DIR/slog/libslog.dylib           $SRC_DIR/base/slog/lib/
  else
    cp -f $SRC_DIR/slog/libslog.so              $RUN_DIR/lib/
    cp -f $SRC_DIR/slog/libslog.so              $SRC_DIR/base/slog/lib/
  fi
  mkdir -p $SRC_DIR/../log
  cp $SRC_DIR/slog/log4cxx.properties      $RUN_DIR/lib/
  rm -rf cp $SRC_DIR/slog/log4cxx.properties

  if [ $? -eq 0 ]; then
    echo "==================================================="
    echo "process complete.[slog]"
    echo "==================================================="
    sleep 1s
  else
      echo "make [slog] failed";
      exit;
  fi

  echo "make [base]"
	make_lib base
  if [ -f $SRC_DIR/base/libbase.a ]
  then
      cp $SRC_DIR/base/libbase.a $SRC_DIR/lib/
  fi
  if [ $? -eq 0 ]; then
    echo "==================================================="
    echo "process complete.[base]"
    echo "==================================================="
    sleep 1s
  else
      echo "make [base] failed";
      exit;
  fi
}

build_project(){
  MODULE=$1
  rm -rf $RUN_DIR/bd_$MODULE
  if [ $MODULE == "all" ]; then
    compile_project http_msg_server
    compile_project db_proxy_server
    compile_project route_server
#    compile_project msg_server
#    compile_project push_server
  else
    compile_project $MODULE
  fi
}

clean() {
	cd $SRC_DIR/base
	make_clean
	cd $SRC_DIR/slog
	make_clean
	cd $SRC_DIR/bd_route_server
	make_clean
	rm -rf bd_route_server

	cd $SRC_DIR/bd_msg_server
	make_clean
	rm -rf bd_msg_server

	cd $SRC_DIR/bd_http_msg_server
  make_clean
	rm -rf bd_http_msg_server

  cd $SRC_DIR/bd_push_server
  make_clean
	rm -rf bd_push_server

	cd $SRC_DIR/bd_db_proxy_server
	make_clean
	rm -rf bd_db_proxy_server

	rm -rf $SRC_DIR/lib/libbase.a

	rm -rf $SRC_DIR/slog/include
	rm -rf $SRC_DIR/slog/lib
	rm -rf $SRC_DIR/slog/libslog.*


  if [ $SYSTEM" " ==  "mac"" " ];
  then
      rm -rf $SRC_DIR/libsecurity/lib/$SYSTEM
  else
      rm -rf $SRC_DIR/libsecurity/lib/unix
      rm -rf $SRC_DIR/base/security/lib/$SYSTEM
  fi

  rm -rf $SRC_DIR/base/pb/google
  rm -rf $SRC_DIR/base/pb/lib/$SYSTEM
  rm -rf $SRC_DIR/base/slog
  rm -rf $SRC_DIR/base/libbase.a
}

print_help() {
  echo "BangDeTalk 1.0"
	echo "Usage:  build_lib -> build -> copy"
	echo "  sh $0 build_lib v1.0.1"
	echo "  sh $0 build v1.0.1 all"
	echo "  sh $0 build v1.0.1 msg_server | http_msg_server | route_server | db_proxy_server | push_server"
	echo "  sh $0 copy v1.0.1  msg_server | http_msg_server | route_server | db_proxy_server | push_server"
	echo "  sh $0 copy v1.0.1  all"
	echo "  sh $0 clean"
}

case $1 in
	clean)
	  get_cur_dir
		echo "clean all build..."
		clean
		;;
	build)
		if [ $# != 3 ]; then
			echo $#
			print_help
			exit
		fi
	  echo "start compiling BangDeTalk 1.0..."
    echo "=================================="
    VERSION=$2
    get_cur_dir
    echo "version: $VERSION"
		build_project $3
		;;
build_lib)
		if [ $# != 2 ]; then
			echo $#
			print_help
			exit
		fi
    VERSION=$2
    get_cur_dir
    echo "version: $VERSION"
		build_lib
		;;
copy)
    if [ $# != 3 ]; then
			echo $#
			print_help
			exit
		fi
    VERSION=$2
    get_cur_dir
		copy $3
		;;
	*)
		print_help
		;;
esac
