/*
** Facility:
**    Xm object oriented support.
** Abstract:
**    XMWlist.h  - This file contains class definitions which allow the client
**                 to manipulate lists of widgets.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
**
**    @(#)XMWlist.h	8.1 6/23/95 
*/
#ifndef _XMWLIST_H
#define _XMWLIST_H

#ifdef unix
#pragma interface
#endif

#include <stdio.h>
#include <errno.h>
#include "XMWidget.h"
#include "XMPushbutton.h"

extern "C" {
  void exit(int code);
}

/*
** Below is a generic list class which is used to construct the widget list
** class.
*/
#define LIST_DEFAULT_SIZE   100
template<class T> class Generic_List {
 protected:
  int index;
  int num_entries;
  int max_entries;
  T   **entries;
 public:
  /*
  ** Constructors and destructors:
  */

  Generic_List(int size = LIST_DEFAULT_SIZE) {
    entries = new T*[size];
    if(entries == NULL) {
      perror("Unable to allocate list table in Generic_list");
      exit(errno);
    }
    num_entries = 0;
    index       = 0;
    max_entries = size;
  }
  ~Generic_List() {
    delete entries;
  }

  /* Manipulate the list: */

  int Add(T *item) {		/* Add item to list. */
    int retcode;
    if(num_entries < max_entries) {
      entries[num_entries] = item;
      num_entries++;
      retcode = 0;
    }
    else retcode = -1;
    return retcode;
  }
  void InitIteration(int loc = 0) {	/* Initialize a list iteration */
    index = loc;
  }
  int Exists() {			/* True if element exists. */
    return (index < num_entries) ? -1 : 0;
  }
  T   *Next() {			/* Returns element and increments index. */
    T *item;
    if(index < num_entries) {
      item = entries[index];
      index++;
    }
    else item = NULL;
    return item;
  }
  int Index() {			/* Get current iteration index */
    return index;
  }
  void Remove(int idx) {		/* Remove item at this index.  */
    for(int i = idx; i < num_entries; i++) {
      entries[idx] = entries[idx+1];
    }
  }
};

class XMWidgetList : public Generic_List<XMWidget>
{
 public:
  XMWidgetList(int num = LIST_DEFAULT_SIZE) : Generic_List<XMWidget>(num) {}
  void SetAttribute(String attribute, XtArgVal value);
  void SetAttribute(String attribute, void *value);
};

class XMButtonList : public XMWidgetList 
{
 public:
  void Enable() {
    SetAttribute(XmNsensitive, (XtArgVal)True);
  }
  void Disable() {
    SetAttribute(XmNsensitive, (XtArgVal)False);
  }
};


#endif
