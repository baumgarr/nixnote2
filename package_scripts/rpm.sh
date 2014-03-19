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

rpmdir="i386"
case "$(uname -m)" in
   "x86_64" ) rpmdir="x86_64";;
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

#start creating the  source tar.  Note, we don't include the /usr/lib directory
# since there is a Thrift RPM package.
cp $package_dir/rpm/nixnote2.spec $package_dir/rpmbuild/SPECS/nixnote2.spec

if [ -d "$package_dir/nixnote2/usr/lib" ] 
then
   mv $package_dir/nixnote2/usr/lib $package_dir/nixnote2-lib
fi
tar -czf $package_dir/rpmbuild/SOURCES/nixnote2_${version}_${arch}.tar.gz ./nixnote2
if [ -d "$package_dir/nixnote2-lib" ] 
then
  mv $package_dir/nixnote2-lib $package_dir/nixnote2/usr/lib
fi


#edit the spec file to update the version & architecture
sed -i "s/__VERSION__/$version/" $package_dir/rpmbuild/SPECS/nixnote2.spec
sed -i "s/__ARCH__/$arch/" $package_dir/rpmbuild/SPECS/nixnote2.spec

#do the actual build
rpmbuild --quiet --define "_topdir $package_dir/rpmbuild" -ba $package_dir/rpmbuild/SPECS/nixnote2.spec

#copy the rpm out
cp $package_dir/rpmbuild/RPMS/$rpmdir/*.rpm $package_dir/

#cleanup
cd -
rm -rf $package_dir/rpmbuild
