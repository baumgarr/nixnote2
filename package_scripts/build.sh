#!/bin/sh
#version="2.0-alpha5"

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

thrift_lib="$package_dir/../thrift-source/lib/cpp/.libs"
sl="s"

read -p "Enter version for build: " version
read -p "Include Thrift? (y/n): " thrift
if [ "$thrift" = "y" ]
then
  echo "Where should I get Thrift from?"
  echo "    s = $thrift_lib"
  echo "    l = /usr/local/lib"
  read -p "(s/l) " sl
fi 

if [ "$sl" = "l" ] 
then
   thrift_lib="/usr/local/lib"
fi

sudo $package_dir/clean.sh arch=$arch version=$version
sudo $package_dir/translate.sh arch=$arch version=$version
sudo $package_dir/copy_files.sh arch=$arch version=$version thrift=$thrift thrift_lib=$thrift_lib

read -p "Build tar.gz (y/n): " yn
if [ "$yn" = "y" ] 
then
  sudo $package_dir/tar.sh arch=$arch version=$version 
fi

read -p "Build deb (y/n): " yn
if [ "$yn" = "y" ]  
then
   sudo $package_dir/dpkg.sh arch=$arch version=$version 
   echo "**********************************"
   echo "* Checking deb for errors"
   echo "**********************************"
   lintian $package_dir/nixnote2-${version}_${arch}.deb 
fi
read -p "Build rpm (y/n): " yn
if [ "$yn" = "y" ] 
then
  sudo $package_dir/rpm.sh arch=$arch version=$version
fi

# Cleanup
echo "Cleaning up"
sudo rm -rf $package_dir/nixnote2

echo "****************************************"
echo "Build complete"
echo "****************************************" 
