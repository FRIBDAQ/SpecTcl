/*
**++
**  FACILITY:
**
**      RIMFIRE    - 8mm tape support via Rimfire SCSI adapter.
**
**  ABSTRACT:
**
**      mtmessage.c - This file is responsible for making sensible error
**		    messages out of error codes that can be returned from
**		    the rimfire/8mm tape subsystem.  There are two types of
**		    error codes to contend with.  The bulk of what we'll see
**		    are MTxxx codes which are in the range 0-MAXMTERRORCODE
**		    The others are the RMF codes which are in VMS status code
**		    form:
**			+-----+--------+------+--------+
**			|CTRL |Facility|Status|Severity|
**			+-----+--------+------+--------+
**		        3    2 2      1 1    0 0      0
**                      1    8 7      6 5    3 2      0
**
**		    The status field is what we care about, once the
**		    top bit (facility specific indicator) is cleared.
**  AUTHORS:
**
**      Ron Fox
**
**
**  CREATION DATE:      31-May-1990
**
**  MODIFICATION HISTORY:
**	Library_Source 1/27/92 @(#)mtmessage.c	2.1
**--
*/
static char *version = "Library_Source 1/27/92 @(#)mtmessage.c	2.1 Ron Fox NSCL";

/*
**
**  INCLUDE FILES
**
*/
#include <stdio.h>
#include <errno.h>
#include "mtshare.h"
#include "rmf_msg.h"
#ifdef VMS
#include "vmsstring"
#endif

typedef struct {
		    unsigned control : 4;
		    unsigned facility : 12;
		    unsigned status   : 13;
		    unsigned severity :  3;
	       } vmsstatus;

static char *mtmsgs[] = { 
			    "MT-S-SUCCESS Successful completion",
			    "MT-F-INSFMEM   Insufficient memory",
			    "MT-F-BADHANDLE Bad Magtape data structure",
			    "MT-F-BADRAM Controller RAM self test failed",
			    "MT-F-BADPROM Controller PROM self test failed",
			    "MT-F-INTERNAL Internally detected error",
			    "MT-E-ALLOCATED Device already alocated",
			    "MT-F-BADUNIT Invalid device unit number",
			    "MT-I-REQSENSE Check sense data for status",
			    "MT-I-NOTALLOC Device is not allocated.",
			    "MT-I-LEOT Logical end of tape encountered",
			    "MT-W-EOM Physical end of tape encountered",
			    "MT-I-OVERRUN Data overrun, buffer too small",
			    "MT-I-EOF End of file encountered in read",
			    "MT-F-INVLBL Invalid label on ANSI tape",
			    "MT-F-NOTANSI Tape in drive is not ANSI",
			    "MT-F-WRONGVOL Tape in drive is not correct volume",
			    "MT-F-BADPROT Invalid protection field.",
			    "MT-F-NOTMOUNTED Device not mounted.",
			    "MT-F-FILEOPEN Device has open files.",
			    "MT-F-PROTECTED Protection violation",
			    "MT-F-FNAMEREQ Filename required for operation",
			    "MT-F-FILECLOSE File is not open",
			    "MT-F-BADLENGTH Invalid buffer size",
			    "MT-I-OFFLINE Tape is offline",
			    "MT-I-AVAILABLE Tape available for use.",
			    "MT-I-LEOV Tape is at logical end of volume.",
			    "MT-F-NOTFOUND File not found on volume",
			    "MT-F-IO Data transfer failure. "
			};

static char *vmsmsgs[] = {
			    "RMF-E-BAD_BOARD_CMD Invalid RIMFIRE board cmd",
			    "RMF-E-BADUNIT Bad unit number",
			    "RMF-E-RESERVED_NONZERO Reserved field not zero",
			    "RMF-I-END_CMDLST Command list stopped",
			    "RMF-E-BAD_CMDLST_LEN Bad command list size",
			    "RMF-W-CMDLST_ACTIVE Command list already active",
			    "RMF-E-BUS_TIMEOUT Software bus timeout",
			    "RMF-E-BUS_ERROR Bus error",
			    "RMF-E-SG_DESC Scatter/gather block read error",
			    "RMF-E-SST SCSI select timeout",
			    "RMF-E-SDT SCSI de-select timeout",
			    "RMF-E-SCSI_PARITY SCSI parity error",
			    "RMF-E-USD Unexpected SCSI disconnect",
			    "RMF-E-SCSI Generic SCSI bus error",
			    "RMF-E-SCSI_STS Device returned a bad status",
			    "RMF-E-USE SCSI phasing error",
			    "RMF-E-BAD_BYTE Bad byte seen by SCSI controller",
			    "RMF-E-SYNCH_XFER Error in synchronous transfer",
			    "RMF-E-SG Error in scatter/gather operation",
			    "RMF-E-SCSI_RESET SCSI reset during operation",
			    "RMF-F-RAM RIMFIRE static ram error",
			    "RMF-F-CHKSUM PROM checksum error",
			    "RMF-E-INV_DIAG Invalid diagnostic",
			    "RMF-F-FIRM Firmware errors",
			    "RMF-W-BSY Device busy, re-try operation",
		    "RMF-E-NO_SENSE SCSI device error indicated, none returned",
			    "RMF-I-FMK File mark encountered",
			    "RMF-I-LEOM Logical end of volume",
			    "RMF-W-ILI Illegal length indicator.",
			    "RMF-E-NOTREADY Device is not ready re-try later",
			    "RMF-E-MEDIUM Media error",
		    "RMF-F-HARD_ERROR Unrecoverable hard read/write error",
			    "RMF-E-ELL_REQUEST Illegal request",
			    "RMF-I-UNIT_ATT Unit attention",
			    "RMF-W-WRITE_PROT Volume is write protected",
			    "RMF-I-BLANK_CHK Read into blank tape",
			    "RMF-E-ABORT Command aborted",
			    "RMF-W-PEOM Physical end of tape encountered",
	    "RMF-E-TMD_XFER Tape mark detect error or transfer abort error",
			    "RMF-I-ADD_DATA Error is in additional sense data",
			    "RMF-F-PF Power fail",
			    "RMF-E-BPE SCSI bus parity error",
			    "RMF-E-FPE Formatted buffer parity error",
			    "RMF-E-ME  Media error",
			    "RMF-E-ECO Error counter overflow",
			    "RMF-E-TME Tape motion error",
			    "RMF-E-TNP Tape not present",
			    "RMF-E-BOT Beginning of tape",
			    "RMF-E-XFR Transfer abort error.",
			    "RMF-E-TMD Tape mark detect error",
			    "RMF-E-FMKE Error writing filemark",
			    "RMF-E-URE Data flow underrun",
		    "RMF-E-WE1 Maximum number of write retries attempted",
			    "RMF-F-SSE Servo system error",
			    "RMF-E-FE Formatter error",
			    "RMF-E-WSEB Write splice error (blank tape)",
			    "RMF-E-WSEO write splice error (overshoot)",
			    "RMF-I-NO_BITS No error bits set",
			    "RMF-E-TYP0ATTMO Timeout on type 0 cmd attention",
		    "RMF-E-CIPRDYTMO Ciprico ready timeout for type 1 cmd",
			    "RMF-F-INRES_TMO CIPRICO reset timeout",
			    "RMF-F-INGOP_TMO Timeout setting general options",
			    "RMF-F-INIDF_TMO Timeout in identify ",
			    "RMF-F-INSCL_TMO Timeout on start command list",
			    "RMF-E-INVSENLEN Invalid sense length",
			    "RMF-S-SUCCESS Successful completion",
			    "RMF-PBLIST_FULL Parameter block list full",
			    "RMF-SBLIST_FULL Status block list full",
			    "RMF-CMD_NOT_COM Command not completed",
			    "RMF_NOMATCH_ID No matching identifier",
		    "RMF_NONINTGRL Buffer non integral number of blocks",
			    "RMF-F-OUT_OF_RANGE_LIST Invalid command list",
			 };

static char *catchall = 
                 "MT-I-NOMSG No message corresponds to status code: 0x%x";
static char nomsg[80];


#ifdef VMS
static    vmsstring vmsmsg;
static    char      vmsmsgtxt[80];
#endif

#ifdef sparc
/*
** Functional Description:
**  strerror - return a pointer to a string describing the error code
** Formal Parameters:
**   int stat   - Status we want text for.
** Returns:
**  Pointer to an entry in the system error list.
*/

static char *noerror="Invalid status value passed to strerror\n";
extern int sys_nerr;
extern char *sys_errlist[];

char *strerror(int status)
{
  if( (status < 0) || (status >= sys_nerr))
    return noerror;
  else
    return sys_errlist[status];
}
#endif
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      mtgetmsg    - This function returns a pointer to a message string
**		      which describes the input status value.
**
**  FORMAL PARAMETERS:
**
**      int status  - Status code to lookup.
**
**  FUNCTION VALUE:
**
**      Pointer to string with descriptive message.  If lookup failed,
**	pointer will be to the catchall string.
**
**
**--
*/
char *mtgetmsg (int status)
{
    char *strerror(int);
    union {
		int input;
		vmsstatus output;
	  } statuscvt;
    char *msg;


    /* First try the errno messages */

    if ( status <= 100)
    {
	msg =  strerror (status);
	if(msg == NULL)
	{
	    sprintf (nomsg, catchall, status);
	    return nomsg;
	}
	else
	    return msg;
    }

#ifdef VMS
    if (status == EVMSERR)
    {
	vmsmsg.dsc$a_pointer = vmsmsgtxt;
	vmsmsg.dsc$b_class   = DSC$K_CLASS_S;
	vmsmsg.dsc$b_dtype   = DSC$K_DTYPE_T;
	vmsmsg.dsc$w_length  = 80;
	lib$sys_getmsg (
	    &vaxc$errno, 
	    0, 
	    &vmsmsg, 
	    0, 
	    0);
	vmsmsgtxt[vmsmsg.dsc$w_length] = '\0';
	return vmsmsgtxt;
    }
#endif

    /* then we try to match MT status codes. */
     
    if ((status >= 100) && (status <= MAXMTERRORCODE))
    {
	return mtmsgs[status-100];
    }
    /* Then we break the status code into vmsstatus fields and try */
    /* for a match there. */
     
    if ((status >= 0x845800a) && (status <= 0x8458242))
    {
	statuscvt.input = status;
	return vmsmsgs[((statuscvt.output.status) & 0xFFF)-1];
    }
    sprintf (nomsg, catchall, status);
    return nomsg;
}
