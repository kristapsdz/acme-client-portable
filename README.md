## Synopsis

This is the portable version of [letskencrypt](letskencrypt), which is
built for OpenBSD.  The portable version has additional goop to allow
compilation and running on Mac OS X and Linux.

If you have issues to report, please either post them here or e-mail
them directly to me.  This repository is really just a read-only mirror
of the main CVS repository.

## Installation

To use *letskencrypt-portable*, just download and run `make` and `make
install` in the usual way.  It should not require any additional tuning
or teaking.

The software has several compile-time dependencies:
[OpenSSL](https://openssl.org) or [LibreSSL](http://www.libressl.org),
[libcurl](https://curl.haxx.se/libcurl), and
[json-c](https://github.com/json-c/json-c).  For Linux, you'll also need
[libbsd](https://libbsd.freedesktop.org).

## License

All sources use the ISC (like OpenBSD) license.
See the [LICENSE.md](LICENSE.md) file for details.
