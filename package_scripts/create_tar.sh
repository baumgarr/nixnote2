#!/bin/sh

version="2.0-alpha5"
arch="amd64"


package_dir=$(cd `dirname $0` && pwd)

source_dir=".."


####################################################
# Make sure we are running as root
####################################################
if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

#Do any parameter overrides
while [ -n "$*" ]
do
   eval $1
   shift
done

# Determine which lib directory to use
case "$arch" in
   "amd64" ) lib="lib64";;
   "i386" ) lib="lib32";;
esac

##################################################
# Banner page              
##################################################
echo "**************************************************"
echo "Building tar.gz for  NixNote $version for $arch"
echo "**************************************************"

# Cleanup any old stuff

if [ -e "$package_dir/nixnote2-${version}_${arch}.tar.gz" ] 
then
   rm $package_dir/nixnote2-${version}_${arch}.tar.gz
fi

cd $package_dir
echo "Building tar"
tar -czf $package_dir/nixnote2-${version}_${arch}.tar.gz ./nixnote2
cd -

