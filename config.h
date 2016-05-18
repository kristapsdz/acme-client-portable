#ifdef __linux__
# define _GNU_SOURCE
# include <bsd/stdlib.h>
# include <grp.h>
int	setresgid(gid_t, gid_t, gid_t);
int	setresuid(gid_t, gid_t, gid_t);
#endif

#ifdef __FreeBSD__
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <resolv.h>
#endif
