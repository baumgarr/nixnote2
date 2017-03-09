#!/bin/sh

version="1.0"
arch="amd64"

package_dir=$(cd `dirname $0` && pwd)
source_dir="$package_dir/../.."


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
echo "Copying files for NixNote Webcam Plugin $version for $arch"
echo "****************************************************"

############################
# Copy the things we need  #
############################

# Create directories
echo "Building directories"
mkdir $package_dir/nixnote2
mkdir $package_dir/nixnote2/usr
mkdir $package_dir/nixnote2/usr/lib
mkdir $package_dir/nixnote2/usr/lib/nixnote2
mkdir $package_dir/nixnote2/usr/share
mkdir $package_dir/nixnote2/usr/share/doc/
mkdir $package_dir/nixnote2/usr/share/doc/nixnote2-hunspell-plugin

# Copy binary, configs, & man pages
echo "Copying files"
cp $source_dir/copyright $package_dir/nixnote2/usr/share/doc/nixnote2-hunspell-plugin/
strip --strip-all $source_dir/plugins/libhunspellplugin.so -o $package_dir/nixnote2/usr/lib/nixnote2/libhunspellplugin.so
cp $source_dir/plugins/hunspell/changelog.txt $package_dir/nixnote2/usr/share/doc/nixnote2-hunspell-plugin/changelog.Debian
gzip -c -n -9 $package_dir/nixnote2/usr/share/doc/nixnote2-hunspell-plugin/changelog.Debian > $package_dir/nixnote2/usr/share/doc/nixnote2-hunspell-plugin/changelog.Debian.gz
rm $package_dir/nixnote2/usr/share/doc/nixnote2-hunspell-plugin/changelog.Debian
 

if [ ! -d $source_dir/spell ] 
then
   mkdir $source_dir/spell
fi

# Reset user permissions
echo "Resetting ownership & permissions"
chown -R root:root $package_dir/nixnote2/
chmod 644 $package_dir/nixnote2/usr/lib/nixnote2/libhunspellplugin.so
