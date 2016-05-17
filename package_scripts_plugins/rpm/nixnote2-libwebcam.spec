Summary: A plugin for NixNote2 to allow for Webcam notes.
Name: nixnote2-libwebcam
Version: __VERSION__
Release: 0
License: GPLv2
Group: Applications/Internet 
Vendor: Randy Baumgarte
URL: http://nevernote.sourceforge.net/
Packager: Randy Baumgarte <randy@fbn.cx>
Source: /home/randy/nn2/package_scripts/nixnote2_libwebcam___VERSION_____ARCH__.tar.gz
AutoReqProv: no
Requires: nixnote2, qt >= 4.8.5, qt-x11 >= 4.8.5, glibc >= 2.18, libgcc >= 4.8.2, libstdc++ >= 4.8.2, OpenEXR >= 1.7, tbb >= 4.1, opencv >= 2.4.7

%description
NixNote:: Evernote client clone for Linux

Copyright 2016 Randy Baumgarte 
Licensed under GNU General Public Lisence version 2

This is a plugin for NixNote2 to prived the ability to create		 
notes via a webcam. 

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
%setup -n nixnote2-libwebcam

%clean
rm -rf ${RPM_BUILD_ROOT}
rm -rf ${RPM_BUILD_DIR}/nixnote2-libwebcam

%define debug_package %{nil}

%build

%install
rm -rf ${RPM_BUILD_ROOT}
mkdir -p ${RPM_BUILD_ROOT}
mv -f usr ${RPM_BUILD_ROOT}

%post

%postun

%files
%attr(755,root,root) /usr/share/nixnote2/plugins

%changelog
* Thu Dec 10 2015 Randy Baumgarte <randy@fbn.cx> - __VERSION__
- See changelog.txt for all changes

