DESTDIR=$(INSTDIR)
CC = $($(OS)CC) -D__STDC__ -O -c   $(PROFILE) -Dunix -D$(OS)
F77 = echo "Need vms-like compiler"  f77 -w -O -L. -o


system: libtape.a
	echo "System completely built."

install: libtape.a mtshare.h mtaccess.f mtaccess.h
	install -m 0664 libtape.a $(DESTDIR)/Lib
	install -m 0664 mtshare.h $(DESTDIR)/Include
	install -m 0664 mtaccess.f $(DESTDIR)/Include
	install -m 0664 mtaccess.h $(DESTDIR)/Include
	install -m 0664  mtinternl.h $(DESTDIR)/Include


clean:
	rm -f *.o *.a

depend:
	makedepend -Y. *.c

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


