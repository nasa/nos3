#include "sofa.h"

int iauApco13(double utc1, double utc2, double dut1,
              double elong, double phi, double hm, double xp, double yp,
              double phpa, double tc, double rh, double wl,
              iauASTROM *astrom, double *eo)
/*
**  - - - - - - - - - -
**   i a u A p c o 1 3
**  - - - - - - - - - -
**
**  For a terrestrial observer, prepare star-independent astrometry
**  parameters for transformations between ICRS and observed
**  coordinates.  The caller supplies UTC, site coordinates, ambient air
**  conditions and observing wavelength, and SOFA models are used to
**  obtain the Earth ephemeris, CIP/CIO and refraction constants.
**
**  The parameters produced by this function are required in the
**  parallax, light deflection, aberration, and bias-precession-nutation
**  parts of the ICRS/CIRS transformations.
**
**  This function is part of the International Astronomical Union's
**  SOFA (Standards of Fundamental Astronomy) software collection.
**
**  Status:  support function.
**
**  Given:
**     utc1   double     UTC as a 2-part...
**     utc2   double     ...quasi Julian Date (Notes 1,2)
**     dut1   double     UT1-UTC (seconds, Note 3)
**     elong  double     longitude (radians, east +ve, Note 4)
**     phi    double     latitude (geodetic, radians, Note 4)
**     hm     double     height above ellipsoid (m, geodetic, Notes 4,6)
**     xp,yp  double     polar motion coordinates (radians, Note 5)
**     phpa   double     pressure at the observer (hPa = mB, Note 6)
**     tc     double     ambient temperature at the observer (deg C)
**     rh     double     relative humidity at the observer (range 0-1)
**     wl     double     wavelength (micrometers, Note 7)
**
**  Returned:
**     astrom iauASTROM* star-independent astrometry parameters:
**      pmt    double       PM time interval (SSB, Julian years)
**      eb     double[3]    SSB to observer (vector, au)
**      eh     double[3]    Sun to observer (unit vector)
**      em     double       distance from Sun to observer (au)
**      v      double[3]    barycentric observer velocity (vector, c)
**      bm1    double       sqrt(1-|v|^2): reciprocal of Lorenz factor
**      bpn    double[3][3] bias-precession-nutation matrix
**      along  double       longitude + s' (radians)
**      xpl    double       polar motion xp wrt local meridian (radians)
**      ypl    double       polar motion yp wrt local meridian (radians)
**      sphi   double       sine of geodetic latitude
**      cphi   double       cosine of geodetic latitude
**      diurab double       magnitude of diurnal aberration vector
**      eral   double       "local" Earth rotation angle (radians)
**      refa   double       refraction constant A (radians)
**      refb   double       refraction constant B (radians)
**     eo     double*    equation of the origins (ERA-GST)
**
**  Returned (function value):
**            int        status: +1 = dubious year (Note 2)
**                                0 = OK
**                               -1 = unacceptable date
**
**  Notes:
**
**  1)  utc1+utc2 is quasi Julian Date (see Note 2), apportioned in any
**      convenient way between the two arguments, for example where utc1
**      is the Julian Day Number and utc2 is the fraction of a day.
**
**      However, JD cannot unambiguously represent UTC during a leap
**      second unless special measures are taken.  The convention in the
**      present function is that the JD day represents UTC days whether
**      the length is 86399, 86400 or 86401 SI seconds.
**
**      Applications should use the function iauDtf2d to convert from
**      calendar date and time of day into 2-part quasi Julian Date, as
**      it implements the leap-second-ambiguity convention just
**      described.
**
**  2)  The warning status "dubious year" flags UTCs that predate the
**      introduction of the time scale or that are too far in the
**      future to be trusted.  See iauDat for further details.
**
**  3)  UT1-UTC is tabulated in IERS bulletins.  It increases by exactly
**      one second at the end of each positive UTC leap second,
**      introduced in order to keep UT1-UTC within +/- 0.9s.  n.b. This
**      practice is under review, and in the future UT1-UTC may grow
**      essentially without limit.
**
**  4)  The geographical coordinates are with respect to the WGS84
**      reference ellipsoid.  TAKE CARE WITH THE LONGITUDE SIGN:  the
**      longitude required by the present function is east-positive
**      (i.e. right-handed), in accordance with geographical convention.
**
**  5)  The polar motion xp,yp can be obtained from IERS bulletins.  The
**      values are the coordinates (in radians) of the Celestial
**      Intermediate Pole with respect to the International Terrestrial
**      Reference System (see IERS Conventions 2003), measured along the
**      meridians 0 and 90 deg west respectively.  For many
**      applications, xp and yp can be set to zero.
**
**      Internally, the polar motion is stored in a form rotated onto
**      the local meridian.
**
**  6)  If hm, the height above the ellipsoid of the observing station
**      in meters, is not known but phpa, the pressure in hPa (=mB), is
**      available, an adequate estimate of hm can be obtained from the
**      expression
**
**            hm = -29.3 * tsl * log ( phpa / 1013.25 );
**
**      where tsl is the approximate sea-level air temperature in K
**      (See Astrophysical Quantities, C.W.Allen, 3rd edition, section
**      52).  Similarly, if the pressure phpa is not known, it can be
**      estimated from the height of the observing station, hm, as
**      follows:
**
**            phpa = 1013.25 * exp ( -hm / ( 29.3 * tsl ) );
**
**      Note, however, that the refraction is nearly proportional to
**      the pressure and that an accurate phpa value is important for
**      precise work.
**
**  7)  The argument wl specifies the observing wavelength in
**      micrometers.  The transition from optical to radio is assumed to
**      occur at 100 micrometers (about 3000 GHz).
**
**  8)  It is advisable to take great care with units, as even unlikely
**      values of the input parameters are accepted and processed in
**      accordance with the models used.
**
**  9)  In cases where the caller wishes to supply his own Earth
**      ephemeris, Earth rotation information and refraction constants,
**      the function iauApco can be used instead of the present function.
**
**  10) This is one of several functions that inserts into the astrom
**      structure star-independent parameters needed for the chain of
**      astrometric transformations ICRS <-> GCRS <-> CIRS <-> observed.
**
**      The various functions support different classes of observer and
**      portions of the transformation chain:
**
**          functions         observer        transformation
**
**       iauApcg iauApcg13    geocentric      ICRS <-> GCRS
**       iauApci iauApci13    terrestrial     ICRS <-> CIRS
**       iauApco iauApco13    terrestrial     ICRS <-> observed
**       iauApcs iauApcs13    space           ICRS <-> GCRS
**       iauAper iauAper13    terrestrial     update Earth rotation
**       iauApio iauApio13    terrestrial     CIRS <-> observed
**
**      Those with names ending in "13" use contemporary SOFA models to
**      compute the various ephemerides.  The others accept ephemerides
**      supplied by the caller.
**
**      The transformation from ICRS to GCRS covers space motion,
**      parallax, light deflection, and aberration.  From GCRS to CIRS
**      comprises frame bias and precession-nutation.  From CIRS to
**      observed takes account of Earth rotation, polar motion, diurnal
**      aberration and parallax (unless subsumed into the ICRS <-> GCRS
**      transformation), and atmospheric refraction.
**
**  11) The context structure astrom produced by this function is used
**      by iauAtioq, iauAtoiq, iauAtciq* and iauAticq*.
**
**  Called:
**     iauUtctai    UTC to TAI
**     iauTaitt     TAI to TT
**     iauUtcut1    UTC to UT1
**     iauEpv00     Earth position and velocity
**     iauPnm06a    classical NPB matrix, IAU 2006/2000A
**     iauBpn2xy    extract CIP X,Y coordinates from NPB matrix
**     iauS06       the CIO locator s, given X,Y, IAU 2006
**     iauEra00     Earth rotation angle, IAU 2000
**     iauSp00      the TIO locator s', IERS 2000
**     iauRefco     refraction constants for given ambient conditions
**     iauApco      astrometry parameters, ICRS-observed
**     iauEors      equation of the origins, given NPB matrix and s
**
**  This revision:   2013 December 5
**
**  SOFA release 2015-02-09
**
**  Copyright (C) 2015 IAU SOFA Board.  See notes at end.
*/
{
   int j;
   double tai1, tai2, tt1, tt2, ut11, ut12, ehpv[2][3], ebpv[2][3],
          r[3][3], x, y, s, theta, sp, refa, refb;

/* UTC to other time scales. */
   j = iauUtctai(utc1, utc2, &tai1, &tai2);
   if ( j < 0 ) return -1;
   j = iauTaitt(tai1, tai2, &tt1, &tt2);
   j = iauUtcut1(utc1, utc2, dut1, &ut11, &ut12);
   if ( j < 0 ) return -1;

/* Earth barycentric & heliocentric position/velocity (au, au/d). */
   (void) iauEpv00(tt1, tt2, ehpv, ebpv);

/* Form the equinox based BPN matrix, IAU 2006/2000A. */
   iauPnm06a(tt1, tt2, r);

/* Extract CIP X,Y. */
   iauBpn2xy(r, &x, &y);

/* Obtain CIO locator s. */
   s = iauS06(tt1, tt2, x, y);

/* Earth rotation angle. */
   theta = iauEra00(ut11, ut12);

/* TIO locator s'. */
   sp = iauSp00(tt1, tt2);

/* Refraction constants A and B. */
   iauRefco(phpa, tc, rh, wl, &refa, &refb);

/* Compute the star-independent astrometry parameters. */
   iauApco(tt1, tt2, ebpv, ehpv[0], x, y, s, theta,
           elong, phi, hm, xp, yp, sp, refa, refb, astrom);

/* Equation of the origins. */
   *eo = iauEors(r, s);

/* Return any warning status. */
   return j;

/* Finished. */

/*----------------------------------------------------------------------
**
**  Copyright (C) 2015
**  Standards Of Fundamental Astronomy Board
**  of the International Astronomical Union.
**
**  =====================
**  SOFA Software License
**  =====================
**
**  NOTICE TO USER:
**
**  BY USING THIS SOFTWARE YOU ACCEPT THE FOLLOWING SIX TERMS AND
**  CONDITIONS WHICH APPLY TO ITS USE.
**
**  1. The Software is owned by the IAU SOFA Board ("SOFA").
**
**  2. Permission is granted to anyone to use the SOFA software for any
**     purpose, including commercial applications, free of charge and
**     without payment of royalties, subject to the conditions and
**     restrictions listed below.
**
**  3. You (the user) may copy and distribute SOFA source code to others,
**     and use and adapt its code and algorithms in your own software,
**     on a world-wide, royalty-free basis.  That portion of your
**     distribution that does not consist of intact and unchanged copies
**     of SOFA source code files is a "derived work" that must comply
**     with the following requirements:
**
**     a) Your work shall be marked or carry a statement that it
**        (i) uses routines and computations derived by you from
**        software provided by SOFA under license to you; and
**        (ii) does not itself constitute software provided by and/or
**        endorsed by SOFA.
**
**     b) The source code of your derived work must contain descriptions
**        of how the derived work is based upon, contains and/or differs
**        from the original SOFA software.
**
**     c) The names of all routines in your derived work shall not
**        include the prefix "iau" or "sofa" or trivial modifications
**        thereof such as changes of case.
**
**     d) The origin of the SOFA components of your derived work must
**        not be misrepresented;  you must not claim that you wrote the
**        original software, nor file a patent application for SOFA
**        software or algorithms embedded in the SOFA software.
**
**     e) These requirements must be reproduced intact in any source
**        distribution and shall apply to anyone to whom you have
**        granted a further right to modify the source code of your
**        derived work.
**
**     Note that, as originally distributed, the SOFA software is
**     intended to be a definitive implementation of the IAU standards,
**     and consequently third-party modifications are discouraged.  All
**     variations, no matter how minor, must be explicitly marked as
**     such, as explained above.
**
**  4. You shall not cause the SOFA software to be brought into
**     disrepute, either by misuse, or use for inappropriate tasks, or
**     by inappropriate modification.
**
**  5. The SOFA software is provided "as is" and SOFA makes no warranty
**     as to its use or performance.   SOFA does not and cannot warrant
**     the performance or results which the user may obtain by using the
**     SOFA software.  SOFA makes no warranties, express or implied, as
**     to non-infringement of third party rights, merchantability, or
**     fitness for any particular purpose.  In no event will SOFA be
**     liable to the user for any consequential, incidental, or special
**     damages, including any lost profits or lost savings, even if a
**     SOFA representative has been advised of such damages, or for any
**     claim by any third party.
**
**  6. The provision of any version of the SOFA software under the terms
**     and conditions specified herein does not imply that future
**     versions will also be made available under the same terms and
**     conditions.
*
**  In any published work or commercial product which uses the SOFA
**  software directly, acknowledgement (see www.iausofa.org) is
**  appreciated.
**
**  Correspondence concerning SOFA software should be addressed as
**  follows:
**
**      By email:  sofa@ukho.gov.uk
**      By post:   IAU SOFA Center
**                 HM Nautical Almanac Office
**                 UK Hydrographic Office
**                 Admiralty Way, Taunton
**                 Somerset, TA1 2DN
**                 United Kingdom
**
**--------------------------------------------------------------------*/

}
