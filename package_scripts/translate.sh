#!/bin/sh


package_dir=$(cd `dirname $0` && pwd)

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

echo "****************************************"
echo "* Translating files" 
echo "****************************************"

lupdate="lupdate"
lrelease="lrelease"

if [ -f "/usr/bin/lupdate-qt4" ]
then
   lupdate="lupdate-qt4"
fi


if [ -f "/usr/bin/lrelease-qt4" ]
then
   lrelease="lrelease-qt4"
fi

# Retranslate
rm  $package_dir/../translations/*.qm
#lupdate $package_dir/../NixNote2.pro
$lupdate -pro $package_dir/../NixNote2.pro -no-obsolete
$lrelease $package_dir/../translations/*.ts
