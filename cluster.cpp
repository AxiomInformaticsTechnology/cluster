// cluster.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <list>

#include <string>
using namespace std;

const int DIMEN = 16;
const int maxt  = 10000;
const int rmod  = 100000;
const int rdiv  = 100000;
const int expd  = 10000;
#define SKYSIZE 10000
#define REJSIZE 10000
#define EXPSIZE 10000
#define RMVSIZE 10000

#define TUPLETEMPLATE " %-10.7g %-10.7g %-10.7g %-10.7g %-10.7g %-10.7g %-10.7g %-10.7g %-10.7g %-10.7g %-10.7g %-10.7g %-10.7g %-10.7g %-10.7g %-10.7g "
#define TUPLELIST(t)   t.c[0], t.c[1], t.c[2], t.c[3], t.c[4], t.c[5], t.c[6], t.c[7], t.c[8], t.c[9], t.c[10], t.c[11], t.c[12], t.c[13], t.c[14], t.c[15]


class sensor
{
public:
	template <int dimens> class cbit
	{
  public:
    long unsigned b : dimens;
    cbit()
    {
      b = ((1<<dimens)-1);
    }
    long unsigned operator&(long unsigned m)
    {
      return b & m;
    }
    long unsigned operator|(long unsigned m)
    {
      return b | m;
    }
    long unsigned operator=(long unsigned m)
    {
      return m;
    }
  };
  template <int dimens> class tuple
	{
	public:
    const static long unsigned int CBMASK = (1<<dimens)-1;
    typedef struct
    {
      int time;
      int duration;
    } TSTAMP;
    TSTAMP       tstamp;
    cbit<dimens> minb;
		cbit<dimens> maxb;
		double       c[dimens];
    bool         mv;
		tuple()
		{
      mv = 0;
			for(int i=0;i<dimens;i++)
				c[i] = (double)0;
		}
    tuple(int mod,int div)
		{
      mv = 0;
			for(int i=0;i<dimens;i++)
				c[i] = (double)((((rand()*rand())%mod))/(double)div) + 1.0/div;
		}
    tuple(TSTAMP ts)
		{
      tstamp = ts;
      mv = 0;
			for(int i=0;i<dimens;i++)
				c[i] = 0;
		}
    bool operator==(tuple t)
    {
      for(int i=0;i<dimens;i++)
        if ( c[i] != t.c[i] ) return false;
      return true;
    }
    void maxs(tuple t)
    {
      maxb.b = 0;
     	for(int i=0;i<dimens;i++)
        if (c[i] < t.c[i])
        {
          c[i] =  t.c[i];
          maxb.b = maxb.b|(1<<i);
        }
    }
    void maxe(tuple t)
    {
      maxb.b = 0;
     	for(int i=0;i<dimens;i++)
        if (c[i] <= t.c[i])
        {
          maxb.b = maxb.b|(1<<i);
        }
    }
    void mins(tuple t)
    {
      minb.b = 0;
     	for(int i=0;i<dimens;i++)
        if (c[i] > t.c[i])
        {
          c[i] =  t.c[i];
          minb.b = minb.b|(1<<i);
        }
    }

    void mine(tuple t)
    {
      minb.b = 0;
     	for(int i=0;i<dimens;i++)
        if (c[i] >= t.c[i])
        {
          minb.b = minb.b|(1<<i);
        }
    }
    bool equs(tuple t)
    {
      for(int i=0;i<dimens;i++)
        if (c[i] != t.c[i]) return false;
      return true;
    }
    bool equsz(tuple t)
    {
      for(int i=0;i<dimens;i++)
      {
        if (t.c[i] != 0)
        {     
          for(int i=0;i<dimens;i++)
            if (c[i] != t.c[i]) return false;
          return true;
        }
      }
      return true;
    }
    bool dominatedby(tuple t)
    {
      minb.b = 0;
     	for(int i=0;i<dimens;i++)
        if (c[i] >= t.c[i])
        {
          minb.b = minb.b|(1<<i);
        }
      return (minb.b==CBMASK);
    }

	};

};


typedef sensor::tuple<DIMEN> TUPLE;

int _tmain(int argc, _TCHAR* argv[])
{

  TUPLE minTuple;

  TUPLE maxTuple;
  bool skyUpdated = false;
  bool rejUpdated = false;
  bool rmvUpdated = false;
  bool expUpdated = false;
 
  list<TUPLE>::iterator  sky_Iter;
  list<TUPLE>            skyTuple;              //skyline

  int skyCount = 0;
 // TUPLE skyTuple[SKYSIZE];   //
  int rejCount = 0;
  TUPLE rejTuple[REJSIZE];   //skipped not min/max and not between adj min/max
  int expCount = 0;
  TUPLE expTuple[EXPSIZE];   //expired from skyline
  int rmvCount = 0;
  TUPLE rmvTuple[RMVSIZE];   //removed from skyline by new entry
  
  TUPLE uTuple;
	for(int t=0;t<maxt;t++)
	{
    sensor::tuple<DIMEN>::TSTAMP ts;
    ts.time = t;
    //ts.duration = 1000; //(rand()%10000)+6000;  //6000-15000

    //if(useexprej)
    if(0) goto stop;

//retup:
    TUPLE rTuple(rmod,rdiv);
    //for(int a = 0; a< skyCount; a++) if(skyTuple[a].equsz(Tuple)) { fprintf(stderr,"REDO \n"); goto retup; }
    uTuple = rTuple;
    goto rtop;
    
stop:
    ;
    TUPLE sTuple;
    uTuple = sTuple;
    goto utop;

rtop: 
    ;
utop:
    ;

    TUPLE Tuple(uTuple);
    
    ts.duration  = expd;// 10 * Tuple.c[0] * Tuple.c[1] + 100;
    Tuple.tstamp = ts;

    skyUpdated = false;
    rejUpdated = false;
    rmvUpdated = false;
    expUpdated = false;

    fprintf(stderr,"%d\r",t);
    if(t==0) 
    { 
      Tuple.mv = true;
      minTuple = maxTuple = Tuple;
       skyTuple.insert(skyTuple.end(),Tuple);//skyTuple[skyCount++] = Tuple;
      skyUpdated = true;
      printf("H %8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X \n",t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK), (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
      continue;
    }
    minTuple.mins(Tuple); Tuple.minb = minTuple.minb;
    maxTuple.maxs(Tuple); Tuple.maxb = maxTuple.maxb;
    Tuple.mv = (Tuple.minb.b != 0 );
    if(Tuple.mv) 
    {
      //one or more compenents are minimum
      printf("H %8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X \n",t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK), (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
    }
    else
    {
      //check for dominance  note:: a min cannot be dominated expect by a min-er min
      TUPLE aTuple = Tuple;
      aTuple.mv = true;
      for ( sky_Iter = skyTuple.begin( ) ; sky_Iter != skyTuple.end( ) ; sky_Iter++ )     
      //for (int a=0;a<skyCount;a++)
      { 
        TUPLE sTuple = *sky_Iter;//skyTuple[a];
        //skipped removals
        if(sTuple.mv == false ) continue;
        //if(sTuple.minb.b != 0) continue;
        
        aTuple.minb.b = 0;
        aTuple.mine(sTuple);
        if(aTuple.minb.b == sensor::tuple<DIMEN>::CBMASK )
        {
          aTuple.mv = false;
          break;
        }
      }
      if(aTuple.mv == true)
      {
        Tuple.mv = true;
        printf("I %8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X \n",t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );     
      }
    }


    if(Tuple.mv==true)
    {
      int a = 0;
      //check for newly dominated
      TUPLE bTuple = Tuple;
      for (sky_Iter = skyTuple.begin( ) ; sky_Iter != skyTuple.end( ) ; sky_Iter++ )      // for (int a = 0;a<skyCount;a++)     
      { 
        TUPLE sTuple = *sky_Iter;//;skyTuple[a]

        if(sTuple.mv == false ) continue;
        bTuple.maxb.b = 0;
        bTuple.maxe(sTuple);
        if(bTuple.maxb.b == sensor::tuple<DIMEN>::CBMASK )
        {
          sTuple.mv = false;
          sky_Iter++;
          skyTuple.remove(sTuple);
          rmvUpdated = true;
          rmvTuple[rmvCount++] = sTuple; 
          if(rmvCount>sizeof(rmvTuple)/sizeof(rmvTuple[0]))
          {
            //ohfuckme
            exit(1);

          }
          printf("R %8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X \n",++a,sTuple.tstamp.time,sTuple.tstamp.duration,sTuple.mv, TUPLELIST(sTuple) , (sTuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (sTuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      
        }
      }
      skyTuple.insert(skyTuple.end(),Tuple);  //skyTuple[skyCount++] = Tuple;
      skyUpdated = true;
    }
    else
    {
      //not in current skyline but maybe will after an expire
      rejUpdated = true;
      rejTuple[rejCount++] = Tuple;
      //Tuple.maxe(maxTuple);
      //Tuple.mine(minTuple);
     // printf("S %8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X \n",t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      
    }

    //coalesce skyline list remove expired
    {
      int outCount=0;
      for ( sky_Iter = skyTuple.begin( ) ; sky_Iter != skyTuple.end( ) ; sky_Iter++ ) // for (int a = 0;a<skyCount;a++)     
      {
        TUPLE Tuple = *sky_Iter;   //skyTuple[a];
        if ((t+1) >= (Tuple.tstamp.time + Tuple.tstamp.duration))
        {
          expTuple[expCount++] = Tuple;
          expUpdated = true;
          skyUpdated = true;
          
          //printf("Xa%8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X \n",t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      
          
          if(expCount>sizeof(expTuple)/sizeof(expTuple[0]))
          {
            //ohfuckme
            exit(2);
          }
          sky_Iter++;
          skyTuple.remove(Tuple);
          //skyTuple[a].mv = false;
        }
        else if( Tuple.mv==true)  //  &&  ((t+1) < (Tuple.tstamp.time + Tuple.tstamp.duration)))
        {
         // if(outCount!=a)
         //   skyTuple[outCount] = Tuple;
          outCount++;
        }
      }
      skyCount = outCount;  //skyCount = skyTuple.size();
    }
    
    //coalesce skipped list remove expired
    {
      int outCount=0;
      for(int a = 0; a< rejCount; a++)
      {
        TUPLE Tuple = rejTuple[a];
        if ((t+1) >= (Tuple.tstamp.time + Tuple.tstamp.duration))
        {
          //printf("Rs%8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X \n",t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      
        }
        else
        {
          if(outCount!=a)
            rejTuple[outCount] = Tuple;
          outCount++;
        }
      }
      rejCount = outCount;
    }
    
    //coalesce removed list remove expired
    {
      int outCount=0;
      for(int a = 0; a< rmvCount; a++)
      {
        TUPLE Tuple = rmvTuple[a];
        if ((t+1) >= (Tuple.tstamp.time + Tuple.tstamp.duration))
        {
          //printf("Rr%8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X \n",t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      
        }
        else
        {
          if(outCount!=a)
            rmvTuple[outCount] = Tuple;
          outCount++;
        }
      }
      rmvCount = outCount;
    }

    //recover skipped or removed items dominated by expired items 
    if(expUpdated)
    {
      int outCount=0;
      for(int a = 0; a< expCount; a++,outCount++)
      {
        TUPLE Tuple = expTuple[a];
        printf("Z %8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X \n",t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      
        for(int b = 0; b< rejCount; b++)
        {
          TUPLE cTuple = rejTuple[b];        //recover un-dominated
          
          if (cTuple.dominatedby(Tuple))
          {
          //  printf("Zc%8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X \n",b,cTuple.tstamp.time,cTuple.tstamp.duration,cTuple.mv, TUPLELIST(cTuple) , (cTuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (cTuple.maxb & sensor::tuple<DIMEN>::CBMASK) );      
            //stick it back into stream
          }
          //
        }
      }
      expCount = outCount;
    }



    int a = 0;
    int tmod = 1000;
    if((t%tmod)==0 || t==maxt-1)
    //if( t==maxt-1)
    //if(skyUpdated)
    //if(0)
    for ( sky_Iter = skyTuple.begin( ) ; sky_Iter != skyTuple.end( ) ; sky_Iter++ )    //   for (int a = 0; a<skyCount; a++)     
    {
      TUPLE Tuple = *sky_Iter;  //skyTuple[a];
      printf("A %8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X\n",++a, Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
    }
    if(0)
    if((t%tmod)==0 || t==maxt-1)
    for(int a = 0; a< rejCount; a++)
    {
      TUPLE Tuple = rejTuple[a];
      printf("B %8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X\n",a,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
    } 
    if(0)
    if((t%tmod)==0 || t==maxt-1)
    for(int a = 0; a< rmvCount; a++)
    {
      TUPLE Tuple = rmvTuple[a];
      printf("B %8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X\n",a,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
    }
    if(0)
    if((t%tmod)==0 || t==maxt-1)
    for(int a = 0; a< expCount; a++)
    {
      TUPLE Tuple = expTuple[a];
      printf("C %8d  %8d  %8d %1X " TUPLETEMPLATE " %3X %3X\n",a,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
    }
    expCount = 0;


    //if(skyCount >= sizeof(skyTuple)/sizeof(skyTuple[0]))
    //{
    //  printf("sky window exceeded!\n");
    //  break;
    //} 
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
    if(rmvCount >= sizeof(rmvTuple)/sizeof(rmvTuple[0]))
    {
      printf("rmv window exceeded!\n");
      break;
    }
  }

	return 0;
}


int _xtmain(int argc, _TCHAR* argv[])
{

	for(int ith=100;ith>=1;ith--)
	{
		for(int jth=1;jth<=100;jth++)
		{ 
			int cluster =  10000*(((float)ith + (float)jth)/((float)ith * (float)jth));
			printf("%5d ",cluster);
		}
		printf("\n");
	}
	for(int ith=100;ith>=1;ith--)
	{
		for(int jth=1;jth<=100;jth++)
		{ 
			int cluster =  1000*(((float)ith + (float)jth)/((float)ith * (float)jth));
			printf("%5d ",cluster);
		}
		printf("\n");
	}


	for(int kth=1;kth<=10;kth++)
	{
		printf("\n%4d\n",kth);
		for(int ith=10;ith>=1;ith--)
		{ 
			for(int jth=1;jth<=10;jth++)
			{ 
				int cluster =  10000*(((float)ith + (float)jth + (float)kth)/((float)ith * (float)jth * (float)kth));
				printf("%5d ",cluster);
			}
			printf("\n");
		}
		
	}
	return 0;
}

