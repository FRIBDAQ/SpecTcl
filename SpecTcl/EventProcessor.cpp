// Class: CEventProcessor

////////////////////////// FILE_NAME.cpp /////////////////////////////////////////////////////
#include "EventProcessor.h"    				
#include <Analyzer.h>
#include <BufferDecoder.h>
#include <Event.h>

CEventProcessor::CEventProcessor ()
 
{   
} 

	// Destructor:

CEventProcessor::~CEventProcessor ( )  //Destructor
{
}

        //Copy constructor alternative to compiler provided default copy constructor      
CEventProcessor::CEventProcessor (const CEventProcessor& aCEventProcessor ) 

{
} 
        //Operator= Assignment Operator alternative to compiler provided operator=  
CEventProcessor& CEventProcessor::operator= (const CEventProcessor& aCEventProcessor)
{ 

  return *this;
}

      //Operator== Equality Operator 
int CEventProcessor::operator== (const CEventProcessor& aCEventProcessor) const
{ 
  return kfFALSE;
}

// Functions for class CEventProcessor

//  Function: 	
//    Bool_t OnAttach(CAnalyzer& rAnalyzer) 
//  Operation Type:
//     Default method
/*  
Purpose: 	

Called once when the event processor is
registered on the analyzer.  The default action
is null.  If this member returns kfFALSE,
the registration is aborted with an error message.

*/
Bool_t 
CEventProcessor::OnAttach(CAnalyzer& rAnalyzer)  
{ 
  return kfTRUE;
}  

//  Function: 	
//    Bool_t OnBegin(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder) 
//  Operation Type:
//     Default Behavior
/*  
Purpose: 	

Called when a begin run buffer/event is received.
If kfFALSE is returned, the pipeline is silently aborted.

*/
Bool_t 
CEventProcessor::OnBegin(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)  
{ 
  return kfTRUE;
}  

//  Function: 	
//    Bool_t OnEnd(CAnalyzer& rAnalyzer, CBufferDecoder& rBuffer) 
//  Operation Type:
//     Default behavior
/*  
Purpose: 	

Called when an end of run event/buffer is received.
If kfFALSE is returned, the processing pipeline is 
silently aborted.  The default implementation is a
no-op returning kfTRUE

*/
Bool_t 
CEventProcessor::OnEnd(CAnalyzer& rAnalyzer, CBufferDecoder& rBuffer)  
{ 
  return kfTRUE;
}  

//  Function: 	
//    Bool_t OnPause(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder) 
//  Operation Type:
//     Default Behavior
/*  
Purpose: 	

Called when a pause run event/buffer is received.
Returning kfFALSE will silently abort the remainder of the
event pipeline.  The default is an no-op which returns kfTRUE

*/
Bool_t 
CEventProcessor::OnPause(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)  
{ 
  return kfTRUE;
}  

//  Function: 	
//    Bool_t OnResume(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder) 
//  Operation Type:
//     Default Behavior
/*  
Purpose: 	

Called when a resume run event/buffer is received
If this function returns kfFALSE, the remainder of the
event pipeline is silently aborted.  The default 
implementation is a no-op which returns kfTRUE.

*/
Bool_t 
CEventProcessor::OnResume(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)  
{ 
  return kfTRUE;
}  

//  Function: 	
//    Bool_t operator()(const Address_t pEvent, CEvent& rEvent, 
//                     CAnzlyer& rAnalyzer, CBufferDecoder& rDecoder) 
//  Operation Type:
//     Base class behavior
/*  
Purpose: 	

This member is called whenever a physics event is 
received.  At least one of the processors in the processing
pipeline must call the analyzer's SetEventSize() member function.
This member informs the analyzer how many bytes are in the raw event.
Other useful members involved in that are:
IncrementEventSize(int nSize=2)      - Adds to the current event size.
GetEventSize()                                 - Retrieves the current event size.

If the function returns kfFALSE, the entire event processing pipeline is 
aborted and the unpacked event being construction is not sent on
to the event sink.

*/
Bool_t 
CEventProcessor::operator()(const Address_t pEvent, CEvent& rEvent, CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)  
{ 
  return kfTRUE;
}











