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

#include <sys/syscall.h>

#include <err.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <seccomp.h>

#include "extern.h"

static int
sandbox_allow(scmp_filter_ctx ctx, int call)
{

	if (0 == seccomp_rule_add(ctx, SCMP_ACT_ALLOW, call, 0))
		return(1);
	warn("seccomp_rule_add");
	return(0);
}

static int
sandbox_allow_cpath(scmp_filter_ctx ctx)
{

	if ( ! sandbox_allow(ctx, SCMP_SYS(open)))
		return(0);
	return(1);
}

static int
sandbox_allow_stdio(scmp_filter_ctx ctx)
{

	if ( ! sandbox_allow(ctx, SCMP_SYS(write)))
		return(0);
	if ( ! sandbox_allow(ctx, SCMP_SYS(read)))
		return(0);
	if ( ! sandbox_allow(ctx, SCMP_SYS(getpid)))
		return(0);
#ifdef __NR_mmap
	if ( ! sandbox_allow(ctx, SCMP_SYS(mmap)))
		return(0);
#endif
	if ( ! sandbox_allow(ctx, SCMP_SYS(munmap)))
		return(0);
	if ( ! sandbox_allow(ctx, SCMP_SYS(sigprocmask)))
		return(0);
	return(1);
}

int
sandbox_before(void)
{

	return(1);
}

/*
 * Use pledge(2) to sandbox based which process we're in.
 */
int
sandbox_after(int arg)
{
	scmp_filter_ctx	 ctx;

	(void)arg;

	switch (proccomp) {
	case (COMP_ACCOUNT):
	case (COMP_CERT):
	case (COMP_KEY):
	case (COMP_REVOKE):
	case (COMP__MAX):
		ctx = seccomp_init(SCMP_ACT_KILL);
		if (NULL == ctx) {
			warn("seccomp_init");
			return(0);
		}
		if ( ! sandbox_allow_stdio(ctx))
			return(0);
		break;
	case (COMP_CHALLENGE):
		ctx = seccomp_init(SCMP_ACT_KILL);
		if (NULL == ctx) {
			warn("seccomp_init");
			return(0);
		}
		if ( ! sandbox_allow_stdio(ctx))
			return(0);
		if ( ! arg && ! sandbox_allow_cpath(ctx))
			return(0);
		break;
	case (COMP_DNS):
	case (COMP_FILE):
	case (COMP_NET):
		break;
	}
	return(1);
}
