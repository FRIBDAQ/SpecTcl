
#include "FragmentIndex.h"
#include <fragment.h> 
#include <DataFormat.h>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <iterator>
#include <algorithm>


FragmentIndex::FragmentIndex()
  : m_frags()
{
}

/**! Given the body pointer, index all of the fragments 
* @param a pointer to the first word in the body (this is b/4 the first fragment)
*
*/
FragmentIndex::FragmentIndex(uint16_t* data)
  : m_frags()
{

  uint32_t max_bytes=0, temp;

  max_bytes = *data++;
  temp = *data++;
  max_bytes |= (temp<<16);

  indexFragments(data, max_bytes-sizeof(uint32_t)); //note that this indexFragments is defined in FragmentsIndex.h --JP
}


/**! Indexes all of the fragments
  @param data a pointer to the first fragment
  @param nbytes the number of bytes from start of first fragment to end of the body
*/
void FragmentIndex::indexFragments(uint16_t* begin, uint16_t* end)
{
  if (begin==0) {
    throw std::runtime_error("Null pointer passed as argument, cannot proceed");
  } 

  // clear what we have already found so we have a fresh search
  m_frags.clear();

  // if we have no data to process, then do nothing!
  if (begin == end ) return;

  uint16_t* data = begin;

  size_t dist = 0;
  do {

	  dist = computeWordsToNextFragment(data);

	  if ((data + dist) > end) {
		  throw std::runtime_error("FragmentIndex::indexFragments() insufficient data in buffer for next fragment!");
	  }

	  EVB::FlatFragment* frag = reinterpret_cast<EVB::FlatFragment*>(data);

	  // Store the body of the fragment in a condensed version
	  FragmentInfo info; 
	  info.s_timestamp = frag->s_header.s_timestamp;
	  info.s_sourceId  = frag->s_header.s_sourceId;
	  info.s_size      = frag->s_header.s_size;
	  info.s_barrier   = frag->s_header.s_barrier;
	  info.s_itemhdr   = reinterpret_cast<uint16_t*>(frag->s_body);
	  uint16_t sizeBodyHeader = *(info.s_itemhdr+4);
	  if (sizeBodyHeader==0) {
		  info.s_itembody = info.s_itemhdr
			  + (sizeof(RingItemHeader)+6)/sizeof(uint16_t);;
	  } else {
		  info.s_itembody = info.s_itemhdr
			  + (sizeof(RingItemHeader)+sizeBodyHeader) /sizeof(uint16_t);
	  }

	  m_frags.push_back(info); //Add current fragment to m_frags list --JP

	  data += dist;

  } while (data < end); 

}

size_t FragmentIndex::computeWordsToNextFragment(uint16_t* data) //This gives the total size of the fragment --JP
{
  // For reference, a fragment looks like this:
  // struct EVB::FlatFragment {
  //    uint64_t tstamp;
  //    uint32_t sourceId;
  //    uint32_t payload_size;
  //    uint32_t barrier_type;
  //    char* body;
  //  }
  //
  
  EVB::FlatFragment* frag = reinterpret_cast<EVB::FlatFragment*>(data);
  uint32_t payload_size = frag->s_header.s_size; // in bytes
  uint32_t fraghdr_size = sizeof(EVB::FragmentHeader); // in bytes

  return (payload_size + fraghdr_size)/sizeof(uint16_t);

}

