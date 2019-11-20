#define  NOT_READY  -1
#define  FILLED     0
#define  TAKEN      1

const int data_size = 10;

struct Memory {
  int  status;
  int  data[data_size];
};
