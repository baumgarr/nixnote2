#!/bin/sh
version="2.0-alpha5"

####################################################
# Make sure we are running as root
####################################################
#if [ "$(id -u)" != "0" ]; then
#  echo "This script must be run as root" 1>&2
#   exit 1
#fi

package_dir=$(cd `dirname $0` && pwd)

# Determine which platform we are on
arch="i386"
case "$(uname -m)" in
   "x86_64" ) arch="amd64";;
esac

#Do any parameter overrides
while [ -n "$*" ]
do
   eval $1
   shift
done




sudo $package_dir/clean.sh arch=$arch version=$version
#sudo $package_dir/translate.sh arch=$arch version=$version
sudo $package_dir/copy_files.sh arch=$arch version=$version
sudo $package_dir/tar.sh arch=$arch version=$version
sudo $package_dir/dpkg.sh arch=$arch version=$version
echo "**********************************"
echo "* Checking deb for errors"
echo "**********************************"
lintian $package_dir/nixnote2-${version}_${arch}.deb 

# Cleanup
echo "Cleaning up"
sudo rm -rf $package_dir/nixnote2

echo "****************************************"
echo "Build complete"
echo "****************************************" 
