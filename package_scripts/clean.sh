#!/bin/sh

version="2.0"

# Determine which platform we are on
arch="i386"
case "$(uname -m)" in
   "x86_64" ) arch="amd64";;
esac

pkgarch="i386"
case "$(uname -m)" in
   "x86_64" ) pkgarch="x86_64";;
esac

#Do any parameter overrides
while [ -n "$*" ]
do
   eval $1
   shift
done

package_dir=$(cd `dirname $0` && pwd)
destination="$package_dir/nixnote/share/nixnote"


# Check that we are runinng as root
if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi


# Delete the tar.gz file if it exists
if [ -e "$package_dir/nixnote2-${version}_${arch}.tar.gz" ]
then
   rm $package_dir/nixnote2-${version}_${arch}.tar.gz
fi

#delete the debif it exists
if [ -e "$package_dir/nixnote2-${version}_${arch}.deb" ]
then
   rm $package_dir/nixnote2-${version}_${arch}.deb
fi

#delete the rpm if it exists
rpmversion=`echo $version | sed -e 's/[-]/_/g'`
if [ -e "$package_dir/nixnote2-${rpmversion}-0.${pkgarch}.rpm" ]
then
   rm $package_dir/nixnote2-${rpmversion}-0.${pkgarch}.rpm
fi

# Cleanup any directory info
if [ -d "$package_dir/nixnote2" ]
then
   rm -rf $package_dir/nixnote2
fi
