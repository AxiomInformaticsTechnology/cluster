// if duration of all the tuples the same constant
// any overdominated removed tuples from the skyline
// never needs to be saved or rechecked

#include <stdio.h>
#include <tchar.h>

#include <time.h>
#include <stdlib.h>

int nextN = 1;


#define SHOWNONE
#define Dimensions 20
#define dimensions 7
#define RANDOM (rand()*rand())    //if rmod > rand_max(7fff)
//efine RANDOM rand()             //if rmod < rand_max(7fff)

const int expd              = 100001;
const int tmod              = 10000;
const int maxt              = 100000;      //max tuples

int       rmod[Dimensions]  = { 100000 , 100000 , 100000 , 10  , 10000000    , 10000000    , 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000,100000, 100000 };     //random mod 100000, 
int       rdiv[Dimensions]  = { 1000   , 1000   , 1000   , 1   , 10000       , 10000       , 100000, 100000, 100000, 100000, 10000 , 10000 , 10000 , 10000 , 10000 , 10000 , 10000 , 10000 ,10000 , 10000  };     //random div 1000  ,        
int       roff[Dimensions]  = { 0      , 0      , 0      , 0   , 100000000   , 100000000   , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     ,0     , 0      };     //random div      1,        


#include "sensor.h"


#ifdef SHOWSOME
  //show at level 0
  #define SHOWALL (1)&&
#else 
  #ifdef SHOWMOST
   //show at any level
    #define SHOWALL (1)||
  #else
    #ifdef SHOWNONE
      //dont show
      #define SHOWALL (0)&&
    #else
      #define SHOWALL (0)&&
    #endif
  #endif
#endif

#define SKYSIZE 10000
#define RMVSIZE 100000

class skyline;

class skyline
{
public:
  int      skyNode;
  bool     skyUpdated;
  int      skyCount;
  int      skyCountMax;
  TUPLEA   skyTuple;
  TUPLE    minTuple;
  TUPLE    maxTuple;
  TUPLE    minTuplePop;
  TUPLE    maxTuplePop;
  skyline* rmvSky;
  skyline* parSky;

  skyline(skyline*par,int sky)
  {
    skyNode     = ::nextN++;
    parSky      = par;
    skyCountMax = sky; 
    skyCount    = 0;
    skyTuple    =  (TUPLEA)malloc(sky*sizeof(TUPLE));
    rmvSky      = 0;
    if(skyTuple==0)
    {
      fprintf(stderr,"BURP!!!!\n");
      exit(-1003);
    }
  } 

  skyline(int sky, int rmv)
  {
    skyNode     = ::nextN++;
    parSky      = 0;
    skyCountMax = sky; 
    skyCount    = 0; 
    skyTuple    = (TUPLEA)malloc(sky*sizeof(TUPLE));
    rmvSky      = new skyline(this, rmv);
    if(skyTuple==0||rmvSky==0)
    {
      fprintf(stderr,"BURP!!!!\n");
      exit(-1002);
    }
  }

  ~skyline()
  {
    if(skyTuple) free(skyTuple);
    skyTuple = 0;
    if(rmvSky) delete rmvSky;
    rmvSky = 0;
  }

  bool coalesceSkyline(int t,int reqQueued)
  {
    //coalesce skyline list remove expired and mv==false
    int outCount=0;
    int skyC = skyCount;
    for (int a = 0;a<skyC;a++) 
    {
      TUPLE Tuple = (*skyTuple)[a];
      if ( Tuple.mv==true && (t+1) >= (Tuple.tstamp.time + Tuple.tstamp.duration))
      {
        (*skyTuple)[a].mv = false;
        skyUpdated = true;
        if(rmvSky==0)
           rmvSky = new skyline(this,skyCountMax);
        if(SHOWALL reqQueued==0) printf("X %4X %4X %8d  %8d  %8d %4X " TUPLETEMPLATE " %4X %4X \n",skyNode,reqQueued,t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );     
        if(rmvSky->popTuple(Tuple,reqQueued+1,t) == false)
        {
          return false;
        }
      }
      else if( Tuple.mv==true)
      {
        if(outCount!=a)
          (*skyTuple)[outCount] = Tuple;
        outCount++;
      }
    }
    skyCount = outCount;
    if (rmvSky && rmvSky->coalesceSkyline(t,reqQueued+1) == false)
    {
      return false ;
    }
    return true;
  }

  bool pushTuple(TUPLE&Tuple,int reqQueued,int t)
  {
    //check 'global' min/max
    if(skyCount==0)
    {
      maxTuple = Tuple;
      minTuple = Tuple;
      Tuple.maxb.b = sensor::tuple<DIMEN>::CBMASK;
      Tuple.minb.b = sensor::tuple<DIMEN>::CBMASK;
    }
    else
    {
      maxTuple.maxs(Tuple); Tuple.maxb = maxTuple.maxb;
      minTuple.mins(Tuple); Tuple.minb = minTuple.minb;
    }

    int a = 0;  //first part of lower for loop is done in upper for lower for picks up where a was in upper  no rechecking of full list 
    int skyC = skyCount;

    
    if (Tuple.minb.b == 0 ) //tuple not a min
    {
      //check for dominance  note:: a min cannot be dominated expect by a min-er min

      //assume tuple is not dominated by any in skyline 
      Tuple.mv = true;
      
      for (;a<skyC;a++)
      {
        if( (*skyTuple)[a].mv == false ) continue;   //skipped removals
        //if(skyTuple[a].minb.b != 0 ) continue;   //note

        CBIT cb = Tuple.febits((*skyTuple)[a]);  // >  
        if (cb.b == 0 )  //tuple is not greater then sky in any dimension  this removes equals eg 0,0 removes 0,0  0,5 removes 0,10  
        {
          //tuple goes into skyline and skyTuple[a] comes out
          (*skyTuple)[a].minb.b = sensor::tuple<DIMEN>::CBMASK;

          if(rmvSky==0)
            rmvSky = new skyline(this,skyCountMax);
          if(SHOWALL  reqQueued==0) printf("Ro%4X %4X %8d  %8d  %8d %4X " TUPLETEMPLATE " %4X %4X \n",skyNode,reqQueued,t,(*skyTuple)[a].tstamp.time,(*skyTuple)[a].tstamp.duration,(*skyTuple)[a].mv, TUPLELIST((*skyTuple)[a]), ((*skyTuple)[a].minb & sensor::tuple<DIMEN>::CBMASK), ((*skyTuple)[a].maxb & sensor::tuple<DIMEN>::CBMASK) );
          if ( rmvSky->pushTuple((*skyTuple)[a],reqQueued+1,t) == false)
          {
            return false;
          }
          (*skyTuple)[a].mv = false;
          Tuple.mv = true;
         
          //keep checking below for more removals -- all this for 1 tuple?
          //yes it saves any rechecking of previous skyline tuples
          a++;
          break;
        }
        cb = Tuple.fcbits((*skyTuple)[a]);  // >=      
        if (cb.b == sensor::tuple<DIMEN>::CBMASK )
        {
          //Tuple dominated
          Tuple.mv = false;
          break;
        }
      }

      if(Tuple.mv == true)
      {      
     //   if(SHOWALL  reqQueued==0) printf("I %4X %8d  %8d  %8d %4X " TUPLETEMPLATE " %4X %4X \n",reqQueued,t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );     
      }
    }
    else
    {
      //one or more compenents are minimum
      Tuple.mv = true;
   //   if(SHOWALL  reqQueued==0) printf("H %4X %8d  %8d  %8d %4X " TUPLETEMPLATE " %4X %4X \n",reqQueued,t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK), (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
    }

    //check the part of skyline that is not done already for newly dominated
    if(Tuple.mv==true)
    {
      for (;a<skyC;a++)
      {
        if((*skyTuple)[a].mv == false ) continue;

        if((*skyTuple)[a].dominatedby(Tuple,sensor::tuple<DIMEN>::CBMASK) ==  true)
        {
          if(rmvSky==0) 
            rmvSky = new skyline(this,skyCountMax);
          if(SHOWALL  reqQueued==0) printf("R %4X %4X %8d  %8d  %8d %4X " TUPLETEMPLATE " %4X %4X \n",skyNode,reqQueued,t,(*skyTuple)[a].tstamp.time,(*skyTuple)[a].tstamp.duration,(*skyTuple)[a].mv, TUPLELIST((*skyTuple)[a]), ((*skyTuple)[a].minb & sensor::tuple<DIMEN>::CBMASK), ((*skyTuple)[a].maxb & sensor::tuple<DIMEN>::CBMASK) );
          if(rmvSky->pushTuple((*skyTuple)[a],reqQueued+1,t) == false)
          {
            return false;
          }
          (*skyTuple)[a].mv = false;
        }
      }

      if(SHOWALL  reqQueued==0) printf("I %4X %4X %8d  %8d  %8d %4X " TUPLETEMPLATE " %4X %4X \n",skyNode,reqQueued,t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
      (*skyTuple)[skyCount++] = Tuple;
      skyUpdated = true;
      if(skyCount>skyCountMax)
      {
        printf("sky window exceeded!\n");
        return false;
      }
    }
    else
    {
      //not in current skyline but maybe will after an expire
      Tuple.mv = true;
      if(rmvSky==0) 
        rmvSky = new skyline(this,skyCountMax);
      if(SHOWALL reqQueued==0) printf("S %4X %4X %8d  %8d  %8d %4X " TUPLETEMPLATE " %4X %4X \n",skyNode,reqQueued,t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
      if(rmvSky->pushTuple(Tuple,reqQueued+1,t) == false)
      {
        return false;
     }
     Tuple.mv = false;
    }
    return true;
  }

  bool popTuple(TUPLE&Tuple,int reqQueued,int t)
  {
    //check 'global' min/max
    if(skyCount==0)
    {
      return true;
    }
    maxTuplePop.maxs(Tuple); Tuple.maxb = maxTuplePop.maxb;
    minTuplePop.mins(Tuple); Tuple.minb = minTuplePop.minb;


    int a = 0;  //first part of lower for loop is done in upper for lower for picks up where a was in upper  no rechecking of full list 
    int skyC = skyCount;
    if (Tuple.minb.b == 0 ) //tuple not a min
    {
      //check for dominance  note:: a min cannot be dominated expect by a min-er min

      //assume tuple is not dominated by any in skyline 
      Tuple.mv = true;
      
      for (;a<skyC;a++)
      { 

        if((*skyTuple)[a].mv == false ) continue;   //skipped removals
        //if(skyTuple[a].minb.b != 0 ) continue;   //note

        CBIT cb = Tuple.febits((*skyTuple)[a]);  // >  
        if (cb.b == 0 )  //tuple is not greater then sky in any dimension  this removes equals eg 0,0 removes 0,0  0,5 removes 0,10  
        {
          //skyTuple[a] goes to parent skyline
          (*skyTuple)[a].minb.b = sensor::tuple<DIMEN>::CBMASK;
          if(parSky==0)
          {
            exit(-1000);
          }
          if(SHOWALL  reqQueued==0) printf("Po%4X %4X %8d  %8d  %8d %4X " TUPLETEMPLATE " %4X %4X \n",skyNode,reqQueued,t,(*skyTuple)[a].tstamp.time,(*skyTuple)[a].tstamp.duration,(*skyTuple)[a].mv, TUPLELIST((*skyTuple)[a]), ((*skyTuple)[a].minb & sensor::tuple<DIMEN>::CBMASK), ((*skyTuple)[a].maxb & sensor::tuple<DIMEN>::CBMASK) );
          if(parSky->pushTuple((*skyTuple)[a],reqQueued-1,t) == false)
          {
            return false;
          }
          Tuple.mv = true;
          (*skyTuple)[a].mv = false;
          //keep checking below for more removals -- all this for 1 tuple?
          //yes it saves any rechecking of previous skyline tuples
          a++;
          break;
        }
        cb = Tuple.fcbits((*skyTuple)[a]);  // >=      
        if (cb.b == sensor::tuple<DIMEN>::CBMASK )
        {
          //Tuple dominated
          Tuple.mv = false;
          break;
        }
      }
    }
    else
    {
      //one or more compenents are minimum
      Tuple.mv = true;
    }

    //check the part of skyline that is not done already for newly dominated
    if(Tuple.mv==true)
    {
      for (;a<skyC;a++)
      {
        if((*skyTuple)[a].mv == false ) continue;

        if((*skyTuple)[a].dominatedby(Tuple,sensor::tuple<DIMEN>::CBMASK) ==  true)
        {
          if(parSky==0)
          {
            exit(-1001);
          }
          if(SHOWALL reqQueued==0) printf("P %4X %4X %8d  %8d  %8d %4X " TUPLETEMPLATE " %4X %4X \n",skyNode,reqQueued,t,(*skyTuple)[a].tstamp.time,(*skyTuple)[a].tstamp.duration,(*skyTuple)[a].mv, TUPLELIST((*skyTuple)[a]), ((*skyTuple)[a].minb & sensor::tuple<DIMEN>::CBMASK), ((*skyTuple)[a].maxb & sensor::tuple<DIMEN>::CBMASK) );
          if(parSky->pushTuple((*skyTuple)[a],reqQueued-1,t) == false)
          {
            return false;
          }
          (*skyTuple)[a].mv = false;
        }
      }
    }
    else
    {
      //not in current skyline   pop == expired
      //should never occur but --- something is always dominated by a parent tuple in rmv list
      Tuple.mv = true;
      if(rmvSky==0)
        rmvSky = new skyline(this,skyCountMax);
      if(SHOWALL  reqQueued==0) printf("T %4X %4X %8d  %8d  %8d %4X " TUPLETEMPLATE " %4X %4X \n",skyNode,reqQueued,t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );
      if(rmvSky->pushTuple(Tuple,true,t) == false)
      {
        return false;
      }
      Tuple.mv = false;
      
    }
    return true;
  }

};



char * getatom(int ind);
int atom = 1;

int _tmain(int argc, _TCHAR* argv[])
{
  srand(147);
  time_t stime = time(0);
  
  skyline sky(SKYSIZE,RMVSIZE);

  TUPLE uTuple;
	for(int t=0;t<maxt;t++)
	{
    sensor::tuple<DIMEN>::TSTAMP ts;
    ts.time = t;
   
//retup:
    TUPLE Tuple(rmod,rdiv,roff);
//    for(int a = 0; a< sky.skyCount; a++) if((*sky.skyTuple)[a].equsz(Tuple)) { fprintf(stderr,"REDO \n"); goto retup; }
    
    ts.duration  = expd ? expd : ((RANDOM)%tmod)+1;  //(int)(Tuple.mrv()/10000);// /100000.0 + 100
    Tuple.tstamp = ts;
    int ctime = (int)( time(0)-stime)+1;
    
    
    
    fprintf(stderr,"%10d %10d %10d %10d %f\r",t,sky.skyCount,sky.rmvSky->skyCount,ctime,(float)(t/ctime));
    
    sky.skyUpdated = false;

     // printf("ATOM %6d  %3.3s ARG A  -1     %7.3f %7.3f %7.3f  1.00  0.00      2RLX  83\n",
       // atom++, getatom(Tuple.c[3]),        100+Tuple.c[0], 100+Tuple.c[1], 100+Tuple.c[2]);

    if ( sky.pushTuple(Tuple, 0, t ) == false)
    {
      //array bounds violated
      break;
    }
    if(1 ||(t && (t%8==0)))
    if ( sky.coalesceSkyline(t,0) == false )
    {
       //array bounds violated
      break;
    }

    if(t==maxt-1)// || (t%tmod==0))
    {
      skyline* skyp = & sky;
      while(skyp && skyp->skyNode<2)
      {
        for(int a = 0; a< skyp->skyCount; a++)
        {
          TUPLE Tuple = (*skyp->skyTuple)[a];
//        printf("A %4X %4d %8d  %8d  %8d %4X " TUPLETEMPLATE " %4X %4X \n",sky.skyNode,a,t,Tuple.tstamp.time,Tuple.tstamp.duration,Tuple.mv, TUPLELIST(Tuple), (Tuple.minb & sensor::tuple<DIMEN>::CBMASK)  , (Tuple.maxb & sensor::tuple<DIMEN>::CBMASK) );     
//      
          printf("ATOM %6d  %3.3s ARG A  -1     %7.3f %7.3f %7.3f  1.00  0.00      XYZZ  %d\n",
            atom++, getatom( Tuple.c[3]),    Tuple.c[0], Tuple.c[1], Tuple.c[2],skyp->skyNode);
        }
        skyp = skyp->rmvSky;
      }
    }
  }
  time_t etime = time(0);
  printf("\n%d\n",etime-stime);
	return 0;

}
char * getatom(int ind)
{
  switch(ind)
  {
    case 0: return "C  "; // Carbon
    case 1: return "N  "; // Nitrogen
    case 2: return "O  "; // Oxygen
    case 3: return "S  "; // Sulfer
    case 4: return "H  "; // Hydrogen
    case 5: return "B  "; // Boron
    case 6: return "F  "; // Flouride
    case 7: return "K  "; // Potassium
    case 8: return "P  "; // Phosphorous
    case 9: return "A  ";
  }
  return "A  ";
}