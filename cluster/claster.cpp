// if duration of all the tuples the same constant
// any overdominated removed tuples from the skyline
// never needs to be saved or rechecked

#include "stdafx.h"
#include <time.h>
#include <stdlib.h>
#include <list>

#include <string>

//remove expired and valid along with invalidsTuple.mv==true &&
#define COALESCE(arrTuple, arrCount) { int outCount=0; for(int a = 0; a< arrCount; a++) { TUPLE Tuple = arrTuple[a]; if ( (t+1) < (Tuple.tstamp.time + Tuple.tstamp.duration)) { if(outCount!=a) arrTuple[outCount] = Tuple; ;outCount++; } } arrCount = outCount; }

#define APPENDREJP(sTuple) {  rejUpdated = true;  rejTuple[rejCount++] = sTuple;  if(rejCount>sizeof(rejTuple)/sizeof(rejTuple[0]))  { printf("rej window exceeded!\n"); return false; } sTuple.mv = false; printf("R %1X %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X \n",reqQueued,a,sTuple.tstamp.time,sTuple.tstamp.duration,sTuple.mv, TUPLELIST(sTuple) , (sTuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (sTuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      }
#define APPENDREJ(sTuple)  {  rejUpdated = true;  rejTuple[rejCount++] = sTuple;  if(rejCount>sizeof(rejTuple)/sizeof(rejTuple[0]))  { printf("rej window exceeded!\n"); return false; } sTuple.mv = false; }
#define APPENDRMV(sTuple)  {  rmvUpdated = true;  rmvTuple[rmvCount++] = sTuple;  if(rmvCount>sizeof(rmvTuple)/sizeof(rmvTuple[0]))  { printf("rmv window exceeded!\n"); return false; } sTuple.mv = false; printf("R %1X %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X \n",reqQueued,a,sTuple.tstamp.time,sTuple.tstamp.duration,sTuple.mv, TUPLELIST(sTuple) , (sTuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (sTuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      }
#define APPENDRMVo(sTuple) {  rmvUpdated = true;  rmvTuple[rmvCount++] = sTuple;  if(rmvCount>sizeof(rmvTuple)/sizeof(rmvTuple[0]))  { printf("rmv window exceeded!\n"); return false; } sTuple.mv = false; printf("Ro%1X %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X \n",reqQueued,a,sTuple.tstamp.time,sTuple.tstamp.duration,sTuple.mv, TUPLELIST(sTuple) , (sTuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (sTuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      }


using namespace std;

#define Dimensions 16
#define dimensions 8

#define RANDOM rand()*rand()    //if rmod > rand_max(7fff)
//#define RANDOM rand()         //if rmod < rand_max(7fff)
const int tmod              = 100000;
const int maxt              = 100000;      //max tuples
int       rmod[Dimensions]  = {10000000    , 10000000    , 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000 };     //random mod 100000, 
int       rdiv[Dimensions]  = {100000      , 10000       , 10000 , 10000 , 10000 , 10000 , 10000 , 10000 , 10000 , 10000 , 10000 , 10000 , 10000 , 10000 , 10000 , 10000  };     //random div 1000  ,        
int       roff[Dimensions]  = {1000000000  , 100000000   , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0      };     //random div      1,        
const int expd              = 0;

#define SKYSIZE 100000
#define REJSIZE 1000000
#define EXPSIZE 10000
#define RMVSIZE 10000

#include "sensor.h"

#if (dimensions == 16)
const int DIMEN = 16;
#define TUPLETEMPLATE " %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f "
#define TUPLELIST(t)   t.c[0], t.c[1], t.c[2], t.c[3], t.c[4], t.c[5], t.c[6], t.c[7], t.c[8], t.c[9], t.c[10], t.c[11], t.c[12], t.c[13], t.c[14], t.c[15]
#elif ( dimensions == 8)
const int DIMEN = 8;
#define TUPLETEMPLATE " %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f "
#define TUPLELIST(t)   t.c[0], t.c[1], t.c[2], t.c[3], t.c[4], t.c[5], t.c[6], t.c[7]
#elif ( dimensions == 4)
const int DIMEN = 4;
#define TUPLETEMPLATE " %-12.7f %-12.7f %-12.7f %-12.7f "
#define TUPLELIST(t)   t.c[0], t.c[1], t.c[2], t.c[3]
#elif ( dimensions == 3)
const int DIMEN = 3;
#define TUPLETEMPLATE " %-12.7f %-12.7f %-12.7f "
#define TUPLELIST(t)   t.c[0], t.c[1], t.c[2]
#elif ( dimensions == 2)
const int DIMEN = 2;
#define TUPLETEMPLATE " %-12.7f %-12.7f "
#define TUPLELIST(t)   t.c[0], t.c[1]
#endif


typedef sensor::tuple<DIMEN> TUPLE;
typedef sensor::cbit<DIMEN>  CBIT;


int   skyCount = 0;
TUPLE skyTuple[SKYSIZE];   //
bool  skyUpdated = false;
int   rejCount = 0;
TUPLE rejTuple[REJSIZE];   //skipped not min/max and not between adj min/max
bool  rejUpdated = false;
int   expCount = 0;
TUPLE expTuple[EXPSIZE];   //expired from skyline
bool  expUpdated = false;
int   rmvCount = 0;
TUPLE rmvTuple[RMVSIZE];   //removed from skyline by new entry
bool  rmvUpdated = false;


bool pushTuple(TUPLE& Tuple,TUPLE&minTuple,TUPLE&maxTuple,bool reqQueued,int t)
{
  //check 'global' min/max
  maxTuple.maxs(Tuple); Tuple.maxb = maxTuple.maxb;
  minTuple.mins(Tuple); Tuple.minb = minTuple.minb;
  
  
  int a = 0;  //first part of lower for loop is done in upper for lower for picks up where a was in upper  no rechecking of full list 
  
  if (Tuple.minb.b == 0 ) //tuple not a min
  {
    //check for dominance  note:: a min cannot be dominated expect by a min-er min
    
    //assume tuple is not dominated by any in skyline 
    Tuple.mv = true;
    for (;a<skyCount;a++)
    { 
   
      if(skyTuple[a].mv == false ) continue;   //skipped removals
    //if(skyTuple[a].minb.b != 0 ) continue;   //note
    
      CBIT cb = Tuple.febits(skyTuple[a]);  // >  
      if (cb.b == 0 )  //tuple is not greater then sky in any dimension  this removes equals eg 0,0 removes 0,0  0,5 removes 0,10  
      {
        //tuple goes into skyline and skyTuple[a] comes out
        skyTuple[a].minb.b = sensor::tuple<DIMEN>::CBMASK;
        
        APPENDRMVo(skyTuple[a]);      
        
        Tuple.mv = true;
        
        //keep checking below for more removals -- all this for 1 tuple?
        //yes it saves any rechecking of previous skyline tuples
        a++;
        break;
      }
      cb = Tuple.fcbits(skyTuple[a]);  // >=      
      if (cb.b == sensor::tuple<DIMEN>::CBMASK )
      {
        //Tuple dominated
        Tuple.mv = false;
        break;
      }
    }

    if(Tuple.mv == true)
    {      
      printf("I %1X %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X \n",reqQueued,t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );     
    }
  }
  else
  {
    //one or more compenents are minimum
    Tuple.mv = true;
    printf("H %1X %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X \n",reqQueued,t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK), (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
  }
  
  //check the part of skyline that is not done already for newly dominated
  if(Tuple.mv==true)
  {
    for (;a<skyCount;a++)
    {
      if(skyTuple[a].mv == false ) continue;

      if(skyTuple[a].dominatedby(Tuple,sensor::tuple<DIMEN>::CBMASK) ==  true)
      {
        APPENDRMV(skyTuple[a]);
      }
    }

    //SENDTOSKY
    skyTuple[skyCount++] = Tuple;
    skyUpdated = true;
    if(skyCount>sizeof(skyTuple)/sizeof(skyTuple[0]))
    {
      printf("sky window exceeded!\n");
      return false;
    }
  }
  else
  {
    //not in current skyline but maybe will after an expire
    Tuple.mv = true;
    APPENDREJ(Tuple);
  }
  return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
  srand(147);
  TUPLE minTuple;
  TUPLE maxTuple;
  time_t stime = time(0);

  TUPLE uTuple;
	for(int t=0;t<maxt;t++)
	{
   // bool reqQueued = false;
    sensor::tuple<DIMEN>::TSTAMP ts;
    ts.time = t;
    //ts.duration = 1000; //(rand()%10000)+6000;  //6000-15000

    
//    if(reqUpdated) goto stop;

//retup:
    TUPLE Tuple(rmod,rdiv,roff);
    //for(int a = 0; a< skyCount; a++) if(skyTuple[a].equsz(Tuple)) { fprintf(stderr,"REDO \n"); goto retup; }
    ts.duration  = expd ? expd : (int)(Tuple.mrv()/10000000.0 + 100);
    Tuple.tstamp = ts;

    skyUpdated = false;
    rejUpdated = false;
    rmvUpdated = false;
    expUpdated = false;
  
    fprintf(stderr,"%10d %10d %10d %10d %10d\r",t,skyCount,rmvCount,rejCount,time(0)-stime);
    if(t==0) 
    { 
      Tuple.mv = true;
      minTuple = maxTuple = Tuple;
      
      skyTuple[skyCount++] = Tuple; 
      skyUpdated = true;
      printf("H - %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X \n",t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK), (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
      continue;
    }

    if ( pushTuple(Tuple, minTuple, maxTuple, false, t ) == false)
    {
      //array bounds violated
      break;
    }

    {
      //coalesce skyline list remove expired and mv==false
      {
        int outCount=0;
        for (int a = 0;a<skyCount;a++) 
        {
          TUPLE Tuple = skyTuple[a];
          if ((t+1) >= (Tuple.tstamp.time + Tuple.tstamp.duration))
          {
            skyTuple[a].mv = false;
            skyUpdated = true;
            
            //printf("Xa%8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X \n",t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      
            
            //APPENDEXP(Tuple);

            expTuple[expCount++] = Tuple;
            expUpdated = true;
            if(expCount>sizeof(expTuple)/sizeof(expTuple[0]))
            {
              break;
            }         
          }
          else if( Tuple.mv==true)
          {
            if(outCount!=a)
              skyTuple[outCount] = Tuple;
            outCount++;
          }
        }
        skyCount = outCount;
        if(expCount>sizeof(expTuple)/sizeof(expTuple[0]))
        {
          printf("exp window exceeded!\n");
          break;
        }
      }

      //coalesce rejected list remove expired or forced retired (duration==0)
      COALESCE(rejTuple, rejCount);

      //coalesce removed list remove expired
      COALESCE(rmvTuple, rmvCount);

      //recover skipped or removed items dominated by tuples in skyline that have expired 
      {
        if(expUpdated)
        {
          
          if(rmvUpdated)
          for(int b = 0; b< rmvCount; b++)
          {
            if(rmvTuple[b].mv==false) continue;
            for(int c = b+1; c< rmvCount-1; c++)
            {
              if(rmvTuple[c].mv==false) continue;

              if(rmvTuple[b].dominatedby(rmvTuple[c],sensor::tuple<DIMEN>::CBMASK))
              {
                rmvTuple[b].mv = false;
                break;
              }
              if(rmvTuple[c].dominatedby(rmvTuple[b],sensor::tuple<DIMEN>::CBMASK))
              {
                rmvTuple[c].mv = false;
                continue;
              }
            }
          }
          
          if(rejUpdated)
          for(int b = 0; b< rejCount; b++)
          {
            if(rejTuple[b].mv==false) continue;
            for(int c = b+1; c< rejCount-1; c++)
            {
              if(rejTuple[c].mv==false) continue;

              if(rejTuple[b].dominatedby(rejTuple[c],sensor::tuple<DIMEN>::CBMASK))
              {
                rejTuple[b].mv = false;
                break;
              }
              if(rejTuple[c].dominatedby(rejTuple[b],sensor::tuple<DIMEN>::CBMASK))
              {
                rejTuple[c].mv = false;
                continue;
              }
            }
          }
          bool fail = false;
          for(int a = 0; fail==false && a< expCount; a++)
          {
            TUPLE Tuple = expTuple[a];
            
            printf("X - %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X \n",t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      
            
            int rmvC = rmvCount;
            for(int b = 0; b< rmvC; b++)
            {
              TUPLE cTuple = rmvTuple[b];        //recover un-dominated
              rmvTuple[b].mv = true;
              if (cTuple.mv ==true && cTuple.dominatedby(Tuple,sensor::tuple<DIMEN>::CBMASK))
              {
                //printf("QX- %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X \n",b,cTuple.tstamp.time,cTuple.tstamp.duration,cTuple.mv, TUPLELIST(cTuple) , (cTuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (cTuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      
                //stick it back into stream
                if ( pushTuple(cTuple, minTuple, maxTuple, true, t ) == false)
                {
                  fail = true;
                  break;
                }
                rmvTuple[b].tstamp.duration = 0;
                rmvTuple[b].mv = false;
              }
            }
            
            int rejC = rejCount;  //pushed items rejected are above rejC
            for(int b = 0; b< rejC; b++)
            {
              TUPLE cTuple = rejTuple[b];        //recover un-dominated
              rejTuple[b].mv = true;
              if (cTuple.mv==true && cTuple.dominatedby(Tuple,sensor::tuple<DIMEN>::CBMASK))
              {
                //printf("QD- %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X \n",b,cTuple.tstamp.time,cTuple.tstamp.duration,cTuple.mv, TUPLELIST(cTuple) , (cTuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (cTuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      
                //stick it back into stream  may be rejected immd
                if ( pushTuple(cTuple, minTuple, maxTuple, true, t ) == false)
                {
                  fail = true;
                  break;
                }
                rejTuple[b].tstamp.duration = 0;
                rejTuple[b].mv = false;
              }
              //
            }
          }
          if(fail==true)
          {
            break;
          }
          expUpdated = false;
          expCount = 0;
        }
      }
    }




    //if((t%tmod)==0 || t==maxt-1)
    if( t==maxt-1)
    //if(skyUpdated)
    //if(0)
    for (int a = 0; a<skyCount; a++) 
    {
      TUPLE Tuple = skyTuple[a];
      printf("A - %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X\n",a, Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
    }
    //if(0)
    if( t==maxt-1)
    //if((t%tmod)==0 || t==maxt-1)
    for(int a = 0; a< rejCount; a++)
    {
      TUPLE Tuple = rejTuple[a];
      printf("B - %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X\n",a,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
    } 


    if( t==maxt-1)  
    for(int b = 0; b< rejCount; b++)
    {
      if(rejTuple[b].mv==false) continue;
      for(int c = b+1; c< rejCount-1; c++)
      {
        if(rejTuple[c].mv==false) continue;

        if(rejTuple[b].dominatedby(rejTuple[c],sensor::tuple<DIMEN>::CBMASK))
        {
          rejTuple[b].mv = false;
          break;
        }
        if(rejTuple[c].dominatedby(rejTuple[b],sensor::tuple<DIMEN>::CBMASK))
        {
          rejTuple[c].mv = false;
          continue;
        }
      }
    }

    if( t==maxt-1)
    //if((t%tmod)==0 || t==maxt-1)
    for(int a = 0; a< rejCount; a++)
    {
      TUPLE Tuple = rejTuple[a];
      printf("B - %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X\n",a,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
    } 

    //if(0)
    if( t==maxt-1)
    //if((t%tmod)==0 || t==maxt-1)
    for(int a = 0; a< rmvCount; a++)
    {
      TUPLE Tuple = rmvTuple[a];
      printf("C - %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X\n",a,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
    }

    //if(0)
    //if( t==maxt-1)
    //if((t%tmod)==0 || t==maxt-1)
    //for(int a = 0; a< expCount; a++)
    //{
    //  TUPLE Tuple = expTuple[a];
    //  printf("D - %8d  %8d  %8d %1X " TUPLETEMPLATE " %4X %4X\n",a,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
    //}
    //expCount = 0;


    if(skyCount >= sizeof(skyTuple)/sizeof(skyTuple[0]))
    {
      printf("sky window exceeded!\n");
      break;
    } 
    if(rejCount >= sizeof(rejTuple)/sizeof(rejTuple[0]))
    {
      printf("rej window exceeded!\n");
      break;
    } 
    if(expCount >= sizeof(expTuple)/sizeof(expTuple[0]))
    {
      printf("exp window exceeded!\n");
      break;
    }
    //if(rmvCount >= sizeof(rmvTuple)/sizeof(rmvTuple[0]))
    //{
    //  printf("rmv window exceeded!\n");
    //  break;
    //}
  }
  time_t etime = time(0);
  printf("\n%d\n",etime-stime);
	return 0;

}
