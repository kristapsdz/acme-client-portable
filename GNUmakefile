.SUFFIXES: .dot .png .1 .1.html

CFLAGS	+= -g -W -Wall `curl-config --cflags` -DHAVE_CONFIG_H
OBJS 	 = acctproc.o \
	   base64.o \
	   certproc.o \
	   chngproc.o \
	   dbg.o \
	   dnsproc.o \
	   fileproc.o \
	   json.o \
	   keyproc.o \
	   main.o \
	   netproc.o \
	   revokeproc.o \
	   util.o

ifeq ($(shell uname), Linux)
# Compiling on Linux.
LIBJSON	 = -ljson
LIBBSD	 = -lbsd
OBJS	+= sandbox-null.o \
	   compat-setresuid.o \
	   compat-setresgid.o
else ifeq ($(shell uname), Darwin)
# Compiling on Mac OS X.
# If we show deprecations, everything in openssl shows up.
CFLAGS	+= -Wno-deprecated-declarations 
OBJS	+= sandbox-darwin.o \
	   compat-setresuid.o \
	   compat-setresgid.o
LIBJSON	 = -ljson-c
else ifeq ($(shell uname), OpenBSD)
# Compiling on OpenBSD.
# Obviously the following is a temporary solution...
ifeq ($(shell uname -r), 5.9)
OBJS	+= sandbox-pledge.o
else
OBJS	+= sandbox-null.o
endif
LIBJSON	 = -ljson-c
else ifeq ($(shell uname), FreeBSD)
# Compiling on FreeBSD.
OBJS	+= sandbox-null.o
LIBJSON	 = -ljson-c
endif

letskencrypt: $(OBJS)
	$(CC) -o $@ $(OBJS) -lssl -lcrypto `curl-config --libs` $(LIBJSON) $(LIBBSD)

rmerge:
	@for f in ../letskencrypt/*.[ch]; do \
		ff=`basename $$f` ; \
		TMP1=`mktemp /tmp/merge.XXXXXX` || exit 1 ; \
		TMP2=`mktemp /tmp/merge.XXXXXX` || exit 1 ; \
		tail -n+2 $$f > $$TMP1 ; \
		tail -n+2 $$ff > $$TMP2 ; \
		cmp $$TMP1 $$TMP2 ; \
		rc=$$? ; \
		rm -f $$TMP1 $$TMP2 ; \
		[ 0 -eq $$rc ] && continue ; \
		diff -u $$f $$ff | less ; \
		/bin/echo -n "Replace [Y/n]: " ; \
		read in ; \
		if [ -z "$$in" -o "y" = "$$in" -o "Y" = "$$in" ]; then \
			cp -f $$ff $$f ; \
		fi \
	done

merge:
	@for f in ../letskencrypt/*.[ch]; do \
		ff=`basename $$f` ; \
		TMP1=`mktemp /tmp/merge.XXXXXX` || exit 1 ; \
		TMP2=`mktemp /tmp/merge.XXXXXX` || exit 1 ; \
		tail -n+2 $$f > $$TMP1 ; \
		tail -n+2 $$ff > $$TMP2 ; \
		cmp $$TMP1 $$TMP2 ; \
		rc=$$? ; \
		rm -f $$TMP1 $$TMP2 ; \
		[ 0 -eq $$rc ] && continue ; \
		diff -u $$ff $$f | less ; \
		/bin/echo -n "Replace [Y/n]: " ; \
		read in ; \
		if [ -z "$$in" -o "y" = "$$in" -o "Y" = "$$in" ]; then \
			cp -f $$f $$ff ; \
		fi \
	done

install: letskencrypt
	mkdir -p $(PREFIX)/bin
	mkdir -p $(PREFIX)/man/man1
	install -m 0755 letskencrypt $(PREFIX)/bin
	install -m 0644 letskencrypt.1 $(PREFIX)/man/man1

$(OBJS): extern.h config.h

clean:
	rm -f letskencrypt $(OBJS)
	rm -f letskencrypt.png letskencrypt.1.html
	rm -rf letskencrypt.dSYM
