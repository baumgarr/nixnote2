Summary: An Evernote client clone for Linux
Name: nixnote2
Version: __VERSION__
Release: 0
License: GPLv2+
Group: Applications/Internet 
Vendor: Randy Baumgarte
URL: http://nevernote.sourceforge.net/
Packager: Randy Baumgarte <randy@fbn.cx>
Source: /home/randy/nn2/package_scripts/nixnote2___VERSION_____ARCH__.tar.gz
AutoReqProv: no
Requires: tidy, bash, qt >= 4.8.5, qt-x11 >= 4.8.5, qtwebkit >= 2.3, glibc >= 2.18, libgcc >= 4.8.2, poppler-qt, libstdc++ >= 4.8.2, openssl >= 1.0.0, OpenEXR >= 1.7, tbb >= 4.1, libcurl >= 3.75.0

%description
NixNote:: Evernote client clone for Linux

Copyright 2009-2015, Randy Baumgarte 
Licensed under GNU General Public License version 2+

This is an incomplete clone of Evernote designed to run on Linux. 
While this is designed to work with Evernote, it is in no way
connected with or supported by Evernote.  Any problems you encounter will not be corrected by them
and, since this is GPL software, you are using this software at your own risk.  

See release.txt for details of what works and what doesn't work.

Documents:

changelog.txt:  ChangeLog and development history.
install.txt:    Build and install instructions.
credit.txt:     Credit of nixnote developers.
gpl.txt:        License description of GPL v2.
license.html:   Legal notices for licenses and trademarks.
colors.txt:     Sample config file to the color menu for note & title backgrounds.
theme.ini:      Theme configuration file.
shortcuts.txt   Shortcut configuration file.

%prep
%setup -n nixnote2

%clean
rm -rf ${RPM_BUILD_ROOT}
rm -rf ${RPM_BUILD_DIR}/nixnote2

%define debug_package %{nil}

%build

%install
rm -rf ${RPM_BUILD_ROOT}
mkdir -p ${RPM_BUILD_ROOT}
mv -f usr ${RPM_BUILD_ROOT}

%post

%postun

%files
%attr(755,root,root) /usr/share/nixnote2
%attr(755,root,root) /usr/bin/nixnote2  
%attr(755,root,root) /usr/share/doc/nixnote2
%attr(755,root,root) /usr/share/applications/nixnote2.desktop
%attr(755,root,root) /usr/share/man/man1/nixnote2.1.gz

%changelog
* Thu Dec 10 2015 Randy Baumgarte <randy@fbn.cx> - __VERSION__
- See changelog.txt for all changes

