/*
** Facility:
**   Xamine  - NSCL display program.
** Abstract:
**   client.c:
**     This file contains client side code for the AEDTSK. It will be put
**     into a library to which clients can link.  There are two sets of
**     calls in this file:
**        C callable  (these are Xamine_xxxx only).
**        native f77 callable (These are f77xamine_xxx(_) only).
**        See client.f for the AED compatibility library.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/


/*
** Includes:
*/
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef Darwin
#include <sys/mman.h>
#endif

#ifdef unix
#include <sys/types.h>
#include <unistd.h>
#ifdef CYGWIN
#include <windows.h>
#else
#include <sys/ipc.h>
#include <sys/shm.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>
#endif
#ifdef VMS
#include <ssdef.h>
#include <descrip.h>
#include <unixlib.h>
#include <psldef.h>
#include <secdef.h>
#include <lnmdef.h>
#include <clidef.h>
#include <jpidef.h>
#endif
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "client.h"
#include "clientgates.h"
#include "prccheck.h" 
#include "allocator.h"
#ifdef __ALPHA
#include <types.h>
#endif
/*
** Definitions.
*/

#ifdef unix
#define NAME_FORMAT "XA%02x"
#endif

#ifdef __ALPHA
#define GRANULE 65536		    /* Bytes in a map granule */
#endif

#ifdef VMS
#define NAME_FORMAT "XAMINE_%x"
#endif

#define SHARENV_FORMAT "XAMINE_SHMEM=%s" /* Environment names/logical names. */
#define SIZEENV_FORMAT "XAMINE_SHMEM_SIZE=%d"

#define XAMINEENV_FILENAME "XAMINE_IMAGE"

#ifndef HOME
#ifdef unix
#define XAMINE_PATH "/daq/bin/%s"
#endif
#ifdef VMS
#define XAMINE_PATH "XAMINE_DIR:%s"
#endif
#endif

#ifdef sparc
#define atexit(function) on_exit(function, 0)
#endif

/*
** DEFINE's.  These DEFINE's are used to derive the names of the message
** queues used by this module:
*/


/*
** External references:
*/

#ifdef VMS
#ifdef __ALPHA
#pragma member_alignment __save
#pragma nomember_alignment
#endif
struct gsmatch { int criterion;
		 short minor, major; /* Not quite accurate but close enough */
	       };
struct itemlist3   {
                    short buflen;
		    short code;
		    char *bufadr;
		    char *retlenadr;
		  };
#ifdef __ALPHA
#pragma member_alignment __restore
#endif
int sys$mgblsc(int *in, int *ret, int acmode, int flags,
	       struct dsc$descriptor *name,
	       struct gsmatch        *match,
	       int relpage);

int sys$crmpsc(int *in, int *ret, int acmode, int flags, 
	       struct dsc$descriptor *name, 
	       struct gsmatch *id, int relpage, int chan, 
	       int pagcnt, int vbn, int protection, int pfc);

int sys$crelnm(int *attr, struct dsc$descriptor *table,
	       struct dsc$descriptor *logname,
	       int *accmode, struct itemlist3 *itemlist);

int sys$getjpiw(int efn, pid_t *pid, struct dsc$descriptor *name,
	       struct itemlist3 *items, int *iosb, int *astadr, int astprm);

int lib$spawn(struct dsc$descriptor *command,
	      struct dsc$descriptor *input, struct dsc$descriptor *output,
	      int *mask, 
	      struct dsc$descriptor *process_name,
	      pid_t *pid, int *status, char *efnum,
	      void *ast, void *astarg,
	      struct dsc$descriptor *prompt,
	      struct dsc$descriptor *cliname);
#endif
/*
** Exported variables;
*/

volatile Xamine_shared *Xamine_memory;
int            Xamine_memsize;
arenaid        Xamine_memory_arena;
/*
** Local storage:
*/
static char *argv[] = { "Xamine",	/* Xamine argv block. */
		 NULL
	       };
static char *(env[8]) = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

static pid_t Xamine_Pid = 0;
static int   Xamine_Memid = -1;



#ifdef VMS			/* VMS version of putenv() */
/*
** Functional Description:
**    putenv:
**      This function is only needed in VMS implementations.
**      it puts a string into the 'environment' of the process.
**      In VMS, the environment of the process is a stripped down
**      environment area and the set of DCL symbols and logical names.
**      We break up the unix formatted name=equivalence into 
**      logicalname = name
**      equivalencestring = equivalence
**      and call sys$crelnm to set the string in the process 'environment'.
** Formal Parameters:
**   char *name:
**     Unix formatted environment name string.
** Returns:
**   0 - success.
**  -1 - Failure (mimics unix putenv()).
*/
static int putenv(char *name)
{
  int namelen;
  char *eqv;
  struct itemlist3 items[2];
  struct dsc$descriptor namedesc;
  struct dsc$descriptor tablename;
  int status;

  eqv = strchr(name, '=');		/* Locate the = sign. */
  if(eqv == NULL)
    return -1;
  else {
    namelen = eqv - name;	/* compute length of name. */
    eqv++;			/* Point to equivalence string. */
  }
  /* Build the name descriptor: */

  namedesc.dsc$a_pointer = name;
  namedesc.dsc$w_length  = namelen;
  namedesc.dsc$b_dtype   = DSC$K_DTYPE_T;
  namedesc.dsc$b_class   = DSC$K_CLASS_S;

  /* build the table name descriptor: */

  tablename.dsc$a_pointer = "LNM$PROCESS";
  tablename.dsc$w_length  = strlen(tablename.dsc$a_pointer);
  tablename.dsc$b_dtype   = DSC$K_DTYPE_T;
  tablename.dsc$b_class   = DSC$K_CLASS_S;
 
  /* Build the item list: */

  items[0].buflen       = strlen(eqv);
  items[0].code         = LNM$_STRING;
  items[0].bufadr       = eqv;
  items[0].retlenadr    = NULL;

  memset(&items[1], 0, sizeof(struct itemlist3));

  /* now do the define: */

  status = sys$crelnm((int *)NULL, &tablename, &namedesc, NULL, &(items[0]));
  if((status & 1) == 0)
    return -1;
  else
    return 0;

}

#endif

#ifdef unix
/*
** Functional Description:
**   killmem:
**      This UNIX only function is a cleanup function that's called to
**      ensure that shared memory segments will be cleaned up after
**      exit.
** Formal Parameters:
**   NONE:
*/
void killmem()
#ifdef CYGWIN
{
  UnmapViewOfFile(Xamine_memory);
}
#else
{
  if(Xamine_Memid > 0) {
    shmctl(Xamine_Memid, IPC_RMID, 0);	/* Give it our best shot. */
  }
}
#endif
#endif

/*
** Functional Description:
**   genname:
**      This local function generates a name for the shared memory region
** Formal Parameters:
**   char *name:
**      Buffer for shared memory region name.
** Returns:
**   TRUE -- success.
*/
static int genname(char *name)
{
  pid_t pid;

  pid = getpid();		/* Get the process id. */
#ifdef unix
  pid = (pid & 0xff);		/* Only take the bottom byte of pid. */
#endif
  sprintf(name, NAME_FORMAT, (int)pid);	/* Format the name. */
  return 1;
}

/*
** Functional Description:
**   genmem:
**     Function to generate the shared memory region and map to it.
** Formal Parameters:
**   char *name:
**     Name of region.
**   void **ptr:
**     Pointer to base buffer (set in VMS to desired base).
**   unsigned int size:
**     Total number of bytes in the region.
** Returns:
**    True - Success
**    False- Failure.
**  NOTE: In the Unix case, the **ptr value is modified to indicate where
**        the shared memory was allocated.
*/
static int genmem(char *name, volatile void **ptr, unsigned int size)
#ifdef unix
#if defined(Darwin)
{
  int fd;
  void* pMem;
  fd = shm_open(name,O_RDWR | O_CREAT  ,S_IRUSR | S_IWUSR);
  if(fd < 0) {
    perror("shm_open failed");
    *ptr = NULL;
    return FALSE;
  }

  if(ftruncate(fd, size) < 0) {
    perror("frtrunate failed");
    *ptr = NULL;
    return FALSE;
  }

  pMem = mmap(NULL, size, PROT_READ |PROT_WRITE, MAP_SHARED, fd, 0);
  if(pMem == (char*)-1) {
    perror("mmap failed");
    *ptr = NULL;
    return FALSE;
  }
  *ptr = pMem;
  close(fd);
  return TRUE;
}

#elif defined(CYGWIN)
{
  HANDLE hMapFile;
  void*  pMemory;
  size += getpagesize()*64;
  hMapFile = CreateFileMapping((HANDLE)0xffffffff,
			       (LPSECURITY_ATTRIBUTES)NULL,
			       (DWORD)PAGE_READWRITE,
			       (DWORD)0,
			       (DWORD)size,
			       (LPCTSTR)name);
  if(!hMapFile) return FALSE;
  pMemory = MapViewOfFile(hMapFile,
			  FILE_MAP_ALL_ACCESS,
			  (DWORD)0, (DWORD)0,
			  (DWORD)size);
  if(!pMemory) return FALSE;

  // BUGBUG - Note: this is only half of the story.
  //          To prevent resource leaks, we also must ensure that 
  //          UnmapViewOfFile is called prior to program exit.
  //          For this version we put that on the todo list.
  //
  //  CloseHandle(hMapFile);
  
  atexit(killmem);

  *ptr = pMemory;
  return TRUE;
}
#else
{				/* UNIX implementation. */
  key_t key;
  int   memid;
  char *base;

  /* Create the shared memory region: */

  memcpy(&key, name, sizeof(key));
  memid   = shmget(key, size, 
 	         (IPC_CREAT | IPC_EXCL) | S_IRUSR | S_IWUSR); /* Owner rd/wr */
  if(memid == -1) 
    return 0;

  /* Attach to the shared memory region: */

  base = (char *)shmat(memid, NULL, 0);
  if(base == NULL)
    return 0;

  Xamine_Memid = memid;		/* Save the memory id. for Atexit<. */
#ifdef unix			/* VMS GBL sections go away by themselves. */
  atexit(killmem);
#endif
  *ptr = (void *)base;
  return -1;			/* Indicate successful finish. */
}				/* Unix implementation. */
#endif
#endif
#ifdef VMS
{				/* VMS Implementation. */
  int inadr[2];		/* Input address... */
  int outadr[2];		/* Output address.  */
  struct dsc$descriptor namedesc; /* Section name descriptor. */
  struct gsmatch match;
  int    status;
#ifdef __ALPHA
  int    gcount;
#endif
  /* Build the descriptor for the section name */

  namedesc.dsc$a_pointer = name;
  namedesc.dsc$w_length  = strlen(name);
  namedesc.dsc$b_dtype   = DSC$K_DTYPE_T;
  namedesc.dsc$b_class   = DSC$K_CLASS_S;


  /* build the requested address block: */

  inadr[0] = (int)*ptr;
#ifdef __ALPHA
  gcount   = (size + GRANULE - 1)/GRANULE;
  inadr[1] = inadr[0] + gcount*GRANULE -1;
#else
  inadr[1] = inadr[0] + size-1;
#endif

  /* Build the match criterion */

  match.criterion = SEC$K_MATALL; /* For now all will match. */
  match.major     = 0;
  match.minor     = 0;

  /* Now try to map to the section. */

  status = sys$crmpsc(inadr, outadr, PSL$C_USER,
		      SEC$M_DZRO | SEC$M_GBL | SEC$M_PAGFIL | SEC$M_WRT,
		      &namedesc, &match, 0, 0, 
		      (size + PAGESIZE - 1)/PAGESIZE, 0, 
		      0xcfff, 4);
  if((status & 1) == 1)
    return 1;
  else {
    errno = EVMSERR;
    vaxc$errno = status;
    return 0;
  }
				     
}				/* VMS Implementation. */
#endif

/*
** Functional Description:
**  genenv:
**    Generate the environment strings needed by Xamine when it is run.
**    The strings are placed in this process' environment so that they can
**    be inherited by Xamine when it is run as a child process.
**    This is done by building up the env array and passing strings one by
**    one to putenv.  On unix, putenv is a library function however 
**    on VMS it is a module local function to create a process wide logical
**    name.
** Formal Parameters:
**   char *name:
**      Name of the global section/shared memory region.
**   int specbytes:
**      number of bytes in the shared memory region.
** Returns:
**    True    - Success
**    False   - Failure
*/
int genenv(char *name, int specbytes)
{
  /* Allocate persistent storage for the strings */

  env[0] = malloc(strlen(name) + strlen(SHARENV_FORMAT) + 1);
  if(env[0] == NULL)
    return 0;

  env[1] = malloc(strlen(SIZEENV_FORMAT) + 20);
  if(env[1] == NULL) {
    free(env[0]);
    return 0;
  }
  /* Generate the environment variables: */

  sprintf(env[0], SHARENV_FORMAT, name);
  sprintf(env[1], SIZEENV_FORMAT, specbytes);

  if(putenv(env[0])) {
    free(env[0]);
    free(env[1]);
    return 0;
  }

  if(putenv(env[1])) {
    free(env[1]);
    return 0;
  }
  return 1;
}

/*
** Functional Description:
**    Xamine_CreateSharedMemory:
**       This function creates the shared memory region for Xamine.
**       On Unix systems, the shared memory region is dynamically located
**       since essentially all UNIX fortrans support POINTER declarations.
**       On VMS systems, the shared memory base address is gotten from the
**       pointer that's passed in to us.
** Formal Parameters:
**   int specbytes:
**      Number of bytes in the spectrum region of the shared memory region.
**   Xamine_shared **ptr:
**      In Unix, this pointer will be filled in with the final location of
**      the shared memory region.  In VMS, this pointer will contain
**      the requested location of the shared memory region.
** Returns:
**      1  - If successful (Boolean true).
**      0  - If failed with error in errno (vaxc$errno if on VMS).
*/

/* Fortran call: */

#ifdef VMS
int f77xamine_createsharedmemory
#endif
#ifdef unix
int f77xamine_createsharedmemory_
#endif
                                  (int *specbytes,volatile Xamine_shared **ptr)
{
  int stat;
  stat = Xamine_CreateSharedMemory(*specbytes, ptr);
#ifdef VMS
  if(!stat) {
    stat = errno;
    if(errno == EVMSERR) stat = vaxc$errno;
  }
  else
    stat = SS$_NORMAL;
#endif
  return stat;
}

/* C call: */

int Xamine_CreateSharedMemory(int specbytes,volatile Xamine_shared **ptr)
{
  char name[33];

  if(!genname(name))		/* Generate the shared memory name. */
    return 0;

  if(!genmem(name, 
	     (volatile void **)ptr,	/* Gen shared memory region. */
	     sizeof(Xamine_shared) - XAMINE_SPECBYTES + specbytes))
    return 0;

  if(!genenv(name, specbytes))	/* Generate the subprocess environment. */
    return 0;

  Xamine_memsize= specbytes;
  Xamine_memory = *ptr;		/* Save poinyter to memory for mgmnt rtns. */
  return 1;			/* set the success code. */
}
int Xamine_DetachSharedMemory()
{
#ifdef CYGWIN
  UnmapViewOfFile(Xamine_memory);
#else
  return shmdt(Xamine_memory);
#endif
}

/*
** Functional Description:
**  Xamine_Start:
**    This function starts the Xamine process.  It is considered an error
**    to start Xamine twice.  We test for non zero Xamine_pid and
**    for a process which matches the pid.  That's the extent of our
**    check for duplicate processes.
** Returns:
**    True for success.
**    False for failure.
*/

/* F77 call: */

#ifdef VMS
int f77xamine_start()
#endif
#ifdef unix
int f77xamine_start_()
#endif
{
  int stat;
  stat =  Xamine_Start();
#ifdef VMS
  if(!stat) {
    stat = errno;
    if(errno == EVMSERR) stat = vaxc$errno;
  }
  else
    stat = SS$_NORMAL;
#endif
  return stat;
}
/* C call:   */

int Xamine_Start()
{
  char filename[100];
  int  vfstat;
  /* Ensure that Xamine is not already running: */

  if(Xamine_Pid != 0) {
    if(ProcessAlive(Xamine_Pid)) {
      errno  = EEXIST;
      return 0;
    }
    else {
      Xamine_Pid = 0;
    }
  }
  /* Now start Xamine: we use vfork/execv since that works on both  */
  /* VMS and unix. */


  if(getenv(XAMINEENV_FILENAME) != NULL) {
    strcpy(filename, getenv(XAMINEENV_FILENAME));
  }
  else {
#ifdef HOME
    sprintf(filename,"%s/Bin/%s", HOME, "Xamine2_0");
#else
    sprintf(filename,XAMINE_PATH, "Xamine2_0");
#endif
  }
#ifdef unix
  /* The code belows makes sure that none of the stdin, stdout and stderr
  ** file descriptors is closed on us.
  */
  {
    int inflg, errflg, outflg;
    inflg  = fcntl(0, F_GETFD, 0);
    outflg = fcntl(1, F_GETFD, 0);
    errflg = fcntl(2, F_GETFD, 0);
    fcntl(0, F_SETFD, 0);
    fcntl(1, F_SETFD, 0);
    fcntl(2, F_SETFD, 0);
    /*
      Unix sockets require an open and accept phase for the ipc pipes.
      here we open the pipes, after xamine starts we accept the
      connections from Xamine
    */
    if(!Xamine_OpenPipes()) {
      return 0;
    }
    vfstat = vfork();
    if(vfstat == 0) {		/* Subprocess context */
      execv(filename, argv);
      return 0;			/* Could not finish. */
    }
    else {			/* Main process context. */
      fcntl(0, F_SETFD, inflg);
      fcntl(1, F_SETFD, outflg);
      fcntl(2, F_SETFD, errflg);
      if(vfstat != -1) {
	Xamine_Pid = vfstat;
	return Xamine_AcceptPipeConnections();
      }
      else {
	return 0;
      }
    }
  }
#endif
#ifdef VMS
{
    int mask;
    struct dsc$descriptor cmd_desc;
    struct dsc$descriptor name_desc;
    struct dsc$descriptor in_desc;
    char   command[100];
    char   name[17];

    /* Build up the mask for the spawn: */

    mask = CLI$M_NOWAIT;    /* Run child process asynch */

    /* Build up the command string and descriptor from the file name: */

    sprintf(command, "RUN %s",filename);
    cmd_desc.dsc$a_pointer = command;
    cmd_desc.dsc$w_length  = strlen(command);
    cmd_desc.dsc$b_dtype   = DSC$K_DTYPE_T;
    cmd_desc.dsc$b_class   = DSC$K_CLASS_S;

    /* Next build up the process name, it's going to be Xamine_PID */
   
    sprintf(name, "Xamine_%x", getpid());
    name_desc.dsc$a_pointer = name;
    name_desc.dsc$w_length  = strlen(name);
    name_desc.dsc$b_dtype   = DSC$K_DTYPE_T;
    name_desc.dsc$b_class   = DSC$K_CLASS_S;

    /* Next build the input filename descriptor.  we use _NLA0: as that will */
    /* make Xamine insensitive to CONTROL-Y's in the parent process          */

    in_desc.dsc$a_pointer   = "_NLA0:";
    in_desc.dsc$w_length    = strlen("_NLA0:");
    in_desc.dsc$b_dtype     = DSC$K_DTYPE_T;
    in_desc.dsc$b_class     = DSC$K_CLASS_S;

    /* Now execute the spawn command:  */

    vfstat = lib$spawn(&cmd_desc, &in_desc, NULL, &mask,
		       &name_desc, &Xamine_Pid, NULL, NULL, 
		       NULL, NULL, NULL, NULL);
    if((vfstat & 1) == 1) {
       return Xamine_OpenPipes();
    }
    else {
      errno = EVMSERR;
      vaxc$errno = vfstat;
      return 0;
   }
}
#endif
}

/*
** Functional Description:
** Xamine_Stop:
**    This function stops the Xamine process.  Unlike the AEDTSK, we leave
**    the client mapped to the shared memory resources.  This is because
**    re-establishing a map to private resources is so system dependent
**    that at this point I don't want to think about it.... later maybe.
**    Therefore the flag: (BUGBUGBUG)
** Returns:
**    True   - Success.
**    False  - Failure.
*/
#ifdef unix
int f77xamine_stop_()
#endif
#ifdef VMS
int f77xamine_stop()
#endif
{
  int stat;
  stat =  Xamine_Stop();

#ifdef VMS
  if(!stat) {
    stat = errno;
    if(errno == EVMSERR) stat = vaxc$errno;
  }
  else
    stat = SS$_NORMAL;
#endif
  return stat;
}
int Xamine_Stop()
{
  if(Xamine_Pid == 0)
    return 0;

  if(!kill(Xamine_Pid, SIGKILL)) {
    Xamine_Pid = 0;
    return 1;
  }
  else
    return 0;
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Xamine_Alive:
**         This function returns true if Xamine is running.
**
**  RETURN VALUE:
**
**      TRUE   - Xamine is running
**      FALSE  - Xamine is not running.
**
**--
*/
int 
#ifdef unix
    f77xamine_alive_()
#endif
#ifdef VMS
    f77xamine_alive()
#endif
{

    return Xamine_Alive();
}
int Xamine_Alive()
{
    if(Xamine_Pid == 0)
      return 0;		    /* Never started Xamine */
    if(!ProcessAlive(Xamine_Pid)) {  /* Started but it exited */
      Xamine_Pid = 0;
      return 0;           
    }
    return -1;
}

/*
** Functional Description:
**   Xamine_GetMemoryName:
**     This function retuzrns the name of the shared memory region
**     that will be used for Xamine.  This allows the client to publish the
**     name in a way that allows other processes to get at it to manipulate
**     the same memory region.
** Formal Parameters:
**   char *namebuffer:
**      Buffer for the name (must be large enough).
*/
void Xamine_GetMemoryName(char *namebuffer)
{
  genname(namebuffer);
}
#ifdef unix
void f77xamine_getmemoryname_(char *namebuffer, int maxlen)
{
  char name[80];

  Xamine_GetMemoryName(name);
  memset(namebuffer, ' ', maxlen); /* Blank fill... */

  /* The trick is to adhere to Fortran blank fill conventions. */

  strncpy(namebuffer, name, maxlen);
  if(strlen(name) < maxlen) namebuffer[strlen(name)-1] = ' ';

}
#endif
#ifdef VMS
void f77xamine_getmemoryname(struct dsc$descriptor *namedesc) 
{
  char name[80];
  int  blanks;
  Xamine_GetMemoryName(name);

  strncpy(namedesc->dsc$a_pointer, name, namedesc->dsc$w_length);
  blanks = namedesc->dsc$w_length - strlen(name);
  memset(&(namedesc->dsc$a_pointer[strlen(name)]), ' ', blanks);
  
}
#endif

/*
** Functional Description:
**   Xamine_MapMemory:
**     This function is used to map to an existing shared memory region
**     given it's name.  The idea is that a histogrammer could fire up Xamine
**     and publish the name and size of the shared memory region using
**     Xamine_GetMemoryName.
** Formal Parameters:
**    char *name:
**       Name of the shared memory region.
**    int specbytes:
**       Number of bytes of spectrum storage.
**    Xamine_shared **ptr:
**       In VMS, this is an input pointer to the desired map location.
**       In UNIX, the map location is determined by the map operation
**        and this argument is an output only.
**       The structure of the INLCUDE files in FORTRAN make this transparent
**       if always used as an input.  In C, conditional compilation allow
**       common sources.
** Returns:
**    1 - If successful.
**    0 - If failed.  Error reason is in the usual errno crap.
*/

/*
** First we take care of the Fortran call interface: 
*/

#ifdef VMS
int f77xamine_mapmemory(struct dsc$descriptor *namedesc, int *specbytes, 
			  volatile Xamine_shared **ptr)
{
  char name[80];		/* Holds the name text. */
  char *p;


  /* figure out how much of the filename to copy and fill the whole
  ** buffer in with NULLs.
  */

  int nchar = sizeof(name);
  if(nchar > namedesc->dsc$w_length) nchar = namedesc->dsc$w_length;
  memset(name, 0, sizeof(name));

  /* copy the name and trim the blanks:
  */
  strncpy(name, namedesc->dsc$a_pointer,
	  nchar);
  nchar = strlen(name);
  p     = &(name[nchar-1]);	/* Point to the last character. */
  while(p != name) {
    if(!isspace(*p)) break;
    *p-- = '\0';		/* Null fill trailing whitespace. */
  }
  /*
  * Now call the Native C routine and return the result: 
  */

  return Xamine_MapMemory(name, *specbytes, ptr);

	  
}
#else
int f77xamine_mapmemory_(char *name, int *specbytes,
			 volatile Xamine_shared **ptr,
			 int namesize)
{
  char n[80];			/* Local copy of name. */
  char *p;

  /* Copy the name, turncating if necessary. with ensured null termination */

  if(namesize > (sizeof(n)-1)) namesize = sizeof(n)-1;
  memset(n, 0, sizeof(n));
  strncpy(n, name, namesize);

  /* Trim the name:  */

  p = &(n[strlen(n)-1]);
  while(n != p) {
    if(!isspace(*p)) break;
    *p-- = '\0';		/* Null fill trailing whitespace. */
  }

  /* call the native C function: */

  return Xamine_MapMemory(n, *specbytes, ptr);
}
#endif
/*
** The code which follows is the native C implementation of 
**  Xamine_MapMemory which is described by the comment header way up there
** It's completely system dependent.
*/
int Xamine_MapMemory(char *name, int specbytes,volatile Xamine_shared **ptr)
#ifdef unix
#ifdef CYGWIN
{
  HANDLE hMapFile;
  void*  pMemory;
  unsigned int memsize;
 
  memsize =  sizeof(Xamine_shared) - XAMINE_SPECBYTES + specbytes;
  hMapFile = CreateFileMapping((HANDLE)NULL,
			       (LPSECURITY_ATTRIBUTES)NULL,
			       (DWORD)PAGE_READWRITE,
			       (DWORD)0,
			       (DWORD)memsize,
			       (LPCTSTR)name);
  if(!hMapFile) return FALSE;
  pMemory = MapViewOfFile(hMapFile,
			  FILE_MAP_ALL_ACCESS,
			  (DWORD)0, (DWORD)0,
			  (DWORD)memsize);
  if(!pMemory) return FALSE;

  // BUGBUG - Note: this is only half of the story.
  //          To prevent resource leaks, we also must ensure that 
  //          UnmapViewOfFile is called prior to program exit.
  //          For this version we put that on the todo list.
  //
  CloseHandle(hMapFile);
  
  atexit(killmem);

  *ptr = pMemory;
  Xamine_memory = pMemory;
  return TRUE;
}
#else
{
  int memsize;
  key_t key;
  int   shmid;

  /* First generate the size of the shared memory region in bytes: */

  memsize = sizeof(Xamine_shared) - XAMINE_SPECBYTES + specbytes;
  key  = *(key_t *)name;		/* Convert name to key. */

  /* get the shared memory id from the key and size: */

  shmid = shmget(key, memsize,0);
  if(shmid == -1) return 0;

  /* Now map to the memory read/write */

  *ptr = (Xamine_shared *)shmat(shmid, 0, 0);
  
  /* Return success if shmat returned a non null pointer */

  Xamine_memory = *ptr;		/* Save memory pointer for mgmnt rtns. */
  return (*ptr ? 1 :
	         0);
}
#endif
#endif
#ifdef VMS
{
  struct dsc$descriptor namedesc;
  struct gsmatch        match;
  int    inadr[2];
  int    istat;
#ifdef __ALPHA
  int gcount;
#endif

  /* Set up the namedesc global section name descriptor:
  */

  namedesc.dsc$a_pointer = name;
  namedesc.dsc$w_length  = strlen(name);
  namedesc.dsc$b_dtype   = DSC$K_DTYPE_T;
  namedesc.dsc$b_class   = DSC$K_CLASS_S;

  /* Set up the global section match criteria: */
 
  match.criterion            = SEC$K_MATALL;
  match.minor  = match.major = 0; /* Match section 0.0 */

  /* Set up the range of addresses to be mapped:  */

  inadr[0] = inadr[1] = (int)*ptr;
#ifdef __ALPHA
  gcount = (sizeof(Xamine_shared) - XAMINE_SPECBYTES + specbytes
	     + GRANULE - 1)/GRANULE;
  inadr[1]= inadr[0] + gcount*GRANULE - 1;
#else
  inadr[1] += (sizeof(Xamine_shared) - XAMINE_SPECBYTES + specbytes +
	       PAGESIZE-1);
#endif
  istat = sys$mgblsc(inadr, 0, PSL$C_USER,
		     SEC$M_WRT, &namedesc, &match, 0);
  if(istat != SS$_NORMAL) {
    errno = EVMSERR;
    vaxc$errno = istat;
    return 0;
  }
  Xamine_memory = *ptr;
  return 1;
}
#endif

/*
** Functional Description:
**    Xamine_ManageMemory:
**      Declare to Xamine's client library that you want it to manage
**      Xamine's shared memory region.
**      NOTE: A client has a choice... either manage the storage him/her
**            self or let us do it.  Not both or things will get horribly
**            confused.
*/
void Xamine_ManageMemory()
{
  Xamine_memory_arena = alloc_init(Xamine_memory->dsp_spectra.XAMINE_b,
				   Xamine_memsize);


}
#ifdef unix
void f77xamine_managememory_()
#endif
#ifdef VMS
void f77xamine_managememory()
#endif
{
  Xamine_ManageMemory();
}

/*
** Functional Description:
**   Xamine_AllocMemory:
**      Allocates memory from the shared memory region and returns
**      a pointer to it if successful.
**      If fails, returns NULL.
**      NOTE:  The fortran version returns the byte offset of the memory
**             region.  If unable, it returns -1.
** Formal Parameters:
**    int (*)size:
**       Number of bytes of storage to allocate.
*/
caddr_t Xamine_AllocMemory(int size)
{
  return alloc_get(Xamine_memory_arena, size);
}

#ifdef unix
long f77xamine_allocmemory_(int *size)
#endif
#ifdef VMS
long f77xamine_allocmemory(int *size)
#endif
{
  caddr_t loc = Xamine_AllocMemory(*size);
  unsigned long    base;
  unsigned long    l;

  if(loc == NULL)
    return -1;

  base = (unsigned long)Xamine_memory->dsp_spectra.XAMINE_b;
  l    = (unsigned long)loc;


  return (int)(l - base);
  
}

/*
** Functional Description:
**    Xamine_FreeMemory:
**       Releases previously allocated spectrum shared memory.
** Formal Parameters:
**    caddr_t loc:
**      location to release.  NOTE: on f77 versions this is a byte offset.
*/
void Xamine_FreeMemory(caddr_t loc)
{
  alloc_free(Xamine_memory_arena, loc);
}

void
#ifdef unix
f77xamine_freememory_
#endif
#ifdef VMS
f77xamine_freememory
#endif
(int *loc)
{
  Xamine_FreeMemory(&(Xamine_memory->dsp_spectra.XAMINE_b[*loc]));
}

/*
** Functional Description:
**   Xamine_DescribeSpectrum:
**      This function fills in the description of the designated spectrum
**      area.  In reading the code below, note that we use Fortran indexing
**      conventions (that's why spno-1 is used).
** Formal Parameters:
**   int spno:
**      Number of the spectrum to modify.
**   int xdim:
**      Number of channels in X direction.
**   int ydim:
**      Number of channels in Y direction ... only used if 2-d.
**   char *title:
**      Name of the spectrum... only referenced if non-null.
**   caddr_t loc:
**      Location of the spectrum (must be in the dsp_spectra array space).
**      NOTE: For fortran this is a byte offset which will be appropriately
**            divided.
**      Memory must be properly aligned.
**  spec_type type:
**      Type of spectrum to create.
*/
void Xamine_DescribeSpectrum(int spno, int xdim, int ydim, char *title,
			     caddr_t loc, spec_type type)
{
  int channels;
  int bpc;
  int bytes;
  unsigned long spec;
  unsigned long  base = (unsigned long)Xamine_memory->dsp_spectra.XAMINE_b;
  int offset;

  /*
  ** Adjust the spectrum number and range check it:
  */
  spno--;			/* Adjust to C indexing. */

  /*
  **  Figure out the spectrum offset.  We had planned to 
  **  assert that the spectrum was in bounds but that can't link well  in UNIX
  */
  channels = xdim;		/* Compute # of channels. */
  if( ( type == twodword ) || ( type == twodbyte )) {
    channels *= ydim;
  }
  switch(type) {		/* Compute bytes per channel. */
  case onedlong:
    bpc = sizeof(int);
    break;
  case onedword:
  case twodword:
    bpc = sizeof(short);
    break;
  case twodbyte:
    bpc = sizeof(char);
    break;

  }
  bytes = channels * bpc;	/* Spectrum size in bytes. */
  spec  = (unsigned long)loc;
  offset = spec - base;
  offset = offset/bpc;


  /* Fill in everything but the title... that needs some fancy footwork */

  Xamine_memory->dsp_xy[spno].xchans = xdim;
  Xamine_memory->dsp_xy[spno].ychans = ydim;
  Xamine_memory->dsp_offsets[spno]   = offset;
  Xamine_memory->dsp_types[spno]     = type;

  /* The title needs to be cleared and then if there is a title present,
  ** We copy it from the user's input.
  */

  memset(Xamine_memory->dsp_titles[spno], 0, sizeof(spec_title));
  if(title != NULL) 
    strncpy(Xamine_memory->dsp_titles[spno], title, sizeof(spec_title)-1);


}
#ifdef unix
void f77xamine_describespectrum_(int *spno, int *xdim, int *ydim,
				char *title, int *loc, spec_type *type,
				int tsize)
{
  int nc = sizeof(spec_title)-1;
  spec_title tstr;
  caddr_t spec = (caddr_t)&Xamine_memory->dsp_spectra.XAMINE_b[*loc];

  if(title == NULL) {
    Xamine_DescribeSpectrum(*spno, *xdim, *ydim, NULL, spec, *type);
  }
  else {
    
    if(tsize < nc) nc = tsize;
    
    memset(tstr, 0, sizeof(spec_title));
    strncpy(tstr, title, nc);
    
    Xamine_DescribeSpectrum(*spno, *xdim, *ydim,
			  tstr, spec, *type);
  }
}
#endif
#ifdef VMS
void f77xamine_describespectrum(int *spno, int *xdim, int *ydim, 
				struct dsc$descriptor *title, int *loc,
				spec_type *type)
{
  caddr_t spec = (caddr_t)&Xamine_memory->dsp_spectra.XAMINE_b[*loc];

  if(title == NULL) {
    Xamine_DescribeSpectrum(*spno, *xdim, *ydim, NULL, spec, *type);
  }
  else {
    spec_title tstr;
    int        nc = title->dsc$w_length;
    memset(tstr, 0, sizeof(spec_title));
    if(( sizeof(spec_title)-1) < nc) nc = sizeof(spec_title)-1;
    strncpy(tstr, title->dsc$a_pointer, nc);
    
    Xamine_DescribeSpectrum(*spno, *xdim, *ydim, tstr, spec, *type);
  }
}
#endif

/*
** Functional Description:
**   Xamine_Allocate1d:
**      Allocates and describes a 1-d spectrum.  Storage and a spectrum
**      number are allocated.
** Formal Parameters:
**    int *spno:
**       Will contain the number of the spectrum allocated on successful
**       return.
**    int xdim:
**       Channels of spectrum.
**    char *title:
**       Title to give the spectrum (NULL gives no change in title).
**    int word:
**       True if the spectrum is word sized not long sized.
** Returns:
**   Pointer to the spectrum storage allocated.
**   Note that f77 versions return the offset word to use for the appropriate
**   spectrum type.
*/
caddr_t Xamine_Allocate1d(int *spno, int xdim, char *title, int word)
{
  int      bytes;
  caddr_t  storage;
  int      num;
  /* Allocate the spectrum and storage for the channels: */

  bytes   = word ? xdim * sizeof(short) : xdim * sizeof(int);
  storage = Xamine_AllocMemory(bytes);
  if(storage == NULL) return NULL;

  num     = Xamine_AllocateSpectrum();
  if(num == 0) {
    Xamine_FreeMemory(storage);
    return NULL;
  }
  /* Now describe the spectrum:   */
  
  Xamine_DescribeSpectrum(num, xdim, 0, title, storage,
			  word ? onedword : onedlong);
  
  /* Return the stuff the user expects to get back */
  
  *spno = num;
  return storage;
}

#ifdef unix
long f77xamine_allocate1d_(int *spno, int *xdim, char *title, int *word,
			  int tlen)
{
  spec_title t;
  int       sz = tlen;
  char     *tx = title;

#endif
#ifdef VMS
long f77xamine_allocate1d(int *spno, int *xdim, struct dsc$descriptor *title,
			 int *word)
{
  spec_title t;
  int       sz = title->dsc$w_length;
  char     *tx = title->dsc$a_pointer;
#endif
  char *spectrum;
  long  offset;
  long  base = (long) Xamine_memory->dsp_spectra.XAMINE_b;

  /* Produce C title string */

  if(sz > (sizeof(spec_title)-1)) sz = sizeof(spec_title)-1;
  memset(t, 0, sizeof(spec_title));
  strncpy(t, tx, sz);

  /* Allocate the spectrum:  */

  spectrum = (char *)Xamine_Allocate1d(spno, *xdim, t, *word);
  if(spectrum == NULL) return -1;

  offset = (long)spectrum - 
           base; /* Byte offset. */
  offset = *word ? offset / sizeof(short)	/* Word offset. */
                 : offset / sizeof(int); /* Long offset. */

  return offset;
}

/*
** Functional Description:
**    Xamine_Allocate2d:
**      This function allocates storage for a 2d spectrum, allocates a 
**      spectrum description entry and fills in the spectrum description
**      as requested.
** Formal Parameters:
**   int *spno:
**     Points to a buffer to hold the spectrum number allocated.
**   int xdim, ydim:
**     Specifieds the dimensions of the spectrum.
**   char *title:
**     Specifies the spectrum title if present (NULL Means untitled).
**   int byte:
**     True if the spectrum is represented as 1 byte per channel or false
**     otherwise.
** Returns:
**    Pointer to the spectrum memory.  NOte that f77 versions return the
**    spectrum offset value.
*/
caddr_t Xamine_Allocate2d(int *spno, int xdim, int ydim, char *title, int byte)
{
  int s_amount = xdim*ydim;	/* Number of channels... */
  caddr_t storage;
  
  /* Allocate the spectrum storage: */

  if(!byte) s_amount = s_amount * sizeof(short); /* Number of bytes of storage. */
  storage = Xamine_AllocMemory(s_amount);
  if(storage == NULL) return NULL;

  /* Allocate a spectrum.  If cannot, release storage and return. NULL */

  *spno = Xamine_AllocateSpectrum();
  if(*spno <= 0) {
    Xamine_FreeMemory(storage);
    return NULL;
  }

  /*  Next Describe the spectrum in the shared memory region.  */

  Xamine_DescribeSpectrum(*spno, xdim, ydim, title, storage,
			  byte ? twodbyte : twodword);

  return storage;
}

#ifdef unix
long f77xamine_allocate2d_(int *spno, int *xdim, int *ydim, char *title,
			  int *byte, int tlen)
#endif
#ifdef VMS
long f77xamine_allocate2d(int *spno, int *xdim, int *ydim,
			 struct dsc$descriptor *title, int *byte)
#endif
{
  spec_title t;
  int        tl;
  char       *txt;
  char       *spec;
  long        offset;

  /* Create the C string title in t: */

  memset(t, 0, sizeof(spec_title));
#ifdef unix
  txt = title;
  tl  = tlen;
#endif
#ifdef VMS
  txt = title->dsc$a_pointer;
  tl  = title->dsc$w_length;
#endif
  if(tl > (sizeof(spec_title) -1)) tl = sizeof(spec_title)-1;
  strncpy(t, txt, tl);

  /* Call the C allocator:  */

  spec = (char *) Xamine_Allocate2d(spno, *xdim, *ydim, t, *byte);

  /* Compute the offset from the pointer: */
  
  offset = (long)spec - (long)Xamine_memory->dsp_spectra.XAMINE_b;
  if(!*byte) offset = offset / sizeof(short); /* Word offset. */

  return offset;
}
