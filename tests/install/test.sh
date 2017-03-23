#!/usr/bin/env sh
exec=`realpath $0`
currdir=`dirname $exec`
parent=`dirname $currdir`
root=`dirname $parent`

cmake -H. -B_build -DCMAKE_INSTALL_PREFIX=$root/_install -DTESTS_DIR=$root/tests
env VERBOSE=1 cmake --build _build --target tests_run
