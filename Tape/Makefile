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

mtblock.o: /usr/include/stdio.h /usr/include/_ansi.h
mtblock.o: /usr/include/sys/config.h /usr/include/sys/reent.h
mtblock.o: /usr/include/sys/_types.h /usr/include/limits.h
mtblock.o: /usr/include/errno.h /usr/include/sys/errno.h mtinternl.h
mtblock.o: /usr/include/time.h /usr/include/machine/time.h
mtblock.o: /usr/include/sys/types.h /usr/include/machine/types.h
mtblock.o: /usr/include/sys/features.h mtshare.h
mtforbind.o: mtaccess.h mtinternl.h /usr/include/time.h /usr/include/_ansi.h
mtforbind.o: /usr/include/sys/config.h /usr/include/machine/time.h
mtforbind.o: /usr/include/sys/types.h /usr/include/machine/types.h
mtforbind.o: /usr/include/sys/features.h mtshare.h /usr/include/ctype.h
mtforbind.o: /usr/include/errno.h /usr/include/sys/errno.h
mtforbind.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
mtmessage.o: /usr/include/stdio.h /usr/include/_ansi.h
mtmessage.o: /usr/include/sys/config.h /usr/include/sys/reent.h
mtmessage.o: /usr/include/sys/_types.h /usr/include/errno.h
mtmessage.o: /usr/include/sys/errno.h mtshare.h rmf_msg.h
vmsstring.o: vmsstring.h
volume.o: /usr/include/stdio.h /usr/include/_ansi.h /usr/include/sys/config.h
volume.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
volume.o: /usr/include/stdlib.h /usr/include/time.h
volume.o: /usr/include/machine/time.h /usr/include/sys/types.h
volume.o: /usr/include/machine/types.h /usr/include/sys/features.h mtaccess.h
volume.o: mtinternl.h mtshare.h /usr/include/ctype.h /usr/include/errno.h
volume.o: /usr/include/sys/errno.h
