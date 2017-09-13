#!/bin/sh
export DEB_BUILD_HARDENING=0

version="2.0.2"


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
echo "Building deb NixNote $version for $arch"
echo "**************************************************"

# Cleanup any old stuff

if [ -e "$package_dir/nixnote2-${version}_${arch}.deb" ] 
then
   rm $package_dir/nixnote2-${version}_${arch}.deb
fi

cd $package_dir

# Copy control file for the package
mkdir $package_dir/nixnote2/DEBIAN
cp $package_dir/deb/control $package_dir/nixnote2/DEBIAN/control

# remove the gpl.txt to avoid a lintian warning
rm $package_dir/nixnote2/usr/share/nixnote2/gpl.txt

# edit the version & architecture
sed -i "s/__ARCH__/$arch/" $package_dir/nixnote2/DEBIAN/control
sed -i "s/__VERSION__/$version/" $package_dir/nixnote2/DEBIAN/control

# build the deb
dpkg -b $package_dir/nixnote2 $package_dir/nixnote2-${version}_${arch}.deb

# remove the temporary directory
rm -rf $package_dir/nixnote2/DEBIAN

# copy the gpl.txt back in 
cp $source_dir/gpl.txt $package_dir/nixnote2/usr/share/nixnote2/gpl.txt

# Exit
cd -

