#!/bin/sh
export DEB_BUILD_HARDENING=0

version="2.0-alpha5"


package_dir=$(cd `dirname $0` && pwd)


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
version=`echo $version | sed -e 's/[-]/_/g'`

case "$arch" in
   "amd64" ) lib="lib64";;
   "i386" ) lib="lib32";;
esac

##################################################
# Banner page              
##################################################
echo "**************************************************"
echo "Building rpm NixNote $version for $arch"
echo "**************************************************"

# Cleanup any old stuff

if [ -e "$package_dir/nixnote2-${version}_${arch}.rpm" ] 
then
   rm $package_dir/nixnote2-${version}_${arch}.rpm
fi

# Copy control file for the package
mkdir $package_dir/rpmbuild
mkdir $package_dir/rpmbuild/SPECS
mkdir $package_dir/rpmbuild/SOURCES


cp $package_dir/rpm/nixnote2.spec $package_dir/rpmbuild/SPECS/nixnote2.spec
#cp $package_dir/nixnote2-${version}_${arch}.tar.gz  $package_dir/rpmbuild/SOURCES/
tar -czf $package_dir/rpmbuild/SOURCES/nixnote2_${version}_${arch}.tar.gz ./nixnote2

sed -i "s/__VERSION__/$version/" $package_dir/rpmbuild/SPECS/nixnote2.spec
sed -i "s/__ARCH__/$arch/" $package_dir/rpmbuild/SPECS/nixnote2.spec

#rpmbuild --define "_topdir `pwd`" -ba ./SPECS/nixnote.spec
rpmbuild --define "_topdir $package_dir/rpmbuild" -ba $package_dir/rpmbuild/SPECS/nixnote2.spec
cp $package_dir/rpmbuild/RPMS/$(uname -m)/*.rpm $package_dir/
cd -
rm -rf $package_dir/rpmbuild
