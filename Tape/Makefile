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

mtblock.o: /usr/include/stdio.h /usr/include/standards.h
mtblock.o: /usr/include/sys/seek.h /usr/include/va_list.h
mtblock.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
mtblock.o: /usr/include/sys/select.h /usr/include/getopt.h
mtblock.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
mtblock.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
mtblock.o: /usr/include/sys/rt_limits.h /usr/include/limits.h
mtblock.o: /usr/include/errno.h mtinternl.h /usr/include/time.h
mtblock.o: /usr/include/sys/timers.h /usr/include/sys/sysmisc.h mtshare.h
mtforbind.o: mtaccess.h mtshare.h /usr/include/ctype.h
mtforbind.o: /usr/include/standards.h /usr/include/sys/types.h
mtforbind.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
mtforbind.o: /usr/include/sys/localedef.h /usr/include/sys/lc_core.h
mtforbind.o: /usr/include/locale.h /usr/include/nl_types.h
mtforbind.o: /usr/include/mesg.h /usr/include/stdio.h /usr/include/sys/seek.h
mtforbind.o: /usr/include/va_list.h /usr/include/getopt.h
mtforbind.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
mtforbind.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
mtforbind.o: /usr/include/sys/rt_limits.h /usr/include/limits.h
mtforbind.o: /usr/include/errno.h
mtmessage.o: /usr/include/stdio.h /usr/include/standards.h
mtmessage.o: /usr/include/sys/seek.h /usr/include/va_list.h
mtmessage.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
mtmessage.o: /usr/include/sys/select.h /usr/include/getopt.h
mtmessage.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
mtmessage.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
mtmessage.o: /usr/include/sys/rt_limits.h /usr/include/errno.h mtshare.h
mtmessage.o: rmf_msg.h
vmsstring.o: vmsstring.h
volume.o: /usr/include/stdio.h /usr/include/standards.h
volume.o: /usr/include/sys/seek.h /usr/include/va_list.h
volume.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
volume.o: /usr/include/sys/select.h /usr/include/getopt.h
volume.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
volume.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
volume.o: /usr/include/sys/rt_limits.h /usr/include/time.h
volume.o: /usr/include/sys/timers.h /usr/include/sys/sysmisc.h mtinternl.h
volume.o: mtshare.h /usr/include/ctype.h /usr/include/sys/localedef.h
volume.o: /usr/include/sys/lc_core.h /usr/include/locale.h
volume.o: /usr/include/nl_types.h /usr/include/mesg.h /usr/include/limits.h
volume.o: /usr/include/errno.h
