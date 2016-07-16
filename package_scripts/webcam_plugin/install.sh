#!/bin/sh

package_dir=$(cd `dirname $0` && pwd)

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

mkdir -p /usr/share/nixnote2/plugins
cp -r $package_dir/* /

echo "Install complete"
