#!/bin/sh


package_dir=$(cd `dirname $0` && pwd)

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

echo "****************************************"
echo "* Translating files" 
echo "****************************************"

# Retranslate
rm  $package_dir/../translations/*.qm
lupdate $package_dir/../NixNote2.pro
lrelease $package_dir/../translations/*.ts
