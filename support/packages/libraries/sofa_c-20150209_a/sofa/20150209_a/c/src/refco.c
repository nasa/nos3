#include "sofa.h"

void iauRefco(double phpa, double tc, double rh, double wl,
              double *refa, double *refb)
/*
**  - - - - - - - - -
**   i a u R e f c o
**  - - - - - - - - -
**
**  Determine the constants A and B in the atmospheric refraction model
**  dZ = A tan Z + B tan^3 Z.
**
**  Z is the "observed" zenith distance (i.e. affected by refraction)
**  and dZ is what to add to Z to give the "topocentric" (i.e. in vacuo)
**  zenith distance.
**
**  This function is part of the International Astronomical Union's
**  SOFA (Standards of Fundamental Astronomy) software collection.
**
**  Status:  support function.
**
**  Given:
**    phpa   double    pressure at the observer (hPa = millibar)
**    tc     double    ambient temperature at the observer (deg C)
**    rh     double    relative humidity at the observer (range 0-1)
**    wl     double    wavelength (micrometers)
**
**  Returned:
**    refa   double*   tan Z coefficient (radians)
**    refb   double*   tan^3 Z coefficient (radians)
**
**  Notes:
**
**  1) The model balances speed and accuracy to give good results in
**     applications where performance at low altitudes is not paramount.
**     Performance is maintained across a range of conditions, and
**     applies to both optical/IR and radio.
**
**  2) The model omits the effects of (i) height above sea level (apart
**     from the reduced pressure itself), (ii) latitude (i.e. the
**     flattening of the Earth), (iii) variations in tropospheric lapse
**     rate and (iv) dispersive effects in the radio.
**
**     The model was tested using the following range of conditions:
**
**       lapse rates 0.0055, 0.0065, 0.0075 deg/meter
**       latitudes 0, 25, 50, 75 degrees
**       heights 0, 2500, 5000 meters ASL
**       pressures mean for height -10% to +5% in steps of 5%
**       temperatures -10 deg to +20 deg with respect to 280 deg at SL
**       relative humidity 0, 0.5, 1
**       wavelengths 0.4, 0.6, ... 2 micron, + radio
**       zenith distances 15, 45, 75 degrees
**
**     The accuracy with respect to raytracing through a model
**     atmosphere was as follows:
**
**                            worst         RMS
**
**       optical/IR           62 mas       8 mas
**       radio               319 mas      49 mas
**
**     For this particular set of conditions:
**
**       lapse rate 0.0065 K/meter
**       latitude 50 degrees
**       sea level
**       pressure 1005 mb
**       temperature 280.15 K
**       humidity 80%
**       wavelength 5740 Angstroms
**
**     the results were as follows:
**
**       ZD       raytrace     iauRefco   Saastamoinen
**
**       10         10.27        10.27        10.27
**       20         21.19        21.20        21.19
**       30         33.61        33.61        33.60
**       40         48.82        48.83        48.81
**       45         58.16        58.18        58.16
**       50         69.28        69.30        69.27
**       55         82.97        82.99        82.95
**       60        100.51       100.54       100.50
**       65        124.23       124.26       124.20
**       70        158.63       158.68       158.61
**       72        177.32       177.37       177.31
**       74        200.35       200.38       200.32
**       76        229.45       229.43       229.42
**       78        267.44       267.29       267.41
**       80        319.13       318.55       319.10
**
**      deg        arcsec       arcsec       arcsec
**
**     The values for Saastamoinen's formula (which includes terms
**     up to tan^5) are taken from Hohenkerk and Sinclair (1985).
**
**  3) A wl value in the range 0-100 selects the optical/IR case and is
**     wavelength in micrometers.  Any value outside this range selects
**     the radio case.
**
**  4) Outlandish input parameters are silently limited to
**     mathematically safe values.  Zero pressure is permissible, and
**     causes zeroes to be returned.
**
**  5) The algorithm draws on several sources, as follows:
**
**     a) The formula for the saturation vapour pressure of water as
**        a function of temperature and temperature is taken from
**        Equations (A4.5-A4.7) of Gill (1982).
**
**     b) The formula for the water vapour pressure, given the
**        saturation pressure and the relative humidity, is from
**        Crane (1976), Equation (2.5.5).
**
**     c) The refractivity of air is a function of temperature,
**        total pressure, water-vapour pressure and, in the case
**        of optical/IR, wavelength.  The formulae for the two cases are
**        developed from Hohenkerk & Sinclair (1985) and Rueger (2002).
**
**     d) The formula for beta, the ratio of the scale height of the
**        atmosphere to the geocentric distance of the observer, is
**        an adaption of Equation (9) from Stone (1996).  The
**        adaptations, arrived at empirically, consist of (i) a small
**        adjustment to the coefficient and (ii) a humidity term for the
**        radio case only.
**
**     e) The formulae for the refraction constants as a function of
**        n-1 and beta are from Green (1987), Equation (4.31).
**
**  References:
**
**     Crane, R.K., Meeks, M.L. (ed), "Refraction Effects in the Neutral
**     Atmosphere", Methods of Experimental Physics: Astrophysics 12B,
**     Academic Press, 1976.
**
**     Gill, Adrian E., "Atmosphere-Ocean Dynamics", Academic Press,
**     1982.
**
**     Green, R.M., "Spherical Astronomy", Cambridge University Press,
**     1987.
**
**     Hohenkerk, C.Y., & Sinclair, A.T., NAO Technical Note No. 63,
**     1985.
**
**     Rueger, J.M., "Refractive Index Formulae for Electronic Distance
**     Measurement with Radio and Millimetre Waves", in Unisurv Report
**     S-68, School of Surveying and Spatial Information Systems,
**     University of New South Wales, Sydney, Australia, 2002.
**
**     Stone, Ronald C., P.A.S.P. 108, 1051-1058, 1996.
**
**  This revision:   2013 October 9
**
**  SOFA release 2015-02-09
**
**  Copyright (C) 2015 IAU SOFA Board.  See notes at end.
*/
{
   int optic;
   double p, t, r, w, ps, pw, tk, wlsq, gamma, beta;

/* Decide whether optical/IR or radio case:  switch at 100 microns. */
   optic = ( wl <= 100.0 );

/* Restrict parameters to safe values. */
   t = gmax ( tc, -150.0 );
   t = gmin ( t, 200.0 );
   p = gmax ( phpa, 0.0 );
   p = gmin ( p, 10000.0 );
   r = gmax ( rh, 0.0 );
   r = gmin ( r, 1.0 );
   w = gmax ( wl, 0.1 );
   w = gmin ( w, 1e6 );

/* Water vapour pressure at the observer. */
   if ( p > 0.0 ) {
      ps = pow ( 10.0, ( 0.7859 + 0.03477*t ) /
                          ( 1.0 + 0.00412*t ) ) *
                 ( 1.0 + p * ( 4.5e-6 + 6e-10*t*t )  );
      pw = r * ps / ( 1.0 - (1.0-r)*ps/p );
   } else {
      pw = 0.0;
   }

/* Refractive index minus 1 at the observer. */
   tk = t + 273.15;
   if ( optic ) {
      wlsq = w * w;
      gamma = ( ( 77.53484e-6 +
                 ( 4.39108e-7 + 3.666e-9/wlsq ) / wlsq ) * p
                    - 11.2684e-6*pw ) / tk;
   } else {
      gamma = ( 77.6890e-6*p - ( 6.3938e-6 - 0.375463/tk ) * pw ) / tk;
   }

/* Formula for beta from Stone, with empirical adjustments. */
   beta = 4.4474e-6 * tk;
   if ( ! optic ) beta -= 0.0074 * pw * beta;

/* Refraction constants from Green. */
   *refa = gamma * ( 1.0 - beta );
   *refb = - gamma * ( beta - gamma / 2.0 );

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
