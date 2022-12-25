#!/bin/bash
adr_dir=~/data/projects/ptp/bd-im/apps/front-client/apps/PtpIm/android_client/app/jni/ptp
case $1 in
    clean)
        echo "clean all build..."
        rm -rf CMakeCache.txt CMakeFiles CTestTestfile.cmake cmake_install.cmake Testing Makefile install_manifest.txt obj *.a *.exe
        rm -rf tests/CMakeCache.txt tests/CMakeFiles tests/cmake_install.cmake tests/Makefile tests/*.exe
        ;;
    *)

        ;;
esac
