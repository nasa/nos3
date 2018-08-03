// CLASSIFICATION: UNCLASSIFIED

#ifndef Precision_H
#define Precision_H


namespace MSP
{
  namespace CCS
  {
    class Precision
    {
    public:

      enum Enum
      {
        degree,
        tenMinute,
        minute,
        tenSecond,
        second,
        tenthOfSecond,
        hundrethOfSecond,
        thousandthOfSecond,
        tenThousandthOfSecond
      };

       static Enum toPrecision( int prec )
       {
          Enum val = tenthOfSecond;

          if( prec == degree )
             val = degree;
          else if( prec == tenMinute )
             val = tenMinute;
          else if( prec == minute )
             val = minute;
          else if( prec == tenSecond )
             val = tenSecond;
          else if( prec == second)
             val = second;
          else if( prec == tenthOfSecond)
             val = tenthOfSecond;
          else if( prec == hundrethOfSecond)
             val = hundrethOfSecond;
          else if( prec == thousandthOfSecond)
             val = thousandthOfSecond;
          else if( prec == tenThousandthOfSecond)
             val = tenThousandthOfSecond;

          return val;
       }
    };
  }
}

#endif 


// CLASSIFICATION: UNCLASSIFIED
