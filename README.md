## Synopsis

This is the portable version of [letskencrypt](https://github.com/kristapsdz/letskencrypt) that runs
on Linux, Mac OS X, and FreeBSD.  The portable version has additional
goop to allow compilation and secure operation.  Please see the main
[letskencrypt](https://github.com/kristapsdz/letskencrypt) repository for detailed information on
operation.

What are the differences?

* Conditional support for OpenBSD's sandbox or Mac OS X.
* Proper preprocessor flags for unlocking some Linux functions.
* Different library names on Linux.
* Uses GNU make instead of BSD make.

This version tries its best to be secure, but some of its supported
operating systems are hostile to security.

On both Linux and Mac OS X, for example, the DNS resolution process is
effectively run in the main file-system and un-sandboxed due to the
complexity of lookups (needing mDNSresponder in the latter case or a
slew of mystery files in the former).

Moreover, while the sandbox on Mac OS X (which is deprecated?) exists,
its behaviour is not well-documented and, morever, is weakened to
co-exist with the file-system jail.  On Linux, the sandbox has not been
implemented in *letskencrypt-portable*, as it's just too complicated.

Anyway, if you have issues to report, please either post them here or
e-mail them directly to me.  This repository is really just a read-only
mirror of the main CVS repository.

What can you do?

* If you know Linux sandboxing, please have at it!  There's only one
function to fill in.
* Same with the FreeBSD sandbox.

## Installation

To use *letskencrypt-portable*, just download and run `make` and `make
install` in the usual way (GNU make).  It should not require any
additional tuning or teaking.

The software has several compile-time dependencies:
[OpenSSL](https://openssl.org) or [LibreSSL](http://www.libressl.org),
[libcurl](https://curl.haxx.se/libcurl), and
[json-c](https://github.com/json-c/json-c).

For Linux, you'll also need [libbsd](https://libbsd.freedesktop.org).

## License

All sources use the ISC (like OpenBSD) license.  See the
[LICENSE.md](LICENSE.md) file for details.
