/*
** Facility:
**     Xamine -- NSCL display program.
** Abstract:
**    buttonreq.h:
**      This file defines functions that process button modification requests.
**      these include the following message codes:
**        Button_CreateBox    - Create a button box.
**        Button_DefineButton - Define a new button.
**        Button_EnableButton - Enable a defined button.
**        Button_DisableButton- Disable a defined button.
**        Button_ModifyButton - Modify the properties of a button.
**        Button_InquireButton- Inquire about the properties of a button.
**        Button_DeleteButton - Kill off a button.
**        Button_DeleteBox    - Destroy the button box.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)buttonreq.h	8.1 6/23/95 
*/
#ifndef _BUTTONREQ_H
#define _BUTTONREQ_H
#include "clientops.h"

void Xamine_ProcessButtonRequest(operation op);

#endif
