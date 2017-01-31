# Introduction

After implementing a sandbox for
[acme-client](https://kristaps.bsd.lv/acme-client) on OpenBSD (easy),
FreeBSD (harder, but still easy-ish), and Mac OS X (easy but arguably
useless), I set out to add a Linux sandbox with
[libseccomp](https://github.com/seccomp/libseccomp).  This isn't the
first system I've sandboxed (kcgi, sblg, dblg, etc.); but with seccomp,
it might be the last.

*Aside: what's a sandbox?*  A sandbox defines the system resources our
application uses.  Using anything else results in program termination.
Each component of acme-client is differently sandboxed: some use only
the file-system, some use only the network, etc.  Sandboxes are your
friend.

![Keep them in the sandbox.](http://1.bp.blogspot.com/-F2qtZSFIrAQ/UYk8KnQqZ7I/AAAAAAAAATg/RKs0uOCPdXU/s1600/sandbox+fight.jpg)

After implementing a filter that worked on a local
[Alpine](https://alpinelinux.org) system and releasing, I began
receiving failure reports for non-Alpine Linux distribution x, y, and z
for architecture foo, bar, and baz.  The failure reports were all for
filter violations, e.g.,
[issue/16](https://github.com/kristapsdz/acme-client-portable/issues/16),
and each presented a new system call that had been violated.
Sometimes a given libc would implement a function call differently.
Sometimes an architecture would so so.  It always differed.

What disturbed me was not only that I couldn't reproduce --- each report
was from a different libc, architecture, or distribution --- but that I
couldn't verify that there was a problem at all.  A crafty adversary
might have requested a given system call be white-listed while knowing
he could exploit it.  I'd never know unless I took care to exactly
reproduce each bug reporter's system.

![This is quite applicable.](http://gifrific.com/wp-content/uploads/2012/08/Aint-Nobody-Got-Time-for-That.gif)

I'm not saying this happened --- I had no reason to distrust any bug
reports, and on the contrary thank everybody who took the time to do so!

The fact remains, however, that it would have been possible.

I consider this a serious problem.  You should, too.

*Note:* I have interspersed this article with funny jokes to make it
more accessable.  In general I'm very serious and dour.  Don't be misled
to thinking I actually make jokes in real life.

# The seccomp problem

Programming seccomp-bpf (even helped via libseccomp) is like assembly
language.  It works on one installation, but a new architecture means
new (or different) system calls; a new libc means new (or different)
system calls; and a new kernel version means new system calls.

This is a problem with any capabilities based system.  (See [Bugs Ex
Ante](https://www.youtube.com/watch?v=lqIXr_Or2s4) for why.)

For historic capabilities systems like systrace(4) on OpenBSD, this
wasn't so bad because OpenBSD's system is uniform: one libc, one system
call interface.

On Linux, this is much more difficult: one can use musl libc or glibc,
or others.  Moreover, the system calls themselves change depending on
the architecture --- and sometimes individual kernels are compiled with
or without certain system calls.

When I set out to implement a seccomp filter, I naively assumed that the
differences wouldn't be so great.  Looking at OpenSSH's seccomp-bpf, for
example, is quite straightforward.  (For eye-rollingly ridiculous values
of "straightforward".)

This is not the case.

First off, the mystery of system calls between hardware.  Some have
`mmap`.  Some have `mmap2` ("EABI ARM only has mmap2()", as OpenSSH
tells us).  Do I white-list both?  What is mmap2?  Oh --- "On
architectures where this system call is present, the glibc mmap()
wrapper function invokes this system call rather than the mmap(2) system
call."  So I might *use* `mmap`, but the system call is `mmap2`.  Great.

This brings us to the second problem.  The C library.  There are several
popular ones on Linux: glibc, musl, uClibc, etc.  Each of these is free
to define any standard function (like `mmap`, above) in any way.  So
while my code might say `read()`, the C library might also invoke
`fstat`.  Great.

# The Linux problem

The seccomp problem means a Linux problem. 

If in order to secure a non-trivial application (though I don't really
consider acme-client to be non-trivial) requires users to baby-sit its
runtime, then I've just narrowed my potential audience to those who have
the time and patience to work with my software.

Not everybody who needs to use my software knows how to program --- in
fact, I'd guess that most don't at all.  So if my program is meant to
provide a service to people, but people can't use it --- what's the
point of releasing it as open source?

![What's the point?](http://www.theimaginativeconservative.org/wp-content/uploads/2014/05/peter-sellers-as-dr-strangelove-1.jpg)

When I first released acme-client, I assumed that users would let me
know if their systems failed, and how.  What I didn't anticipate was how
much the software would fail to run properly.  

In the end, I took a lot of my potential users' time, took a lot of my
own time tracking down system call violations, and alienated everybody
else would just gave up when it failed.

What's more embarrassing is that, to most users who simply downloaded
the software, the application just *failed*.  Not *seccomp failed*, but
*my software failed*.

What solutions do I have?  If I stay with seccomp, I continue to
alienate my users, require skilled users to jump through hoops, and lots
of my own time to interpret incoming violations.

All the while, other people trying the software just think it's poorly
written.

Fact is, using seccomp is impossible unless

1. the system really is trivial (pure computation), which reduces the
   probability of users having violations;
2. the users are extremely skilled, which reduces the problem that users
   will abandon the software; or
3. I have the resources to properly test on most systems before
   releasing.

It seems to me that seccomp is designed for big companies who can afford
the resources, big open source projects with the testing infrastructure,
or simple systems.

That leaves me out of the loop.

Is this a problem?  It depends on how significant my software is.  But
it's not *my* software that should scare you.  It's the many unwritten
pieces of software, or written but unsecured, that you will never know
about, or never use.  Because an environment hostile to small-time folks
like myself cuts away the life-blood of the community.

Fortunately, there's always OpenBSD!

![You called?](http://www.openbsd.org/art/puffy/ppuf800X725.gif)

This article isn't about OpenBSD's pledge.  Or even FreeBSD's capsicum,
which is eminently more usable than seccomp-bpf or libseccomp.

But really it kinda is.

# Solutions

I've now disabled the seccomp code in the GNUmakefile.

If you want to enable it, you're welcome to.  And if you add system
calls to the filter white-list, go right ahead.  If you want them to be
upstream, then I'll merge them.

However, there's still the issue of trust.  Can I trust that what I put
in that code --- even if it's "only for experts" --- isn't a whole
waiting to be exploited?  Am I putting other skilled people at risk?

There isn't an easy answer to this.
