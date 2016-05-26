/*	$Id$ */
/*
 * Copyright (c) 2016 Kristaps Dzonsons <kristaps@bsd.lv>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHORS DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <err.h>
#include <unistd.h>

#include "extern.h"

int
dropfs(const char *path)
{

	/*
	 * Pity these systems have such a shitty or complicated security
	 * model that we need to do this.
	 */

	if (-1 == chroot(path))
		warn("%s: chroot", path);
	else if (-1 == chdir("/")) 
		warn("/: chdir");
	else
		return(1);

	return(0);
}

int
checkprivs(void)
{

	/*
	 * Since we're using chroot() and dropping privileges, we need
	 * to be root.
	 * So sad...
	 */

	return(0 == getuid());
}

int
dropprivs(uid_t uid, gid_t gid)
{

	/*
	 * Safely drop privileges into the given credentials.
	 * Pity we need to do this...
	 */

	if (setgroups(1, &gid) ||
	    setresgid(gid, gid, gid) ||
	    setresuid(uid, uid, uid)) {
		warnx("drop privileges");
		return(0);
	}

	if (getgid() != gid || getegid() != gid) {
		warnx("failed to drop gid");
		return(0);
	}
	if (getuid() != uid || geteuid() != uid) {
		warnx("failed to drop uid");
		return(0);
	}

	return(1);
}
