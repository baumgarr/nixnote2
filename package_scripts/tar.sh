#!/bin/sh

version="2.0-beta5"


package_dir=$(cd `dirname $0` && pwd)

source_dir="$package_dir/.."


####################################################
# Make sure we are running as root
####################################################
if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi


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
cp $source_dir/install.sh $package_dir/nixnote2/
cp $source_dir/uninstall.sh $package_dir/nixnote2/
tar -czf $package_dir/nixnote2-${version}_${arch}.tar.gz ./nixnote2
rm $package_dir/nixnote2/install.sh
rm $package_dir/nixnote2/uninstall.sh
cd -

