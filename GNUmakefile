.SUFFIXES: .dot .png .1 .1.html

CFLAGS	+= -g -W -Wall -Wno-deprecated-declarations `curl-config --cflags` -DHAVE_CONFIG_H
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
LIBJSON	 = -ljson
LIBBSD	 = -lbsd
OBJS	+= sandbox-null.o
else ifeq ($(shell uname), Darwin)
OBJS	+= sandbox-darwin.o
LIBJSON	 = -ljson-c
else ifeq ($(shell uname), OpenBSD)
# 
# Obviously this is a temporary solution!
# 
ifeq ($(shell uname -r), 5.9)
OBJS	+= sandbox-pledge.o
else
OBJS	+= sandbox-null.o
endif
LIBJSON	 = -ljson-c
endif

letskencrypt: $(OBJS)
	$(CC) -o $@ $(OBJS) -lssl -lcrypto `curl-config --libs` $(LIBJSON) $(LIBBSD)

merge:
	cp -f ../letskencrypt/*.[ch] .

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
