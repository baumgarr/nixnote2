#!/bin/sh

version="2.0-beta9"
eversion="2.0_beta9"
package_dir=$(cd `dirname $0` && pwd)


####################################################
# Make sure we are running as root
####################################################
if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi


read -p "Enter GitHub tag for build: " tag


##################################################
# Banner page              
##################################################
echo "**************************************************"
echo "Building Gentoo ebuild for NixNote $version "
echo "**************************************************"

# Cleanup any old stuff

if [ -e "$package_dir/nixnote2-${eversion}.ebuild" ] 
then
   rm $package_dir/nixnote2-${eversion}.ebuild
fi

cp $package_dir/gentoo/ebuild $package_dir/nixnote2-${eversion}.ebuild


#edit the spec file to update the version & architecture
sed -i "s/__TAG__/$tag/g" $package_dir/nixnote2-${eversion}.ebuild
sed -i "s/__VERSION__/$version/g" $package_dir/nixnote2-${eversion}.ebuild

