/******************************************************************************
 * Filename        : testCoordinateConversionSample.h
 *
 * Classification  : UNCLASSIFIED
 *
 * 
 *    Copyright 2007 BAE Systems National Security Solutions Inc. 1989-2006
 *                            ALL RIGHTS RESERVED
 *
 * MODIFICATION HISTORY:
 *
 * DATE        NAME              DR#               DESCRIPTION
 * 
 * 05/12/10    S Gillis          BAEts26542        MSP TS MSL-HAE conversion 
 *                                                 should use CCS         
 * 06/11/10    S. Gillis         BAEts26724        Fixed memory error problem
 *                                                 when MSPCCS_DATA is not set 
 * 08/26/11    K Ou              BAEts27716        Improved CCS sample code
 *
 ******************************************************************************/

#include <iostream>
#include <string>

#include "CoordinateConversionService.h"
#include "CoordinateSystemParameters.h"
#include "GeodeticParameters.h"
#include "CoordinateTuple.h"
#include "GeodeticCoordinates.h"
#include "CartesianCoordinates.h"
#include "Accuracy.h"
#include "MGRSorUSNGCoordinates.h"
#include "UTMParameters.h"
#include "UTMCoordinates.h"
#include "CoordinateType.h"
#include "HeightType.h"
#include "CoordinateConversionException.h"

using namespace std;
using namespace MSP::CCS;


/**
 * Sample code to demontrate how to use the MSP Coordinate Conversion Service.
 * 
 * Includes the following conversions:
 *
 * |=============================|=============================|
 * | Source                      | Target                      |
 * |=============================+=============================|
 * | Geodetic (Ellipsoid Height) | Geocentric                  |
 * | Geocentric                  | Geodetic (Ellipsoid Height) |
 * |-----------------------------+-----------------------------|
 * | Geocentric                  | Geodetic (MSL EGM 96 15M)   |
 * |-----------------------------+-----------------------------|
 * | Geodetic (Ellipsoid Height) | Geodetic (MSL EGM 96 15M)   |
 * | Geodetic (MSL EGM 96 15M)   | Geodetic (Ellipsoid Height) |
 * |-----------------------------+-----------------------------|
 * | Geocentric                  | UTM                         |
 * |-----------------------------+-----------------------------|
 * | Geocentric                  | MGRS                        |
 * |-----------------------------+-----------------------------|
 *
 **/


/**
 * Function which uses the given Geodetic (Ellipsoid Height) to Geocentric 
 * Coordinate Conversion Service, 'ccsGeodeticEllipsoidToGeocentric', to
 * convert the given lat, lon, and height to x, y, z coordinates.
 **/
void convertGeodeticEllipsoidToGeocentric(
   CoordinateConversionService& ccsGeodeticEllipsoidToGeocentric,
   double lat, 
   double lon, 
   double height, 
   double& x, 
   double& y, 
   double& z)
{    
   Accuracy sourceAccuracy;
   Accuracy targetAccuracy;
   GeodeticCoordinates sourceCoordinates(CoordinateType::geodetic, lon, lat, height);
   CartesianCoordinates targetCoordinates(CoordinateType::geocentric);

   ccsGeodeticEllipsoidToGeocentric.convertSourceToTarget(
      &sourceCoordinates, 
      &sourceAccuracy, 
      targetCoordinates, 
      targetAccuracy);

   x = targetCoordinates.x();
   y = targetCoordinates.y();
   z = targetCoordinates.z();
}


/**
 * Function which uses the given Geodetic (Ellipsoid Height) to Geocentric 
 * Coordinate Conversion Service, 'ccsGeodeticEllipsoidToGeocentric', to
 * convert the given x, y, z coordinates to a lat, lon, and height.
 **/
void convertGeocentricToGeodeticEllipsoid(
   CoordinateConversionService& ccsGeodeticEllipsoidToGeocentric,
   double x, 
   double y, 
   double z, 
   double& lat,
   double& lon, 
   double& height)
{
   Accuracy geocentricAccuracy;
   Accuracy geodeticAccuracy;
   CartesianCoordinates geocentricCoordinates(CoordinateType::geocentric, x, y, z);
   GeodeticCoordinates geodeticCoordinates;

   // Note that the Geodetic (Ellipsoid Height) to Geocentric Coordinate
   // Conversion Service is used here in conjunction with the
   // convertTargetToSource() method (as opposed to a Geocentric to
   // Geodetic (Ellipsoid Height) Coordinate Conversion Service in
   // conjunction with the convertSourceToTarget() method)
   ccsGeodeticEllipsoidToGeocentric.convertTargetToSource(
      &geocentricCoordinates, 
      &geocentricAccuracy,
      geodeticCoordinates, 
      geodeticAccuracy); 

   lat = geodeticCoordinates.latitude();
   lon = geodeticCoordinates.longitude();
   height = geodeticCoordinates.height();
}


/**
 * Function which uses the given Geocentric to Geodetic (MSL EGM 96 15M)
 * Coordinate Conversion Service, 'ccsGeocentricToGeodeticMslEgm96', to
 * convert the given x, y, z coordinates to a lat, lon, and height.
 **/
void convertGeocentricToGeodeticMslEgm96(
   CoordinateConversionService& ccsGeocentricToGeodeticMslEgm96,
   double x,
   double y,
   double z, 
   double& lat,
   double& lon, 
   double& height)
{
   Accuracy sourceAccuracy;
   Accuracy targetAccuracy;
   CartesianCoordinates sourceCoordinates(CoordinateType::geocentric, x, y, z);
   GeodeticCoordinates targetCoordinates(CoordinateType::geodetic, lon, lat, height);

   ccsGeocentricToGeodeticMslEgm96.convertSourceToTarget(
      &sourceCoordinates, 
      &sourceAccuracy, 
      targetCoordinates, 
      targetAccuracy );

   lat = targetCoordinates.latitude();
   lon = targetCoordinates.longitude();
   height = targetCoordinates.height();
}


/**
 * Function which uses the given Geodetic (MSL EGM 96 15M) to Geodetic
 * (Ellipsoid Height) Coordinate Conversion Service,
 * 'ccsMslEgm96ToEllipsoidHeight', to convert the given MSL height at the
 * given lat, lon, to an Ellipsoid height.
 **/
void convertMslEgm96ToEllipsoidHeight(
   CoordinateConversionService& ccsMslEgm96ToEllipsoidHeight,
   double lat, 
   double lon,
   double mslHeight,
   double& ellipsoidHeight)
{
   Accuracy sourceAccuracy;
   Accuracy targetAccuracy;
   GeodeticCoordinates sourceCoordinates(CoordinateType::geodetic, lon, lat, mslHeight);
   GeodeticCoordinates targetCoordinates;

   ccsMslEgm96ToEllipsoidHeight.convertSourceToTarget(
      &sourceCoordinates, 
      &sourceAccuracy, 
      targetCoordinates, 
      targetAccuracy);

   ellipsoidHeight = targetCoordinates.height();
}


/**
 * Function which uses the given Geodetic (Ellipsoid Height) to Geodetic
 * (MSL EGM 96 15M) Coordinate Conversion Service,
 * 'ccsEllipsoidHeightToMslEgm96', to convert the given Ellipsoid height at
 * the given lat, lon, to an MSL height.
 **/
void convertEllipsoidHeightToMslEgm96(
   CoordinateConversionService& ccsEllipsoidHeightToMslEgm96,
   double lat, 
   double lon, 
   double ellipsoidHeight, 
   double& mslHeight)
{
   Accuracy sourceAccuracy;
   Accuracy targetAccuracy;

   GeodeticCoordinates sourceCoordinates(CoordinateType::geodetic, lon, lat, ellipsoidHeight);
   GeodeticCoordinates targetCoordinates;

   ccsEllipsoidHeightToMslEgm96.convertSourceToTarget(
      &sourceCoordinates, 
      &sourceAccuracy, 
      targetCoordinates, 
      targetAccuracy);

   mslHeight = targetCoordinates.height();
}


/**
 * Function which uses the given Geocentric to UTM Coordinate Conversion
 * Service, 'ccsGeocentricToUtm', to convert the given x, y, z coordinates
 * a UTM zone, hemisphere, Easting and Northing.
 **/
void convertGeocentricToUtm(
   CoordinateConversionService& ccsGeocentricToUtm,
   double x,
   double y,
   double z, 
   long& zone,
   char& hemisphere, 
   double& easting,
   double& northing)
{
   Accuracy sourceAccuracy;
   Accuracy targetAccuracy;
   CartesianCoordinates sourceCoordinates(CoordinateType::geocentric, x, y, z);
   UTMCoordinates targetCoordinates;

   ccsGeocentricToUtm.convertSourceToTarget(
      &sourceCoordinates, 
      &sourceAccuracy, 
      targetCoordinates, 
      targetAccuracy);

   zone = targetCoordinates.zone();
   hemisphere = targetCoordinates.hemisphere();
   easting = targetCoordinates.easting();
   northing = targetCoordinates.northing();
}


/**
 * Function which uses the given Geocentric to MGRS Coordinate Conversion
 * Service, 'ccsGeocentricToMgrs', to convert the given x, y, z coordinates
 * to an MGRS string and precision.
 **/
string convertGeocentricToMgrs(
   CoordinateConversionService& ccsGeocentricToMgrs,
   double x,
   double y,
   double z, 
   Precision::Enum& precision)
{
   char* p;
   string mgrsString;

   Accuracy sourceAccuracy;
   Accuracy targetAccuracy;
   CartesianCoordinates sourceCoordinates(CoordinateType::geocentric, x, y, z);
   MGRSorUSNGCoordinates targetCoordinates;

   ccsGeocentricToMgrs.convertSourceToTarget(
      &sourceCoordinates, 
      &sourceAccuracy, 
      targetCoordinates, 
      targetAccuracy );

   // Returned value, 'p', points to targetCoordinate's internal character
   // array so assign/copy the character array to mgrsString to avoid
   // introducing memory management issues
   p = targetCoordinates.MGRSString();
   mgrsString = p;

   precision = targetCoordinates.precision();

   return mgrsString;
}


/******************************************************************************
 * Main function
 ******************************************************************************/

int main(int argc, char **argv)
{
   const char* WGE = "WGE";

   // initialize status value to one, indicating an error condition
   int status = 1; 

   try {

   cout << "Coordinate Conversion Service Sample Test Driver" << endl;
   cout << endl;

   //
   // Coordinate System Parameters 
   //
   GeodeticParameters ellipsoidParameters(
      CoordinateType::geodetic, 
      HeightType::ellipsoidHeight);

   CoordinateSystemParameters geocentricParameters(CoordinateType::geocentric);

   GeodeticParameters mslEgm96Parameters(
      CoordinateType::geodetic, 
      HeightType::EGM96FifteenMinBilinear);

   UTMParameters utmParameters(
      CoordinateType::universalTransverseMercator, 
      1, 
      0);

   CoordinateSystemParameters mgrsParameters(
      CoordinateType::militaryGridReferenceSystem);

   //
   // Coordinate Conversion Services 
   //
   CoordinateConversionService ccsGeodeticEllipsoidToGeocentric(
      WGE, &ellipsoidParameters, 
      WGE, &geocentricParameters);

   CoordinateConversionService ccsGeocentricToGeodeticMslEgm96(
      WGE, &geocentricParameters, 
      WGE, &mslEgm96Parameters);

   CoordinateConversionService ccsMslEgm96ToEllipsoidHeight(
      WGE, &mslEgm96Parameters, 
      WGE, &ellipsoidParameters);
   CoordinateConversionService ccsEllipsoidHeightToMslEgm96(
      WGE, &ellipsoidParameters, 
      WGE, &mslEgm96Parameters);

   CoordinateConversionService ccsGeocentricToUtm(
      WGE, &geocentricParameters, 
      WGE, &utmParameters);
   CoordinateConversionService ccsGeocentricToMgrs(
      WGE, &geocentricParameters, 
      WGE, &mgrsParameters);


      //
      // Geodetic (Ellipsoid Height) to Geocentric
      //
      double lat = 0.56932;
      double lon = -2.04552;
      double height = 0.0;

      double x, y, z;

      convertGeodeticEllipsoidToGeocentric(
         ccsGeodeticEllipsoidToGeocentric, 
         lat, lon, height, 
         x, y, z);

      cout << "Convert Geodetic (Ellipsoid Height) to Geocentric" << endl
           << endl
           << "Lat (radians): " << lat << endl
           << "Lon (radians): " << lon << endl
           << "Height(m): " << height << endl
           << endl 
           << "x: " << x << endl
           << "y: " << y << endl
           << "z: " << z << endl
           << endl;

      //
      // Geocentric to Geodetic (Ellipsoid Height)
      //

      // function convertGeocentricToGeodeticEllipsoid() reuses the
      // ccsGeodeticEllipsoidToGeocentric instance to perform the reverse
      // conversion
      convertGeocentricToGeodeticEllipsoid(
         ccsGeodeticEllipsoidToGeocentric, 
         x, y, z, 
         lat, lon, height);

      cout << "Revert Geocentric To Geodetic (Ellipsoid Height): " << endl
           << endl
           << "x: " << x << endl
           << "y: " << y << endl
           << "z: " << z << endl
           << endl
           << "Lat (radians): " << lat << endl
           << "Lon (radians): " << lon << endl
           << "Height(m): " << height << endl
           << endl;


      // reuse ccsGeodeticEllipsoidToGeocentric instance to perform another
      // Geodetic (Ellipsoid Height) to Geocentric conversions
      lat = 0.76388;
      lon = 0.60566;
      height = 11.0;

      convertGeodeticEllipsoidToGeocentric(
         ccsGeodeticEllipsoidToGeocentric, 
         lat, lon, height, 
         x, y, z);

      cout << "Convert Geodetic (Ellipsoid Height) to Geocentric" << endl
           << endl
           << "Lat (radians): " << lat << endl
           << "Lon (radians): " << lon << endl
           << "Height(m): " << height << endl
           << endl
           << "x: " << x << endl
           << "y: " << y << endl
           << "z: " << z << endl
           << endl;

      // reuse ccsGeodeticEllipsoidToGeocentric instance to perform another
      // Geodetic (Ellipsoid Height) to Geocentric conversions
      lat = 0.71458;
      lon = 0.88791;
      height = 22.0;

      convertGeodeticEllipsoidToGeocentric(
         ccsGeodeticEllipsoidToGeocentric, 
         lat, lon, height, 
         x, y, z);

      cout << "Convert Geodetic (Ellipsoid Height) to Geocentric" << endl
           << endl
           << "Lat (radians): " << lat << endl
           << "Lon (radians): " << lon << endl
           << "Height(m): " << height << endl
           << endl
           << "x: " << x << endl
           << "y: " << y << endl
           << "z: " << z << endl
           << endl;

      //
      // Geocentric to Geodetic (MSL EGM96 15M)
      //
      x = 3851747;
      y = 3719589;
      z = 3454013;

      double mslHeight;

      convertGeocentricToGeodeticMslEgm96(
         ccsGeocentricToGeodeticMslEgm96, 
         x, y, z, 
         lat, lon, mslHeight);

      cout << "Convert Geocentric To Geodetic MSL EGM96: " << endl
           << endl
           << "x: " << x << endl
           << "y: " << y << endl
           << "z: " << z << endl
           << endl
           << "Lat (radians): " << lat << endl
           << "Lon (radians): " << lon << endl
           << "MSL EGM96 15M Height: " << mslHeight << endl
           << endl;

      //
      // Geodetic (MSL EGM96 15M) to Geodetic (Ellipsoid Height)   
      //
      convertMslEgm96ToEllipsoidHeight(
         ccsMslEgm96ToEllipsoidHeight, 
         lat, lon, mslHeight, 
         height);

      cout << "Convert Geodetic (MSL EMG96 15M Height) To Geodetic (Ellipsoid Height)" << endl
           << endl
           << "Lat (radians): " << lat << endl
           << "Lon (radians): " << lon << endl
           << "MSL EGM96 15M Height: " << mslHeight << endl
           << endl
           << "Ellipsoid Height: " << height << endl
           << endl;

      //
      // Geodetic (Ellipsoid Height) to Geodetic (MSL EMG96 15M)
      //
      convertEllipsoidHeightToMslEgm96(
         ccsEllipsoidHeightToMslEgm96, 
         lat, lon, height, 
         mslHeight);

      cout << "Revert Geodetic (Ellipsoid Height) To Geodetic (MSL EGM96 15M) Height" << endl
           << endl
           << "Lat (radians): " << lat << endl
           << "Lon (radians): " << lon << endl
           << "Height(m): " << height << endl
           << endl
           << "MSL EGM96 15M Height: " << mslHeight << endl
           << endl;

      //
      // Geocentric to UTM
      //
      long zone;
      char hemi;
      double easting, northing;
      convertGeocentricToUtm(
         ccsGeocentricToUtm, 
         x, y, z, 
         zone, hemi, easting, northing);

      cout << "Convert Geocentric To UTM: " << endl
           << endl
           << "x: " << x << endl
           << "y: " << y << endl
           << "z: " << z << endl
           << endl
           << "Zone: " << zone << endl
           << "Hemisphere: " << hemi << endl
           << "Easting: " << easting << endl
           << "Northing: " << northing<< endl
           << endl;

      //
      // Geocentric to MGRS
      //
      string mgrsString;
      Precision::Enum precision;

      mgrsString = convertGeocentricToMgrs(
         ccsGeocentricToMgrs, 
         x, y, z, 
         precision);

      cout << "Convert Geocentric To MGRS: " << endl
           << endl
           << "x: " << x << endl
           << "y: " << y << endl
           << "z: " << z << endl
           << endl
           << "MGRS: " << mgrsString << endl
           << "Precision: " << precision << endl
           << endl;
      
      // set status value to zero to indicate successful completion
      status = 0;

   } catch(CoordinateConversionException& e) {
      // catch and report any exceptions thrown by the Coordinate
      // Conversion Service
      cerr << "ERROR: Coordinate Conversion Service exception encountered - " 
           << e.getMessage() 
           << endl;

   } catch(exception& e) {
      // catch and report any unexpected exceptions thrown
      cerr << "ERROR: Unexpected exception encountered - " << e.what() << endl;
   }

   return status;
}
