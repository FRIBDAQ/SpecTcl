DESTDIR=$(INSTDIR)
CC = gcc -O -c   $(PROFILE) -Dunix -D$(OS)
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


clean:
	rm -f *.o *.a

depend:
	makedepend *.c

libtape.a: mtblock.o volume.o mtmessage.o mtforbind.o
	ar cq libtape.a mtblock.o volume.o mtmessage.o mtforbind.o
	ranlib libtape.a

mtblock.o: mtblock.c mtinternl.h mtshare.h
	$(CC) mtblock.c

mtforbind.o: mtforbind.c mtaccess.h mtshare.h
	$(CC) mtforbind.c

mtmessage.o: mtmessage.c mtshare.h rmf_msg.h
	$(CC) mtmessage.c

volume.o: volume.c mtinternl.h mtshare.h 
	$(CC) volume.c

tpvolread: tpvolread.f libtape.a mtaccess.f MTACCESS.FOR
	$(F77) tpvolread tpvolread.f -ltape

tpvolwrite: tpvolwrite.f libtape.a mtaccess.f MTACCESS.FOR
	$(F77) tpvolwrite tpvolwrite.f -ltape
	echo "Fortran not installed"


# DO NOT DELETE

mtblock.o: /usr/include/stdio.h /usr/include/features.h
mtblock.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
mtblock.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
mtblock.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
mtblock.o: /usr/include/bits/types.h /usr/include/libio.h
mtblock.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
mtblock.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
mtblock.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
mtblock.o: /usr/include/bits/posix2_lim.h /usr/include/errno.h
mtblock.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
mtblock.o: /usr/include/asm/errno.h mtinternl.h /usr/include/time.h
mtblock.o: /usr/include/bits/time.h mtshare.h
mtforbind.o: mtaccess.h mtshare.h /usr/include/ctype.h
mtforbind.o: /usr/include/features.h /usr/include/sys/cdefs.h
mtforbind.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
mtforbind.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
mtforbind.o: /usr/include/endian.h /usr/include/bits/endian.h
mtforbind.o: /usr/include/errno.h /usr/include/bits/errno.h
mtforbind.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
mtmessage.o: /usr/include/stdio.h /usr/include/features.h
mtmessage.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
mtmessage.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
mtmessage.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
mtmessage.o: /usr/include/bits/types.h /usr/include/libio.h
mtmessage.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
mtmessage.o: /usr/include/errno.h /usr/include/bits/errno.h
mtmessage.o: /usr/include/linux/errno.h /usr/include/asm/errno.h mtshare.h
mtmessage.o: rmf_msg.h
vmsstring.o: vmsstring.h
volume.o: /usr/include/stdio.h /usr/include/features.h
volume.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
volume.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
volume.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
volume.o: /usr/include/bits/types.h /usr/include/libio.h
volume.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
volume.o: /usr/include/time.h /usr/include/bits/time.h mtinternl.h mtshare.h
volume.o: /usr/include/ctype.h /usr/include/endian.h
volume.o: /usr/include/bits/endian.h /usr/include/errno.h
volume.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
volume.o: /usr/include/asm/errno.h
