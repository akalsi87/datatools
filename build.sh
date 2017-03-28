#!/usr/bin/env sh

exec=`realpath $0`
root=`dirname $exec`

cmake -H$root -B$root/_build -DCMAKE_INSTALL_PREFIX=$root/_install -DCMAKE_BUILD_TYPE=Debug
cmake --build $root/_build --target check
cmake --build $root/_build --target install
