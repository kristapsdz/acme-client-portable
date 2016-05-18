#ifdef __linux__
# define _GNU_SOURCE
# include <bsd/stdlib.h>
# include <grp.h>
#endif

#ifdef __FreeBSD__
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <resolv.h>
#endif
