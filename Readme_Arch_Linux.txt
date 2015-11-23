For Arch Linux, you need to install additional libraries.  

There is a "libcurl-compat" package for arch: https://aur.archlinux.org/packages/libcurl-compat/
Install it and then run:
$ LD_PRELOAD=libcurl.so.3 nixnote2
