/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   gateio.h:
**      This file contains definitions for clients of the gateio.cc module.
**      That module handles all I/O of gates.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** SCCS:
**   @(#)gateio.h	1.1 1/28/94 
*/
#ifndef _GATEIO_H
#define _GATEIO_H

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

#include "dispgrob.h"
#include "messages.h"
#include "clientops.h"

int   Xamine_ReportGate(grobj_generic *object); /* Send gate to client prc. */
int   Xamine_DeclareClientEvent(msg_XamineEvent *evt);
void  Xamine_SetupRequestNotification(XtAppContext ctx,
				      XtInputCallbackProc proc);
int   Xamine_CheckForCommand(msg_command *cmd); /* Check for client request */
int   Xamine_EnterAck(msg_status status); /* Ack enter cmd. */
int   Xamine_DeleteAck(msg_status status); /* Ack remove command */
int   Xamine_InqAck(msg_status status, int object_count);  /* Ack inquire */
int   Xamine_ListGate(grobj_generic *object);    /* List gate */
int   Xamine_ReadGate(grobj_generic **object);    /* Read an object from user */
int   Xamine_ReadPeak(grobj_generic** object);   /* Read peak position */
int   Xamine_ReadFitline(grobj_generic** object); /* Read a fitline specification */
int   Xamine_ReadDeleteData(int *spec, int *id, /* Read obj to del */
			    grobj_type *type);
int   Xamine_ReadInqData(int *spec);

int   Xamine_ReadButtonGeometry(Cardinal *cols, Cardinal *rows);
int   Xamine_ReadButtonDefinition(msg_DefineButton *bdef);

int   Xamine_SetClientStatus(msg_status status);

int Xamine_ReadEnableLocation(msg_EnableButton *which);
int Xamine_ReadDisableLocation(msg_DisableButton *which);

int Xamine_ReadDeleteLocation(msg_DeleteButton *which);

int Xamine_ReadInquireLocation(msg_InquireButton *which);

int Xamine_SupplyButtonInfo(msg_InquireButtonAck *ack);

int Xamine_ReadBModifyBlock(msg_ModifyButton *mblock);
#endif




