#/bin/sh
#start or stop the server
CUR_DIR=
ROOT_DIR=
VERSION=$2
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
    # Change the current directory to the location of the script
    CUR_DIR=$(dirname "${REALPATH}")
}

function build_app() {
  name=$1
  if [ -e $ROOT_DIR/$name  ]; then
    echo build... $name
    cd $ROOT_DIR/$name
    cmake .
    make clean
    make
  else
    echo " ===>  $name not exists"
  fi

}

function make_app() {
  name=$1
  if [ -e $ROOT_DIR/$name  ]; then
    cd $ROOT_DIR/$name
    make clean
    echo make ...
    make
  else
    echo " ===>  $name not exists"
  fi

}

function build() {
  servers_str=$@
  if [ "$servers_str" == "build" ]; then
    servers_str="business msg file route login push ws msfs"
  fi
  servers=$(echo $servers_str | tr " " "\n")
  for server in $servers
  do
    if [ "$server" != "build" ]; then
      name="bd_${server}_server"
      build_app $name
    fi
  done
}

function make_() {
  servers_str=$@
  if [ "$servers_str" == "make" ]; then
    servers_str="business msg file route login push ws msfs"
  fi
  echo "${servers_str}" > /tmp/make.log
  servers=$(echo $servers_str | tr " " "\n")
  for server in $servers
  do
    if [ "$server" != "make" ]; then

      name="bd_${server}_server"
      echo "make... ${name}" >> /tmp/make.log
      make_app $name
      echo "make ok!" >> /tmp/make.log
    fi
  done
  cat /tmp/make.log
}

function make_bd_common() {
  name="bd_common"
  if [ -e $ROOT_DIR/$name  ]; then
    echo make... $name
    cd $ROOT_DIR/$name
    make
  else
    echo " ===>  $name not exists"
  fi
}

get_cur_dir
cd $CUR_DIR
cd ../../
ROOT_DIR=$PWD

echo SYSTEM: $SYSTEM
echo CUR_DIR: $CUR_DIR
echo ROOT_DIR: $ROOT_DIR
echo "========"

case $1 in
	build)
		build $@
  ;;
	make)
		make_ $@
  ;;
	make_bd_common)
		make_bd_common $@
  ;;
	build_common)
		build_app bd_common
  ;;
	*)
		echo "Usage: "
		echo "  $0 build business msg file route login push ws msfs"
		echo "  $0 make business msg file route login push ws msfs"
		echo "  $0 build_common"
		echo "  $0 make_bd_common "
		;;
esac
