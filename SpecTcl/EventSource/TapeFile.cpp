//  CTapeFile.cpp
// Encapsulates an ANSI labelled tape
// Ansi labelled tapes have multiple files
// on them.  Each file is named and
// described by a set of header and
// trailer labels.    This class is built on
// top of the mtaccess library already existing
// in the old DAQ library.
//
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file////////////////////////////////////////////

//
// Header Files:
//


#include "TapeFile.h"
#include <errno.h>
#include "ErrnoException.h"
#include "TapeException.h"
#include <mtaccess.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>

extern "C" {
#include <mtaccess.h>
}
                            
static const char* Copyright = 
"CTapeFile.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CTapeFile

//////////////////////////////////////////////////////////////////////////
// 
//  Function:
//     CTapeFile(const std::string& am_sDevice)
//  Operation Type:
//     Constructor
//
CTapeFile::CTapeFile(const std::string& am_sDevice) : 
  CFile()			// Start with Default constructor.
{
  // The constructor opens the CTapeFile, mounts it as a volume and
  // saves the m_pVcb.  Note that we don't mark the file as open.
  // That requires the 'open' call.  Note as well, that we assume the
  // tape is already a valid volume, else we throw an exception.
  // 
  //   Tape initialization is done via the static function:
  //     InitializeVolume.
  //
  Int_t fd = open(am_sDevice.c_str(),
		  O_RDWR);
  if(fd < 0) {			// device Open failed...
    if(errno == EACCES) {	// May allow Readonly acces... 
      fd = open(am_sDevice.c_str(),
		O_RDONLY);
      if(fd < 0) {
	CErrnoException openfailed("CTapeFile::CTapeFile() - device open");
	throw openfailed;
      }
    }
  }
  setFd((UInt_t)fd);		// Save the file descriptor in base class obj.

  //  By now the device is open, and we have to mount it.  This is done
  //  within a try block.  If any exception is caught, we:
  //   1. Close the tape device.
  //   2. rethrow the exception.
  //
  volume* vcb;
  try {
    volume_item ItemList = { VOL_ENDLIST, (char*)0 }; // Boring item list.
    vcb  = volmount(fd, &ItemList);
    if(!vcb){			// Report mount failure as exception.
      CErrnoException except("CTapeFile::CTapeFile() - mounting volume");
      throw except;
    }
  }
  catch(...) {
    close(fd);
    throw;
  }
  CRefcountedPtr<volume> v(vcb);
  m_pVcb = v;			// Save tape volume control block in
				// reference counted item.
  m_sDevice = am_sDevice;	// Now it's safe to save the string..
				// Doing it earlier leaves us vulnerable
				// to memory leaks if one of the
				// exceptions was thrown, since we're not
				// fully constructed.
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//     ~CTapeFile()
//  Operation Type:
//     Destructor.
//
CTapeFile::~CTapeFile()
{
  // Destruction only takes effect if we're the last reference of the
  // volume control block, otherwise we could be sharing state with
  // another guy.
  //
  if(m_pVcb.refcount() == 1) {
    if(getState() == kfsOpen) {
      Close();			// Close any open file.
    }
    voldmount(m_pVcb.operator->());
  }
  setState(kfsClosed);		// Don't want base class to close file.
  close(getFd());		// Close the tape device too.

}
//////////////////////////////////////////////////////////////////////////
//  Function:
//    CTapeFile(const CTapeFile& aCTapeFile)
//  Operation Type:
//    Copy Constructor.
//
CTapeFile::CTapeFile(const CTapeFile& acTapeFile)
{
  DoAssign(acTapeFile);
}
//////////////////////////////////////////////////////////////////////////
// Function:
//    CTapeFile& operator=(const CTapeFile& aCTapeFile)
// Operation Type:
//    Assignment operator.
//
CTapeFile&
CTapeFile::operator=(const CTapeFile& aCTapeFile)
{
  if(this != &aCTapeFile) {
    CFile::operator=(aCTapeFile);
    DoAssign(aCTapeFile);
  }
  return *this;
}
//////////////////////////////////////////////////////////////////////////
//  Function:
//     Int_t operator==(const CTapeFile& aCTapeFile)
//  Operation Type:
//     Equality comparison.
//
Int_t 
CTapeFile::operator==(const CTapeFile& aCTapeFile)
{
  // == if device name is the same, and the volume control block
  // pointer are also the same, and  base class gives equality:
  //
  return ((m_sDevice == aCTapeFile.m_sDevice) &&
	  m_pVcb == aCTapeFile.m_pVcb &&
	  CFile::operator==(aCTapeFile)
	  );
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int Read ( Address_t pBuffer, UInt_t nBytes )
//  Operation Type:
//     I/O transfer.
//
Int_t 
CTapeFile::Read(Address_t pBuffer, UInt_t nBytes)
{
// Reads a block of data from a tape file.
// Tape data is stored in fixed length blocks.
//  The read size should match this blocksize
//  else an exception will be thrown.
//
// Formal Parameters:
//     Address_t pBuffer:
//        Pointer to the data buffer into which data is read.
//     UInt_t       nBytes:
//        Number of bytes to read.
// Returns:
//    0   - End of file read.
//    n   - Number of bytes read (should be same as nBytes).
//
// Exceptions:
//     CErrnoException - if the mtaccess lib returns a MTCHECKERRNO error.
//     CTapeException - if the mtaccess lib returns another error (except e.g. 
//     MTEOF.
//

  UInt_t nRead;			// Mtaccess needs this..
  AssertOpen();			// Throw if file not open.

  Int_t status = volread(m_pVcb.operator->(), pBuffer, (Int_t)nBytes,
			 &nRead);

  // What we do/return, depends on the status:

  switch(status) {
  case MTSUCCESS:		// Normal read...
    return nRead;
  case MTEOF:			// End file encountered.
    return 0;
    //  case MTCHECKERRNO:		// throw CErrnoException..
    //  CErrnoException except("CTapeFile::Read() - read failed.");
    // throw except;
  default:			// Magtape exception...
    CTapeException texcept(status, "CTapeFile::Read() - read failed.");
    throw texcept;
  }
  assert(0);			// should not get here.
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t Write ( const Address_t pBuffer, UInt_t  nBytes )
//  Operation Type:
//     I/O transfer
//
Int_t 
CTapeFile::Write(const Address_t pBuffer, UInt_t  nBytes)
{
// Writes a block of data to a tape file.
// 
// Formal Parameters:
//      const Address_t pBuffer:
//           Pointer to the data to write.
//      UInt_t nBytes:
//           Number of data bytes to write.
// 
// Exceptions:  

  AssertOpen();
  
  Int_t status = volwrite(m_pVcb.operator->(), pBuffer, (Int_t)nBytes);
  switch(status) {
  case MTSUCCESS:		// Normal Completion.
    return nBytes;
  case MTEOMED:
    return 0;
  case MTLEOT:
    return 0;
    //  case MTCHECKERRNO:
    //  CErrnoException except("CTapeFile::Write() - Write failed");
    //  throw except;
  default:
    CTapeException texcept(status, "CTapeFile::Write() - Write failed");
    throw texcept;
  }
  assert(0);			// Should never get here.
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Open ( const std::string&  rsFilename, UInt_t nAccess )
//  Operation Type:
//     I/O connection
//
void 
CTapeFile::Open(const std::string&  
		rsFilename, UInt_t nAccess) 
{
//  Opens a file on the tape.
//  Semantics are 'mostly' the same as the
//  CFile::Open() function except that
//  if kacCreate is specified in the access
//  bits, the file is unconditionally created.
//
// Formal Parameters:
//     const std::string& rsFilename:
//        Name of the tape file to open.
//        If the filename string is completely blank/empty, 
//        the next file on the tape is opened (only for kacRead access).
//  
//     UInt_t nAccess:
//         Specifies the access requested.
//
//
  if(getState() == kfsOpen) {	// Close the file first if open...
    Close();
  }
  //    There are some requirements on the access bits:
  //    kacRead must be by itself.
  //    kacWrite must be with kacCreate, and not with kacRead.
  //    kacAppend is not allowed.
  //    Failure to abide by these rules gives an MTPROTECTED exception.
  //
  switch(nAccess) {
  case kacRead:			// Open old file on tape.
    TapeOpen(rsFilename);
    break;
  case (kacWrite | kacCreate):	// Create new file on tape.
    TapeCreate(rsFilename);
    break;
  default:			// bad file access combination:
    CTapeException e(MTPROTECTED,
		     "CTapeFile::Open - Access attribute combination bad");
    throw e;
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Close (  )
//  Operation Type:
//     I/O Disconnect
//
void 
CTapeFile::Close() 
{
// Closes the currently open tape file.
//  Note that the tape device itself
//  remains open and the client can
//  Open or create a new file on the tape.
//

  AssertOpen();			// A tape file must be open.
  Int_t status = volclose(m_pVcb.operator->());
  setState(kfsClosed);		// Set file state to closed...even if errors.

  switch (status) {
  case MTSUCCESS:
    return;
    //  case MTCHECKERRNO:
    // CErrnoException e("CTapeFile::Close() volclose failed.");
    // throw e;
  default:
    CTapeException t(status, 
		     "CTapeFile::Close() volclose failed.");
    throw t;
  }
  assert(0);			// should not get here.
  

}
////////////////////////////////////////////////////////////////////
// 
//  Function:
//    void DoAssign(const CTapeFile& rhs)
//  Operation Type:
//    protected assignment utility.
//
void
CTapeFile::DoAssign(const CTapeFile& rhs)
{

  // Action is: do the base class assign,
  // Copy rhs to us. 
  // The fd must be duped over regardless of the file state since it
  // is held open from construction to teardown due to the
  // 

  // Since this can throw, we do this first so that other dynamic
  // allocations associated with e.g. string copying won't leak
  // on the throw.
  //

  setFd(dup(rhs.getFd()));
  if(getFd() < 0) {		// dup failed... throw...
    CErrnoException e("CTapeFile::DoAssign - fd duplication failed");
    throw e;
  }


  CTapeFile::DoAssign(rhs);
  m_sDevice    = rhs.m_sDevice;
  m_nBlocksize = rhs.m_nBlocksize;
  m_pVcb       = rhs.m_pVcb;	// It's a refcounted ptr so this works.
  m_sFilename  = rhs.m_sFilename;

  
}
//////////////////////////////////////////////////////////////////////
//
//  Function:
//     void Init(const std::string& device, const std::string& label)
//  Operation Type:
//     Static tape initialization.
//
void 
CTapeFile::Initialize(const std::string& device, const std::string& label)
{
  // Initializes a tape volume.  Note that this is a static member.
  // to write a blank tape volume, the user should do something like:
  //
  //   CTapeFile::Initialize("/dev/rmt0h", "MYTAPE");
  //   CTapeFile  tape("/dev/rmt0h");
  //   tape.open("NEWFILE", kacCREATE | kacWRITE);
  //
  // etc.
  //   otherwise, files will be appended to files already on the volume
  //
  // Formal Parameters:
  //    const std::string& device:
  //        Name of the tape device (e.g. "/dev/rmt0h").
  //    const std::string& label:
  //        Label for tape.. .must conform to ANSI restrictions
  //        on the label character set.
  // Throws: 
  //   CErrnoException - if openfailed or volinit returned
  //                     MTCHECKERRNO
  //   CTapeException  - if volinit failed, but not MTCHECKERRNO
  //
  Int_t fd = open(device.c_str(), O_RDWR);
  if(fd < 0) {
    CErrnoException e("CTapeFile::Initialize - open failed");
    throw e;
  }

  Int_t status = volinit(fd, (char*)(label.c_str()));
  close(fd);
  switch (status){
  case MTSUCCESS:		// Success.
    return;
    //  case MTCHECKERRNO:		// Error is in errno:
    // CErrnoException e("CTapeFile::Initialize - volinit failed");
    // throw e;
  default:			// MT status of some sort.
    CTapeException t(status, "CTapeFile;;Initialize - volinit failed");
    throw t;

  }
  assert(0);			// should not get here.
		   
}
//////////////////////////////////////////////////////////////////////
//
// Function:
//   void TapeCreate(const std::string& name)
// Operation type:
//   Protected utility
//
void
CTapeFile::TapeCreate(const std::string& name)
{
  //  Creates a new file on the open volume.
  //  This is an internal function.
  //  A file is created on the output tape:
  // Formal Parameters:
  //    const std::string& name:
  //         Name of the file to create.. must conform to ANSI
  //         labelled tape restrictions on characters and length.
  // Implicit inputs:
  //    m_nBlocksize:
  //       Number of bytes per file block.
  //
  // Throws:
  //  CErrnoException - if volcreate() fails with MTCHECKERRNO
  //  CTapeException  - if volcreate()  failed otherwise.
  //

  file_item items[] = {
    { FILE_NAMEREQ,   (char*)(name.c_str()) },
    { FILE_REQRECLEN, (char*)m_nBlocksize },
    { FILE_ENDLIST,   (char*)0 }
  };
  Int_t status = volcreate(m_pVcb.operator->(), items);

  switch(status) {
  case MTSUCCESS:
    m_sFilename = name;		// Update the filename.
    setState(kfsOpen);
    return;
    //  case MTCHECKERRNO:
    // CErrnoException e("CTapeFile::TapeCreate - volcreate failed");
    // throw e;
  default:
    CTapeException t(status, "CTapeFile::TapeCreate - volcreate failed");
    throw t;
  }
  assert(0);
}
/////////////////////////////////////////////////////////////////////
//
// Function:
//    void TapeOpen(const std::string& name)
// Operation Type:
//    protected utility
void
CTapeFile::TapeOpen(const std::string& name)
{
  // Opens an existing file on the tape.
  // This is done by doing a volopen().  Note that volopen()
  // only searches forward on the tape.  Therefore, if the
  // first volopen() indicates MTNOTFOUND, a second volopen()
  // is done to search the entire tape.  When processing files
  // on tape, therefore it is best to process in the forward direction
  // since this will optimize the search.
  // If the open succeeds, then the filename is put in m_sFilename.
  //
  // Formal Parameters:
  //    const std::string& name:
  //       Requested filename.  This affects the search
  //       as follows: If the name is nonblank, then
  //       that file searched for.  If the name is blank,
  //       then the next file on tape is open.  If the
  //       name is blank and we're at the end of  tape,
  //       CTapeError(MTNOTFOUND,...) is thrown as an exception.
  //  
  // Throws:
  //  CErrnoException
  //  CTapeException
  //
  char* pName = 0;
  UInt_t nLength = strlen(name.c_str());
  if(nLength) {
    pName = new Char_t[nLength+1];
    if(!pName) {
    }
    strcpy(pName, name.c_str());
    UInt_t nActual = strcspn(pName, " \t");
    if(nActual == 0) {
      delete []pName;
      pName = 0;
    }
  }
  // If the name is nonblank it's in pName, otherwise, pName is 0.
  //
 
  Char_t  szName[18];
  file_item items[] = {
    { FILE_BLOCK, (char*)&m_nBlocksize }, // Get the blocksize.            0
    { FILE_NAME , szName },	   // Get the found filename.       1
    { FILE_ENDLIST, (char*)0 },           // Space for requested filename. 2
    { FILE_ENDLIST, (char*)0 }		// Very latest end of list.         3
  };
  Int_t status;

  if(pName) {			// Open specific file:
    items[2].code = FILE_NAMEREQ; 
    items[2].data = pName;
    status = volopen(m_pVcb.operator->(), items);
    if(status == MTNOTFOUND)   // For specific file we can try twice:
      status = volopen(m_pVcb.operator->(), items);
  } 
  else {			// Open next file on tape.
    status = volopen(m_pVcb.operator->(), items);
  }
  //  When control reaches here, status contains the final status of the
  //  open request this can be:
  //     MTSUCCESS - m_nBlocksize has the blocksize, szName has the
  //                 name of the file we opened.
  //     MTCHECKERRNO - some error reported in errno will be thrown.
  //     others    - some error (includes MTNOTFOUND) will be thrown.
  //
  switch(status) {
  case MTSUCCESS:
    m_sFilename = szName;
    setState(kfsOpen);
    return;
    //  case MTCHECKERRNO:
    // CErrnoException e("CTapeFile::TapeOpen - volopen() failed");
    // throw e;
  default:
    CTapeException t(status, "CTapeFile::TapeOpen - volopen() failed");
    throw t;
  }
  assert(0);
}










