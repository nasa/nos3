#include "sofa.h"

void iauP06e(double date1, double date2,
             double *eps0, double *psia, double *oma, double *bpa,
             double *bqa, double *pia, double *bpia,
             double *epsa, double *chia, double *za, double *zetaa,
             double *thetaa, double *pa,
             double *gam, double *phi, double *psi)
/*
**  - - - - - - - -
**   i a u P 0 6 e
**  - - - - - - - -
**
**  Precession angles, IAU 2006, equinox based.
**
**  This function is part of the International Astronomical Union's
**  SOFA (Standards Of Fundamental Astronomy) software collection.
**
**  Status:  canonical models.
**
**  Given:
**     date1,date2   double   TT as a 2-part Julian Date (Note 1)
**
**  Returned (see Note 2):
**     eps0          double   epsilon_0
**     psia          double   psi_A
**     oma           double   omega_A
**     bpa           double   P_A
**     bqa           double   Q_A
**     pia           double   pi_A
**     bpia          double   Pi_A
**     epsa          double   obliquity epsilon_A
**     chia          double   chi_A
**     za            double   z_A
**     zetaa         double   zeta_A
**     thetaa        double   theta_A
**     pa            double   p_A
**     gam           double   F-W angle gamma_J2000
**     phi           double   F-W angle phi_J2000
**     psi           double   F-W angle psi_J2000
**
**  Notes:
**
**  1) The TT date date1+date2 is a Julian Date, apportioned in any
**     convenient way between the two arguments.  For example,
**     JD(TT)=2450123.7 could be expressed in any of these ways,
**     among others:
**
**            date1          date2
**
**         2450123.7           0.0       (JD method)
**         2451545.0       -1421.3       (J2000 method)
**         2400000.5       50123.2       (MJD method)
**         2450123.5           0.2       (date & time method)
**
**     The JD method is the most natural and convenient to use in
**     cases where the loss of several decimal digits of resolution
**     is acceptable.  The J2000 method is best matched to the way
**     the argument is handled internally and will deliver the
**     optimum resolution.  The MJD method and the date & time methods
**     are both good compromises between resolution and convenience.
**
**  2) This function returns the set of equinox based angles for the
**     Capitaine et al. "P03" precession theory, adopted by the IAU in
**     2006.  The angles are set out in Table 1 of Hilton et al. (2006):
**
**     eps0   epsilon_0   obliquity at J2000.0
**     psia   psi_A       luni-solar precession
**     oma    omega_A     inclination of equator wrt J2000.0 ecliptic
**     bpa    P_A         ecliptic pole x, J2000.0 ecliptic triad
**     bqa    Q_A         ecliptic pole -y, J2000.0 ecliptic triad
**     pia    pi_A        angle between moving and J2000.0 ecliptics
**     bpia   Pi_A        longitude of ascending node of the ecliptic
**     epsa   epsilon_A   obliquity of the ecliptic
**     chia   chi_A       planetary precession
**     za     z_A         equatorial precession: -3rd 323 Euler angle
**     zetaa  zeta_A      equatorial precession: -1st 323 Euler angle
**     thetaa theta_A     equatorial precession: 2nd 323 Euler angle
**     pa     p_A         general precession
**     gam    gamma_J2000 J2000.0 RA difference of ecliptic poles
**     phi    phi_J2000   J2000.0 codeclination of ecliptic pole
**     psi    psi_J2000   longitude difference of equator poles, J2000.0
**
**     The returned values are all radians.
**
**  3) Hilton et al. (2006) Table 1 also contains angles that depend on
**     models distinct from the P03 precession theory itself, namely the
**     IAU 2000A frame bias and nutation.  The quoted polynomials are
**     used in other SOFA functions:
**
**     . iauXy06  contains the polynomial parts of the X and Y series.
**
**     . iauS06  contains the polynomial part of the s+XY/2 series.
**
**     . iauPfw06  implements the series for the Fukushima-Williams
**       angles that are with respect to the GCRS pole (i.e. the variants
**       that include frame bias).
**
**  4) The IAU resolution stipulated that the choice of parameterization
**     was left to the user, and so an IAU compliant precession
**     implementation can be constructed using various combinations of
**     the angles returned by the present function.
**
**  5) The parameterization used by SOFA is the version of the Fukushima-
**     Williams angles that refers directly to the GCRS pole.  These
**     angles may be calculated by calling the function iauPfw06.  SOFA
**     also supports the direct computation of the CIP GCRS X,Y by
**     series, available by calling iauXy06.
**
**  6) The agreement between the different parameterizations is at the
**     1 microarcsecond level in the present era.
**
**  7) When constructing a precession formulation that refers to the GCRS
**     pole rather than the dynamical pole, it may (depending on the
**     choice of angles) be necessary to introduce the frame bias
**     explicitly.
**
**  8) It is permissible to re-use the same variable in the returned
**     arguments.  The quantities are stored in the stated order.
**
**  Reference:
**
**     Hilton, J. et al., 2006, Celest.Mech.Dyn.Astron. 94, 351
**
**  Called:
**     iauObl06     mean obliquity, IAU 2006
**
**  This revision:  2013 June 18
**
**  SOFA release 2015-02-09
**
**  Copyright (C) 2015 IAU SOFA Board.  See notes at end.
*/
{
   double t;

/* Interval between fundamental date J2000.0 and given date (JC). */
   t = ((date1 - DJ00) + date2) / DJC;

/* Obliquity at J2000.0. */

   *eps0 = 84381.406 * DAS2R;

/* Luni-solar precession. */

   *psia = ( 5038.481507     +
           (   -1.0790069    +
           (   -0.00114045   +
           (    0.000132851  +
           (   -0.0000000951 )
           * t) * t) * t) * t) * t * DAS2R;

/* Inclination of mean equator with respect to the J2000.0 ecliptic. */

   *oma = *eps0 + ( -0.025754     +
                  (  0.0512623    +
                  ( -0.00772503   +
                  ( -0.000000467  +
                  (  0.0000003337 )
                  * t) * t) * t) * t) * t * DAS2R;

/* Ecliptic pole x, J2000.0 ecliptic triad. */

   *bpa = (  4.199094     +
          (  0.1939873    +
          ( -0.00022466   +
          ( -0.000000912  +
          (  0.0000000120 )
          * t) * t) * t) * t) * t * DAS2R;

/* Ecliptic pole -y, J2000.0 ecliptic triad. */

   *bqa = ( -46.811015     +
          (   0.0510283    +
          (   0.00052413   +
          (  -0.000000646  +
          (  -0.0000000172 )
          * t) * t) * t) * t) * t * DAS2R;

/* Angle between moving and J2000.0 ecliptics. */

   *pia = ( 46.998973     +
          ( -0.0334926    +
          ( -0.00012559   +
          (  0.000000113  +
          ( -0.0000000022 )
          * t) * t) * t) * t) * t * DAS2R;

/* Longitude of ascending node of the moving ecliptic. */

   *bpia = ( 629546.7936      +
           (   -867.95758     +
           (      0.157992    +
           (     -0.0005371   +
           (     -0.00004797  +
           (      0.000000072 )
           * t) * t) * t) * t) * t) * DAS2R;

/* Mean obliquity of the ecliptic. */

   *epsa = iauObl06(date1, date2);

/* Planetary precession. */

   *chia = ( 10.556403     +
           ( -2.3814292    +
           ( -0.00121197   +
           (  0.000170663  +
           ( -0.0000000560 )
           * t) * t) * t) * t) * t * DAS2R;

/* Equatorial precession: minus the third of the 323 Euler angles. */

   *za = (   -2.650545     +
         ( 2306.077181     +
         (    1.0927348    +
         (    0.01826837   +
         (   -0.000028596  +
         (   -0.0000002904 )
         * t) * t) * t) * t) * t) * DAS2R;

/* Equatorial precession: minus the first of the 323 Euler angles. */

   *zetaa = (    2.650545     +
            ( 2306.083227     +
            (    0.2988499    +
            (    0.01801828   +
            (   -0.000005971  +
            (   -0.0000003173 )
            * t) * t) * t) * t) * t) * DAS2R;

/* Equatorial precession: second of the 323 Euler angles. */

   *thetaa = ( 2004.191903     +
             (   -0.4294934    +
             (   -0.04182264   +
             (   -0.000007089  +
             (   -0.0000001274 )
             * t) * t) * t) * t) * t * DAS2R;

/* General precession. */

   *pa = ( 5028.796195     +
         (    1.1054348    +
         (    0.00007964   +
         (   -0.000023857  +
         (    0.0000000383 )
         * t) * t) * t) * t) * t * DAS2R;

/* Fukushima-Williams angles for precession. */

   *gam = ( 10.556403     +
          (  0.4932044    +
          ( -0.00031238   +
          ( -0.000002788  +
          (  0.0000000260 )
          * t) * t) * t) * t) * t * DAS2R;

   *phi = *eps0 + ( -46.811015     +
                  (   0.0511269    +
                  (   0.00053289   +
                  (  -0.000000440  +
                  (  -0.0000000176 )
                  * t) * t) * t) * t) * t * DAS2R;

   *psi = ( 5038.481507     +
          (    1.5584176    +
          (   -0.00018522   +
          (   -0.000026452  +
          (   -0.0000000148 )
          * t) * t) * t) * t) * t * DAS2R;

   return;

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
