#include "common.h"

//source: http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
timespec diff(timespec start, timespec end)
{
        timespec temp;
        if ((end.tv_nsec-start.tv_nsec)<0) {
                temp.tv_sec = end.tv_sec-start.tv_sec-1;
                temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
        } else {
                temp.tv_sec = end.tv_sec-start.tv_sec;
                temp.tv_nsec = end.tv_nsec-start.tv_nsec;
        }
        return temp;
}




void gettime(timespec& t)
{
  clock_gettime(CLOCK_REALTIME,&t);
}

//QEM
//Add quadric matrix b to quadric matrix a
void sumQuadrics(double a[4][4], double b[4][4])
{
    for(int i = 0 ; i < 4 ; ++i)
    {
        for(int j = 0 ; j < 4 ; ++j)
        {
            a[i][j]+=b[i][j];
        }
    }
}

//Copy b into a
void copyQuadrics(double a[4][4], double b[4][4])
{
  for(int i = 0 ; i < 4 ; ++ i)
  {
    for (int j = 0 ; j < 4 ; ++j)
    {
      a[i][j] = b[i][j];
    }
  }
}
