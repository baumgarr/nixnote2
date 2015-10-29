#!/bin/sh

package_dir=$(cd `dirname $0` && pwd)

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

cp $package_dir/usr/share/applications/nixnote2.desktop /usr/share/applications/nixnote2.desktop
mkdir -p /usr/share/nixnote2
cp -r $package_dir/usr/share/nixnote2/* /usr/share/nixnote2/
cp $package_dir/usr/bin/nixnote2 /usr/bin/nixnote2

echo "Install complete"
