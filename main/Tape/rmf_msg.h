/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


/* DEC/CMS REPLACEMENT HISTORY, Element RMF_MSG.H*/
/* *2    21-FEB-1990 09:04:40 FOX "FFix some compilation errors."*/
/* *1    14-FEB-1990 11:21:29 FOX "VMS style erorr message file for RIMFIRE"*/
/* DEC/CMS REPLACEMENT HISTORY, Element RMF_MSG.H*/
/**++
**  FACILITY:
**
**      RIMFIRE
**
**  ABSTRACT:
**
**      This module defines the RMF error codes.  These status codes can
**	be returned from the Ciprico interface routines.
**
**  AUTHORS:
**
**      FermiLab
**
**
**  CREATION DATE:      14-Feb-1990
**
**  MODIFICATION HISTORY:
**	Include_file 1/27/92 @(#)rmf_msg.h	2.1
**--
*/

#ifndef RMF_MSG_H
#define RMF_MSG_H

#define RMF_S_BAD_BOARD_CMD 0x845800A 		/* invalid RIMFIRE board command */
#define RMF_S_BAD_UNIT 0x8458012 	        /* bad unit number */
#define RMF_S_RESERVED_NONZERO 0x845801A 	/* reserved word non-zero */
#define RMF_S_END_CMDLST 0x8458023 		/* command list stopped */
#define RMF_S_BAD_CMDLST_LEN 0x845802A 		/* bad command list size */
#define RMF_S_CMDLST_ACTIVE 0x8458030 		/* command list already active */
#define RMF_S_BUS_TIMEOUT 0x845803A 		/* software bus timeout */
#define RMF_S_BUS_ERROR 0x8458042 		/* bus error */
#define RMF_S_SG_DESC 0x845804A 		/* scatter/gather descriptor block read error */
#define RMF_S_SST 0x8458052 			/* scsi select timeout */
#define RMF_S_SDT 0x845805A 			/* scsi disconnect timeout */
#define RMF_S_SCSI_PARITY 0x8458062 		/* scsi parity error */
#define RMF_S_USD 0x845806A 			/* unexpected scsi disconnect */
#define RMF_S_SCSI 0x8458072 			/* general scsi bus error */
#define RMF_S_SCSI_STS 0x845807A 		/* SCSI device returned a bad status */
#define RMF_S_USE 0x8458082 			/* unexpected scsi phase error */
#define RMF_S_BAD_BYTE 0x845808A 		/* bad byte seen by scsi controller chip */
#define RMF_S_SYNCH_XFER 0x8458092 		/* error in synchronous transfer negotiation */
#define RMF_S_SG 0x845809A 			/* error in scatter gather operation */
#define RMF_S_SCSI_RESET 0x84580A0 		/* scsi bus reset during operation */
#define RMF_S_RAM 0x84580AC 			/* rimfire static ram error */
#define RMF_S_CHKSUM 0x84580B4 			/* prom checksum error */
#define RMF_S_INV_DIAG 0x84580BA 		/* requested invalid diagnostics */
#define RMF_S_FIRM 0x84580C4 			/* firmware errors */
#define RMF_S_BSY 0x84580C8 		        /* device busy */
#define RMF_S_NO_SENSE 0x84580D2		/* scsi device error indicated, none returned */
#define RMF_S_FMK 0x84580DB 			/* filemark */
#define RMF_S_LEOM 0x84580E3 			/* logical end of medium */
#define RMF_S_ILI 0x84580E8 		        /* illegal length indicator */
#define RMF_S_NOTREADY 0x84580F2 		/* device not ready */
#define RMF_S_MEDIUM 0x84580FA 			/* medium error */
#define RMF_S_HARD_ERROR 0x8458104 		/* device hardware error */
#define RMF_S_ILL_REQUEST 0x845810A 		/* illegal request */
#define RMF_S_UNIT_ATT 0x8458113 	        /* unit attention */
#define RMF_S_WRITE_PROT 0x8458118 		/* volume is write protected */
#define RMF_S_BLANK_CHK 0x8458123 	        /* blank check */
#define RMF_S_ABORT 0x845812A 			/* command aborted */
#define RMF_S_PEOM 0x8458130 			/* physical end of medium (volume overflow) */
#define RMF_S_TMD_XFER 0x845813A		/* tape mark detect error or transfer abort error */
#define RMF_S_ADD_DATA 0x8458143		/* Error can be found in additional sense data */
#define RMF_S_PF 0x845814C 			/* power fail */
#define RMF_S_BPE 0x8458152 			/* scsi bus parity */
#define RMF_S_FPE 0x845815A 			/* formatted buffer parity error */
#define RMF_S_ME 0x8458162 			/* media error */
#define RMF_S_ECO 0x845816A 			/* error counter overflow */
#define RMF_S_TME 0x8458172 			/* tape motion error */
#define RMF_S_TNP 0x845817A 			/* tape not present */
#define RMF_S_BOT 0x8458183 			/* beginning of tape */
#define RMF_S_XFR 0x845818A 			/* tansfer abort error */
#define RMF_S_TMD 0x8458192 			/* tape mark detect error */
#define RMF_S_FMKE 0x845819A 			/* error while writing filemark */
#define RMF_S_URE 0x84581A2 			/* data flow underrun */
#define RMF_S_WE1 0x84581AA 			/* maximum number of write retries attempted */
#define RMF_S_SSE 0x84581B4 			/* servo system error */
#define RMF_S_FE 0x84581BA 			/* formatter error */
#define RMF_S_WSEB 0x84581C2 			/* write splice error (blank tape) */
#define RMF_S_WSEO 0x84581CA 			/* write splice error (overshoot) */
#define RMF_S_NO_BITS 0x84581D3 		/* no error bits set */
#define RMF_S_TYP0ATTMO 0x84581DA 	        /* Timeout on type 0 command attention */ 
#define RMF_S_CIPRDYTMO 0x84581E2 	        /* Timeout on Ciprico ready for a type 1 command */
#define RMF_S_INRES_TMO 0x84581EC 	        /* Timeout on reset in Ciprico initialization */
#define RMF_S_INGOP_TMO 0x84581F4 	        /* Timeout on general options in Ciprico initialization */
#define RMF_S_INIDF_TMO 0x84581FC 	        /* Timeout on identify in Ciprico initialization */
#define RMF_S_INSCL_TMO 0x8458204 	        /* Timeout on start command list in Ciprico initialization */
#define RMF_S_INVSENLEN 0x845820A 	        /* Invalid sense length less than 4 bytes specified */
#define RMF_S_SUCCESS 0x8458211 	        /* success */
#define RMF_S_PBLIST_FULL 0x845821A		/* parameter block list full */
#define RMF_S_SBLIST_FULL 0x8458222		/* status block list full */
#define RMF_S_CMD_NOT_COM 0x8458228 	        /* command not complete */
#define RMF_S_NOMATCH_ID 0x8458233		/* No matching identifier */
#define RMF_S_NONINTGRL 0x845823A		/* Buffer size is non-integral number of blocks */
#define RMF_S_OUT_OF_RANGE_LIST 0x8458242	/* Invalid command list */

#endif