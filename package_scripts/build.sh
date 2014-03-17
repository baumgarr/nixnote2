#!/bin/sh
version="2.0-alpha5"
arch=amd64

####################################################
# Make sure we are running as root
####################################################
#if [ "$(id -u)" != "0" ]; then
#  echo "This script must be run as root" 1>&2
#   exit 1
#fi

package_dir=$(cd `dirname $0` && pwd)

sudo $package_dir/clean.sh arch=$arch version=$version
sudo $package_dir/translate.sh arch=$arch version=$version
sudo $package_dir/copy_files.sh arch=$arch version=$version
sudo $package_dir/create_tar.sh arch=$arch version=$version

# Cleanup
echo "Cleaning up"
sudo rm -rf $package_dir/nixnote2

echo "****************************************"
echo "Build complete"
echo "****************************************" 
