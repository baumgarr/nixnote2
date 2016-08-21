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
#mkdir $package_dir/nixnote2/usr/lib
mkdir $package_dir/nixnote2/usr/share
mkdir $package_dir/nixnote2/usr/share/lintian
mkdir $package_dir/nixnote2/usr/share/lintian/overrides
mkdir $package_dir/nixnote2/usr/share/nixnote2
mkdir $package_dir/nixnote2/usr/share/nixnote2/plugins
#mkdir $package_dir/nixnote2/usr/share/man 
#mkdir $package_dir/nixnote2/usr/share/man/man1
#mkdir $package_dir/nixnote2/usr/bin
mkdir $package_dir/nixnote2/usr/share/doc/
mkdir $package_dir/nixnote2/usr/share/doc/nixnote2-webcam-plugin

# Copy binary, configs, & man pages
echo "Copying files"
#cp $source_dir/*.ini $package_dir/nixnote2/usr/share/nixnote2/
#cp $source_dir/*.txt $package_dir/nixnote2/usr/share/nixnote2/
#cp $source_dir/*.html $package_dir/nixnote2/usr/share/nixnote2/
#cp $source_dir/nixnote2.desktop $package_dir/nixnote2/usr/share/applications/
cp $source_dir/copyright $package_dir/nixnote2/usr/share/doc/nixnote2-webcam-plugin/
strip --strip-all $source_dir/plugins/libwebcamplugin.so -o $package_dir/nixnote2/usr/share/nixnote2/plugins/libwebcamplugin.so
#strip --strip-all $source_dir/plugins/libwebcamplugin.so -o $package_dir/nixnote2/usr/lib/libnixnotewebcamplugin.so.1.0
#ln -s /usr/lib/libnixnotewebcamplugin.so.1.0 ./nixnote2/usr/share/nixnote2/plugins/libwebcamplugin.so
#ln -s /usr/lib/libnixnotewebcamplugin.so.1.0 ./nixnote2/usr/share/nixnote2/plugins/libwebcamplugin.so
#gzip -c -n -9 $source_dir/man/nixnote2.1 > $package_dir/nixnote2/usr/share/man/man1/nixnote2.1.gz
cp $source_dir/changelog.txt $package_dir/nixnote2/usr/share/doc/nixnote2-webcam-plugin/changelog.Debian
gzip -c -n -9 $package_dir/nixnote2/usr/share/doc/nixnote2-webcam-plugin/changelog.Debian > $package_dir/nixnote2/usr/share/doc/nixnote2-webcam-plugin/changelog.Debian.gz
rm $package_dir/nixnote2/usr/share/doc/nixnote2-webcam-plugin/changelog.Debian
cp $source_dir/package_scripts/webcam_plugin/deb/nixnote* $package_dir/nixnote2/usr/share/lintian/overrides/

if [ ! -d $source_dir/spell ] 
then
   mkdir $source_dir/spell
fi

# Copy subdirectories
#cp -r $source_dir/images $package_dir/nixnote2/usr/share/nixnote2/
#cp -r $source_dir/spell $package_dir/nixnote2/usr/share/nixnote2/
#cp -r $source_dir/translations $package_dir/nixnote2/usr/share/nixnote2/
#cp -r $source_dir/certs $package_dir/nixnote2/usr/share/nixnote2/
#cp -r $source_dir/qss $package_dir/nixnote2/usr/share/nixnote2/
#cp -r $source_dir/java $package_dir/nixnote2/usr/share/nixnote2/
#cp -r $source_dir/help $package_dir/nixnote2/usr/share/nixnote2/


#set help page
#sed -i "s/__VERSION__/$version/" $package_dir/nixnote2/usr/share/nixnote2/help/about.html


# Reset user permissions
echo "Resetting ownership & permissions"
chown -R root:root $package_dir/nixnote2/
#chmod 644 $package_dir/nixnote2/usr/lib/libnixnotewebcamplugin.so.*.*
chmod 644 $package_dir/nixnote2/usr/share/nixnote2/plugins/libwebcamplugin.so

# Cleanup
#echo "Cleaning up"
#rm -rf $package_dir/nixnote2
