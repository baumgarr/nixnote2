#!/bin/sh

version="2.0-beta7"
arch="amd64"

package_dir=$(cd `dirname $0` && pwd)
source_dir="$package_dir/.."


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
echo "****************************************************"
echo "Copying files for NixNote $version for $arch"
echo "****************************************************"

############################
# Copy the things we need  #
############################

# Create directories
echo "Building directories"
#mkdir $package_dir/nixnote2-libwebcam
#mkdir $package_dir/nixnote2-libwebcam/usr
#mkdir $package_dir/nixnote2-libwebcam/usr/share
#mkdir $package_dir/nixnote2-libwebcam/usr/share/nixnote2
mkdir -p $package_dir/nixnote2-libwebcam/usr/share/nixnote2/plugins

# Copy binary, configs, & man pages
echo "Copying files"
strip --strip-all $source_dir/plugins/libwebcamplugin.so -o $package_dir/nixnote2-libwebcam/usr/share/nixnote2/plugins/libwebcamplugin.so
cp $source_dir/install_plugins.sh $/package_dir/nixnote2-libwebcam/install.sh

# Reset user permissions
echo "Resetting ownership & permissions"
chown -R root:root $package_dir/nixnote2-libwebcam/
