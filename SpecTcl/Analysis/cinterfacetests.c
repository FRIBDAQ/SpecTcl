
#include <string.h>
#include <check.h>
#include "BufferAnalysis.h"

#include <buffer.h>
#include <buftypes.h>




/* The tests:  */


START_TEST(initial_case)	/* This test must be first!!! */
{
  fail_unless(!ScalersSeen(), NULL);
  fail_unless(ScalerCount() == -1, NULL);
  fail_unless(LastIncrement(0) == -1, NULL);
  fail_unless(Total(0) == -1, NULL);
  fail_unless(LastIntervalStart() == -1, NULL);
  fail_unless(LastIntervalEnd()  == -1, NULL);
  fail_unless(strcmp(Title(), "") == 0, NULL);
  fail_unless(RunNumber() == -1, NULL);
  fail_unless(strcmp(RunStartTime(), "") == 0, NULL);
  fail_unless(strcmp(RunEndTime(), "") == 0, NULL);

}
END_TEST

START_TEST(cbeginprocessed)
{
  struct buffer {
    struct bheader head;
    struct ctlbody body;
  } Buf;

  /* Minimial header: Type and signatures: */

  Buf.head.type = BEGRUNBF;
  Buf.head.run        = 123;
  Buf.head.ssignature = 0x0102;
  Buf.head.lsignature = 0x01020304;

  strcpy(Buf.body.title, "This is a test");
  Buf.body.sortim = 0;
  Buf.body.tod.month    = 2;
  Buf.body.tod.day      = 15;
  Buf.body.tod.year     = 2005;
  Buf.body.tod.hours    = 14;
  Buf.body.tod.min      = 56;
  Buf.body.tod.sec      = 59;
  
  ProcessNSCLBuffer(&Buf);

  fail_unless(RunNumber() == 123, NULL);
  fail_unless(strcmp(Title(), "This is a test") == 0, NULL);
  fail_unless(strcmp(RunStartTime(), "February 15, 2005, 14:56:59") == 0, NULL);
  fail_unless(strcmp(RunEndTime(),  "") == 0, NULL);



}
END_TEST

START_TEST(cendprocessed)
{
  struct buffer {
    struct bheader head;
    struct ctlbody body;
  } Buf;

  /* Minimial header: Type and signatures: */

  Buf.head.type = ENDRUNBF;
  Buf.head.run        = 123;
  Buf.head.ssignature = 0x0102;
  Buf.head.lsignature = 0x01020304;

  strcpy(Buf.body.title, "This is a test");
  Buf.body.sortim = 0;
  Buf.body.tod.month    = 2;
  Buf.body.tod.day      = 15;
  Buf.body.tod.year     = 2005;
  Buf.body.tod.hours    = 16;
  Buf.body.tod.min      = 56;
  Buf.body.tod.sec      = 59;
  
  ProcessNSCLBuffer(&Buf);

  fail_unless(strcmp(RunEndTime(), "February 15, 2005, 16:56:59") == 0, NULL);
}
END_TEST


START_TEST(scalerprocessed)
{
  int i;
  struct buffer {
    struct bheader head;
    struct sclbody body;
    INT32  scalers [32];	/* More storage for scalers. */
  } Buf;

  /* Minimial header: Type and signatures: */

  Buf.head.type = SCALERBF;
  Buf.head.run        = 123;
  Buf.head.ssignature = 0x0102;
  Buf.head.lsignature = 0x01020304;
  Buf.head.nevt       = 32;

  Buf.body.etime = 100;		/* 100 seconds into the run. */
  Buf.body.btime = 90;		/* 90 seconds into the run. */
  for (i =0; i < 32; i++) {
    Buf.body.scalers[i] = i;	/* Use that scalers is up against body. */
  }
  
  ProcessNSCLBuffer(&Buf);


  fail_unless(ScalersSeen(), NULL);
  fail_unless(ScalerCount() ==32, NULL);
  fail_unless(LastIntervalStart() == 90, NULL);
  fail_unless(LastIntervalEnd() == 100, NULL);

  for(i = 0; i < 32; i++) {
    fail_unless(LastIncrement(i) == i, NULL);
    fail_unless(Total(i) == (float)i, NULL);
  }

}
END_TEST

/* The stuff below is needed for the c callback test. */

static int called;
static struct bheader CallbackBuffer;

static void Callback(unsigned int ntype,
		     const void* pBuffer,
		     void*       pData)
{
  fail_unless(ntype == 5, NULL);
  fail_unless(pBuffer == &CallbackBuffer, NULL);
  fail_unless(pData == &called, NULL );
  called++;
}

START_TEST(ccallbacks)
{
  called = 0;
  CallbackBuffer.type = 5;
  CallbackBuffer.ssignature = 0x0102;
  CallbackBuffer.lsignature = 0x01020304;

  AddBufferCallback(Callback, 5, &called);
  ProcessNSCLBuffer(&CallbackBuffer);

  fail_unless(called == 1, NULL);

  RemoveBufferCallback(Callback, 5, &called);
  fail_unless(called == 1, NULL);


}
END_TEST

/* Test boilerplate: */

Suite* 
test_suite(void)
{
  Suite* s = suite_create("c interface");
  TCase* tc_core = tcase_create("core");
  
  suite_add_tcase(s, tc_core);

  /* Add the tests: */

  tcase_add_test(tc_core, initial_case);
  tcase_add_test(tc_core, cbeginprocessed);
  tcase_add_test(tc_core, cendprocessed);
  tcase_add_test(tc_core, scalerprocessed);
  tcase_add_test(tc_core, ccallbacks);

  return s;

}
int
main(void)
{
  int nFailures;
  Suite *s = test_suite();

  SRunner* sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  nFailures = srunner_ntests_failed(sr);
  srunner_free(sr);
  suite_free(s);

  return (nFailures == 0) ? 0 : -1;

}



void* gpTCLApplication;
