// CLASSIFICATION: UNCLASSIFIED

/***************************************************************************/
/* FILE: TransverseMercator.cpp
 *
 * ABSTRACT
 *
 *    This component provides conversions between Geodetic coordinates 
 *    (latitude and longitude) and Transverse Mercator projection coordinates
 *    (easting and northing).
 *
 * MODIFICATIONS
 *
 *    Date     Description
 *    ----     -----------
 *    2-26-07  Original C++ Code
 *    7/19/10  N. Lundgren BAEts27271 Correct the test for TRANMERC_LON_WARNING
 *             in convertToGeodetic, by removing the multiply by cos(latitude)
 *    3/23/11  N. Lundgren BAEts28583 Updated memory leak checks for 
 *             code consistency.
 *    7/02/14  Updated to algorithm in NGA Transverse Mercator document.
 *
 */
#include <iostream>

#include <math.h>
#include "TransverseMercator.h"
#include "MapProjection5Parameters.h"
#include "MapProjectionCoordinates.h"
#include "GeodeticCoordinates.h"
#include "CoordinateConversionException.h"
#include "ErrorMessages.h"

using MSP::CCS::TransverseMercator;
// Terms in series A and B coefficients
#define N_TERMS   6
#define MAX_TERMS 8

//                  DEFINES
#define PI                3.14159265358979323e0
#define PI_OVER_2         (PI/2.0e0)
#define MAX_DELTA_LONG    ((PI * 70)/180.0)
#define MIN_SCALE_FACTOR   0.1
#define MAX_SCALE_FACTOR  10.0


TransverseMercator::TransverseMercator(
   double ellipsoidSemiMajorAxis,
   double ellipsoidFlattening,
   double centralMeridian,
   double latitudeOfTrueScale,
   double falseEasting,
   double falseNorthing,
   double scaleFactor ) :
   CoordinateSystem( ellipsoidSemiMajorAxis, ellipsoidFlattening ),
   TranMerc_Origin_Long( centralMeridian ),
   TranMerc_Origin_Lat( latitudeOfTrueScale ),
   TranMerc_False_Easting( falseEasting ),
   TranMerc_False_Northing( falseNorthing ),
   TranMerc_Scale_Factor( scaleFactor  ),
   TranMerc_Delta_Easting(  20000000.0 ),
   TranMerc_Delta_Northing( 10000000.0 )
{
   double TranMerc_b; // Semi-minor axis of ellipsoid, in meters
   double invFlattening = 1.0 / ellipsoidFlattening;

   if (ellipsoidSemiMajorAxis <= 0.0)
   { // Semi-major axis must be greater than zero
      throw CoordinateConversionException( ErrorMessages::semiMajorAxis );
   }
   if ( invFlattening < 150 )
   {
      throw CoordinateConversionException( ErrorMessages::ellipsoidFlattening );
   }
   if ((latitudeOfTrueScale < -PI_OVER_2) || (latitudeOfTrueScale > PI_OVER_2))
   { // latitudeOfTrueScale out of range
      throw CoordinateConversionException( ErrorMessages::originLatitude );
   }
   if ((centralMeridian < -PI) || (centralMeridian > (2*PI)))
   { // centralMeridian out of range
      throw CoordinateConversionException( ErrorMessages::centralMeridian );
   }
   if ((scaleFactor < MIN_SCALE_FACTOR) || (scaleFactor > MAX_SCALE_FACTOR))
   {
      throw CoordinateConversionException( ErrorMessages::scaleFactor );
   }

   if (TranMerc_Origin_Long > PI)
      TranMerc_Origin_Long -= (2*PI);

   // Eccentricity
   TranMerc_eps = sqrt( 2 * flattening - flattening * flattening );

   double n1, R4oa;
   generateCoefficients(
      invFlattening, n1, TranMerc_aCoeff, TranMerc_bCoeff, R4oa );

   TranMerc_K0R4    = R4oa * TranMerc_Scale_Factor * ellipsoidSemiMajorAxis;
   TranMerc_K0R4inv = 1.0 / TranMerc_K0R4;
}


TransverseMercator::TransverseMercator( const TransverseMercator &tm )
{
   *this = tm;
}


TransverseMercator::~TransverseMercator()
{
}

TransverseMercator& TransverseMercator::operator=( 
   const TransverseMercator &tm )
{
   if( this != &tm )
   {
      semiMajorAxis           = tm.semiMajorAxis;
      flattening              = tm.flattening;
      TranMerc_eps            = tm.TranMerc_eps;

      TranMerc_K0R4           = tm.TranMerc_K0R4;
      TranMerc_K0R4inv        = tm.TranMerc_K0R4inv;

      for( int i = 0; i < MAX_TERMS; i++ )
      {
         TranMerc_aCoeff[i] = tm.TranMerc_aCoeff[i];
         TranMerc_bCoeff[i] = tm.TranMerc_bCoeff[i];
      }

      TranMerc_Origin_Long    = tm.TranMerc_Origin_Long; 
      TranMerc_Origin_Lat     = tm.TranMerc_Origin_Lat; 
      TranMerc_False_Easting  = tm.TranMerc_False_Easting; 
      TranMerc_False_Northing = tm.TranMerc_False_Northing; 
      TranMerc_Scale_Factor   = tm.TranMerc_Scale_Factor;

      TranMerc_Delta_Easting  = tm.TranMerc_Delta_Easting; 
      TranMerc_Delta_Northing = tm.TranMerc_Delta_Northing; 
   }

   return *this;
}


MSP::CCS::MapProjection5Parameters* TransverseMercator::getParameters() const
{
   return new MapProjection5Parameters(
      CoordinateType::transverseMercator,
      TranMerc_Origin_Long, TranMerc_Origin_Lat, TranMerc_Scale_Factor,
      TranMerc_False_Easting, TranMerc_False_Northing );
}


MSP::CCS::MapProjectionCoordinates* TransverseMercator::convertFromGeodetic(
   MSP::CCS::GeodeticCoordinates* geodeticCoordinates )
{
   double longitude = geodeticCoordinates->longitude();
   double latitude  = geodeticCoordinates->latitude();

   if (longitude > PI)
      longitude -= (2 * PI);
   if (longitude < -PI)
      longitude += (2 * PI);

   //  Convert longitude (Greenwhich) to longitude from the central meridian
   //  (-Pi, Pi] equivalent needed for checkLatLon.
   //  Compute its cosine and sine.
   double lamda  = longitude - TranMerc_Origin_Long;
   if (lamda > PI)
      lamda -= (2 * PI);
   if (lamda < -PI)
      lamda += (2 * PI);
   checkLatLon( latitude, lamda );

   double easting, northing;
   latLonToNorthingEasting( latitude, longitude, northing, easting );

   // The origin may move form (0,0) and this is represented by 
   // a change in the false Northing/Easting values. 
   double falseEasting, falseNorthing;
   latLonToNorthingEasting(
      TranMerc_Origin_Lat, TranMerc_Origin_Long, falseNorthing, falseEasting );

   easting  += TranMerc_False_Easting  - falseEasting;
   northing += TranMerc_False_Northing - falseNorthing;

   char warning[256] = "";
   warning[0] = '\0';
   double invFlattening = 1.0 / flattening;
   if( invFlattening < 290.0 || invFlattening > 301.0 )
      strcat( warning,
         "Eccentricity is outside range that algorithm accuracy has been tested." );

   return new MapProjectionCoordinates(
      CoordinateType::transverseMercator, warning, easting, northing );
}


void TransverseMercator::latLonToNorthingEasting( 
   const double &latitude,
   const double &longitude,
   double       &northing,
   double       &easting )
{
   //  Convert longitude (Greenwhich) to longitude from the central meridian
   //  (-Pi, Pi] equivalent needed for checkLatLon.
   //  Compute its cosine and sine.
   double lamda  = longitude - TranMerc_Origin_Long;
   if (lamda > PI)
      lamda -= (2 * PI);
   if (lamda < -PI)
      lamda += (2 * PI);
   checkLatLon( latitude, lamda );

   double cosLam = cos(lamda);
   double sinLam = sin(lamda);
   double cosPhi = cos(latitude);
   double sinPhi = sin(latitude);

   double P, part1, part2, denom, cosChi, sinChi;
   double U, V;
   double c2ku[MAX_TERMS], s2ku[MAX_TERMS];
   double c2kv[MAX_TERMS], s2kv[MAX_TERMS];

   //  Ellipsoid to sphere
   //  --------- -- ------ 

   //  Convert geodetic latitude, Phi, to conformal latitude, Chi
   //  Only the cosine and sine of Chi are actually needed.
   P      = exp(TranMerc_eps * aTanH(TranMerc_eps * sinPhi));
   part1  = (1 + sinPhi) / P;
   part2  = (1 - sinPhi) * P;
   denom  = part1 + part2;
   cosChi = 2 * cosPhi / denom;
   sinChi = (part1 - part2) / denom;

   //  Sphere to first plane
   //  ------ -- ----- ----- 

   // Apply spherical theory of transverse Mercator to get (u,v) coord.s
   U = aTanH(cosChi * sinLam);
   V = atan2(sinChi, cosChi * cosLam);

   // Use trig identities to compute cosh(2kU), sinh(2kU), cos(2kV), sin(2kV)
   computeHyperbolicSeries( 2.0 * U, c2ku, s2ku );
   computeTrigSeries( 2.0 * V, c2kv, s2kv );

   //  First plane to second plane
   //  Accumulate terms for X and Y
   double xStar = 0;
   double yStar = 0;

   for (int k = N_TERMS - 1; k >= 0; k--)
   {
      xStar += TranMerc_aCoeff[k] * s2ku[k] * c2kv[k];
      yStar += TranMerc_aCoeff[k] * c2ku[k] * s2kv[k];
   }

   xStar += U;
   yStar += V;

   // Apply isoperimetric radius, scale adjustment, and offsets
   easting  = (TranMerc_K0R4 * xStar);
   northing = (TranMerc_K0R4 * yStar);
}


MSP::CCS::GeodeticCoordinates* TransverseMercator::convertToGeodetic(
   MSP::CCS::MapProjectionCoordinates* mapProjectionCoordinates )
{
   double easting  = mapProjectionCoordinates->easting();
   double northing = mapProjectionCoordinates->northing();

   if (  (easting < (TranMerc_False_Easting - TranMerc_Delta_Easting))
       ||(easting > (TranMerc_False_Easting + TranMerc_Delta_Easting)))
   { // easting out of range
      throw CoordinateConversionException( ErrorMessages::easting );
   }

   if (   (northing < (TranMerc_False_Northing - TranMerc_Delta_Northing))
       || (northing > (TranMerc_False_Northing + TranMerc_Delta_Northing)))
   { // northing out of range
      throw CoordinateConversionException( ErrorMessages::northing );
   }

   double longitude, latitude;
   // The origin may move form (0,0) and this is represented by 
   // a change in the false Northing/Easting values. 
   double falseEasting, falseNorthing;
   latLonToNorthingEasting(
      TranMerc_Origin_Lat, TranMerc_Origin_Long, falseNorthing, falseEasting );

   easting  -= (TranMerc_False_Easting  - falseEasting);
   northing -= (TranMerc_False_Northing - falseNorthing);

   northingEastingToLatLon( northing, easting, latitude, longitude );

   longitude = (longitude >   PI) ? longitude - (2 * PI): longitude;
   longitude = (longitude <= -PI) ? longitude + (2 * PI): longitude;

   if(fabs(latitude) > (90.0 * PI / 180.0))
   {
      throw CoordinateConversionException( ErrorMessages::northing );
   }
   if((longitude) > (PI))
   {
      longitude -= (2 * PI);
      if(fabs(longitude) > PI)
         throw CoordinateConversionException( ErrorMessages::easting );
   }
   else if((longitude) < (-PI))
   {
      longitude += (2 * PI);
      if(fabs(longitude) > PI)
         throw CoordinateConversionException( ErrorMessages::easting );
   }

   char warning[256];
   warning[0] = '\0';
   double invFlattening = 1.0 / flattening;
   if( invFlattening < 290.0 || invFlattening > 301.0 )
      strcat( warning,
         "Eccentricity is outside range that algorithm accuracy has been tested." );

   return new GeodeticCoordinates(
      CoordinateType::geodetic, warning, longitude, latitude );
}

void TransverseMercator::northingEastingToLatLon( 
   const double &northing,
   const double &easting,
   double       &latitude,
   double       &longitude )
{
   double c2kx[MAX_TERMS], s2kx[MAX_TERMS], c2ky[MAX_TERMS], s2ky[MAX_TERMS];
   double U, V;
   double lamda;
   double sinChi;

   //  Undo offsets, scale change, and factor R4
   //  ---- -------  ----- ------  --- ------ --
   double xStar = TranMerc_K0R4inv * (easting);
   double yStar = TranMerc_K0R4inv * (northing);

   // Use trig identities to compute cosh(2kU), sinh(2kU), cos(2kV), sin(2kV)
   computeHyperbolicSeries( 2.0 * xStar, c2kx, s2kx );
   computeTrigSeries( 2.0 * yStar, c2ky, s2ky );

   //  Second plane (x*, y*) to first plane (u, v)
   //  ------ ----- -------- -- ----- ----- ------
   U = 0;
   V = 0;

   for (int k = N_TERMS - 1; k >= 0; k--)
   {
      U += TranMerc_bCoeff[k] * s2kx[k] * c2ky[k];
      V += TranMerc_bCoeff[k] * c2kx[k] * s2ky[k];
   }

   U += xStar;
   V += yStar;

   //  First plane to sphere
   //  ----- ----- -- ------
   double coshU = cosh(U);
   double sinhU = sinh(U);
   double cosV  = cos(V);
   double sinV  = sin(V);

   //   Longitude from central meridian
   if ((fabs(cosV) < 10E-12) && (fabs(coshU) < 10E-12))
      lamda = 0;
   else
      lamda = atan2(sinhU, cosV);

   //   Conformal latitude
   sinChi = sinV / coshU;
   latitude = geodeticLat( sinChi, TranMerc_eps );

   // Longitude from Greenwich
   // --------  ---- ---------
   longitude = TranMerc_Origin_Long + lamda;
}

//                PRIVATE FUNCTIONS

void TransverseMercator::generateCoefficients(
	double  invfla,
	double &n1,
	double  aCoeff[MAX_TERMS],
	double  bCoeff[MAX_TERMS],
	double &R4oa)
{
   /*  Generate Coefficients for Transverse Mercator algorithms
       ===----- ===---------
   Algorithm developed by: C. Rollins   April 18, 2006

   INPUT
   -----
      invfla    Inverse flattening (reciprocal flattening)

   OUTPUT
   ------
      n1        Helmert's "n"
      aCoeff    Coefficients for omega as a trig series in chi
      bBoeff    Coefficients for chi as a trig series in omega
      R4oa      Ratio "R4 over a", i.e. R4/a

   EXPLANATIONS
   ------------
      omega is rectifying latitude
      chi is conformal latitude
      psi is geocentric latitude
      phi is geodetic latitude, commonly, "the latitude"
      R4 is the meridional isoperimetric radius
      "a" is the semi-major axis of the ellipsoid
      "b" is the semi-minor axis of the ellipsoid
      Helmert's n = (a - b)/(a + b)
 
      This calculation depends only on the shape of the ellipsoid and are
      independent of the ellipsoid size.
 
      The array Acoeff(8) stores eight coefficients corresponding
         to k = 2, 4, 6, 8, 10, 12, 14, 16 in the notation "a sub k".
      Likewise Bcoeff(8) etc.
*/

   double n2, n3, n4, n5, n6, n7, n8, n9, n10, coeff;

   n1  = 1.0 / (2*invfla - 1.0);

   n2  = n1 * n1;
   n3  = n2 * n1;
   n4  = n3 * n1;
   n5  = n4 * n1;
   n6  = n5 * n1;
   n7  = n6 * n1;
   n8  = n7 * n1;
   n9  = n8 * n1;
   n10 = n9 * n1;

   //   Computation of coefficient a2 
   coeff = 0.0;
   coeff += (-18975107.0) * n8 / 50803200.0;
   coeff += (72161.0)     * n7 / 387072.0;
   coeff += (7891.0)      * n6 / 37800.0;
   coeff += (-127.0)      * n5 / 288.0;
   coeff += (41.0)        * n4 / 180.0;
   coeff += (5.0)         * n3 / 16.0;
   coeff += (-2.0)        * n2 / 3.0;
   coeff += (1.0)         * n1 / 2.0;

   aCoeff[0] = coeff;

   //   Computation of coefficient a4 
   coeff = 0.0;
   coeff += (148003883.0) * n8 / 174182400.0;
   coeff += (13769.0)     * n7 / 28800.0;
   coeff += (-1983433.0)  * n6 / 1935360.0;
   coeff += (281.0)       * n5 / 630.0;
   coeff += (557.0)       * n4 / 1440.0;
   coeff += (-3.0)        * n3 / 5.0;
   coeff += (13.0)        * n2 / 48.0;

   aCoeff[1] = coeff;

   //   Computation of coefficient a6 
   coeff = 0.0;
   coeff += (79682431.0)  * n8 / 79833600.0;
   coeff += (-67102379.0) * n7 / 29030400.0;
   coeff += (167603.0)    * n6 / 181440.0;
   coeff += (15061.0)     * n5 / 26880.0;
   coeff += (-103.0)      * n4 / 140.0;
   coeff += (61.0)        * n3 / 240.0;

   aCoeff[2] = coeff;

   //   Computation of coefficient a8 
   coeff = 0.0;
   coeff += (-40176129013.0) * n8 / 7664025600.0;
   coeff += (97445.0)        * n7 / 49896.0;
   coeff += (6601661.0)      * n6 / 7257600.0;
   coeff += (-179.0)         * n5 / 168.0;
   coeff += (49561.0)        * n4 / 161280.0;

   aCoeff[3] = coeff;

   //   Computation of coefficient a10 
   coeff = 0.0;
   coeff += (2605413599.0) * n8 / 622702080.0;
   coeff += (14644087.0)   * n7 / 9123840.0;
   coeff += (-3418889.0)   * n6 / 1995840.0;
   coeff += (34729.0)      * n5 / 80640.0;

   aCoeff[4] = coeff;

   //   Computation of coefficient a12 
   coeff = 0.0;
   coeff += (175214326799.0) * n8 / 58118860800.0;
   coeff += (-30705481.0)    * n7 / 10378368.0;
   coeff += (212378941.0)    * n6 / 319334400.0;

   aCoeff[5] = coeff;

   //   Computation of coefficient a14 
   coeff = 0.0;
   coeff += (-16759934899.0) * n8 / 3113510400.0;
   coeff += (1522256789.0)   * n7 / 1383782400.0;

   aCoeff[6] = coeff;

   //   Computation of coefficient a16 
   coeff = 0.0;
   coeff += (1424729850961.0) * n8 / 743921418240.0;

   aCoeff[7] = coeff;
      
   //   Computation of coefficient b2 
   coeff = 0.0;
   coeff += (-7944359.0) * n8 / 67737600.0;
   coeff += (5406467.0)  * n7 / 38707200.0;
   coeff += (-96199.0)   * n6 / 604800.0;
   coeff += (81.0)       * n5 / 512.0;
   coeff += (1.0)        * n4 / 360.0;
   coeff += (-37.0)      * n3 / 96.0;
   coeff += (2.0)        * n2 / 3.0;
   coeff += (-1.0)       * n1 / 2.0;

   bCoeff[0] = coeff;

   //   Computation of coefficient b4 
   coeff = 0.0;
   coeff += (-24749483.0) * n8 / 348364800.0;
   coeff += (-51841.0)    * n7 / 1209600.0;
   coeff += (1118711.0)   * n6 / 3870720.0;
   coeff += (-46.0)       * n5 / 105.0;
   coeff += (437.0)       * n4 / 1440.0;
   coeff += (-1.0)        * n3 / 15.0;
   coeff += (-1.0)        * n2 / 48.0;

   bCoeff[1] = coeff;

   //   Computation of coefficient b6 
   coeff = 0.0;
   coeff += (6457463.0)  * n8 / 17740800.0;
   coeff += (-9261899.0) * n7 / 58060800.0;
   coeff += (-5569.0)    * n6 / 90720.0;
   coeff += (209.0)      * n5 / 4480.0;
   coeff += (37.0)       * n4 / 840.0;
   coeff += (-17.0)      * n3 / 480.0;

   bCoeff[2] = coeff;

   //   Computation of coefficient b8 
   coeff = 0.0;
   coeff += (-324154477.0) * n8 / 7664025600.0;
   coeff += (-466511.0)    * n7 / 2494800.0;
   coeff += (830251.0)     * n6 / 7257600.0;
   coeff += (11.0)         * n5 / 504.0;
   coeff += (-4397.0)      * n4 / 161280.0;

   bCoeff[3] = coeff;

   //   Computation of coefficient b10 
   coeff = 0.0;
   coeff += (-22894433.0) * n8 / 124540416.0;
   coeff += (8005831.0)   * n7 / 63866880.0;
   coeff += (108847.0)    * n6 / 3991680.0;
   coeff += (-4583.0)     * n5 / 161280.0;

   bCoeff[4] = coeff;

   //   Computation of coefficient b12 
   coeff = 0.0;
   coeff += (2204645983.0) * n8 / 12915302400.0;
   coeff += (16363163.0)   * n7 / 518918400.0;
   coeff += (-20648693.0)  * n6 / 638668800.0;

   bCoeff[5] = coeff;

   //   Computation of coefficient b14 
   coeff = 0.0;
   coeff += (497323811.0)  * n8 / 12454041600.0;
   coeff += (-219941297.0) * n7 / 5535129600.0;

   bCoeff[6] = coeff;

   //   Computation of coefficient b16 
   coeff = 0.0;
   coeff += (-191773887257.0) * n8 / 3719607091200.0;

   bCoeff[7] = coeff;

   //   Computation of ratio R4/a
   coeff = 0.0;
   coeff += (83349.0)  * n10 / 65536.0;
   coeff += (-20825.0) * n9 / 16384.0;
   coeff += (20825.0)  * n8 / 16384.0;
   coeff += (-325.0)   * n7 / 256.0;
   coeff += (325.0)    * n6 / 256.0;
   coeff += (-81.0)    * n5 / 64.0;
   coeff += (81.0)     * n4 / 64.0;
   coeff += (-5.0)     * n3 / 4.0;
   coeff += (5.0)      * n2 / 4.0;
   coeff += (-1.0)     * n1 / 1.0;
   coeff += 1.0;

   R4oa  = coeff;
}


void TransverseMercator::checkLatLon( double latitude, double deltaLon )
{
   // test is based on distance from central meridian = deltaLon
   if (deltaLon > PI)
      deltaLon -= (2 * PI);
   if (deltaLon < -PI)
      deltaLon += (2 * PI);

   double testAngle = fabs( deltaLon );

   double delta = fabs( deltaLon - PI );
   if( delta < testAngle )
      testAngle = delta;

   delta = fabs( deltaLon + PI );
   if( delta < testAngle )
      testAngle = delta;

   // Away from the equator, is also valid
   delta = PI_OVER_2 - latitude;
   if( delta < testAngle )
      testAngle = delta;

   delta = PI_OVER_2 + latitude;
   if( delta < testAngle )
      testAngle = delta;

   if( testAngle > MAX_DELTA_LONG )
   {
      throw CoordinateConversionException( ErrorMessages::longitude );
   }
}


double TransverseMercator::aTanH(double x)
{
   return(0.5 * log((1 + x) / (1 - x)));
}


double TransverseMercator::geodeticLat(
   double sinChi,
   double e )
{
   double p;
   double pSq;
   double s_old = 1.0e99;
   double s = sinChi;
   double onePlusSinChi  = 1.0+sinChi;
   double oneMinusSinChi = 1.0-sinChi;

   for( int n = 0; n < 30; n++ )
   {
      p = exp( e * aTanH( e * s ) );
      pSq = p * p;
      s = ( onePlusSinChi * pSq - oneMinusSinChi ) 
         /( onePlusSinChi * pSq + oneMinusSinChi );

      if( fabs( s - s_old ) < 1.0e-12 )
      {
         break;
      }
      s_old = s;
   }
   return asin(s);
}

void TransverseMercator::computeHyperbolicSeries(
   double twoX,
   double c2kx[],
   double s2kx[])
{
   // Use trig identities to compute
   // c2kx[k] = cosh(2kX), s2kx[k] = sinh(2kX)   for k = 0 .. 8
   c2kx[0] = cosh(twoX);
   s2kx[0] = sinh(twoX);
   c2kx[1] = 2.0 * c2kx[0] * c2kx[0] - 1.0;
   s2kx[1] = 2.0 * c2kx[0] * s2kx[0];
   c2kx[2] = c2kx[0] * c2kx[1] + s2kx[0] * s2kx[1];
   s2kx[2] = c2kx[1] * s2kx[0] + c2kx[0] * s2kx[1];
   c2kx[3] = 2.0 * c2kx[1] * c2kx[1] - 1.0;
   s2kx[3] = 2.0 * c2kx[1] * s2kx[1];
   c2kx[4] = c2kx[0] * c2kx[3] + s2kx[0] * s2kx[3];
   s2kx[4] = c2kx[3] * s2kx[0] + c2kx[0] * s2kx[3];
   c2kx[5] = 2.0 * c2kx[2] * c2kx[2] - 1.0;
   s2kx[5] = 2.0 * c2kx[2] * s2kx[2];
   c2kx[6] = c2kx[0] * c2kx[5] + s2kx[0] * s2kx[5];
   s2kx[6] = c2kx[5] * s2kx[0] + c2kx[0] * s2kx[5];
   c2kx[7] = 2.0 * c2kx[3] * c2kx[3] - 1.0;
   s2kx[7] = 2.0 * c2kx[3] * s2kx[3];
}

void TransverseMercator::computeTrigSeries(
   double twoY,
   double c2ky[],
   double s2ky[])
{
   // Use trig identities to compute
   // c2ky[k] = cos(2kY), s2ky[k] = sin(2kY)   for k = 0 .. 8
   c2ky[0] = cos(twoY);
   s2ky[0] = sin(twoY);
   c2ky[1] = 2.0 * c2ky[0] * c2ky[0] - 1.0;
   s2ky[1] = 2.0 * c2ky[0] * s2ky[0];
   c2ky[2] = c2ky[1] * c2ky[0] - s2ky[1] * s2ky[0];
   s2ky[2] = c2ky[1] * s2ky[0] + c2ky[0] * s2ky[1];
   c2ky[3] = 2.0 * c2ky[1] * c2ky[1] - 1.0;
   s2ky[3] = 2.0 * c2ky[1] * s2ky[1];
   c2ky[4] = c2ky[3] * c2ky[0] - s2ky[3] * s2ky[0];
   s2ky[4] = c2ky[3] * s2ky[0] + c2ky[0] * s2ky[3];
   c2ky[5] = 2.0 * c2ky[2] * c2ky[2] - 1.0;
   s2ky[5] = 2.0 * c2ky[2] * s2ky[2];
   c2ky[6] = c2ky[5] * c2ky[0] - s2ky[5] * s2ky[0];
   s2ky[6] = c2ky[5] * s2ky[0] + c2ky[0] * s2ky[5];
   c2ky[7] = 2.0 * c2ky[3] * c2ky[3] - 1.0;
   s2ky[7] = 2.0 * c2ky[3] * s2ky[3];
}

// CLASSIFICATION: UNCLASSIFIED
