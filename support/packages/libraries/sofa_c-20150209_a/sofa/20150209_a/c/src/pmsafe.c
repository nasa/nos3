#include "sofa.h"

int iauPmsafe(double ra1, double dec1, double pmr1, double pmd1,
              double px1, double rv1,
              double ep1a, double ep1b, double ep2a, double ep2b,
              double *ra2, double *dec2, double *pmr2, double *pmd2,
              double *px2, double *rv2)
/*
**  - - - - - - - - - -
**   i a u P m s a f e
**  - - - - - - - - - -
**
**  Star proper motion:  update star catalog data for space motion, with
**  special handling to handle the zero parallax case.
**
**  This function is part of the International Astronomical Union's
**  SOFA (Standards of Fundamental Astronomy) software collection.
**
**  Status:  support function.
**
**  Given:
**     ra1    double      right ascension (radians), before
**     dec1   double      declination (radians), before
**     pmr1   double      RA proper motion (radians/year), before
**     pmd1   double      Dec proper motion (radians/year), before
**     px1    double      parallax (arcseconds), before
**     rv1    double      radial velocity (km/s, +ve = receding), before
**     ep1a   double      "before" epoch, part A (Note 1)
**     ep1b   double      "before" epoch, part B (Note 1)
**     ep2a   double      "after" epoch, part A (Note 1)
**     ep2b   double      "after" epoch, part B (Note 1)
**
**  Returned:
**     ra2    double      right ascension (radians), after
**     dec2   double      declination (radians), after
**     pmr2   double      RA proper motion (radians/year), after
**     pmd2   double      Dec proper motion (radians/year), after
**     px2    double      parallax (arcseconds), after
**     rv2    double      radial velocity (km/s, +ve = receding), after
**
**  Returned (function value):
**            int         status:
**                         -1 = system error (should not occur)
**                          0 = no warnings or errors
**                          1 = distance overridden (Note 6)
**                          2 = excessive velocity (Note 7)
**                          4 = solution didn't converge (Note 8)
**                       else = binary logical OR of the above warnings
**
**  Notes:
**
**  1) The starting and ending TDB epochs ep1a+ep1b and ep2a+ep2b are
**     Julian Dates, apportioned in any convenient way between the two
**     parts (A and B).  For example, JD(TDB)=2450123.7 could be
**     expressed in any of these ways, among others:
**
**            epNa            epNb
**
**         2450123.7           0.0       (JD method)
**         2451545.0       -1421.3       (J2000 method)
**         2400000.5       50123.2       (MJD method)
**         2450123.5           0.2       (date & time method)
**
**     The JD method is the most natural and convenient to use in cases
**     where the loss of several decimal digits of resolution is
**     acceptable.  The J2000 method is best matched to the way the
**     argument is handled internally and will deliver the optimum
**     resolution.  The MJD method and the date & time methods are both
**     good compromises between resolution and convenience.
**
**  2) In accordance with normal star-catalog conventions, the object's
**     right ascension and declination are freed from the effects of
**     secular aberration.  The frame, which is aligned to the catalog
**     equator and equinox, is Lorentzian and centered on the SSB.
**
**     The proper motions are the rate of change of the right ascension
**     and declination at the catalog epoch and are in radians per TDB
**     Julian year.
**
**     The parallax and radial velocity are in the same frame.
**
**  3) Care is needed with units.  The star coordinates are in radians
**     and the proper motions in radians per Julian year, but the
**     parallax is in arcseconds.
**
**  4) The RA proper motion is in terms of coordinate angle, not true
**     angle.  If the catalog uses arcseconds for both RA and Dec proper
**     motions, the RA proper motion will need to be divided by cos(Dec)
**     before use.
**
**  5) Straight-line motion at constant speed, in the inertial frame, is
**     assumed.
**
**  6) An extremely small (or zero or negative) parallax is overridden
**     to ensure that the object is at a finite but very large distance,
**     but not so large that the proper motion is equivalent to a large
**     but safe speed (about 0.1c using the chosen constant).  A warning
**     status of 1 is added to the status if this action has been taken.
**
**  7) If the space velocity is a significant fraction of c (see the
**     constant VMAX in the function iauStarpv), it is arbitrarily set
**     to zero.  When this action occurs, 2 is added to the status.
**
**  8) The relativistic adjustment carried out in the iauStarpv function
**     involves an iterative calculation.  If the process fails to
**     converge within a set number of iterations, 4 is added to the
**     status.
**
**  Called:
**     iauSeps      angle between two points
**     iauStarpm    update star catalog data for space motion
**
**  This revision:   2014 July 1
**
**  SOFA release 2015-02-09
**
**  Copyright (C) 2015 IAU SOFA Board.  See notes at end.
*/
{

/* Minimum allowed parallax (arcsec) */
   const double PXMIN = 5e-7;

/* Factor giving maximum allowed transverse speed of about 1% c */
   const double F = 326.0;

   int jpx, j;
   double pm, px1a;

/* Proper motion in one year (radians). */
   pm = iauSeps(ra1, dec1, ra1+pmr1, dec1+pmd1);

/* Override the parallax to reduce the chances of a warning status. */
   jpx = 0;
   px1a = px1;
   pm *= F;
   if (px1a < pm) {jpx = 1; px1a = pm;}
   if (px1a < PXMIN) {jpx = 1; px1a = PXMIN;}

/* Carry out the transformation using the modified parallax. */
   j = iauStarpm(ra1, dec1, pmr1, pmd1, px1a, rv1,
                 ep1a, ep1b, ep2a, ep2b,
                 ra2, dec2, pmr2, pmd2, px2, rv2);

/* Revise and return the status. */
   if ( !(j%2) ) j += jpx;
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
