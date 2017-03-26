Summary: A webcam plugin for NixNote
Name: nixnote2_webcam_plugin
Version: __VERSION__
Release: 0
License: GPLv2
Group: Applications/Internet 
Vendor: Randy Baumgarte
URL: http://nevernote.sourceforge.net/
Packager: Randy Baumgarte <randy@fbn.cx>
Source: /home/randy/nn2/package_scripts/webcam_plugin/nixnote2_webcam_plugin___VERSION_____ARCH__.tar.gz
AutoReqProv: no
Requires: nixnote2, opencv >= 2.4.7

%description
NixNote-Webcam-Plugin:: Plugin allowing for a webcam to be used in NixNote2.

Copyright 2009-2015, Randy Baumgarte 
Licensed under GNU General Public License version 2

This is a plugin for NixNote2. It allows for the creation of notes by using a webcam as input.

See release.txt for details of what works and what doesn't work.

Documents:

changelog.txt:  ChangeLog and development history.
gpl.txt:        License description of GPL v2.
license.html:   Legal notices for licenses and trademarks.

%prep
%setup -n nixnote2

%clean
rm -rf /usr/share/doc/nixnote2-webcam-plugin
rm -rf /usr/share/nixnote2/plugins/libwebcamplugin.so

%define debug_package %{nil}

%build

%install
mkdir -p /usr/share/nixnote2/plugins
mkdir -p /usr/share/doc/nixnote2-webcam-plugin
mv -f usr ${RPM_BUILD_ROOT}

%post

%postun

%files
%attr(755,root,root) /usr/share/nixnote2/plugins/libwebcamplugin.so
%attr(755,root,root) /usr/share/doc/nixnote2-webcam-plugin

%changelog
* Thu Dec 10 2015 Randy Baumgarte <randy@fbn.cx> - __VERSION__
- See changelog.txt for all changes

