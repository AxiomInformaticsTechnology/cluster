
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
    tuple(int mod[dimens],int div[dimens],int off[dimens])
		{
      mv = 0;
			for(int i=0;i<dimens;i++)
				c[i] = (double)((((RANDOM)%mod[i]))/(double)div[i]) + ((double)(off[i]))/div[i];
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
    bool dominatedby(tuple t,int m)
    {
      minb.b = 0;
     	for(int i=0;i<dimens;i++)
        if (c[i] >= t.c[i])
        {
          minb.b = minb.b|(1<<i);
        }
      return (minb.b==m);
    }
    cbit<dimens> fcbits(tuple t)
    {
      cbit<dimens> minb;
      minb.b = 0;
     	for(int i=0;i<dimens;i++)
        if (c[i] >= t.c[i])
        {
          minb.b = minb.b|(1<<i);
        }
      return minb;
    }
    cbit<dimens> febits(tuple t)
    {
      cbit<dimens> minb;
      minb.b = 0;
     	for(int i=0;i<dimens;i++)
        if (c[i] > t.c[i])
        {
          minb.b = minb.b|(1<<i);
        }
      return minb;
    }
    double mrv(double scale = 1.0 )
    {
      double ma = scale;
    	for(int i=0;i<dimens;i++)
        ma *= c[i];
      return ma;
    }
	};

};


#if (dimensions == 16)
const int DIMEN = 16;
#define TUPLETEMPLATE " %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f "
#define TUPLELIST(t)   t.c[0], t.c[1], t.c[2], t.c[3], t.c[4], t.c[5], t.c[6], t.c[7], t.c[8], t.c[9], t.c[10], t.c[11], t.c[12], t.c[13], t.c[14], t.c[15]
#elif ( dimensions == 8)
const int DIMEN = 8;
#define TUPLETEMPLATE " %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f "
#define TUPLELIST(t)   t.c[0], t.c[1], t.c[2], t.c[3], t.c[4], t.c[5], t.c[6], t.c[7]
#elif ( dimensions == 7)
const int DIMEN = 7;
#define TUPLETEMPLATE " %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f "
#define TUPLELIST(t)   t.c[0], t.c[1], t.c[2], t.c[3], t.c[4], t.c[5], t.c[6]
#elif ( dimensions == 6)
const int DIMEN = 6;
#define TUPLETEMPLATE " %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f "
#define TUPLELIST(t)   t.c[0], t.c[1], t.c[2], t.c[3], t.c[4], t.c[5]
#elif ( dimensions == 5)
const int DIMEN = 5;
#define TUPLETEMPLATE " %-12.7f %-12.7f %-12.7f %-12.7f %-12.7f "
#define TUPLELIST(t)   t.c[0], t.c[1], t.c[2], t.c[3], t.c[4]
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
typedef sensor::tuple<DIMEN>(*TUPLEA)[] ;