Summary: An Evernote client clone for Linux
Name: nixnote2
Version: __VERSION__
Release: 0
License: GPLv2
Group: Applications/Internet 
Vendor: Randy Baumgarte
URL: http://nevernote.sourceforge.net/
Packager: Randy Baumgarte <randy@fbn.cx>
Source: /home/randy/nn2/package_scripts/nixnote2___VERSION_____ARCH__.tar.gz
AutoReqProv: no
Requires: tidy, bash, qt >= 4.8.5, qt-x11 >= 4.8.5, qtwebkit >= 2.3, glibc >= 2.18, libgcc >= 4.8.2, hunspell >= 1.3.2, poppler-qt, libstdc++ >= 4.8.2, openssl >= 1.0.0, OpenEXR >= 1.7, tbb >= 4.1, opencv >= 2.4.7, thrift >= 0.9.1, libcurl >= 3.75.0

%description
NixNote:: Evernote client clone for Linux

Copyright 2009-2014, Randy Baumgarte 
Licensed under GNU General Public Lisence version 2

This is an incomplete clone of Evernote designed to run on Linux. 
While this is designed to work with Evernote, it is in no way
connected with or supported by Evernote.  Any problems you encounter will not be corrected by them
and, since this is GPL software, you are using this software at your own risk.  

See release.txt for details of what works and what doesn't work.

Documents:

release.txt:    Includes new features, known bugs and limitations.

changelog.txt:  ChangeLog and development history
install.txt:    Build and install instructions
credit.txt:     Credit of nixnote developers
gpl.txt:        License description of GPL v2.
license.txt:    Legal notices for licenses and trademarks

shortcut_howto.txt:  How-to document to setup shortcut keys.
shurtcut_sample.txt: Its configuration sample


%configure
sed -i 's|^hardcode_libdir_flag_spec=.*|hardcode_libdir_flag_spec=""|g' libtool
sed -i 's|^runpath_var=LD_RUN_PATH|runpath_var=DIE_RPATH_DIE|g' libtool

%clean
rm -rf ${RPM_BUILD_ROOT}
rm -rf ${RPM_BUILD_DIR}/nixnote2

%define debug_package %{nil}

%prep
%setup -n nixnote2

%define debug_package %{nil}

%build

%install
rm -rf ${RPM_BUILD_ROOT}
mkdir -p ${RPM_BUILD_ROOT}
mv -f usr ${RPM_BUILD_ROOT}

%post

%postun
if [ -d /usr/share/nixnote2 ]; then
    rm -rf /usr/share/nixnote2
fi
if [ -f /usr/share/applications/nixnote2.desktop ]; then
    rm -f /usr/share/applications/nixnote2.desktop
fi
if [ -f /usr/bin/nixnote2 ]; then
	rm -f /usr/bin/nixnote2
fi

%files
%attr(755,root,root) /usr/share/nixnote2
%attr(755,root,root) /usr/bin/nixnote2  
%attr(755,root,root) /usr/share/doc/nixnote2
%attr(755,root,root) /usr/share/applications/nixnote2.desktop
%attr(755,root,root) /usr/share/man/man1/nixnote2.1.gz

%changelog
* Sun Nov 22 2015 Randy Baumgarte <randy@fbn.cx> - __VERSION__
- See changelog.txt for all changes

