#include "dispshare.h"
#include "dispwind.h"
 
extern spec_shared spectra;
int main()
{
  win_db db;			/* Test database. */

 
  if(db.read("testfile.win")){	/* Read it in. */
    fprintf(stderr, "Success\n");
  }
  else {
    fprintf(stderr, "Failure\n");
  }

  if(db.write("testoutput.win")) { /* Write it out */
    fprintf(stderr, "Success\n");
  } else {
    fprintf(stderr, "Failure\n");
  }
  return 0;
}
