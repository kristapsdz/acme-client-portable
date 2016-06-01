/*
 * If your system doesn't use "nobody" as a privilege separated user,
 * then change it here or in the GNUmakefile.
 */
#ifndef NOBODY_USER
# define NOBODY_USER "nobody"
#endif

/*
 * Require libbsd's stdlib.h for arc4random() etc.
 * Require <grp.h> for setgroups().
 */
#ifdef __linux__
# define _GNU_SOURCE
# include <bsd/stdlib.h>
# include <bsd/string.h>
# include <grp.h>
#endif

/*
 * Neither Linux nor Apple have these.
 */
#if defined(__linux__) || defined(__APPLE__)
# include <unistd.h>
int	setresgid(gid_t, gid_t, gid_t);
int	setresuid(gid_t, gid_t, gid_t);
#endif

/*
 * FreeBSD goop.
 */
#ifdef __FreeBSD__
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <resolv.h>
#endif
