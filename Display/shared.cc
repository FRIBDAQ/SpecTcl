/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   shared.cc:
**     This file is responsible for all of the shared memory handling in
**     Xamine.   There is quite a bit of system dependent stuff in here to
**     make Xamine port between Unix and VMS, since those two systems have
**     very different models of shared memory handling.  When POSIX-Rt becomes
**     adopted and common, then we'll have a third model to deal with.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo = "@(#)shared.cc	2.3 5/27/94 \n";

/*
**  Include files:
*/

//
// Note: CYGWIN does not yet support shmat etc. we must therefore use
//       native MSWin32 calls: CreateFileMapping and MapViewOfFile to
//       manage the shared memory regions used by Xamine.
//
#ifdef unix
#include <sys/types.h>
#include <unistd.h>
#ifdef CYGWIN
#include <windows.h>
#else                         // CYGWIN
#include <sys/ipc.h>
#include <sys/shm.h>
#endif                        // CYGWIN
#include <sys/stat.h>
#endif
#ifdef Darwin
#include <sys/fcntl.h>
#include <sys/mman.h>
#endif

#ifdef VMS
#include <ssdef.h>
#include <descrip.h>
#include <psldef.h>
#include <secdef.h>
#include <lnmdef.h>
#endif
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <assert.h>
#include "dispshare.h"
/*
** Definitions:
*/

#define XAMINE_ENVNAME "XAMINE_SHMEM"
#define XAMINE_ENVSIZ  "XAMINE_SHMEM_SIZE"

#ifdef VMS
#define PAGESIZE 512		/* Bytes per VMS page(let). */
#endif
/*
** External references:
*/


extern volatile  spec_shared *xamine_shared;
extern volatile  spec_shared *spectra;


/*
** Static defs:
*/

static int memsize;


#ifdef VMS
#ifdef __ALPHA
#pragma member_alignment __save
#pragma nomember_alignment 
#endif
struct gsmatch {
                int criterion;
		short minor;	/* Not quite accurate def, but ok for now. */
		short major;
	      };
#ifdef __ALPHA
#pragma member_alignment __restore
#endif
extern "C" {
int sys$mgblsc(int *inadr, int *retadr, int acmode,
		int flags, struct dsc$descriptor *gsdnam,
		struct gsmatch *ident, int relpag);
int sys$adjwsl(int pagecnt, unsigned int *wsetlm);
}
#endif

/*
** Functional Description:
**   mapmemory:
**     This local function is actually responsible for performing the
**     map.  There is a distinct unix an VMS implementation of this
**     function body.
** Formal Parameters:
**   char *name:
**      Name of the shared memory region.
**   unsigned int size:
**      bytes in shared memory region.
** Returns:
**   Pointer to the shared memory or NULL on failure.
*/
static spec_shared *mapmemory(char *name, unsigned int size)
#ifdef unix
#if defined(Darwin)
{
  int fd = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR);
  if(fd < 0) {
    perror("shm_open failed in Xamine");
    return NULL;
  }

  void* pMem = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  if(pMem == (char*)-1) {
    perror("mmap failed in Xamine");
    return NULL;
  }
  close(fd);
  shm_unlink(name);
  return (spec_shared*)pMem;
}
#elif defined(CYGWIN)
{
  HANDLE hMapFile;
  LPVOID lpErrorMessage;

  /*   Works on NT but not 95/98 ???
  size += getpagesize()*64;
  hMapFile = CreateFileMapping((HANDLE)NULL,
			       (LPSECURITY_ATTRIBUTES)NULL,
			       (DWORD)PAGE_READWRITE,
			       (DWORD)0,
			       (DWORD)size,
			       (LPCTSTR)name);
  */
  hMapFile = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
			     FALSE, (LPCTSTR)name);
  if(! hMapFile ){
    FormatMessage(
		  FORMAT_MESSAGE_ALLOCATE_BUFFER |
		  FORMAT_MESSAGE_FROM_SYSTEM      |
		  FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPSTR) &lpErrorMessage, 0, NULL);
    fprintf(stderr, "OpenFileMapping Failed:\n%s", lpErrorMessage);
    fflush(stderr);
    LocalFree(lpErrorMessage);
    exit(-1);
  }

  void *pMemory = MapViewOfFile(hMapFile,
				FILE_MAP_ALL_ACCESS,
				(DWORD)0, (DWORD)0,
				(DWORD)size);
  assert(pMemory);		// Require that we got a map...

  // BUGBUG - Note: this is only half of the story.
  //          To prevent resource leaks, we also must ensure that 
  //          UnmapViewOfFile is called prior to program exit.
  //          For this version we put that on the todo list.
  //
  //  CloseHandle(hMapFile);
	
  return (spec_shared*)pMemory;
}
#else
{
  key_t key;			/* Shared memory key. */
  int   id;			/* Shared memory size. */
  char *memory;


  /* In the UNIX implementation the name is 4 chars which map to the key */

  memcpy(&key, name, sizeof(key));
  id  = shmget(key, size, 0);	/* Get the memory key.. */
  if(id < 0)
    return (spec_shared *)NULL;
 
  memory = (char *)shmat(id, NULL, SHM_RDONLY);

  // After attaching the shared memory region, it's marked for 
  // deletion. This prevents shared memory regions from hanging around
  // after the program exits.
  //
  shmctl(id, IPC_RMID, 0);        // Mark for deletion.
  return (spec_shared *)memory;
  
}

#endif // CYGWIN or not
#endif // UNIX
#ifdef VMS
{
  struct dsc$descriptor name_desc;
  struct gsmatch ident;
  int    inaddr[2], outaddr[2];
  int    status;

  /* First build the name descriptor for the global section name: */

  name_desc.dsc$a_pointer  = name;
  name_desc.dsc$w_length   = strlen(name);
  name_desc.dsc$b_dtype    = DSC$K_DTYPE_T;
  name_desc.dsc$b_class    = DSC$K_CLASS_S;

  /* Then build the match criteria */

  ident.criterion = SEC$K_MATALL; /* For now all can match. */
  ident.major     = 0;
  ident.minor     = 0;

  /* Build the inaddr and outaddr array: */
 
  inaddr[0]  = 0;
  inaddr[1]  = 0;

  /* now try the map: */

  status = sys$mgblsc(inaddr, outaddr,
		      PSL$C_USER,
		      SEC$M_EXPREG,
		      &name_desc,
		      &ident,
		      0);
  if((status & 1) != 1) {
    errno = EVMSERR;
    vaxc$errno = status;
    return (spec_shared *)NULL;
  }
  else {
    int pagecnt = (outaddr[1] - outaddr[0])/PAGESIZE;
    sys$adjwsl(pagecnt, 0);	/* Attempt to put shmem in wsl. */
    return (spec_shared *)outaddr[0];
  }
}
#endif

/*
** Functional Description:
**   Xamine_initspectra:
**     This function initializes Xamine's access to the shared spectra
**     This is done in two steps:
**     1. Translate the environment variables which describe the shared
**        memory region.
**     2. Map to the shared memory region.
**     The second step is highly system dependent, so there are separate
**     functions to do that work for each operating system supported.
** SIDE Effects:
**    The externals xamine_shared and spectra are set to point to the
**    shared memory region.
*/
void Xamine_initspectra()
{
  char *name;
  char *size_string;
  unsigned int size;


  /* First fetch the name and size string from the environment */

  name = getenv(XAMINE_ENVNAME); /* Get the shared memory name. */
  if(name == NULL) {
    perror("Xamine -- Could not translate shared memory environment name");
    exit(errno);
  }
  size_string = getenv(XAMINE_ENVSIZ);
  if(size_string == NULL) {
    perror("Xamine -- Could not translate shared memory size environment name");
    exit(errno);
  }
  /* Now convert the size string to a number: */

  size = atoi(size_string);
  if(size == 0) {
    fprintf(stderr, "Xamine -- Shared memory size string was illegal\n");
    exit(-1);
  }

  /* Now map to the memory: */



  memsize = sizeof(spec_shared) - DISPLAY_SPECBYTES + size;
  xamine_shared = mapmemory(name, size);
  spectra       = xamine_shared;
  if(spectra == (spec_shared *)NULL) {
    perror("Xamine -- map to shared memory failed");
    exit(errno);
  }

}

/*
** Functional Description:
**   Xamine_MemoryTop:
**     This function returns a pointer to the last mapped memory
**     cell in the Xamine shared memory region.
*/
char *Xamine_MemoryTop()
{
  char *bottom = (char *)xamine_shared;
  bottom      += (memsize-1);
  return      bottom;
}
