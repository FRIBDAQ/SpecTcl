DESTDIR=$(INSTDIR)
CC = $($(OS)CC) -D__STDC__ -O -c   $(PROFILE) -Dunix -D$(OS)
F77 = echo "Need vms-like compiler"  f77 -w -O -L. -o


system: libtape.a
	echo "System completely built."

install: libtape.a mtshare.h mtaccess.f mtaccess.h
	chmod -R u+w $(DESTDIR)/Include
	chmod -R g+w $(DESTDIR)/Include
	cp libtape.a $(DESTDIR)/Lib
	cp mtshare.h $(DESTDIR)/Include
	cp mtaccess.f $(DESTDIR)/Include
	cp mtaccess.h $(DESTDIR)/Include
	cp mtinternl.h $(DESTDIR)/Include


clean:
	rm -f *.o *.a

depend:
	makedepend *.c

libtape.a: mtblock.o volume.o mtmessage.o mtforbind.o
	ar cr libtape.a mtblock.o mtmessage.o mtforbind.o volume.o
	ranlib libtape.a

fake.o: fake.c
	$(CC) fake.c

mtblock.o: mtblock.c mtinternl.h mtshare.h
	$(CC) mtblock.c

mtforbind.o: mtforbind.c mtaccess.h mtshare.h
	$(CC) mtforbind.c

mtmessage.o: mtmessage.c mtshare.h rmf_msg.h
	$(CC) mtmessage.c

volume.o: volume.c mtinternl.h mtshare.h mtaccess.h
	$(CC) volume.c

tpvolread: tpvolread.f libtape.a mtaccess.f MTACCESS.FOR
	$(F77) tpvolread tpvolread.f -ltape

tpvolwrite: tpvolwrite.f libtape.a mtaccess.f MTACCESS.FOR
	$(F77) tpvolwrite tpvolwrite.f -ltape
	echo "Fortran not installed"


# DO NOT DELETE

mtblock.o: /usr/include/stdio.h /usr/include/features.h
mtblock.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
mtblock.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
mtblock.o: /usr/include/bits/types.h /usr/include/libio.h
mtblock.o: /usr/include/_G_config.h /usr/include/wchar.h
mtblock.o: /usr/include/bits/wchar.h /usr/include/gconv.h
mtblock.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
mtblock.o: /usr/include/bits/stdio_lim.h /usr/include/limits.h
mtblock.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/limits.h
mtblock.o: /usr/include/errno.h /usr/include/bits/errno.h
mtblock.o: /usr/include/linux/errno.h /usr/include/asm/errno.h mtinternl.h
mtblock.o: /usr/include/time.h /usr/include/bits/time.h mtshare.h
mtforbind.o: mtaccess.h mtinternl.h /usr/include/time.h
mtforbind.o: /usr/include/features.h /usr/include/sys/cdefs.h
mtforbind.o: /usr/include/gnu/stubs.h
mtforbind.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
mtforbind.o: /usr/include/bits/time.h /usr/include/bits/types.h mtshare.h
mtforbind.o: /usr/include/ctype.h /usr/include/endian.h
mtforbind.o: /usr/include/bits/endian.h /usr/include/errno.h
mtforbind.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
mtforbind.o: /usr/include/asm/errno.h
mtmessage.o: /usr/include/stdio.h /usr/include/features.h
mtmessage.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
mtmessage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
mtmessage.o: /usr/include/bits/types.h /usr/include/libio.h
mtmessage.o: /usr/include/_G_config.h /usr/include/wchar.h
mtmessage.o: /usr/include/bits/wchar.h /usr/include/gconv.h
mtmessage.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
mtmessage.o: /usr/include/bits/stdio_lim.h /usr/include/errno.h
mtmessage.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
mtmessage.o: /usr/include/asm/errno.h mtshare.h rmf_msg.h
vmsstring.o: vmsstring.h
volume.o: /usr/include/stdio.h /usr/include/features.h
volume.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
volume.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stddef.h
volume.o: /usr/include/bits/types.h /usr/include/libio.h
volume.o: /usr/include/_G_config.h /usr/include/wchar.h
volume.o: /usr/include/bits/wchar.h /usr/include/gconv.h
volume.o: /usr/lib/gcc-lib/i386-linux/2.95.4/include/stdarg.h
volume.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
volume.o: /usr/include/time.h /usr/include/bits/time.h mtaccess.h mtinternl.h
volume.o: mtshare.h /usr/include/ctype.h /usr/include/endian.h
volume.o: /usr/include/bits/endian.h /usr/include/errno.h
volume.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
volume.o: /usr/include/asm/errno.h
