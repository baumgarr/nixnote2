#!/bin/sh

version="2.0-beta4"
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

if [ -e "$package_dir/nixnote2-${version}.ebuild" ] 
then
   rm $package_dir/nixnote2-${version}.ebuild
fi

cp $package_dir/gentoo/ebuild $package_dir/nixnote2-${version}.ebuild


#edit the spec file to update the version & architecture
sed -i "s/__TAG__/$tag/g" $package_dir/nixnote2-${version}.ebuild
sed -i "s/__VERSION__/$version/g" $package_dir/nixnote2-${version}.ebuild

