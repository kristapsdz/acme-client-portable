## Synopsis

This is the portable version of [letskencrypt](letskencrypt), which is
built for OpenBSD.  The portable version has additional goop to allow
compilation and running (and system-specific security, such as using the
Seatbelt sandbox instead of pledge) on Mac OS X and Linux.  Please see
the main [letskencrypt](letskencrypt) repository for detailed
information on operation.

This version is **not** as secure as the OpenBSD version.  This is due
to deficiencies in the supported portable operating systems.  

On both Linux and Mac OS X, for example, the DNS resolution process is
effectively run in the main file-system and un-sandboxed due to the
complexity of lookups (needing mDNSresponder in the latter case or a
slew of mystery files in the former).

Moreover, while the sandbox on Mac OS X (which is deprecated?) exists,
its behaviour is not well-documented and, morever, is weakened to
co-exist with the file-system jail.  On Linux, the sandbox does not
exist as, for the time being, it's just too complicated.

In all cases, *letskencrypt-portable* does the best it can do.

Anyway, if you have issues to report, please either post them here or
e-mail them directly to me.  This repository is really just a read-only
mirror of the main CVS repository.

## Installation

To use *letskencrypt-portable*, just download and run `make` and `make
install` in the usual way.  It should not require any additional tuning
or teaking.

The software has several compile-time dependencies:
[OpenSSL](https://openssl.org) or [LibreSSL](http://www.libressl.org),
[libcurl](https://curl.haxx.se/libcurl), and
[json-c](https://github.com/json-c/json-c).

For Linux, you'll also need [libbsd](https://libbsd.freedesktop.org).

## License

All sources use the ISC (like OpenBSD) license.  See the
[LICENSE.md](LICENSE.md) file for details.
