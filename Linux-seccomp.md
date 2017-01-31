# Introduction

After implementing a sandbox for
[acme-client](https://kristaps.bsd.lv/acme-client) with
[pledge(2)](http://man.openbsd.org/pledge.2) (easy),
[capsicum(4)](https://www.freebsd.org/cgi/man.cgi?query=capsicum&sektion=4)
(harder, but still easy-ish), and
[sandbox\_init(3)](https://developer.apple.com/legacy/library/documentation/Darwin/Reference/ManPages/man3/sandbox_init.3.html)
(easy but arguably useless), I set out to add a Linux sandbox with
[libseccomp](https://github.com/seccomp/libseccomp).

This isn't the first system I've sandboxed
([kcgi](https://kristaps.bsd.lv/kcgi),
[sblg](https://kristaps.bsd.lv/sblg), etc.); but for seccomp, it might
be the last.

*Aside: what's a sandbox?*  A sandbox defines the system resources our
application uses.  Using anything else results in program termination.
Each component of [acme-client](https://kristaps.bsd.lv/acme-client) is
differently sandboxed: some use only the file-system, some use only the
network, etc.  Sandboxes are your friend:
[pledge(2)](http://man.openbsd.org/pledge.2) on
[OpenBSD](http://www.openbsd.org),
[capsicum(4)](https://www.freebsd.org/cgi/man.cgi?query=capsicum&sektion=4)
on [FreeBSD](https://www.freebsd.org), and 
[libseccomp](https://github.com/seccomp/libseccomp) implementing
[seccomp-bpf](https://www.kernel.org/doc/Documentation/prctl/seccomp_filter.txt)
on [Linux](https://www.kernel.org).  There are others.

![Keep them in the sandbox!](http://1.bp.blogspot.com/-F2qtZSFIrAQ/UYk8KnQqZ7I/AAAAAAAAATg/RKs0uOCPdXU/s1600/sandbox+fight.jpg)

After implementing a filter that worked on a local
[Alpine](https://alpinelinux.org) system and releasing, I began
receiving failure reports for non-Alpine Linux distribution x, y, and z
for architecture foo, bar, and baz.  The failure reports were all for
filter violations, e.g.,
[issue/16](https://github.com/kristapsdz/acme-client-portable/issues/16),
and each presented a new system call that had been violated.
A filter violation is when a system call I didn't white-list is invoked.
Sometimes a system's C library would implement a function call
differently.  Sometimes an architecture would so so.  It always
differed.

What disturbed me was not only that I couldn't reproduce --- each report
was from a different C library, architecture, or distribution --- but
that I couldn't verify that there was a problem at all.  A crafty
adversary could have requested a given system call be white-listed while
knowing he could exploit it.  I wouldn't be the wiser unless I took care
to exactly reproduce each bug reporter's system.

![This is quite applicable.](http://gifrific.com/wp-content/uploads/2012/08/Aint-Nobody-Got-Time-for-That.gif)

I'm not saying this happened --- I had no reason to distrust any bug
reports, and on the contrary thank everybody who took the time to do so!
The fact remains, however, that it's possible.

I consider this a serious problem.  You should, too.

*Note:* I have interspersed this article with funny jokes.  In general
I'm very dour.  Don't be misled that I actually make jokes in real life.

# The seccomp problem

Programming
[seccomp-bpf](https://www.kernel.org/doc/Documentation/prctl/seccomp_filter.txt)
(even helped via [libseccomp](https://github.com/seccomp/libseccomp)) is
like programming assembly language.  Not because it's "hard" (it is ---
do you understand
[this](https://github.com/openssh/openssh-portable/blob/master/sandbox-seccomp-filter.c)?)
but that it's not portable: a new architecture means new (or different)
system calls; a new C library means new (or different) system calls; and
a new kernel version means new system calls.  "Same-same, but
different."

This is a problem with any capabilities system.  (See [Bugs Ex
Ante](https://www.youtube.com/watch?v=lqIXr_Or2s4) for a historical
perspective.)

For some capabilities systems like
[systrace(4)](http://www.citi.umich.edu/u/provos/systrace/) on OpenBSD,
this wasn't so bad because OpenBSD's system is uniform: one C library,
one system call interface.

On Linux, this is much more difficult. One can use
[musl](https://www.musl-libc.org/),
[glibc](https://www.gnu.org/software/libc/), or others.  Moreover, the
system calls themselves change depending on the architecture --- and
sometimes individual kernels are compiled with or without certain system
calls.  (Anything with "rt" --- ugh.)

When I set out to implement a seccomp filter, I naïvely assumed that the
differences wouldn't be so great between Linux systems.  Looking at OpenSSH's
[sandbox-seccomp-filter.c](https://github.com/openssh/openssh-portable/blob/master/sandbox-seccomp-filter.c),
for example, is quite straightforward.  (For eye-rollingly ridiculous
values of "straightforward".)

But this is not the case.  Why?

First, the mystery of system calls between hardware.  Some have `mmap`.
Some have `mmap2` (from a previous version of OpenSSH's filter, noted as
"EABI ARM only has `mmap2`").  Do I white-list both?  Just what is
`mmap2`?  [mmap2(2)](http://man7.org/linux/man-pages/man2/mmap2.2.html)
writes:

> On architectures where this system call is present, the glibc mmap()
> wrapper function invokes this system call rather than the mmap(2)
> system call.  

So I might *use* `mmap`, but the system call is `mmap2`.  Great.

This brings us to the second problem.  The C library.  There are several
popular ones on Linux: [glibc](https://www.gnu.org/software/libc),
[musl](https://www.musl-libc.org), [uClibc](https://uclibc.org/), etc.
Each of these is free to define any standard function (like `mmap`,
above) in any way.  So while my code might say `read`, the C library
might also invoke `fstat`.  Great.

Then there's local modifications.  And not just between special embedded
systems.  But Debian and Arch, both using
[glibc](https://www.gnu.org/software/libc), might have different kernels
installed with different features.  Great.

# The Linux problem

The seccomp problem is a Linux problem. 

If in order to secure a non-trivial application (where trivial is "pure
computation", i.e., no system calls, and not a measure of complexity)
requires users to baby-sit its runtime, then I've just narrowed my
potential audience to those who have the time and patience to do so.
Which, for them, means they want the software so badly they're willing
to sacrifice the time.

How often does that happen to you?

![What's the point?](http://www.theimaginativeconservative.org/wp-content/uploads/2014/05/peter-sellers-as-dr-strangelove-1.jpg)

Moreover, not everybody who really wants to use my software knows how to
program --- in fact, I'd guess that most don't at all, being that
[acme-client](https://kristaps.bsd.lv/acme-client) is for administering
certificates, not writing code.  So if my program is meant to provide a
service to people, but people can't use it --- what's the point of
releasing it as open source?

# The developer problem

When I first released
[acme-client](https://kristaps.bsd.lv/acme-client), I assumed that users
would let me know if their systems failed, and how.  What I didn't
anticipate was how much the seccomp filter would be violated even though
I carefully verified my test environment with a popular Linux on a
popular architecture.

In the end, I took a lot of my potential users' time, took a lot of my
own time tracking down system call violations, and alienated everybody
else would just gave up when it failed.

What's more embarrassing is that, to most users who simply downloaded
the software, the application just *failed*.  Not *seccomp failed*, but
*[acme-client](https://kristaps.bsd.lv/acme-client) failed*.

Ouch.  What solutions do I have?  If I stay with seccomp, I continue to
alienate my users, require skilled users to jump through hoops, and
spend lots of my own time to interpret incoming violation reports from
said mystical users.

All the while, other people trying the software just think it's poorly
written.

Fact is, using seccomp is difficult unless

1. the system really is trivial (pure computation), which reduces the
   probability of users having violations;
2. the users are extremely skilled, which reduces the problem that users
   will abandon the software; or
3. I have the resources to properly test on most systems before
   releasing.

It seems to me that seccomp is designed for big companies who can afford
the resources (like those rhyming with "doodle"), big open source
projects with the testing infrastructure like OpenSSH, or niche
scientific systems.

That leaves [acme-client](https://kristaps.bsd.lv/acme-client) out of
the loop.

Is this a problem?  It depends on the software, I guess.  But it's not
*my* software that should scare you.  It's the many unwritten pieces of
software, or written but unsecured, that you will never know about, or
never use.  Because an environment hostile to small-time folks like
myself cuts away the life-blood of the community.

Fortunately, there's always OpenBSD!

![You called?](http://www.openbsd.org/art/puffy/ppuf800X725.gif)

(This article isn't about OpenBSD's
[pledge(2)](http://man.openbsd.org/pledge.2).  Or even FreeBSD's
[capsicum(4)](https://www.freebsd.org/cgi/man.cgi?query=capsicum&sektion=4),
which is eminently more usable than seccomp-bpf or libseccomp.

But really it kinda is.

# Solutions

I've now disabled the seccomp code in the [GNUmakefile](GNUmakefile).

If you want to enable it, you're welcome to --- all of the code is still
there in [sandbox-seccomp.c](sandbox-seccomp.c), and easy to read through.  
Note that the debugging is enabled by default, and the functions it
isn't are safe to use in a signal context.  So if you use it a lot, that
might get annoying.

If you add system calls to the filter white-list, please notify me and
I'll make sure it makes its way into
[sandbox-seccomp.c](sandbox-seccomp.c) so you don't need to maintain a
downstream.

However, there's still the issue of trust.  Can I trust that what I put
in that code --- even if it's "only for experts" --- isn't a whole
waiting to be exploited?  Am I putting other skilled people at risk?
Can you trust what you see there, knowing it was provided by others?

There isn't an easy answer to this.

# Disclaimers

I don't have anything against libseccomp or seccomp except for something
that [I was
told](https://mail-index.netbsd.org/tech-kern/2007/12/29/0007.html) many
years ago: "I find the documentation inscrutable".
