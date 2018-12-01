**Attention *acme-client-portable* is no longer maintained.  Since
*[acme-client](https://github.com/kristapsdz/acme-client)* made its way
into OpenBSD, I've only been using the base version.**

**So if you're using this client, you're using old code!  If you want to
use the current version of *acme-client*, you'll need to use OpenBSD.
Which I recommend doing anyway, of course.**

If you can't use OpenBSD, it's not a difficult challenge to port the
OpenBSD *acme-client* to your operating system.  However, be aware that
it won't share the security mechanisms available to the downstream code.
Regardless, if anybody would like to take a stab at this, I'd be happy
to review your work!

I suggest grabbing a copy of the OpenBSD version and setting up an
[oconfigure](https://github.com/kristapsdz/oconfigure) scaffold around
the code.  This will give you all of the usual string functions that
might not be available.  Next, you'll need to add in your system's
sandbox mechanisms.  This will require your own code.  You can use the
existing portability shim as a guide.  Good luck!

## Synopsis

*acme-client-portable* is yet another
[ACME](https://letsencrypt.github.io/acme-spec/) client, specifically for
[Let's Encrypt](https://letsencrypt.org), but one with a strong focus on
security. 

It was named *letskencrypt-portable* until version 0.1.11.

Please see
[kristaps.bsd.lv/acme-client](https://kristaps.bsd.lv/acme-client) for
stable releases: this repository is for current development of the
portable branch, which tracks
[acme-client](https://github.com/kristapsdz/acme-client) with goop to
allow compilation and secure operation on Linux, Mac OS X, NetBSD, and
FreeBSD (hence "-portable").
You will need [libressl](http://www.libressl.org/) on all systems and
[libbsd](https://libbsd.freedesktop.org/wiki/) on Linux (except for
[musl](https://www.musl-libc.org) libc systems like
[Alpine](https://alpinelinux.org/)).

Linux has an experimental
[libseccomp](https://github.com/seccomp/libseccomp) sandbox, but you
must enable it yourself.  Details in
[Linux-seccomp.md](Linux-seccomp.md).

This repository mirrors the master CVS repository: any source changes
will occur on the master and be pushed periodically to GitHub.  If you
have bug reports or patches, either file them here or e-mail them to me.
Feature requests will be ignored unless joined by a patch.

What are the difference between this and the non-portable release?

* Conditional support for OpenBSD's sandbox, Mac OS X, or
  **experimentally** on Linux.
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
co-exist with the file-system jail.

**Feature requests will be ignored unless joined by a patch.**  If
there's something you need, I'm happy to work with you to make it
happen.  If you really need it, I'm available for contract (contact me
by e-mail).

## Configuration

Since your system might not be one of the tested ones (FreeBSD, Linux,
Linux with musl libc, etc.), you may need to tune some
of the values in the [GNUmakefile](GNUmakefile) or [config.h](config.h).
Please **tell me** if you do so, so I can accommodate in future
releases.

In the former, you can adjust system-specific compilation flags.

In the latter, you can set the `NOBODY_USER` value to be the name of an
unprivileged user for privilege dropping.
You can also set `DEFAULT_CA_FILE` for the location of the certificate
file loaded by libtls.
There's also `PATH_VAR_EMPTY`, which should be an empty directory into
which we can create our jail.

## License

Sources use the ISC (like OpenBSD) license.  See the
[LICENSE.md](LICENSE.md) file for details.

The [jsmn.c](jsmn.c) and [jsmn.h](jsmn.h) files use the MIT license.
See [https://github.com/zserge/jsmn](https://github.com/zserge/jsmn) for
details.
