#!/bin/sh

version="2.0-alpha5"
arch="amd64"

package_dir=$(cd `dirname $0` && pwd)
source_dir=".."

thrift_lib="$source_dir/thrift-source/lib/cpp/.libs"
thrift="y"


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
mkdir $package_dir/nixnote2
mkdir $package_dir/nixnote2/usr
mkdir $package_dir/nixnote2/usr/share
mkdir $package_dir/nixnote2/usr/share/applications
mkdir $package_dir/nixnote2/usr/share/nixnote2
mkdir $package_dir/nixnote2/usr/share/man 
mkdir $package_dir/nixnote2/usr/share/man/man1
mkdir $package_dir/nixnote2/usr/bin
mkdir $package_dir/nixnote2/usr/share/doc/
mkdir $package_dir/nixnote2/usr/share/doc/nixnote2

# Copy binary, configs, & man pages
echo "Copying files"
cp $source_dir/*.txt $package_dir/nixnote2/usr/share/nixnote2/
cp $source_dir/*.html $package_dir/nixnote2/usr/share/nixnote2/
cp $source_dir/nixnote2.desktop $package_dir/nixnote2/usr/share/applications/
cp $source_dir/copyright $package_dir/nixnote2/usr/share/doc/nixnote2/
strip --strip-all $source_dir/nixnote -o $package_dir/nixnote2/usr/bin/nixnote2
gzip -c -9 $source_dir/man/nixnote2.1 > $package_dir/nixnote2/usr/share/man/man1/nixnote2.1.gz
cp $source_dir/changelog.txt $package_dir/nixnote2/usr/share/doc/nixnote2/changelog.Debian
gzip -c -9 $package_dir/nixnote2/usr/share/doc/nixnote2/changelog.Debian > $package_dir/nixnote2/usr/share/doc/nixnote2/changelog.Debian.gz
rm $package_dir/nixnote2/usr/share/doc/nixnote2/changelog.Debian
 
if [ $thrift = "y" ]
then
   mkdir $package_dir/nixnote2/usr/lib
   mkdir $package_dir/nixnote2/usr/lib/nixnote2
   strip --strip-all $thrift_lib/libthrift-0.9.1.so -o $package_dir/nixnote2/usr/lib/nixnote2/libthrift-0.9.1.so
fi


# Copy subdirectories
cp -r $source_dir/images $package_dir/nixnote2/usr/share/nixnote2/
cp -r $source_dir/spell $package_dir/nixnote2/usr/share/nixnote2/
cp -r $source_dir/translations $package_dir/nixnote2/usr/share/nixnote2/
cp -r $source_dir/certs $package_dir/nixnote2/usr/share/nixnote2/
cp -r $source_dir/qss $package_dir/nixnote2/usr/share/nixnote2/


# Reset user permissions
echo "Resetting ownership & permissions"
chown -R root:root $package_dir/nixnote2/

if [ "$thrift" = "y" ] 
then
  chmod 644 $package_dir/nixnote2/usr/lib/nixnote2/*.*
fi

# Cleanup
#echo "Cleaning up"
#rm -rf $package_dir/nixnote2
