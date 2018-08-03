#include <stdio.h>
#include <sofa.h>

static int verbose = 0;

/*
**  - - - - - - - - -
**   t _ s o f a _ c
**  - - - - - - - - -
**
**  Validate the SOFA C functions.
**
**  Each SOFA function is at least called and a usually quite basic test
**  is performed.  Successful completion is signalled by a confirming
**  message.  Failure of a given function or group of functions results
**  in error messages.
**
**  All messages go to stdout.
**
**  This revision:  2015 January 30
**
**  SOFA release 2015-02-09
**
**  Copyright (C) 2015 IAU SOFA Board.  See notes at end.
*/

static void viv(int ival, int ivalok,
                const char *func, const char *test, int *status)
/*
**  - - - -
**   v i v
**  - - - -
**
**  Validate an integer result.
**
**  Internal function used by t_sofa_c program.
**
**  Given:
**     ival     int          value computed by function under test
**     ivalok   int          correct value
**     func     char[]       name of function under test
**     test     char[]       name of individual test
**
**  Given and returned:
**     status   int          set to TRUE if test fails
**
**  This revision:  2013 August 7
*/
{
   if (ival != ivalok) {
      *status = 1;
      printf("%s failed: %s want %d got %d\n",
             func, test, ivalok, ival);
   } else if (verbose) {
      printf("%s passed: %s want %d got %d\n",
                    func, test, ivalok, ival);
   }

}

static void vvd(double val, double valok, double dval,
                const char *func, const char *test, int *status)
/*
**  - - - -
**   v v d
**  - - - -
**
**  Validate a double result.
**
**  Internal function used by t_sofa_c program.
**
**  Given:
**     val      double       value computed by function under test
**     valok    double       expected value
**     dval     double       maximum allowable error
**     func     char[]       name of function under test
**     test     char[]       name of individual test
**
**  Given and returned:
**     status   int          set to TRUE if test fails
**
**  This revision:  2013 August 7
*/
{
   double a, f;   /* absolute and fractional error */


   a = val - valok;
   if (fabs(a) > dval) {
      f = fabs(valok / a);
      *status = 1;
      printf("%s failed: %s want %.20g got %.20g (1/%.3g)\n",
             func, test, valok, val, f);
   } else if (verbose) {
      printf("%s passed: %s want %.20g got %.20g\n",
             func, test, valok, val);
   }

}

static void t_a2af(int *status)
/*
**  - - - - - - -
**   t _ a 2 a f
**  - - - - - - -
**
**  Test iauA2af function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauA2af, viv
**
**  This revision:  2013 August 7
*/
{
   int idmsf[4];
   char s;


   iauA2af(4, 2.345, &s, idmsf);

   viv(s, '+', "iauA2af", "s", status);

   viv(idmsf[0],  134, "iauA2af", "0", status);
   viv(idmsf[1],   21, "iauA2af", "1", status);
   viv(idmsf[2],   30, "iauA2af", "2", status);
   viv(idmsf[3], 9706, "iauA2af", "3", status);

}

static void t_a2tf(int *status)
/*
**  - - - - - - -
**   t _ a 2 t f
**  - - - - - - -
**
**  Test iauA2tf function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauA2tf, viv
**
**  This revision:  2013 August 7
*/
{
   int ihmsf[4];
   char s;


   iauA2tf(4, -3.01234, &s, ihmsf);

   viv((int)s, '-', "iauA2tf", "s", status);

   viv(ihmsf[0],   11, "iauA2tf", "0", status);
   viv(ihmsf[1],   30, "iauA2tf", "1", status);
   viv(ihmsf[2],   22, "iauA2tf", "2", status);
   viv(ihmsf[3], 6484, "iauA2tf", "3", status);

}

static void t_ab(int *status)
/*
**  - - - - -
**   t _ a b
**  - - - - -
**
**  Test iauAb function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauAb, vvd
**
**  This revision:  2013 October 1
*/
{
   double pnat[3], v[3], s, bm1, ppr[3];


   pnat[0] = -0.76321968546737951;
   pnat[1] = -0.60869453983060384;
   pnat[2] = -0.21676408580639883;
   v[0] =  2.1044018893653786e-5;
   v[1] = -8.9108923304429319e-5;
   v[2] = -3.8633714797716569e-5;
   s = 0.99980921395708788;
   bm1 = 0.99999999506209258;

   iauAb(pnat, v, s, bm1, ppr);

   vvd(ppr[0], -0.7631631094219556269, 1e-12, "iauAb", "1", status);
   vvd(ppr[1], -0.6087553082505590832, 1e-12, "iauAb", "2", status);
   vvd(ppr[2], -0.2167926269368471279, 1e-12, "iauAb", "3", status);

}

static void t_af2a(int *status)
/*
**  - - - - - - -
**   t _ a f 2 a
**  - - - - - - -
**
**  Test iauAf2a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauAf2a, viv
**
**  This revision:  2013 August 7
*/
{
   double a;
   int j;


   j = iauAf2a('-', 45, 13, 27.2, &a);

   vvd(a, -0.7893115794313644842, 1e-12, "iauAf2a", "a", status);
   viv(j, 0, "iauAf2a", "j", status);

}

static void t_anp(int *status)
/*
**  - - - - - -
**   t _ a n p
**  - - - - - -
**
**  Test iauAnp function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauAnp, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauAnp(-0.1), 6.183185307179586477, 1e-12, "iauAnp", "", status);
}

static void t_anpm(int *status)
/*
**  - - - - - - -
**   t _ a n p m
**  - - - - - - -
**
**  Test iauAnpm function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauAnpm, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauAnpm(-4.0), 2.283185307179586477, 1e-12, "iauAnpm", "", status);
}

static void t_apcg(int *status)
/*
**  - - - - - - -
**   t _ a p c g
**  - - - - - - -
**
**  Test iauApcg function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApcg, vvd
**
**  This revision:  2013 October 3
*/
{
   double date1, date2, ebpv[2][3], ehp[3];
   iauASTROM astrom;


   date1 = 2456165.5;
   date2 = 0.401182685;
   ebpv[0][0] =  0.901310875;
   ebpv[0][1] = -0.417402664;
   ebpv[0][2] = -0.180982288;
   ebpv[1][0] =  0.00742727954;
   ebpv[1][1] =  0.0140507459;
   ebpv[1][2] =  0.00609045792;
   ehp[0] =  0.903358544;
   ehp[1] = -0.415395237;
   ehp[2] = -0.180084014;

   iauApcg(date1, date2, ebpv, ehp, &astrom);

   vvd(astrom.pmt, 12.65133794027378508, 1e-11,
                   "iauApcg", "pmt", status);
   vvd(astrom.eb[0], 0.901310875, 1e-12,
                     "iauApcg", "eb(1)", status);
   vvd(astrom.eb[1], -0.417402664, 1e-12,
                     "iauApcg", "eb(2)", status);
   vvd(astrom.eb[2], -0.180982288, 1e-12,
                     "iauApcg", "eb(3)", status);
   vvd(astrom.eh[0], 0.8940025429324143045, 1e-12,
                     "iauApcg", "eh(1)", status);
   vvd(astrom.eh[1], -0.4110930268679817955, 1e-12,
                     "iauApcg", "eh(2)", status);
   vvd(astrom.eh[2], -0.1782189004872870264, 1e-12,
                     "iauApcg", "eh(3)", status);
   vvd(astrom.em, 1.010465295811013146, 1e-12,
                  "iauApcg", "em", status);
   vvd(astrom.v[0], 0.4289638897813379954e-4, 1e-16,
                    "iauApcg", "v(1_", status);
   vvd(astrom.v[1], 0.8115034021720941898e-4, 1e-16,
                    "iauApcg", "v(2)", status);
   vvd(astrom.v[2], 0.3517555123437237778e-4, 1e-16,
                    "iauApcg", "v(3)", status);
   vvd(astrom.bm1, 0.9999999951686013336, 1e-12,
                   "iauApcg", "bm1", status);
   vvd(astrom.bpn[0][0], 1.0, 0.0,
                         "iauApcg", "bpn(1,1)", status);
   vvd(astrom.bpn[1][0], 0.0, 0.0,
                         "iauApcg", "bpn(2,1)", status);
   vvd(astrom.bpn[2][0], 0.0, 0.0,
                         "iauApcg", "bpn(3,1)", status);
   vvd(astrom.bpn[0][1], 0.0, 0.0,
                         "iauApcg", "bpn(1,2)", status);
   vvd(astrom.bpn[1][1], 1.0, 0.0,
                         "iauApcg", "bpn(2,2)", status);
   vvd(astrom.bpn[2][1], 0.0, 0.0,
                         "iauApcg", "bpn(3,2)", status);
   vvd(astrom.bpn[0][2], 0.0, 0.0,
                         "iauApcg", "bpn(1,3)", status);
   vvd(astrom.bpn[1][2], 0.0, 0.0,
                         "iauApcg", "bpn(2,3)", status);
   vvd(astrom.bpn[2][2], 1.0, 0.0,
                         "iauApcg", "bpn(3,3)", status);

}

static void t_apcg13(int *status)
/*
**  - - - - - - - - -
**   t _ a p c g 1 3
**  - - - - - - - - -
**
**  Test iauApcg13 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApcg13, vvd
**
**  This revision:  2013 October 3
*/
{
   double date1, date2;
   iauASTROM astrom;


   date1 = 2456165.5;
   date2 = 0.401182685;

   iauApcg13(date1, date2, &astrom);

   vvd(astrom.pmt, 12.65133794027378508, 1e-11,
                   "iauApcg13", "pmt", status);
   vvd(astrom.eb[0], 0.9013108747340644755, 1e-12,
                   "iauApcg13", "eb(1)", status);
   vvd(astrom.eb[1], -0.4174026640406119957, 1e-12,
                   "iauApcg13", "eb(2)", status);
   vvd(astrom.eb[2], -0.1809822877867817771, 1e-12,
                   "iauApcg13", "eb(3)", status);
   vvd(astrom.eh[0], 0.8940025429255499549, 1e-12,
                   "iauApcg13", "eh(1)", status);
   vvd(astrom.eh[1], -0.4110930268331896318, 1e-12,
                   "iauApcg13", "eh(2)", status);
   vvd(astrom.eh[2], -0.1782189006019749850, 1e-12,
                   "iauApcg13", "eh(3)", status);
   vvd(astrom.em, 1.010465295964664178, 1e-12,
                   "iauApcg13", "em", status);
   vvd(astrom.v[0], 0.4289638897157027528e-4, 1e-16,
                   "iauApcg13", "v(1)", status);
   vvd(astrom.v[1], 0.8115034002544663526e-4, 1e-16,
                   "iauApcg13", "v(2)", status);
   vvd(astrom.v[2], 0.3517555122593144633e-4, 1e-16,
                   "iauApcg13", "v(3)", status);
   vvd(astrom.bm1, 0.9999999951686013498, 1e-12,
                   "iauApcg13", "bm1", status);
   vvd(astrom.bpn[0][0], 1.0, 0.0,
                         "iauApcg13", "bpn(1,1)", status);
   vvd(astrom.bpn[1][0], 0.0, 0.0,
                         "iauApcg13", "bpn(2,1)", status);
   vvd(astrom.bpn[2][0], 0.0, 0.0,
                         "iauApcg13", "bpn(3,1)", status);
   vvd(astrom.bpn[0][1], 0.0, 0.0,
                         "iauApcg13", "bpn(1,2)", status);
   vvd(astrom.bpn[1][1], 1.0, 0.0,
                         "iauApcg13", "bpn(2,2)", status);
   vvd(astrom.bpn[2][1], 0.0, 0.0,
                         "iauApcg13", "bpn(3,2)", status);
   vvd(astrom.bpn[0][2], 0.0, 0.0,
                         "iauApcg13", "bpn(1,3)", status);
   vvd(astrom.bpn[1][2], 0.0, 0.0,
                         "iauApcg13", "bpn(2,3)", status);
   vvd(astrom.bpn[2][2], 1.0, 0.0,
                         "iauApcg13", "bpn(3,3)", status);

}

static void t_apci(int *status)
/*
**  - - - - - - -
**   t _ a p c i
**  - - - - - - -
**
**  Test iauApci function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApci, vvd
**
**  This revision:  2013 October 3
*/
{
   double date1, date2, ebpv[2][3], ehp[3], x, y, s;
   iauASTROM astrom;


   date1 = 2456165.5;
   date2 = 0.401182685;
   ebpv[0][0] =  0.901310875;
   ebpv[0][1] = -0.417402664;
   ebpv[0][2] = -0.180982288;
   ebpv[1][0] =  0.00742727954;
   ebpv[1][1] =  0.0140507459;
   ebpv[1][2] =  0.00609045792;
   ehp[0] =  0.903358544;
   ehp[1] = -0.415395237;
   ehp[2] = -0.180084014;
   x =  0.0013122272;
   y = -2.92808623e-5;
   s =  3.05749468e-8;

   iauApci(date1, date2, ebpv, ehp, x, y, s, &astrom);

   vvd(astrom.pmt, 12.65133794027378508, 1e-11,
                   "iauApci", "pmt", status);
   vvd(astrom.eb[0], 0.901310875, 1e-12,
                     "iauApci", "eb(1)", status);
   vvd(astrom.eb[1], -0.417402664, 1e-12,
                     "iauApci", "eb(2)", status);
   vvd(astrom.eb[2], -0.180982288, 1e-12,
                     "iauApci", "eb(3)", status);
   vvd(astrom.eh[0], 0.8940025429324143045, 1e-12,
                     "iauApci", "eh(1)", status);
   vvd(astrom.eh[1], -0.4110930268679817955, 1e-12,
                     "iauApci", "eh(2)", status);
   vvd(astrom.eh[2], -0.1782189004872870264, 1e-12,
                     "iauApci", "eh(3)", status);
   vvd(astrom.em, 1.010465295811013146, 1e-12,
                  "iauApci", "em", status);
   vvd(astrom.v[0], 0.4289638897813379954e-4, 1e-16,
                    "iauApci", "v(1)", status);
   vvd(astrom.v[1], 0.8115034021720941898e-4, 1e-16,
                    "iauApci", "v(2)", status);
   vvd(astrom.v[2], 0.3517555123437237778e-4, 1e-16,
                    "iauApci", "v(3)", status);
   vvd(astrom.bm1, 0.9999999951686013336, 1e-12,
                   "iauApci", "bm1", status);
   vvd(astrom.bpn[0][0], 0.9999991390295159156, 1e-12,
                         "iauApci", "bpn(1,1)", status);
   vvd(astrom.bpn[1][0], 0.4978650072505016932e-7, 1e-12,
                         "iauApci", "bpn(2,1)", status);
   vvd(astrom.bpn[2][0], 0.1312227200000000000e-2, 1e-12,
                         "iauApci", "bpn(3,1)", status);
   vvd(astrom.bpn[0][1], -0.1136336653771609630e-7, 1e-12,
                         "iauApci", "bpn(1,2)", status);
   vvd(astrom.bpn[1][1], 0.9999999995713154868, 1e-12,
                         "iauApci", "bpn(2,2)", status);
   vvd(astrom.bpn[2][1], -0.2928086230000000000e-4, 1e-12,
                         "iauApci", "bpn(3,2)", status);
   vvd(astrom.bpn[0][2], -0.1312227200895260194e-2, 1e-12,
                         "iauApci", "bpn(1,3)", status);
   vvd(astrom.bpn[1][2], 0.2928082217872315680e-4, 1e-12,
                         "iauApci", "bpn(2,3)", status);
   vvd(astrom.bpn[2][2], 0.9999991386008323373, 1e-12,
                         "iauApci", "bpn(3,3)", status);

}

static void t_apci13(int *status)
/*
**  - - - - - - - - -
**   t _ a p c i 1 3
**  - - - - - - - - -
**
**  Test iauApci13 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApci13, vvd
**
**  This revision:  2013 October 3
*/
{
   double date1, date2, eo;
   iauASTROM astrom;


   date1 = 2456165.5;
   date2 = 0.401182685;

   iauApci13(date1, date2, &astrom, &eo);

   vvd(astrom.pmt, 12.65133794027378508, 1e-11,
                   "iauApci13", "pmt", status);
   vvd(astrom.eb[0], 0.9013108747340644755, 1e-12,
                     "iauApci13", "eb(1)", status);
   vvd(astrom.eb[1], -0.4174026640406119957, 1e-12,
                     "iauApci13", "eb(2)", status);
   vvd(astrom.eb[2], -0.1809822877867817771, 1e-12,
                     "iauApci13", "eb(3)", status);
   vvd(astrom.eh[0], 0.8940025429255499549, 1e-12,
                     "iauApci13", "eh(1)", status);
   vvd(astrom.eh[1], -0.4110930268331896318, 1e-12,
                     "iauApci13", "eh(2)", status);
   vvd(astrom.eh[2], -0.1782189006019749850, 1e-12,
                     "iauApci13", "eh(3)", status);
   vvd(astrom.em, 1.010465295964664178, 1e-12,
                  "iauApci13", "em", status);
   vvd(astrom.v[0], 0.4289638897157027528e-4, 1e-16,
                    "iauApci13", "v(1)", status);
   vvd(astrom.v[1], 0.8115034002544663526e-4, 1e-16,
                    "iauApci13", "v(2)", status);
   vvd(astrom.v[2], 0.3517555122593144633e-4, 1e-16,
                    "iauApci13", "v(3)", status);
   vvd(astrom.bm1, 0.9999999951686013498, 1e-12,
                   "iauApci13", "bm1", status);
   vvd(astrom.bpn[0][0], 0.9999992060376761710, 1e-12,
                         "iauApci13", "bpn(1,1)", status);
   vvd(astrom.bpn[1][0], 0.4124244860106037157e-7, 1e-12,
                         "iauApci13", "bpn(2,1)", status);
   vvd(astrom.bpn[2][0], 0.1260128571051709670e-2, 1e-12,
                         "iauApci13", "bpn(3,1)", status);
   vvd(astrom.bpn[0][1], -0.1282291987222130690e-7, 1e-12,
                         "iauApci13", "bpn(1,2)", status);
   vvd(astrom.bpn[1][1], 0.9999999997456835325, 1e-12,
                         "iauApci13", "bpn(2,2)", status);
   vvd(astrom.bpn[2][1], -0.2255288829420524935e-4, 1e-12,
                         "iauApci13", "bpn(3,2)", status);
   vvd(astrom.bpn[0][2], -0.1260128571661374559e-2, 1e-12,
                         "iauApci13", "bpn(1,3)", status);
   vvd(astrom.bpn[1][2], 0.2255285422953395494e-4, 1e-12,
                         "iauApci13", "bpn(2,3)", status);
   vvd(astrom.bpn[2][2], 0.9999992057833604343, 1e-12,
                         "iauApci13", "bpn(3,3)", status);
   vvd(eo, -0.2900618712657375647e-2, 1e-12,
           "iauApci13", "eo", status);

}

static void t_apco(int *status)
/*
**  - - - - - - -
**   t _ a p c o
**  - - - - - - -
**
**  Test iauApco function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApco, vvd
**
**  This revision:  2013 October 3
*/
{
   double date1, date2, ebpv[2][3], ehp[3], x, y, s,
          theta, elong, phi, hm, xp, yp, sp, refa, refb;
   iauASTROM astrom;


   date1 = 2456384.5;
   date2 = 0.970031644;
   ebpv[0][0] = -0.974170438;
   ebpv[0][1] = -0.211520082;
   ebpv[0][2] = -0.0917583024;
   ebpv[1][0] = 0.00364365824;
   ebpv[1][1] = -0.0154287319;
   ebpv[1][2] = -0.00668922024;
   ehp[0] = -0.973458265;
   ehp[1] = -0.209215307;
   ehp[2] = -0.0906996477;
   x = 0.0013122272;
   y = -2.92808623e-5;
   s = 3.05749468e-8;
   theta = 3.14540971;
   elong = -0.527800806;
   phi = -1.2345856;
   hm = 2738.0;
   xp = 2.47230737e-7;
   yp = 1.82640464e-6;
   sp = -3.01974337e-11;
   refa = 0.000201418779;
   refb = -2.36140831e-7;

   iauApco(date1, date2, ebpv, ehp, x, y, s,
           theta, elong, phi, hm, xp, yp, sp,
           refa, refb, &astrom);

   vvd(astrom.pmt, 13.25248468622587269, 1e-11,
                   "iauApco", "pmt", status);
   vvd(astrom.eb[0], -0.9741827110630897003, 1e-12,
                     "iauApco", "eb(1)", status);
   vvd(astrom.eb[1], -0.2115130190135014340, 1e-12,
                     "iauApco", "eb(2)", status);
   vvd(astrom.eb[2], -0.09179840186968295686, 1e-12,
                     "iauApco", "eb(3)", status);
   vvd(astrom.eh[0], -0.9736425571689670428, 1e-12,
                     "iauApco", "eh(1)", status);
   vvd(astrom.eh[1], -0.2092452125848862201, 1e-12,
                     "iauApco", "eh(2)", status);
   vvd(astrom.eh[2], -0.09075578152261439954, 1e-12,
                     "iauApco", "eh(3)", status);
   vvd(astrom.em, 0.9998233241710617934, 1e-12,
                  "iauApco", "em", status);
   vvd(astrom.v[0], 0.2078704985147609823e-4, 1e-16,
                    "iauApco", "v(1)", status);
   vvd(astrom.v[1], -0.8955360074407552709e-4, 1e-16,
                    "iauApco", "v(2)", status);
   vvd(astrom.v[2], -0.3863338980073114703e-4, 1e-16,
                    "iauApco", "v(3)", status);
   vvd(astrom.bm1, 0.9999999950277561600, 1e-12,
                   "iauApco", "bm1", status);
   vvd(astrom.bpn[0][0], 0.9999991390295159156, 1e-12,
                         "iauApco", "bpn(1,1)", status);
   vvd(astrom.bpn[1][0], 0.4978650072505016932e-7, 1e-12,
                         "iauApco", "bpn(2,1)", status);
   vvd(astrom.bpn[2][0], 0.1312227200000000000e-2, 1e-12,
                         "iauApco", "bpn(3,1)", status);
   vvd(astrom.bpn[0][1], -0.1136336653771609630e-7, 1e-12,
                         "iauApco", "bpn(1,2)", status);
   vvd(astrom.bpn[1][1], 0.9999999995713154868, 1e-12,
                         "iauApco", "bpn(2,2)", status);
   vvd(astrom.bpn[2][1], -0.2928086230000000000e-4, 1e-12,
                         "iauApco", "bpn(3,2)", status);
   vvd(astrom.bpn[0][2], -0.1312227200895260194e-2, 1e-12,
                         "iauApco", "bpn(1,3)", status);
   vvd(astrom.bpn[1][2], 0.2928082217872315680e-4, 1e-12,
                         "iauApco", "bpn(2,3)", status);
   vvd(astrom.bpn[2][2], 0.9999991386008323373, 1e-12,
                         "iauApco", "bpn(3,3)", status);
   vvd(astrom.along, -0.5278008060301974337, 1e-12,
                     "iauApco", "along", status);
   vvd(astrom.xpl, 0.1133427418174939329e-5, 1e-17,
                   "iauApco", "xpl", status);
   vvd(astrom.ypl, 0.1453347595745898629e-5, 1e-17,
                   "iauApco", "ypl", status);
   vvd(astrom.sphi, -0.9440115679003211329, 1e-12,
                    "iauApco", "sphi", status);
   vvd(astrom.cphi, 0.3299123514971474711, 1e-12,
                    "iauApco", "cphi", status);
   vvd(astrom.diurab, 0, 0,
                      "iauApco", "diurab", status);
   vvd(astrom.eral, 2.617608903969802566, 1e-12,
                    "iauApco", "eral", status);
   vvd(astrom.refa, 0.2014187790000000000e-3, 1e-15,
                    "iauApco", "refa", status);
   vvd(astrom.refb, -0.2361408310000000000e-6, 1e-18,
                    "iauApco", "refb", status);

}

static void t_apco13(int *status)
/*
**  - - - - - - - - -
**   t _ a p c o 1 3
**  - - - - - - - - -
**
**  Test iauApco13 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApco13, vvd, viv
**
**  This revision:  2013 October 4
*/
{
   double utc1, utc2, dut1, elong, phi, hm, xp, yp,
          phpa, tc, rh, wl, eo;
   iauASTROM astrom;
   int j;


   utc1 = 2456384.5;
   utc2 = 0.969254051;
   dut1 = 0.1550675;
   elong = -0.527800806;
   phi = -1.2345856;
   hm = 2738.0;
   xp = 2.47230737e-7;
   yp = 1.82640464e-6;
   phpa = 731.0;
   tc = 12.8;
   rh = 0.59;
   wl = 0.55;

   j = iauApco13(utc1, utc2, dut1, elong, phi, hm, xp, yp,
                 phpa, tc, rh, wl, &astrom, &eo);

   vvd(astrom.pmt, 13.25248468622475727, 1e-11,
                   "iauApco13", "pmt", status);
   vvd(astrom.eb[0], -0.9741827107321449445, 1e-12,
                   "iauApco13", "eb(1)", status);
   vvd(astrom.eb[1], -0.2115130190489386190, 1e-12,
                     "iauApco13", "eb(2)", status);
   vvd(astrom.eb[2], -0.09179840189515518726, 1e-12,
                     "iauApco13", "eb(3)", status);
   vvd(astrom.eh[0], -0.9736425572586866640, 1e-12,
                     "iauApco13", "eh(1)", status);
   vvd(astrom.eh[1], -0.2092452121602867431, 1e-12,
                     "iauApco13", "eh(2)", status);
   vvd(astrom.eh[2], -0.09075578153903832650, 1e-12,
                     "iauApco13", "eh(3)", status);
   vvd(astrom.em, 0.9998233240914558422, 1e-12,
                  "iauApco13", "em", status);
   vvd(astrom.v[0], 0.2078704986751370303e-4, 1e-16,
                    "iauApco13", "v(1)", status);
   vvd(astrom.v[1], -0.8955360100494469232e-4, 1e-16,
                    "iauApco13", "v(2)", status);
   vvd(astrom.v[2], -0.3863338978840051024e-4, 1e-16,
                    "iauApco13", "v(3)", status);
   vvd(astrom.bm1, 0.9999999950277561368, 1e-12,
                   "iauApco13", "bm1", status);
   vvd(astrom.bpn[0][0], 0.9999991390295147999, 1e-12,
                         "iauApco13", "bpn(1,1)", status);
   vvd(astrom.bpn[1][0], 0.4978650075315529277e-7, 1e-12,
                         "iauApco13", "bpn(2,1)", status);
   vvd(astrom.bpn[2][0], 0.001312227200850293372, 1e-12,
                         "iauApco13", "bpn(3,1)", status);
   vvd(astrom.bpn[0][1], -0.1136336652812486604e-7, 1e-12,
                         "iauApco13", "bpn(1,2)", status);
   vvd(astrom.bpn[1][1], 0.9999999995713154865, 1e-12,
                         "iauApco13", "bpn(2,2)", status);
   vvd(astrom.bpn[2][1], -0.2928086230975367296e-4, 1e-12,
                         "iauApco13", "bpn(3,2)", status);
   vvd(astrom.bpn[0][2], -0.001312227201745553566, 1e-12,
                         "iauApco13", "bpn(1,3)", status);
   vvd(astrom.bpn[1][2], 0.2928082218847679162e-4, 1e-12,
                         "iauApco13", "bpn(2,3)", status);
   vvd(astrom.bpn[2][2], 0.9999991386008312212, 1e-12,
                         "iauApco13", "bpn(3,3)", status);
   vvd(astrom.along, -0.5278008060301974337, 1e-12,
                     "iauApco13", "along", status);
   vvd(astrom.xpl, 0.1133427418174939329e-5, 1e-17,
                   "iauApco13", "xpl", status);
   vvd(astrom.ypl, 0.1453347595745898629e-5, 1e-17,
                   "iauApco13", "ypl", status);
   vvd(astrom.sphi, -0.9440115679003211329, 1e-12,
                    "iauApco13", "sphi", status);
   vvd(astrom.cphi, 0.3299123514971474711, 1e-12,
                    "iauApco13", "cphi", status);
   vvd(astrom.diurab, 0, 0,
                      "iauApco13", "diurab", status);
   vvd(astrom.eral, 2.617608909189066140, 1e-12,
                    "iauApco13", "eral", status);
   vvd(astrom.refa, 0.2014187785940396921e-3, 1e-15,
                    "iauApco13", "refa", status);
   vvd(astrom.refb, -0.2361408314943696227e-6, 1e-18,
                    "iauApco13", "refb", status);
   vvd(eo, -0.003020548354802412839, 1e-14,
           "iauApco13", "eo", status);
   viv(j, 0, "iauApco13", "j", status);

}

static void t_apcs(int *status)
/*
**  - - - - - - -
**   t _ a p c s
**  - - - - - - -
**
**  Test iauApcs function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApcs, vvd
**
**  This revision:  2013 October 3
*/
{
   double date1, date2, pv[2][3], ebpv[2][3], ehp[3];
   iauASTROM astrom;


   date1 = 2456384.5;
   date2 = 0.970031644;
   pv[0][0] = -1836024.09;
   pv[0][1] = 1056607.72;
   pv[0][2] = -5998795.26;
   pv[1][0] = -77.0361767;
   pv[1][1] = -133.310856;
   pv[1][2] = 0.0971855934;
   ebpv[0][0] = -0.974170438;
   ebpv[0][1] = -0.211520082;
   ebpv[0][2] = -0.0917583024;
   ebpv[1][0] = 0.00364365824;
   ebpv[1][1] = -0.0154287319;
   ebpv[1][2] = -0.00668922024;
   ehp[0] = -0.973458265;
   ehp[1] = -0.209215307;
   ehp[2] = -0.0906996477;

   iauApcs(date1, date2, pv, ebpv, ehp, &astrom);

   vvd(astrom.pmt, 13.25248468622587269, 1e-11,
                   "iauApcs", "pmt", status);
   vvd(astrom.eb[0], -0.9741827110630456169, 1e-12,
                     "iauApcs", "eb(1)", status);
   vvd(astrom.eb[1], -0.2115130190136085494, 1e-12,
                     "iauApcs", "eb(2)", status);
   vvd(astrom.eb[2], -0.09179840186973175487, 1e-12,
                     "iauApcs", "eb(3)", status);
   vvd(astrom.eh[0], -0.9736425571689386099, 1e-12,
                     "iauApcs", "eh(1)", status);
   vvd(astrom.eh[1], -0.2092452125849967195, 1e-12,
                     "iauApcs", "eh(2)", status);
   vvd(astrom.eh[2], -0.09075578152266466572, 1e-12,
                     "iauApcs", "eh(3)", status);
   vvd(astrom.em, 0.9998233241710457140, 1e-12,
                  "iauApcs", "em", status);
   vvd(astrom.v[0], 0.2078704985513566571e-4, 1e-16,
                    "iauApcs", "v(1)", status);
   vvd(astrom.v[1], -0.8955360074245006073e-4, 1e-16,
                    "iauApcs", "v(2)", status);
   vvd(astrom.v[2], -0.3863338980073572719e-4, 1e-16,
                    "iauApcs", "v(3)", status);
   vvd(astrom.bm1, 0.9999999950277561601, 1e-12,
                   "iauApcs", "bm1", status);
   vvd(astrom.bpn[0][0], 1, 0,
                         "iauApcs", "bpn(1,1)", status);
   vvd(astrom.bpn[1][0], 0, 0,
                         "iauApcs", "bpn(2,1)", status);
   vvd(astrom.bpn[2][0], 0, 0,
                         "iauApcs", "bpn(3,1)", status);
   vvd(astrom.bpn[0][1], 0, 0,
                         "iauApcs", "bpn(1,2)", status);
   vvd(astrom.bpn[1][1], 1, 0,
                         "iauApcs", "bpn(2,2)", status);
   vvd(astrom.bpn[2][1], 0, 0,
                         "iauApcs", "bpn(3,2)", status);
   vvd(astrom.bpn[0][2], 0, 0,
                         "iauApcs", "bpn(1,3)", status);
   vvd(astrom.bpn[1][2], 0, 0,
                         "iauApcs", "bpn(2,3)", status);
   vvd(astrom.bpn[2][2], 1, 0,
                         "iauApcs", "bpn(3,3)", status);

}

static void t_apcs13(int *status)
/*
**  - - - - - - - - -
**   t _ a p c s 1 3
**  - - - - - - - - -
**
**  Test iauApcs13 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApcs13, vvd
**
**  This revision:  2013 October 3
*/
{
   double date1, date2, pv[2][3];
   iauASTROM astrom;


   date1 = 2456165.5;
   date2 = 0.401182685;
   pv[0][0] = -6241497.16;
   pv[0][1] = 401346.896;
   pv[0][2] = -1251136.04;
   pv[1][0] = -29.264597;
   pv[1][1] = -455.021831;
   pv[1][2] = 0.0266151194;

   iauApcs13(date1, date2, pv, &astrom);

   vvd(astrom.pmt, 12.65133794027378508, 1e-11,
                   "iauApcs13", "pmt", status);
   vvd(astrom.eb[0], 0.9012691529023298391, 1e-12,
                     "iauApcs13", "eb(1)", status);
   vvd(astrom.eb[1], -0.4173999812023068781, 1e-12,
                     "iauApcs13", "eb(2)", status);
   vvd(astrom.eb[2], -0.1809906511146821008, 1e-12,
                     "iauApcs13", "eb(3)", status);
   vvd(astrom.eh[0], 0.8939939101759726824, 1e-12,
                     "iauApcs13", "eh(1)", status);
   vvd(astrom.eh[1], -0.4111053891734599955, 1e-12,
                     "iauApcs13", "eh(2)", status);
   vvd(astrom.eh[2], -0.1782336880637689334, 1e-12,
                     "iauApcs13", "eh(3)", status);
   vvd(astrom.em, 1.010428384373318379, 1e-12,
                  "iauApcs13", "em", status);
   vvd(astrom.v[0], 0.4279877278327626511e-4, 1e-16,
                    "iauApcs13", "v(1)", status);
   vvd(astrom.v[1], 0.7963255057040027770e-4, 1e-16,
                    "iauApcs13", "v(2)", status);
   vvd(astrom.v[2], 0.3517564000441374759e-4, 1e-16,
                    "iauApcs13", "v(3)", status);
   vvd(astrom.bm1, 0.9999999952947981330, 1e-12,
                   "iauApcs13", "bm1", status);
   vvd(astrom.bpn[0][0], 1, 0,
                         "iauApcs13", "bpn(1,1)", status);
   vvd(astrom.bpn[1][0], 0, 0,
                         "iauApcs13", "bpn(2,1)", status);
   vvd(astrom.bpn[2][0], 0, 0,
                         "iauApcs13", "bpn(3,1)", status);
   vvd(astrom.bpn[0][1], 0, 0,
                         "iauApcs13", "bpn(1,2)", status);
   vvd(astrom.bpn[1][1], 1, 0,
                         "iauApcs13", "bpn(2,2)", status);
   vvd(astrom.bpn[2][1], 0, 0,
                         "iauApcs13", "bpn(3,2)", status);
   vvd(astrom.bpn[0][2], 0, 0,
                         "iauApcs13", "bpn(1,3)", status);
   vvd(astrom.bpn[1][2], 0, 0,
                         "iauApcs13", "bpn(2,3)", status);
   vvd(astrom.bpn[2][2], 1, 0,
                         "iauApcs13", "bpn(3,3)", status);

}

static void t_aper(int *status)
/*
**  - - - - - - -
**   t _ a p e r
**  - - - - - - -
*
**  Test iauAper function.
*
**  Returned:
**     status    int         FALSE = success, TRUE = fail
*
**  Called:  iauAper, vvd
*
**  This revision:  2013 October 3
*/
{
   double theta;
   iauASTROM astrom;


   astrom.along = 1.234;
   theta = 5.678;

   iauAper(theta, &astrom);

   vvd(astrom.eral, 6.912000000000000000, 1e-12,
                    "iauAper", "pmt", status);

}

static void t_aper13(int *status)
/*
**  - - - - - - - - -
**   t _ a p e r 1 3
**  - - - - - - - - -
**
**  Test iauAper13 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauAper13, vvd
**
**  This revision:  2013 October 3
*/
{
   double ut11, ut12;
   iauASTROM astrom;


   astrom.along = 1.234;
   ut11 = 2456165.5;
   ut12 = 0.401182685;

   iauAper13(ut11, ut12, &astrom);

   vvd(astrom.eral, 3.316236661789694933, 1e-12,
                    "iauAper13", "pmt", status);

}

static void t_apio(int *status)
/*
**  - - - - - - -
**   t _ a p i o
**  - - - - - - -
**
**  Test iauApio function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApio, vvd
**
**  This revision:  2013 October 3
*/
{
   double sp, theta, elong, phi, hm, xp, yp, refa, refb;
   iauASTROM astrom;


   sp = -3.01974337e-11;
   theta = 3.14540971;
   elong = -0.527800806;
   phi = -1.2345856;
   hm = 2738.0;
   xp = 2.47230737e-7;
   yp = 1.82640464e-6;
   refa = 0.000201418779;
   refb = -2.36140831e-7;

   iauApio(sp, theta, elong, phi, hm, xp, yp, refa, refb, &astrom);

   vvd(astrom.along, -0.5278008060301974337, 1e-12,
                     "iauApio", "along", status);
   vvd(astrom.xpl, 0.1133427418174939329e-5, 1e-17,
                   "iauApio", "xpl", status);
   vvd(astrom.ypl, 0.1453347595745898629e-5, 1e-17,
                   "iauApio", "ypl", status);
   vvd(astrom.sphi, -0.9440115679003211329, 1e-12,
                    "iauApio", "sphi", status);
   vvd(astrom.cphi, 0.3299123514971474711, 1e-12,
                    "iauApio", "cphi", status);
   vvd(astrom.diurab, 0.5135843661699913529e-6, 1e-12,
                      "iauApio", "diurab", status);
   vvd(astrom.eral, 2.617608903969802566, 1e-12,
                    "iauApio", "eral", status);
   vvd(astrom.refa, 0.2014187790000000000e-3, 1e-15,
                    "iauApio", "refa", status);
   vvd(astrom.refb, -0.2361408310000000000e-6, 1e-18,
                    "iauApio", "refb", status);

}

static void t_apio13(int *status)
/*
**  - - - - - - - - -
**   t _ a p i o 1 3
**  - - - - - - - - -
**
**  Test iauApio13 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApio13, vvd, viv
**
**  This revision:  2013 October 4
*/
{
   double utc1, utc2, dut1, elong, phi, hm, xp, yp, phpa, tc, rh, wl;
   int j;
   iauASTROM astrom;


   utc1 = 2456384.5;
   utc2 = 0.969254051;
   dut1 = 0.1550675;
   elong = -0.527800806;
   phi = -1.2345856;
   hm = 2738.0;
   xp = 2.47230737e-7;
   yp = 1.82640464e-6;
   phpa = 731.0;
   tc = 12.8;
   rh = 0.59;
   wl = 0.55;

   j = iauApio13(utc1, utc2, dut1, elong, phi, hm, xp, yp,
                 phpa, tc, rh, wl, &astrom);

   vvd(astrom.along, -0.5278008060301974337, 1e-12,
                     "iauApio13", "along", status);
   vvd(astrom.xpl, 0.1133427418174939329e-5, 1e-17,
                   "iauApio13", "xpl", status);
   vvd(astrom.ypl, 0.1453347595745898629e-5, 1e-17,
                   "iauApio13", "ypl", status);
   vvd(astrom.sphi, -0.9440115679003211329, 1e-12,
                    "iauApio13", "sphi", status);
   vvd(astrom.cphi, 0.3299123514971474711, 1e-12,
                    "iauApio13", "cphi", status);
   vvd(astrom.diurab, 0.5135843661699913529e-6, 1e-12,
                      "iauApio13", "diurab", status);
   vvd(astrom.eral, 2.617608909189066140, 1e-12,
                    "iauApio13", "eral", status);
   vvd(astrom.refa, 0.2014187785940396921e-3, 1e-15,
                    "iauApio13", "refa", status);
   vvd(astrom.refb, -0.2361408314943696227e-6, 1e-18,
                    "iauApio13", "refb", status);
   viv(j, 0, "iauApio13", "j", status);

}

static void t_atci13(int *status)
/*
**  - - - - - - - - -
**   t _ a t c i 1 3
**  - - - - - - - - -
**
**  Test iauAtci13 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauAtci13, vvd
**
**  This revision:  2013 October 3
*/
{
   double rc, dc, pr, pd, px, rv, date1, date2, ri, di, eo;


   rc = 2.71;
   dc = 0.174;
   pr = 1e-5;
   pd = 5e-6;
   px = 0.1;
   rv = 55.0;
   date1 = 2456165.5;
   date2 = 0.401182685;

   iauAtci13(rc, dc, pr, pd, px, rv, date1, date2, &ri, &di, &eo);

   vvd(ri, 2.710121572969038991, 1e-12,
           "iauAtci13", "ri", status);
   vvd(di, 0.1729371367218230438, 1e-12,
           "iauAtci13", "di", status);
   vvd(eo, -0.002900618712657375647, 1e-14,
           "iauAtci13", "eo", status);

}

static void t_atciq(int *status)
/*
**  - - - - - - - -
**   t _ a t c i q
**  - - - - - - - -
**
**  Test iauAtciq function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApci13, iauAtciq, vvd
**
**  This revision:  2013 October 3
*/
{
   double date1, date2, eo, rc, dc, pr, pd, px, rv, ri, di;
   iauASTROM astrom;

   date1 = 2456165.5;
   date2 = 0.401182685;
   iauApci13(date1, date2, &astrom, &eo);
   rc = 2.71;
   dc = 0.174;
   pr = 1e-5;
   pd = 5e-6;
   px = 0.1;
   rv = 55.0;

   iauAtciq(rc, dc, pr, pd, px, rv, &astrom, &ri, &di);

   vvd(ri, 2.710121572969038991, 1e-12, "iauAtciq", "ri", status);
   vvd(di, 0.1729371367218230438, 1e-12, "iauAtciq", "di", status);

}

static void t_atciqn(int *status)
/*
**  - - - - - - - - -
**   t _ a t c i q n
**  - - - - - - - - -
**
**  Test iauAtciqn function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApci13, iauAtciqn, vvd
**
**  This revision:  2013 October 3
*/
{
   iauLDBODY b[3];
   double date1, date2, eo, rc, dc, pr, pd, px, rv, ri, di;
   iauASTROM astrom;

   date1 = 2456165.5;
   date2 = 0.401182685;
   iauApci13(date1, date2, &astrom, &eo);
   rc = 2.71;
   dc = 0.174;
   pr = 1e-5;
   pd = 5e-6;
   px = 0.1;
   rv = 55.0;
   b[0].bm = 0.00028574;
   b[0].dl = 3e-10;
   b[0].pv[0][0] = -7.81014427;
   b[0].pv[0][1] = -5.60956681;
   b[0].pv[0][2] = -1.98079819;
   b[0].pv[1][0] =  0.0030723249;
   b[0].pv[1][1] = -0.00406995477;
   b[0].pv[1][2] = -0.00181335842;
   b[1].bm = 0.00095435;
   b[1].dl = 3e-9;
   b[1].pv[0][0] =  0.738098796;
   b[1].pv[0][1] =  4.63658692;
   b[1].pv[0][2] =  1.9693136;
   b[1].pv[1][0] = -0.00755816922;
   b[1].pv[1][1] =  0.00126913722;
   b[1].pv[1][2] =  0.000727999001;
   b[2].bm = 1.0;
   b[2].dl = 6e-6;
   b[2].pv[0][0] = -0.000712174377;
   b[2].pv[0][1] = -0.00230478303;
   b[2].pv[0][2] = -0.00105865966;
   b[2].pv[1][0] =  6.29235213e-6;
   b[2].pv[1][1] = -3.30888387e-7;
   b[2].pv[1][2] = -2.96486623e-7;

   iauAtciqn ( rc, dc, pr, pd, px, rv, &astrom, 3, b, &ri, &di);

   vvd(ri, 2.710122008105325582, 1e-12, "iauAtciqn", "ri", status);
   vvd(di, 0.1729371916491459122, 1e-12, "iauAtciqn", "di", status);

}

static void t_atciqz(int *status)
/*
**  - - - - - - - - -
**   t _ a t c i q z
**  - - - - - - - - -
**
**  Test iauAtciqz function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApci13, iauAtciqz, vvd
**
**  This revision:  2013 October 3
*/
{
   double date1, date2, eo, rc, dc, ri, di;
   iauASTROM astrom;


   date1 = 2456165.5;
   date2 = 0.401182685;
   iauApci13(date1, date2, &astrom, &eo);
   rc = 2.71;
   dc = 0.174;

   iauAtciqz(rc, dc, &astrom, &ri, &di);

   vvd(ri, 2.709994899247599271, 1e-12, "iauAtciqz", "ri", status);
   vvd(di, 0.1728740720983623469, 1e-12, "iauAtciqz", "di", status);

}

static void t_atco13(int *status)
/*
**  - - - - - - - - -
**   t _ a t c o 1 3
**  - - - - - - - - -
**
**  Test iauAtco13 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauAtco13, vvd, viv
**
**  This revision:  2013 October 4
*/
{
   double rc, dc, pr, pd, px, rv, utc1, utc2, dut1,
          elong, phi, hm, xp, yp, phpa, tc, rh, wl,
          aob, zob, hob, dob, rob, eo;
   int j;


   rc = 2.71;
   dc = 0.174;
   pr = 1e-5;
   pd = 5e-6;
   px = 0.1;
   rv = 55.0;
   utc1 = 2456384.5;
   utc2 = 0.969254051;
   dut1 = 0.1550675;
   elong = -0.527800806;
   phi = -1.2345856;
   hm = 2738.0;
   xp = 2.47230737e-7;
   yp = 1.82640464e-6;
   phpa = 731.0;
   tc = 12.8;
   rh = 0.59;
   wl = 0.55;

   j = iauAtco13(rc, dc, pr, pd, px, rv,
                 utc1, utc2, dut1, elong, phi, hm, xp, yp,
                 phpa, tc, rh, wl,
                 &aob, &zob, &hob, &dob, &rob, &eo);

   vvd(aob, 0.09251774485358230653, 1e-12, "iauAtco13", "aob", status);
   vvd(zob, 1.407661405256767021, 1e-12, "iauAtco13", "zob", status);
   vvd(hob, -0.09265154431403157925, 1e-12, "iauAtco13", "hob", status);
   vvd(dob, 0.1716626560075591655, 1e-12, "iauAtco13", "dob", status);
   vvd(rob, 2.710260453503097719, 1e-12, "iauAtco13", "rob", status);
   vvd(eo, -0.003020548354802412839, 1e-14, "iauAtco13", "eo", status);
   viv(j, 0, "iauAtco13", "j", status);

}

static void t_atic13(int *status)
/*
**  - - - - - - - - -
**   t _ a t i c 1 3
**  - - - - - - - - -
**
**  Test iauAtic13 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauAtic13, vvd
**
**  This revision:  2013 October 3
*/
{
   double ri, di, date1, date2, rc, dc, eo;


   ri = 2.710121572969038991;
   di = 0.1729371367218230438;
   date1 = 2456165.5;
   date2 = 0.401182685;

   iauAtic13(ri, di, date1, date2, &rc, &dc, &eo);

   vvd(rc, 2.710126504531374930, 1e-12, "iauAtic13", "rc", status);
   vvd(dc, 0.1740632537628342320, 1e-12, "iauAtic13", "dc", status);
   vvd(eo, -0.002900618712657375647, 1e-14, "iauAtic13", "eo", status);

}

static void t_aticq(int *status)
/*
**  - - - - - - - -
**   t _ a t i c q
**  - - - - - - - -
**
**  Test iauAticq function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApci13, iauAticq, vvd
**
**  This revision:  2013 October 3
*/
{
   double date1, date2, eo, ri, di, rc, dc;
   iauASTROM astrom;


   date1 = 2456165.5;
   date2 = 0.401182685;
   iauApci13(date1, date2, &astrom, &eo);
   ri = 2.710121572969038991;
   di = 0.1729371367218230438;

   iauAticq(ri, di, &astrom, &rc, &dc);

   vvd(rc, 2.710126504531374930, 1e-12, "iauAticq", "rc", status);
   vvd(dc, 0.1740632537628342320, 1e-12, "iauAticq", "dc", status);

}

static void t_aticqn(int *status)
/*
**  - - - - - - - - -
**   t _ a t i c q n
**  - - - - - - - - -
**
**  Test iauAticqn function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApci13, iauAticqn, vvd
**
**  This revision:  2013 October 3
*/
{
   double date1, date2, eo, ri, di, rc, dc;
   iauLDBODY b[3];
   iauASTROM astrom;


   date1 = 2456165.5;
   date2 = 0.401182685;
   iauApci13(date1, date2, &astrom, &eo);
   ri = 2.709994899247599271;
   di = 0.1728740720983623469;
   b[0].bm = 0.00028574;
   b[0].dl = 3e-10;
   b[0].pv[0][0] = -7.81014427;
   b[0].pv[0][1] = -5.60956681;
   b[0].pv[0][2] = -1.98079819;
   b[0].pv[1][0] =  0.0030723249;
   b[0].pv[1][1] = -0.00406995477;
   b[0].pv[1][2] = -0.00181335842;
   b[1].bm = 0.00095435;
   b[1].dl = 3e-9;
   b[1].pv[0][0] =  0.738098796;
   b[1].pv[0][1] =  4.63658692;
   b[1].pv[0][2] =  1.9693136;
   b[1].pv[1][0] = -0.00755816922;
   b[1].pv[1][1] =  0.00126913722;
   b[1].pv[1][2] =  0.000727999001;
   b[2].bm = 1.0;
   b[2].dl = 6e-6;
   b[2].pv[0][0] = -0.000712174377;
   b[2].pv[0][1] = -0.00230478303;
   b[2].pv[0][2] = -0.00105865966;
   b[2].pv[1][0] =  6.29235213e-6;
   b[2].pv[1][1] = -3.30888387e-7;
   b[2].pv[1][2] = -2.96486623e-7;

   iauAticqn(ri, di, &astrom, 3, b, &rc, &dc);

   vvd(rc, 2.709999575032685412, 1e-12, "iauAtciqn", "rc", status);
   vvd(dc, 0.1739999656317778034, 1e-12, "iauAtciqn", "dc", status);

}

static void t_atio13(int *status)
/*
**  - - - - - - - - -
**   t _ a t i o 1 3
**  - - - - - - - - -
**
**  Test iauAtio13 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauAtio13, vvd, viv
**
**  This revision:  2013 October 3
*/
{
   double ri, di, utc1, utc2, dut1, elong, phi, hm, xp, yp,
          phpa, tc, rh, wl, aob, zob, hob, dob, rob;
   int j;


   ri = 2.710121572969038991;
   di = 0.1729371367218230438;
   utc1 = 2456384.5;
   utc2 = 0.969254051;
   dut1 = 0.1550675;
   elong = -0.527800806;
   phi = -1.2345856;
   hm = 2738.0;
   xp = 2.47230737e-7;
   yp = 1.82640464e-6;
   phpa = 731.0;
   tc = 12.8;
   rh = 0.59;
   wl = 0.55;

   j = iauAtio13(ri, di, utc1, utc2, dut1, elong, phi, hm,
                 xp, yp, phpa, tc, rh, wl,
                 &aob, &zob, &hob, &dob, &rob);

   vvd(aob, 0.09233952224794989993, 1e-12, "iauAtio13", "aob", status);
   vvd(zob, 1.407758704513722461, 1e-12, "iauAtio13", "zob", status);
   vvd(hob, -0.09247619879782006106, 1e-12, "iauAtio13", "hob", status);
   vvd(dob, 0.1717653435758265198, 1e-12, "iauAtio13", "dob", status);
   vvd(rob, 2.710085107986886201, 1e-12, "iauAtio13", "rob", status);
   viv(j, 0, "iauAtio13", "j", status);

}

static void t_atioq(int *status)
/*
**  - - - - - - - -
**   t _ a t i o q
**  - - - - - - - -
**
**  Test iauAtioq function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauApio13, iauAtioq, vvd, viv
**
**  This revision:  2013 October 4
*/
{
   double utc1, utc2, dut1, elong, phi, hm, xp, yp,
          phpa, tc, rh, wl, ri, di, aob, zob, hob, dob, rob;
   iauASTROM astrom;


   utc1 = 2456384.5;
   utc2 = 0.969254051;
   dut1 = 0.1550675;
   elong = -0.527800806;
   phi = -1.2345856;
   hm = 2738.0;
   xp = 2.47230737e-7;
   yp = 1.82640464e-6;
   phpa = 731.0;
   tc = 12.8;
   rh = 0.59;
   wl = 0.55;
   (void) iauApio13(utc1, utc2, dut1, elong, phi, hm, xp, yp,
                    phpa, tc, rh, wl, &astrom);
   ri = 2.710121572969038991;
   di = 0.1729371367218230438;

   iauAtioq(ri, di, &astrom, &aob, &zob, &hob, &dob, &rob);

   vvd(aob, 0.09233952224794989993, 1e-12, "iauAtioq", "aob", status);
   vvd(zob, 1.407758704513722461, 1e-12, "iauAtioq", "zob", status);
   vvd(hob, -0.09247619879782006106, 1e-12, "iauAtioq", "hob", status);
   vvd(dob, 0.1717653435758265198, 1e-12, "iauAtioq", "dob", status);
   vvd(rob, 2.710085107986886201, 1e-12, "iauAtioq", "rob", status);

}

static void t_atoc13(int *status)
/*
**  - - - - - - - - -
**   t _ a t o c 1 3
**  - - - - - - - - -
**
**  Test iauAtoc13 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauAtoc13, vvd, viv
**
**  This revision:  2013 October 3
*/
{
   double utc1, utc2, dut1,
          elong, phi, hm, xp, yp, phpa, tc, rh, wl,
          ob1, ob2, rc, dc;
   int j;


   utc1 = 2456384.5;
   utc2 = 0.969254051;
   dut1 = 0.1550675;
   elong = -0.527800806;
   phi = -1.2345856;
   hm = 2738.0;
   xp = 2.47230737e-7;
   yp = 1.82640464e-6;
   phpa = 731.0;
   tc = 12.8;
   rh = 0.59;
   wl = 0.55;

   ob1 = 2.710085107986886201;
   ob2 = 0.1717653435758265198;
   j = iauAtoc13 ( "R", ob1, ob2, utc1, utc2, dut1,
                   elong, phi, hm, xp, yp, phpa, tc, rh, wl,
                   &rc, &dc);
   vvd(rc, 2.709956744661000609, 1e-12, "iauAtoc13", "R/rc", status);
   vvd(dc, 0.1741696500895398562, 1e-12, "iauAtoc13", "R/dc", status);
   viv(j, 0, "iauAtoc13", "R/j", status);

   ob1 = -0.09247619879782006106;
   ob2 = 0.1717653435758265198;
   j = iauAtoc13 ( "H", ob1, ob2, utc1, utc2, dut1,
                   elong, phi, hm, xp, yp, phpa, tc, rh, wl,
                   &rc, &dc);
   vvd(rc, 2.709956744661000609, 1e-12, "iauAtoc13", "H/rc", status);
   vvd(dc, 0.1741696500895398562, 1e-12, "iauAtoc13", "H/dc", status);
   viv(j, 0, "iauAtoc13", "H/j", status);

   ob1 = 0.09233952224794989993;
   ob2 = 1.407758704513722461;
   j = iauAtoc13 ( "A", ob1, ob2, utc1, utc2, dut1,
                   elong, phi, hm, xp, yp, phpa, tc, rh, wl,
                   &rc, &dc);
   vvd(rc, 2.709956744661000609, 1e-12, "iauAtoc13", "A/rc", status);
   vvd(dc, 0.1741696500895398565, 1e-12, "iauAtoc13", "A/dc", status);
   viv(j, 0, "iauAtoc13", "A/j", status);

}

static void t_atoi13(int *status)
/*
**  - - - - - - - - -
**   t _ a t o i 1 3
**  - - - - - - - - -
**
**  Test iauAtoi13 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauAtoi13, vvd, viv
**
**  This revision:  2013 October 3
*/
{
   double utc1, utc2, dut1, elong, phi, hm, xp, yp, phpa, tc, rh, wl,
          ob1, ob2, ri, di;
   int j;


   utc1 = 2456384.5;
   utc2 = 0.969254051;
   dut1 = 0.1550675;
   elong = -0.527800806;
   phi = -1.2345856;
   hm = 2738.0;
   xp = 2.47230737e-7;
   yp = 1.82640464e-6;
   phpa = 731.0;
   tc = 12.8;
   rh = 0.59;
   wl = 0.55;

   ob1 = 2.710085107986886201;
   ob2 = 0.1717653435758265198;
   j = iauAtoi13 ( "R", ob1, ob2, utc1, utc2, dut1,
                   elong, phi, hm, xp, yp, phpa, tc, rh, wl,
                   &ri, &di);
   vvd(ri, 2.710121574449135955, 1e-12, "iauAtoi13", "R/ri", status);
   vvd(di, 0.1729371839114567725, 1e-12, "iauAtoi13", "R/di", status);
   viv(j, 0, "iauAtoi13", "R/J", status);

   ob1 = -0.09247619879782006106;
   ob2 = 0.1717653435758265198;
   j = iauAtoi13 ( "H", ob1, ob2, utc1, utc2, dut1,
                   elong, phi, hm, xp, yp, phpa, tc, rh, wl,
                   &ri, &di);
   vvd(ri, 2.710121574449135955, 1e-12, "iauAtoi13", "H/ri", status);
   vvd(di, 0.1729371839114567725, 1e-12, "iauAtoi13", "H/di", status);
   viv(j, 0, "iauAtoi13", "H/J", status);

   ob1 = 0.09233952224794989993;
   ob2 = 1.407758704513722461;
   j = iauAtoi13 ( "A", ob1, ob2, utc1, utc2, dut1,
                   elong, phi, hm, xp, yp, phpa, tc, rh, wl,
                   &ri, &di);
   vvd(ri, 2.710121574449135955, 1e-12, "iauAtoi13", "A/ri", status);
   vvd(di, 0.1729371839114567728, 1e-12, "iauAtoi13", "A/di", status);
   viv(j, 0, "iauAtoi13", "A/J", status);

}

static void t_atoiq(int *status)
/*
**  - - - - - - - -
**   t _ a t o i q
**  - - - - - - - -
*
**  Test iauAtoiq function.
*
**  Returned:
**     status    int         FALSE = success, TRUE = fail
*
**  Called:  iauApio13, iauAtoiq, vvd
*
**  This revision:  2013 October 4
*/
{
   double utc1, utc2, dut1, elong, phi, hm, xp, yp, phpa, tc, rh, wl,
          ob1, ob2, ri, di;
   iauASTROM astrom;


   utc1 = 2456384.5;
   utc2 = 0.969254051;
   dut1 = 0.1550675;
   elong = -0.527800806;
   phi = -1.2345856;
   hm = 2738.0;
   xp = 2.47230737e-7;
   yp = 1.82640464e-6;
   phpa = 731.0;
   tc = 12.8;
   rh = 0.59;
   wl = 0.55;
   (void) iauApio13(utc1, utc2, dut1, elong, phi, hm, xp, yp,
                    phpa, tc, rh, wl, &astrom);

   ob1 = 2.710085107986886201;
   ob2 = 0.1717653435758265198;
   iauAtoiq("R", ob1, ob2, &astrom, &ri, &di);
   vvd(ri, 2.710121574449135955, 1e-12,
           "iauAtoiq", "R/ri", status);
   vvd(di, 0.1729371839114567725, 1e-12,
           "iauAtoiq", "R/di", status);

   ob1 = -0.09247619879782006106;
   ob2 = 0.1717653435758265198;
   iauAtoiq("H", ob1, ob2, &astrom, &ri, &di);
   vvd(ri, 2.710121574449135955, 1e-12,
           "iauAtoiq", "H/ri", status);
   vvd(di, 0.1729371839114567725, 1e-12,
           "iauAtoiq", "H/di", status);

   ob1 = 0.09233952224794989993;
   ob2 = 1.407758704513722461;
   iauAtoiq("A", ob1, ob2, &astrom, &ri, &di);
   vvd(ri, 2.710121574449135955, 1e-12,
           "iauAtoiq", "A/ri", status);
   vvd(di, 0.1729371839114567728, 1e-12,
           "iauAtoiq", "A/di", status);

}

static void t_bi00(int *status)
/*
**  - - - - - - -
**   t _ b i 0 0
**  - - - - - - -
**
**  Test iauBi00 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauBi00, vvd
**
**  This revision:  2013 August 7
*/
{
   double dpsibi, depsbi, dra;

   iauBi00(&dpsibi, &depsbi, &dra);

   vvd(dpsibi, -0.2025309152835086613e-6, 1e-12,
      "iauBi00", "dpsibi", status);
   vvd(depsbi, -0.3306041454222147847e-7, 1e-12,
      "iauBi00", "depsbi", status);
   vvd(dra, -0.7078279744199225506e-7, 1e-12,
      "iauBi00", "dra", status);
}

static void t_bp00(int *status)
/*
**  - - - - - - -
**   t _ b p 0 0
**  - - - - - - -
**
**  Test iauBp00 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauBp00, vvd
**
**  This revision:  2013 August 7
*/
{
   double rb[3][3], rp[3][3], rbp[3][3];


   iauBp00(2400000.5, 50123.9999, rb, rp, rbp);

   vvd(rb[0][0], 0.9999999999999942498, 1e-12,
       "iauBp00", "rb11", status);
   vvd(rb[0][1], -0.7078279744199196626e-7, 1e-16,
       "iauBp00", "rb12", status);
   vvd(rb[0][2], 0.8056217146976134152e-7, 1e-16,
       "iauBp00", "rb13", status);
   vvd(rb[1][0], 0.7078279477857337206e-7, 1e-16,
       "iauBp00", "rb21", status);
   vvd(rb[1][1], 0.9999999999999969484, 1e-12,
       "iauBp00", "rb22", status);
   vvd(rb[1][2], 0.3306041454222136517e-7, 1e-16,
       "iauBp00", "rb23", status);
   vvd(rb[2][0], -0.8056217380986972157e-7, 1e-16,
       "iauBp00", "rb31", status);
   vvd(rb[2][1], -0.3306040883980552500e-7, 1e-16,
       "iauBp00", "rb32", status);
   vvd(rb[2][2], 0.9999999999999962084, 1e-12,
       "iauBp00", "rb33", status);

   vvd(rp[0][0], 0.9999995504864048241, 1e-12,
       "iauBp00", "rp11", status);
   vvd(rp[0][1], 0.8696113836207084411e-3, 1e-14,
       "iauBp00", "rp12", status);
   vvd(rp[0][2], 0.3778928813389333402e-3, 1e-14,
       "iauBp00", "rp13", status);
   vvd(rp[1][0], -0.8696113818227265968e-3, 1e-14,
       "iauBp00", "rp21", status);
   vvd(rp[1][1], 0.9999996218879365258, 1e-12,
       "iauBp00", "rp22", status);
   vvd(rp[1][2], -0.1690679263009242066e-6, 1e-14,
       "iauBp00", "rp23", status);
   vvd(rp[2][0], -0.3778928854764695214e-3, 1e-14,
       "iauBp00", "rp31", status);
   vvd(rp[2][1], -0.1595521004195286491e-6, 1e-14,
       "iauBp00", "rp32", status);
   vvd(rp[2][2], 0.9999999285984682756, 1e-12,
       "iauBp00", "rp33", status);

   vvd(rbp[0][0], 0.9999995505175087260, 1e-12,
       "iauBp00", "rbp11", status);
   vvd(rbp[0][1], 0.8695405883617884705e-3, 1e-14,
       "iauBp00", "rbp12", status);
   vvd(rbp[0][2], 0.3779734722239007105e-3, 1e-14,
       "iauBp00", "rbp13", status);
   vvd(rbp[1][0], -0.8695405990410863719e-3, 1e-14,
       "iauBp00", "rbp21", status);
   vvd(rbp[1][1], 0.9999996219494925900, 1e-12,
       "iauBp00", "rbp22", status);
   vvd(rbp[1][2], -0.1360775820404982209e-6, 1e-14,
       "iauBp00", "rbp23", status);
   vvd(rbp[2][0], -0.3779734476558184991e-3, 1e-14,
       "iauBp00", "rbp31", status);
   vvd(rbp[2][1], -0.1925857585832024058e-6, 1e-14,
       "iauBp00", "rbp32", status);
   vvd(rbp[2][2], 0.9999999285680153377, 1e-12,
       "iauBp00", "rbp33", status);
}

static void t_bp06(int *status)
/*
**  - - - - - - -
**   t _ b p 0 6
**  - - - - - - -
**
**  Test iauBp06 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauBp06, vvd
**
**  This revision:  2013 August 7
*/
{
   double rb[3][3], rp[3][3], rbp[3][3];


   iauBp06(2400000.5, 50123.9999, rb, rp, rbp);

   vvd(rb[0][0], 0.9999999999999942497, 1e-12,
       "iauBp06", "rb11", status);
   vvd(rb[0][1], -0.7078368960971557145e-7, 1e-14,
       "iauBp06", "rb12", status);
   vvd(rb[0][2], 0.8056213977613185606e-7, 1e-14,
       "iauBp06", "rb13", status);
   vvd(rb[1][0], 0.7078368694637674333e-7, 1e-14,
       "iauBp06", "rb21", status);
   vvd(rb[1][1], 0.9999999999999969484, 1e-12,
       "iauBp06", "rb22", status);
   vvd(rb[1][2], 0.3305943742989134124e-7, 1e-14,
       "iauBp06", "rb23", status);
   vvd(rb[2][0], -0.8056214211620056792e-7, 1e-14,
       "iauBp06", "rb31", status);
   vvd(rb[2][1], -0.3305943172740586950e-7, 1e-14,
       "iauBp06", "rb32", status);
   vvd(rb[2][2], 0.9999999999999962084, 1e-12,
       "iauBp06", "rb33", status);

   vvd(rp[0][0], 0.9999995504864960278, 1e-12,
       "iauBp06", "rp11", status);
   vvd(rp[0][1], 0.8696112578855404832e-3, 1e-14,
       "iauBp06", "rp12", status);
   vvd(rp[0][2], 0.3778929293341390127e-3, 1e-14,
       "iauBp06", "rp13", status);
   vvd(rp[1][0], -0.8696112560510186244e-3, 1e-14,
       "iauBp06", "rp21", status);
   vvd(rp[1][1], 0.9999996218880458820, 1e-12,
       "iauBp06", "rp22", status);
   vvd(rp[1][2], -0.1691646168941896285e-6, 1e-14,
       "iauBp06", "rp23", status);
   vvd(rp[2][0], -0.3778929335557603418e-3, 1e-14,
       "iauBp06", "rp31", status);
   vvd(rp[2][1], -0.1594554040786495076e-6, 1e-14,
       "iauBp06", "rp32", status);
   vvd(rp[2][2], 0.9999999285984501222, 1e-12,
       "iauBp06", "rp33", status);

   vvd(rbp[0][0], 0.9999995505176007047, 1e-12,
       "iauBp06", "rbp11", status);
   vvd(rbp[0][1], 0.8695404617348208406e-3, 1e-14,
       "iauBp06", "rbp12", status);
   vvd(rbp[0][2], 0.3779735201865589104e-3, 1e-14,
       "iauBp06", "rbp13", status);
   vvd(rbp[1][0], -0.8695404723772031414e-3, 1e-14,
       "iauBp06", "rbp21", status);
   vvd(rbp[1][1], 0.9999996219496027161, 1e-12,
       "iauBp06", "rbp22", status);
   vvd(rbp[1][2], -0.1361752497080270143e-6, 1e-14,
       "iauBp06", "rbp23", status);
   vvd(rbp[2][0], -0.3779734957034089490e-3, 1e-14,
       "iauBp06", "rbp31", status);
   vvd(rbp[2][1], -0.1924880847894457113e-6, 1e-14,
       "iauBp06", "rbp32", status);
   vvd(rbp[2][2], 0.9999999285679971958, 1e-12,
       "iauBp06", "rbp33", status);
}

static void t_bpn2xy(int *status)
/*
**  - - - - - - - - -
**   t _ b p n 2 x y
**  - - - - - - - - -
**
**  Test iauBpn2xy function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauBpn2xy, vvd
**
**  This revision:  2013 August 7
*/
{
   double rbpn[3][3], x, y;


   rbpn[0][0] =  9.999962358680738e-1;
   rbpn[0][1] = -2.516417057665452e-3;
   rbpn[0][2] = -1.093569785342370e-3;

   rbpn[1][0] =  2.516462370370876e-3;
   rbpn[1][1] =  9.999968329010883e-1;
   rbpn[1][2] =  4.006159587358310e-5;

   rbpn[2][0] =  1.093465510215479e-3;
   rbpn[2][1] = -4.281337229063151e-5;
   rbpn[2][2] =  9.999994012499173e-1;

   iauBpn2xy(rbpn, &x, &y);

   vvd(x,  1.093465510215479e-3, 1e-12, "iauBpn2xy", "x", status);
   vvd(y, -4.281337229063151e-5, 1e-12, "iauBpn2xy", "y", status);

}

static void t_c2i00a(int *status)
/*
**  - - - - - - - - -
**   t _ c 2 i 0 0 a
**  - - - - - - - - -
**
**  Test iauC2i00a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2i00a, vvd
**
**  This revision:  2013 August 7
*/
{
   double rc2i[3][3];


   iauC2i00a(2400000.5, 53736.0, rc2i);

   vvd(rc2i[0][0], 0.9999998323037165557, 1e-12,
       "iauC2i00a", "11", status);
   vvd(rc2i[0][1], 0.5581526348992140183e-9, 1e-12,
       "iauC2i00a", "12", status);
   vvd(rc2i[0][2], -0.5791308477073443415e-3, 1e-12,
       "iauC2i00a", "13", status);

   vvd(rc2i[1][0], -0.2384266227870752452e-7, 1e-12,
       "iauC2i00a", "21", status);
   vvd(rc2i[1][1], 0.9999999991917405258, 1e-12,
       "iauC2i00a", "22", status);
   vvd(rc2i[1][2], -0.4020594955028209745e-4, 1e-12,
       "iauC2i00a", "23", status);

   vvd(rc2i[2][0], 0.5791308472168152904e-3, 1e-12,
       "iauC2i00a", "31", status);
   vvd(rc2i[2][1], 0.4020595661591500259e-4, 1e-12,
       "iauC2i00a", "32", status);
   vvd(rc2i[2][2], 0.9999998314954572304, 1e-12,
       "iauC2i00a", "33", status);

}

static void t_c2i00b(int *status)
/*
**  - - - - - - - - -
**   t _ c 2 i 0 0 b
**  - - - - - - - - -
**
**  Test iauC2i00b function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2i00b, vvd
**
**  This revision:  2013 August 7
*/
{
   double rc2i[3][3];


   iauC2i00b(2400000.5, 53736.0, rc2i);

   vvd(rc2i[0][0], 0.9999998323040954356, 1e-12,
       "iauC2i00b", "11", status);
   vvd(rc2i[0][1], 0.5581526349131823372e-9, 1e-12,
       "iauC2i00b", "12", status);
   vvd(rc2i[0][2], -0.5791301934855394005e-3, 1e-12,
       "iauC2i00b", "13", status);

   vvd(rc2i[1][0], -0.2384239285499175543e-7, 1e-12,
       "iauC2i00b", "21", status);
   vvd(rc2i[1][1], 0.9999999991917574043, 1e-12,
       "iauC2i00b", "22", status);
   vvd(rc2i[1][2], -0.4020552974819030066e-4, 1e-12,
       "iauC2i00b", "23", status);

   vvd(rc2i[2][0], 0.5791301929950208873e-3, 1e-12,
       "iauC2i00b", "31", status);
   vvd(rc2i[2][1], 0.4020553681373720832e-4, 1e-12,
       "iauC2i00b", "32", status);
   vvd(rc2i[2][2], 0.9999998314958529887, 1e-12,
       "iauC2i00b", "33", status);

}

static void t_c2i06a(int *status)
/*
**  - - - - - - - - -
**   t _ c 2 i 0 6 a
**  - - - - - - - - -
**
**  Test iauC2i06a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2i06a, vvd
**
**  This revision:  2013 August 7
*/
{
   double rc2i[3][3];


   iauC2i06a(2400000.5, 53736.0, rc2i);

   vvd(rc2i[0][0], 0.9999998323037159379, 1e-12,
       "iauC2i06a", "11", status);
   vvd(rc2i[0][1], 0.5581121329587613787e-9, 1e-12,
       "iauC2i06a", "12", status);
   vvd(rc2i[0][2], -0.5791308487740529749e-3, 1e-12,
       "iauC2i06a", "13", status);

   vvd(rc2i[1][0], -0.2384253169452306581e-7, 1e-12,
       "iauC2i06a", "21", status);
   vvd(rc2i[1][1], 0.9999999991917467827, 1e-12,
       "iauC2i06a", "22", status);
   vvd(rc2i[1][2], -0.4020579392895682558e-4, 1e-12,
       "iauC2i06a", "23", status);

   vvd(rc2i[2][0], 0.5791308482835292617e-3, 1e-12,
       "iauC2i06a", "31", status);
   vvd(rc2i[2][1], 0.4020580099454020310e-4, 1e-12,
       "iauC2i06a", "32", status);
   vvd(rc2i[2][2], 0.9999998314954628695, 1e-12,
       "iauC2i06a", "33", status);

}

static void t_c2ibpn(int *status)
/*
**  - - - - - - - - -
**   t _ c 2 i b p n
**  - - - - - - - - -
**
**  Test iauC2ibpn function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2ibpn, vvd
**
**  This revision:  2013 August 7
*/
{
   double rbpn[3][3], rc2i[3][3];


   rbpn[0][0] =  9.999962358680738e-1;
   rbpn[0][1] = -2.516417057665452e-3;
   rbpn[0][2] = -1.093569785342370e-3;

   rbpn[1][0] =  2.516462370370876e-3;
   rbpn[1][1] =  9.999968329010883e-1;
   rbpn[1][2] =  4.006159587358310e-5;

   rbpn[2][0] =  1.093465510215479e-3;
   rbpn[2][1] = -4.281337229063151e-5;
   rbpn[2][2] =  9.999994012499173e-1;

   iauC2ibpn(2400000.5, 50123.9999, rbpn, rc2i);

   vvd(rc2i[0][0], 0.9999994021664089977, 1e-12,
       "iauC2ibpn", "11", status);
   vvd(rc2i[0][1], -0.3869195948017503664e-8, 1e-12,
       "iauC2ibpn", "12", status);
   vvd(rc2i[0][2], -0.1093465511383285076e-2, 1e-12,
       "iauC2ibpn", "13", status);

   vvd(rc2i[1][0], 0.5068413965715446111e-7, 1e-12,
       "iauC2ibpn", "21", status);
   vvd(rc2i[1][1], 0.9999999990835075686, 1e-12,
       "iauC2ibpn", "22", status);
   vvd(rc2i[1][2], 0.4281334246452708915e-4, 1e-12,
       "iauC2ibpn", "23", status);

   vvd(rc2i[2][0], 0.1093465510215479000e-2, 1e-12,
       "iauC2ibpn", "31", status);
   vvd(rc2i[2][1], -0.4281337229063151000e-4, 1e-12,
       "iauC2ibpn", "32", status);
   vvd(rc2i[2][2], 0.9999994012499173103, 1e-12,
       "iauC2ibpn", "33", status);

}

static void t_c2ixy(int *status)
/*
**  - - - - - - - -
**   t _ c 2 i x y
**  - - - - - - - -
**
**  Test iauC2ixy function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2ixy, vvd
**
**  This revision:  2013 August 7
*/
{
   double x, y, rc2i[3][3];


   x = 0.5791308486706011000e-3;
   y = 0.4020579816732961219e-4;

   iauC2ixy(2400000.5, 53736, x, y, rc2i);

   vvd(rc2i[0][0], 0.9999998323037157138, 1e-12,
       "iauC2ixy", "11", status);
   vvd(rc2i[0][1], 0.5581526349032241205e-9, 1e-12,
       "iauC2ixy", "12", status);
   vvd(rc2i[0][2], -0.5791308491611263745e-3, 1e-12,
       "iauC2ixy", "13", status);

   vvd(rc2i[1][0], -0.2384257057469842953e-7, 1e-12,
       "iauC2ixy", "21", status);
   vvd(rc2i[1][1], 0.9999999991917468964, 1e-12,
       "iauC2ixy", "22", status);
   vvd(rc2i[1][2], -0.4020579110172324363e-4, 1e-12,
       "iauC2ixy", "23", status);

   vvd(rc2i[2][0], 0.5791308486706011000e-3, 1e-12,
       "iauC2ixy", "31", status);
   vvd(rc2i[2][1], 0.4020579816732961219e-4, 1e-12,
       "iauC2ixy", "32", status);
   vvd(rc2i[2][2], 0.9999998314954627590, 1e-12,
       "iauC2ixy", "33", status);

}

static void t_c2ixys(int *status)
/*
**  - - - - - - - - -
**   t _ c 2 i x y s
**  - - - - - - - - -
**
**  Test iauC2ixys function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2ixys, vvd
**
**  This revision:  2013 August 7
*/
{
   double x, y, s, rc2i[3][3];


   x =  0.5791308486706011000e-3;
   y =  0.4020579816732961219e-4;
   s = -0.1220040848472271978e-7;

   iauC2ixys(x, y, s, rc2i);

   vvd(rc2i[0][0], 0.9999998323037157138, 1e-12,
       "iauC2ixys", "11", status);
   vvd(rc2i[0][1], 0.5581984869168499149e-9, 1e-12,
       "iauC2ixys", "12", status);
   vvd(rc2i[0][2], -0.5791308491611282180e-3, 1e-12,
       "iauC2ixys", "13", status);

   vvd(rc2i[1][0], -0.2384261642670440317e-7, 1e-12,
       "iauC2ixys", "21", status);
   vvd(rc2i[1][1], 0.9999999991917468964, 1e-12,
       "iauC2ixys", "22", status);
   vvd(rc2i[1][2], -0.4020579110169668931e-4, 1e-12,
       "iauC2ixys", "23", status);

   vvd(rc2i[2][0], 0.5791308486706011000e-3, 1e-12,
       "iauC2ixys", "31", status);
   vvd(rc2i[2][1], 0.4020579816732961219e-4, 1e-12,
       "iauC2ixys", "32", status);
   vvd(rc2i[2][2], 0.9999998314954627590, 1e-12,
       "iauC2ixys", "33", status);

}

static void t_c2s(int *status)
/*
**  - - - - - -
**   t _ c 2 s
**  - - - - - -
**
**  Test iauC2s function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2s, vvd
**
**  This revision:  2013 August 7
*/
{
   double p[3], theta, phi;


   p[0] = 100.0;
   p[1] = -50.0;
   p[2] =  25.0;

   iauC2s(p, &theta, &phi);

   vvd(theta, -0.4636476090008061162, 1e-14, "iauC2s", "theta", status);
   vvd(phi, 0.2199879773954594463, 1e-14, "iauC2s", "phi", status);

}

static void t_c2t00a(int *status)
/*
**  - - - - - - - - -
**   t _ c 2 t 0 0 a
**  - - - - - - - - -
**
**  Test iauC2t00a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2t00a, vvd
**
**  This revision:  2013 August 7
*/
{
   double tta, ttb, uta, utb, xp, yp, rc2t[3][3];


   tta = 2400000.5;
   uta = 2400000.5;
   ttb = 53736.0;
   utb = 53736.0;
   xp = 2.55060238e-7;
   yp = 1.860359247e-6;

   iauC2t00a(tta, ttb, uta, utb, xp, yp, rc2t);

   vvd(rc2t[0][0], -0.1810332128307182668, 1e-12,
       "iauC2t00a", "11", status);
   vvd(rc2t[0][1], 0.9834769806938457836, 1e-12,
       "iauC2t00a", "12", status);
   vvd(rc2t[0][2], 0.6555535638688341725e-4, 1e-12,
       "iauC2t00a", "13", status);

   vvd(rc2t[1][0], -0.9834768134135984552, 1e-12,
       "iauC2t00a", "21", status);
   vvd(rc2t[1][1], -0.1810332203649520727, 1e-12,
       "iauC2t00a", "22", status);
   vvd(rc2t[1][2], 0.5749801116141056317e-3, 1e-12,
       "iauC2t00a", "23", status);

   vvd(rc2t[2][0], 0.5773474014081406921e-3, 1e-12,
       "iauC2t00a", "31", status);
   vvd(rc2t[2][1], 0.3961832391770163647e-4, 1e-12,
       "iauC2t00a", "32", status);
   vvd(rc2t[2][2], 0.9999998325501692289, 1e-12,
       "iauC2t00a", "33", status);

}

static void t_c2t00b(int *status)
/*
**  - - - - - - - - -
**   t _ c 2 t 0 0 b
**  - - - - - - - - -
**
**  Test iauC2t00b function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2t00b, vvd
**
**  This revision:  2013 August 7
*/
{
   double tta, ttb, uta, utb, xp, yp, rc2t[3][3];


   tta = 2400000.5;
   uta = 2400000.5;
   ttb = 53736.0;
   utb = 53736.0;
   xp = 2.55060238e-7;
   yp = 1.860359247e-6;

   iauC2t00b(tta, ttb, uta, utb, xp, yp, rc2t);

   vvd(rc2t[0][0], -0.1810332128439678965, 1e-12,
       "iauC2t00b", "11", status);
   vvd(rc2t[0][1], 0.9834769806913872359, 1e-12,
       "iauC2t00b", "12", status);
   vvd(rc2t[0][2], 0.6555565082458415611e-4, 1e-12,
       "iauC2t00b", "13", status);

   vvd(rc2t[1][0], -0.9834768134115435923, 1e-12,
       "iauC2t00b", "21", status);
   vvd(rc2t[1][1], -0.1810332203784001946, 1e-12,
       "iauC2t00b", "22", status);
   vvd(rc2t[1][2], 0.5749793922030017230e-3, 1e-12,
       "iauC2t00b", "23", status);

   vvd(rc2t[2][0], 0.5773467471863534901e-3, 1e-12,
       "iauC2t00b", "31", status);
   vvd(rc2t[2][1], 0.3961790411549945020e-4, 1e-12,
       "iauC2t00b", "32", status);
   vvd(rc2t[2][2], 0.9999998325505635738, 1e-12,
       "iauC2t00b", "33", status);

}

static void t_c2t06a(int *status)
/*
**  - - - - - - - - -
**   t _ c 2 t 0 6 a
**  - - - - - - - - -
**
**  Test iauC2t06a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2t06a, vvd
**
**  This revision:  2013 August 7
*/
{
   double tta, ttb, uta, utb, xp, yp, rc2t[3][3];


   tta = 2400000.5;
   uta = 2400000.5;
   ttb = 53736.0;
   utb = 53736.0;
   xp = 2.55060238e-7;
   yp = 1.860359247e-6;

   iauC2t06a(tta, ttb, uta, utb, xp, yp, rc2t);

   vvd(rc2t[0][0], -0.1810332128305897282, 1e-12,
       "iauC2t06a", "11", status);
   vvd(rc2t[0][1], 0.9834769806938592296, 1e-12,
       "iauC2t06a", "12", status);
   vvd(rc2t[0][2], 0.6555550962998436505e-4, 1e-12,
       "iauC2t06a", "13", status);

   vvd(rc2t[1][0], -0.9834768134136214897, 1e-12,
       "iauC2t06a", "21", status);
   vvd(rc2t[1][1], -0.1810332203649130832, 1e-12,
       "iauC2t06a", "22", status);
   vvd(rc2t[1][2], 0.5749800844905594110e-3, 1e-12,
       "iauC2t06a", "23", status);

   vvd(rc2t[2][0], 0.5773474024748545878e-3, 1e-12,
       "iauC2t06a", "31", status);
   vvd(rc2t[2][1], 0.3961816829632690581e-4, 1e-12,
       "iauC2t06a", "32", status);
   vvd(rc2t[2][2], 0.9999998325501747785, 1e-12,
       "iauC2t06a", "33", status);

}

static void t_c2tcio(int *status)
/*
**  - - - - - - - - -
**   t _ c 2 t c i o
**  - - - - - - - - -
**
**  Test iauC2tcio function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2tcio, vvd
**
**  This revision:  2013 August 7
*/
{
   double rc2i[3][3], era, rpom[3][3], rc2t[3][3];


   rc2i[0][0] =  0.9999998323037164738;
   rc2i[0][1] =  0.5581526271714303683e-9;
   rc2i[0][2] = -0.5791308477073443903e-3;

   rc2i[1][0] = -0.2384266227524722273e-7;
   rc2i[1][1] =  0.9999999991917404296;
   rc2i[1][2] = -0.4020594955030704125e-4;

   rc2i[2][0] =  0.5791308472168153320e-3;
   rc2i[2][1] =  0.4020595661593994396e-4;
   rc2i[2][2] =  0.9999998314954572365;

   era = 1.75283325530307;

   rpom[0][0] =  0.9999999999999674705;
   rpom[0][1] = -0.1367174580728847031e-10;
   rpom[0][2] =  0.2550602379999972723e-6;

   rpom[1][0] =  0.1414624947957029721e-10;
   rpom[1][1] =  0.9999999999982694954;
   rpom[1][2] = -0.1860359246998866338e-5;

   rpom[2][0] = -0.2550602379741215275e-6;
   rpom[2][1] =  0.1860359247002413923e-5;
   rpom[2][2] =  0.9999999999982369658;


   iauC2tcio(rc2i, era, rpom, rc2t);

   vvd(rc2t[0][0], -0.1810332128307110439, 1e-12,
       "iauC2tcio", "11", status);
   vvd(rc2t[0][1], 0.9834769806938470149, 1e-12,
       "iauC2tcio", "12", status);
   vvd(rc2t[0][2], 0.6555535638685466874e-4, 1e-12,
       "iauC2tcio", "13", status);

   vvd(rc2t[1][0], -0.9834768134135996657, 1e-12,
       "iauC2tcio", "21", status);
   vvd(rc2t[1][1], -0.1810332203649448367, 1e-12,
       "iauC2tcio", "22", status);
   vvd(rc2t[1][2], 0.5749801116141106528e-3, 1e-12,
       "iauC2tcio", "23", status);

   vvd(rc2t[2][0], 0.5773474014081407076e-3, 1e-12,
       "iauC2tcio", "31", status);
   vvd(rc2t[2][1], 0.3961832391772658944e-4, 1e-12,
       "iauC2tcio", "32", status);
   vvd(rc2t[2][2], 0.9999998325501691969, 1e-12,
       "iauC2tcio", "33", status);

}

static void t_c2teqx(int *status)
/*
**  - - - - - - - - -
**   t _ c 2 t e q x
**  - - - - - - - - -
**
**  Test iauC2teqx function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2teqx, vvd
**
**  This revision:  2013 August 7
*/
{
   double rbpn[3][3], gst, rpom[3][3], rc2t[3][3];


   rbpn[0][0] =  0.9999989440476103608;
   rbpn[0][1] = -0.1332881761240011518e-2;
   rbpn[0][2] = -0.5790767434730085097e-3;

   rbpn[1][0] =  0.1332858254308954453e-2;
   rbpn[1][1] =  0.9999991109044505944;
   rbpn[1][2] = -0.4097782710401555759e-4;

   rbpn[2][0] =  0.5791308472168153320e-3;
   rbpn[2][1] =  0.4020595661593994396e-4;
   rbpn[2][2] =  0.9999998314954572365;

   gst = 1.754166138040730516;

   rpom[0][0] =  0.9999999999999674705;
   rpom[0][1] = -0.1367174580728847031e-10;
   rpom[0][2] =  0.2550602379999972723e-6;

   rpom[1][0] =  0.1414624947957029721e-10;
   rpom[1][1] =  0.9999999999982694954;
   rpom[1][2] = -0.1860359246998866338e-5;

   rpom[2][0] = -0.2550602379741215275e-6;
   rpom[2][1] =  0.1860359247002413923e-5;
   rpom[2][2] =  0.9999999999982369658;

   iauC2teqx(rbpn, gst, rpom, rc2t);

   vvd(rc2t[0][0], -0.1810332128528685730, 1e-12,
       "iauC2teqx", "11", status);
   vvd(rc2t[0][1], 0.9834769806897685071, 1e-12,
       "iauC2teqx", "12", status);
   vvd(rc2t[0][2], 0.6555535639982634449e-4, 1e-12,
       "iauC2teqx", "13", status);

   vvd(rc2t[1][0], -0.9834768134095211257, 1e-12,
       "iauC2teqx", "21", status);
   vvd(rc2t[1][1], -0.1810332203871023800, 1e-12,
       "iauC2teqx", "22", status);
   vvd(rc2t[1][2], 0.5749801116126438962e-3, 1e-12,
       "iauC2teqx", "23", status);

   vvd(rc2t[2][0], 0.5773474014081539467e-3, 1e-12,
       "iauC2teqx", "31", status);
   vvd(rc2t[2][1], 0.3961832391768640871e-4, 1e-12,
       "iauC2teqx", "32", status);
   vvd(rc2t[2][2], 0.9999998325501691969, 1e-12,
       "iauC2teqx", "33", status);

}

static void t_c2tpe(int *status)
/*
**  - - - - - - - -
**   t _ c 2 t p e
**  - - - - - - - -
**
**  Test iauC2tpe function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2tpe, vvd
**
**  This revision:  2013 August 7
*/
{
   double tta, ttb, uta, utb, dpsi, deps, xp, yp, rc2t[3][3];


   tta = 2400000.5;
   uta = 2400000.5;
   ttb = 53736.0;
   utb = 53736.0;
   deps =  0.4090789763356509900;
   dpsi = -0.9630909107115582393e-5;
   xp = 2.55060238e-7;
   yp = 1.860359247e-6;

   iauC2tpe(tta, ttb, uta, utb, dpsi, deps, xp, yp, rc2t);

   vvd(rc2t[0][0], -0.1813677995763029394, 1e-12,
       "iauC2tpe", "11", status);
   vvd(rc2t[0][1], 0.9023482206891683275, 1e-12,
       "iauC2tpe", "12", status);
   vvd(rc2t[0][2], -0.3909902938641085751, 1e-12,
       "iauC2tpe", "13", status);

   vvd(rc2t[1][0], -0.9834147641476804807, 1e-12,
       "iauC2tpe", "21", status);
   vvd(rc2t[1][1], -0.1659883635434995121, 1e-12,
       "iauC2tpe", "22", status);
   vvd(rc2t[1][2], 0.7309763898042819705e-1, 1e-12,
       "iauC2tpe", "23", status);

   vvd(rc2t[2][0], 0.1059685430673215247e-2, 1e-12,
       "iauC2tpe", "31", status);
   vvd(rc2t[2][1], 0.3977631855605078674, 1e-12,
       "iauC2tpe", "32", status);
   vvd(rc2t[2][2], 0.9174875068792735362, 1e-12,
       "iauC2tpe", "33", status);

}

static void t_c2txy(int *status)
/*
**  - - - - - - - -
**   t _ c 2 t x y
**  - - - - - - - -
**
**  Test iauC2txy function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauC2txy, vvd
**
**  This revision:  2013 August 7
*/
{
   double tta, ttb, uta, utb, x, y, xp, yp, rc2t[3][3];


   tta = 2400000.5;
   uta = 2400000.5;
   ttb = 53736.0;
   utb = 53736.0;
   x = 0.5791308486706011000e-3;
   y = 0.4020579816732961219e-4;
   xp = 2.55060238e-7;
   yp = 1.860359247e-6;

   iauC2txy(tta, ttb, uta, utb, x, y, xp, yp, rc2t);

   vvd(rc2t[0][0], -0.1810332128306279253, 1e-12,
       "iauC2txy", "11", status);
   vvd(rc2t[0][1], 0.9834769806938520084, 1e-12,
       "iauC2txy", "12", status);
   vvd(rc2t[0][2], 0.6555551248057665829e-4, 1e-12,
       "iauC2txy", "13", status);

   vvd(rc2t[1][0], -0.9834768134136142314, 1e-12,
       "iauC2txy", "21", status);
   vvd(rc2t[1][1], -0.1810332203649529312, 1e-12,
       "iauC2txy", "22", status);
   vvd(rc2t[1][2], 0.5749800843594139912e-3, 1e-12,
       "iauC2txy", "23", status);

   vvd(rc2t[2][0], 0.5773474028619264494e-3, 1e-12,
       "iauC2txy", "31", status);
   vvd(rc2t[2][1], 0.3961816546911624260e-4, 1e-12,
       "iauC2txy", "32", status);
   vvd(rc2t[2][2], 0.9999998325501746670, 1e-12,
       "iauC2txy", "33", status);

}

static void t_cal2jd(int *status)
/*
**  - - - - - - - - -
**   t _ c a l 2 j d
**  - - - - - - - - -
**
**  Test iauCal2jd function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauCal2jd, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   int j;
   double djm0, djm;


   j = iauCal2jd(2003, 06, 01, &djm0, &djm);

   vvd(djm0, 2400000.5, 0.0, "iauCal2jd", "djm0", status);
   vvd(djm,    52791.0, 0.0, "iauCal2jd", "djm", status);

   viv(j, 0, "iauCal2jd", "j", status);

}

static void t_cp(int *status)
/*
**  - - - - -
**   t _ c p
**  - - - - -
**
**  Test iauCp function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauCp, vvd
**
**  This revision:  2013 August 7
*/
{
   double p[3], c[3];


   p[0] =  0.3;
   p[1] =  1.2;
   p[2] = -2.5;

   iauCp(p, c);

   vvd(c[0],  0.3, 0.0, "iauCp", "1", status);
   vvd(c[1],  1.2, 0.0, "iauCp", "2", status);
   vvd(c[2], -2.5, 0.0, "iauCp", "3", status);
}

static void t_cpv(int *status)
/*
**  - - - - - -
**   t _ c p v
**  - - - - - -
**
**  Test iauCpv function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauCpv, vvd
**
**  This revision:  2013 August 7
*/
{
   double pv[2][3], c[2][3];


   pv[0][0] =  0.3;
   pv[0][1] =  1.2;
   pv[0][2] = -2.5;

   pv[1][0] = -0.5;
   pv[1][1] =  3.1;
   pv[1][2] =  0.9;

   iauCpv(pv, c);

   vvd(c[0][0],  0.3, 0.0, "iauCpv", "p1", status);
   vvd(c[0][1],  1.2, 0.0, "iauCpv", "p2", status);
   vvd(c[0][2], -2.5, 0.0, "iauCpv", "p3", status);

   vvd(c[1][0], -0.5, 0.0, "iauCpv", "v1", status);
   vvd(c[1][1],  3.1, 0.0, "iauCpv", "v2", status);
   vvd(c[1][2],  0.9, 0.0, "iauCpv", "v3", status);

}

static void t_cr(int *status)
/*
**  - - - - -
**   t _ c r
**  - - - - -
**
**  Test iauCr function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauCr, vvd
**
**  This revision:  2013 August 7
*/
{
   double r[3][3], c[3][3];


   r[0][0] = 2.0;
   r[0][1] = 3.0;
   r[0][2] = 2.0;

   r[1][0] = 3.0;
   r[1][1] = 2.0;
   r[1][2] = 3.0;

   r[2][0] = 3.0;
   r[2][1] = 4.0;
   r[2][2] = 5.0;

   iauCr(r, c);

   vvd(c[0][0], 2.0, 0.0, "iauCr", "11", status);
   vvd(c[0][1], 3.0, 0.0, "iauCr", "12", status);
   vvd(c[0][2], 2.0, 0.0, "iauCr", "13", status);

   vvd(c[1][0], 3.0, 0.0, "iauCr", "21", status);
   vvd(c[1][1], 2.0, 0.0, "iauCr", "22", status);
   vvd(c[1][2], 3.0, 0.0, "iauCr", "23", status);

   vvd(c[2][0], 3.0, 0.0, "iauCr", "31", status);
   vvd(c[2][1], 4.0, 0.0, "iauCr", "32", status);
   vvd(c[2][2], 5.0, 0.0, "iauCr", "33", status);
}

static void t_d2dtf(int *status )
/*
**  - - - - - - - -
**   t _ d 2 d t f
**  - - - - - - - -
**
**  Test iauD2dtf function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauD2dtf, viv
**
**  This revision:  2013 August 7
*/
{
   int j, iy, im, id, ihmsf[4];


   j = iauD2dtf("UTC", 5, 2400000.5, 49533.99999, &iy, &im, &id, ihmsf);

   viv(iy, 1994, "iauD2dtf", "y", status);
   viv(im, 6, "iauD2dtf", "mo", status);
   viv(id, 30, "iauD2dtf", "d", status);
   viv(ihmsf[0], 23, "iauD2dtf", "h", status);
   viv(ihmsf[1], 59, "iauD2dtf", "m", status);
   viv(ihmsf[2], 60, "iauD2dtf", "s", status);
   viv(ihmsf[3], 13599, "iauD2dtf", "f", status);
   viv(j, 0, "iauD2dtf", "j", status);

}

static void t_d2tf(int *status)
/*
**  - - - - - - -
**   t _ d 2 t f
**  - - - - - - -
**
**  Test iauD2tf function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauD2tf, viv, vvd
**
**  This revision:  2013 August 7
*/
{
   int ihmsf[4];
   char s;


   iauD2tf(4, -0.987654321, &s, ihmsf);

   viv((int)s, '-', "iauD2tf", "s", status);

   viv(ihmsf[0], 23, "iauD2tf", "0", status);
   viv(ihmsf[1], 42, "iauD2tf", "1", status);
   viv(ihmsf[2], 13, "iauD2tf", "2", status);
   viv(ihmsf[3], 3333, "iauD2tf", "3", status);

}

static void t_dat(int *status)
/*
**  - - - - - -
**   t _ d a t
**  - - - - - -
**
**  Test iauDat function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauDat, vvd, viv
**
**  This revision:  2015 January 30
*/
{
   int j;
   double deltat;


   j = iauDat(2003, 6, 1, 0.0, &deltat);

   vvd(deltat, 32.0, 0.0, "iauDat", "d1", status);
   viv(j, 0, "iauDat", "j1", status);

   j = iauDat(2008, 1, 17, 0.0, &deltat);

   vvd(deltat, 33.0, 0.0, "iauDat", "d2", status);
   viv(j, 0, "iauDat", "j2", status);

   j = iauDat(2015, 9, 1, 0.0, &deltat);

   vvd(deltat, 36.0, 0.0, "iauDat", "d3", status);
   viv(j, 0, "iauDat", "j3", status);

}

static void t_dtdb(int *status)
/*
**  - - - - - - -
**   t _ d t d b
**  - - - - - - -
**
**  Test iauDtdb function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauDtdb, vvd
**
**  This revision:  2013 August 7
*/
{
   double dtdb;


   dtdb = iauDtdb(2448939.5, 0.123, 0.76543, 5.0123, 5525.242, 3190.0);

   vvd(dtdb, -0.1280368005936998991e-2, 1e-15, "iauDtdb", "", status);

}

static void t_dtf2d(int *status)
/*
**  - - - - - - - -
**   t _ d t f 2 d
**  - - - - - - - -
**
**  Test iauDtf2d function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauDtf2d, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double u1, u2;
   int j;


   j = iauDtf2d("UTC", 1994, 6, 30, 23, 59, 60.13599, &u1, &u2);

   vvd(u1+u2, 2449534.49999, 1e-6, "iauDtf2d", "u", status);
   viv(j, 0, "iauDtf2d", "j", status);

}

static void t_ee00(int *status)
/*
**  - - - - - - -
**   t _ e e 0 0
**  - - - - - - -
**
**  Test iauEe00 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEe00, vvd
**
**  This revision:  2013 August 7
*/
{
   double epsa, dpsi, ee;


   epsa =  0.4090789763356509900;
   dpsi = -0.9630909107115582393e-5;

   ee = iauEe00(2400000.5, 53736.0, epsa, dpsi);

   vvd(ee, -0.8834193235367965479e-5, 1e-18, "iauEe00", "", status);

}

static void t_ee00a(int *status)
/*
**  - - - - - - - -
**   t _ e e 0 0 a
**  - - - - - - - -
**
**  Test iauEe00a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEe00a, vvd
**
**  This revision:  2013 August 7
*/
{
   double ee;


   ee = iauEe00a(2400000.5, 53736.0);

   vvd(ee, -0.8834192459222588227e-5, 1e-18, "iauEe00a", "", status);

}

static void t_ee00b(int *status)
/*
**  - - - - - - - -
**   t _ e e 0 0 b
**  - - - - - - - -
**
**  Test iauEe00b function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEe00b, vvd
**
**  This revision:  2013 August 7
*/
{
   double ee;


   ee = iauEe00b(2400000.5, 53736.0);

   vvd(ee, -0.8835700060003032831e-5, 1e-18, "iauEe00b", "", status);

}

static void t_ee06a(int *status)
/*
**  - - - - - - - -
**   t _ e e 0 6 a
**  - - - - - - - -
**
**  Test iauEe06a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEe06a, vvd
**
**  This revision:  2013 August 7
*/
{
   double ee;


   ee = iauEe06a(2400000.5, 53736.0);

   vvd(ee, -0.8834195072043790156e-5, 1e-15, "iauEe06a", "", status);
}

static void t_eect00(int *status)
/*
**  - - - - - - - - -
**   t _ e e c t 0 0
**  - - - - - - - - -
**
**  Test iauEect00 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEect00, vvd
**
**  This revision:  2013 August 7
*/
{
   double eect;


   eect = iauEect00(2400000.5, 53736.0);

   vvd(eect, 0.2046085004885125264e-8, 1e-20, "iauEect00", "", status);

}

static void t_eform(int *status)
/*
**  - - - - - - - -
**   t _ e f o r m
**  - - - - - - - -
**
**  Test iauEform function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEform, viv, vvd
**
**  This revision:  2013 August 7
*/
{
   int j;
   double a, f;

   j = iauEform(0, &a, &f);

   viv(j, -1, "iauEform", "j0", status);

   j = iauEform(WGS84, &a, &f);

   viv(j, 0, "iauEform", "j1", status);
   vvd(a, 6378137.0, 1e-10, "iauEform", "a1", status);
   vvd(f, 0.0033528106647474807, 1e-18, "iauEform", "f1", status);

   j = iauEform(GRS80, &a, &f);

   viv(j, 0, "iauEform", "j2", status);
   vvd(a, 6378137.0, 1e-10, "iauEform", "a2", status);
   vvd(f, 0.0033528106811823189, 1e-18, "iauEform", "f2", status);

   j = iauEform(WGS72, &a, &f);

   viv(j, 0, "iauEform", "j2", status);
   vvd(a, 6378135.0, 1e-10, "iauEform", "a3", status);
   vvd(f, 0.0033527794541675049, 1e-18, "iauEform", "f3", status);

   j = iauEform(4, &a, &f);
   viv(j, -1, "iauEform", "j3", status);
}

static void t_eo06a(int *status)
/*
**  - - - - - - - -
**   t _ e o 0 6 a
**  - - - - - - - -
**
**  Test iauEo06a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEo06a, vvd
**
**  This revision:  2013 August 7
*/
{
   double eo;


   eo = iauEo06a(2400000.5, 53736.0);

   vvd(eo, -0.1332882371941833644e-2, 1e-15, "iauEo06a", "", status);

}

static void t_eors(int *status)
/*
**  - - - - - - -
**   t _ e o r s
**  - - - - - - -
**
**  Test iauEors function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEors, vvd
**
**  This revision:  2013 August 7
*/
{
   double rnpb[3][3], s, eo;


   rnpb[0][0] =  0.9999989440476103608;
   rnpb[0][1] = -0.1332881761240011518e-2;
   rnpb[0][2] = -0.5790767434730085097e-3;

   rnpb[1][0] =  0.1332858254308954453e-2;
   rnpb[1][1] =  0.9999991109044505944;
   rnpb[1][2] = -0.4097782710401555759e-4;

   rnpb[2][0] =  0.5791308472168153320e-3;
   rnpb[2][1] =  0.4020595661593994396e-4;
   rnpb[2][2] =  0.9999998314954572365;

   s = -0.1220040848472271978e-7;

   eo = iauEors(rnpb, s);

   vvd(eo, -0.1332882715130744606e-2, 1e-14, "iauEors", "", status);

}

static void t_epb(int *status)
/*
**  - - - - - -
**   t _ e p b
**  - - - - - -
**
**  Test iauEpb function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEpb, vvd
**
**  This revision:  2013 August 7
*/
{
   double epb;


   epb = iauEpb(2415019.8135, 30103.18648);

   vvd(epb, 1982.418424159278580, 1e-12, "iauEpb", "", status);

}

static void t_epb2jd(int *status)
/*
**  - - - - - - - - -
**   t _ e p b 2 j d
**  - - - - - - - - -
**
**  Test iauEpb2jd function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEpb2jd, vvd
**
**  This revision:  2013 August 7
*/
{
   double epb, djm0, djm;


   epb = 1957.3;

   iauEpb2jd(epb, &djm0, &djm);

   vvd(djm0, 2400000.5, 1e-9, "iauEpb2jd", "djm0", status);
   vvd(djm, 35948.1915101513, 1e-9, "iauEpb2jd", "mjd", status);

}

static void t_epj(int *status)
/*
**  - - - - - -
**   t _ e p j
**  - - - - - -
**
**  Test iauEpj function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEpj, vvd
**
**  This revision:  2013 August 7
*/
{
   double epj;


   epj = iauEpj(2451545, -7392.5);

   vvd(epj, 1979.760438056125941, 1e-12, "iauEpj", "", status);

}

static void t_epj2jd(int *status)
/*
**  - - - - - - - - -
**   t _ e p j 2 j d
**  - - - - - - - - -
**
**  Test iauEpj2jd function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEpj2jd, vvd
**
**  This revision:  2013 August 7
*/
{
   double epj, djm0, djm;


   epj = 1996.8;

   iauEpj2jd(epj, &djm0, &djm);

   vvd(djm0, 2400000.5, 1e-9, "iauEpj2jd", "djm0", status);
   vvd(djm,    50375.7, 1e-9, "iauEpj2jd", "mjd",  status);

}

static void t_epv00(int *status)
/*
**  - - - - - - - -
**   t _ e p v 0 0
**  - - - - - - - -
**
**  Test iauEpv00 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called: iauEpv00, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double pvh[2][3], pvb[2][3];
   int j;


   j = iauEpv00(2400000.5, 53411.52501161, pvh, pvb);

   vvd(pvh[0][0], -0.7757238809297706813, 1e-14,
       "iauEpv00", "ph(x)", status);
   vvd(pvh[0][1], 0.5598052241363340596, 1e-14,
       "iauEpv00", "ph(y)", status);
   vvd(pvh[0][2], 0.2426998466481686993, 1e-14,
       "iauEpv00", "ph(z)", status);

   vvd(pvh[1][0], -0.1091891824147313846e-1, 1e-15,
       "iauEpv00", "vh(x)", status);
   vvd(pvh[1][1], -0.1247187268440845008e-1, 1e-15,
       "iauEpv00", "vh(y)", status);
   vvd(pvh[1][2], -0.5407569418065039061e-2, 1e-15,
       "iauEpv00", "vh(z)", status);

   vvd(pvb[0][0], -0.7714104440491111971, 1e-14,
       "iauEpv00", "pb(x)", status);
   vvd(pvb[0][1], 0.5598412061824171323, 1e-14,
       "iauEpv00", "pb(y)", status);
   vvd(pvb[0][2], 0.2425996277722452400, 1e-14,
       "iauEpv00", "pb(z)", status);

   vvd(pvb[1][0], -0.1091874268116823295e-1, 1e-15,
       "iauEpv00", "vb(x)", status);
   vvd(pvb[1][1], -0.1246525461732861538e-1, 1e-15,
       "iauEpv00", "vb(y)", status);
   vvd(pvb[1][2], -0.5404773180966231279e-2, 1e-15,
       "iauEpv00", "vb(z)", status);

   viv(j, 0, "iauEpv00", "j", status);

}

static void t_eqeq94(int *status)
/*
**  - - - - - - - - -
**   t _ e q e q 9 4
**  - - - - - - - - -
**
**  Test iauEqeq94 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEqeq94, vvd
**
**  This revision:  2013 August 7
*/
{
   double eqeq;


   eqeq = iauEqeq94(2400000.5, 41234.0);

   vvd(eqeq, 0.5357758254609256894e-4, 1e-17, "iauEqeq94", "", status);

}

static void t_era00(int *status)
/*
**  - - - - - - - -
**   t _ e r a 0 0
**  - - - - - - - -
**
**  Test iauEra00 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauEra00, vvd
**
**  This revision:  2013 August 7
*/
{
   double era00;


   era00 = iauEra00(2400000.5, 54388.0);

   vvd(era00, 0.4022837240028158102, 1e-12, "iauEra00", "", status);

}

static void t_fad03(int *status)
/*
**  - - - - - - - -
**   t _ f a d 0 3
**  - - - - - - - -
**
**  Test iauFad03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFad03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFad03(0.80), 1.946709205396925672, 1e-12,
       "iauFad03", "", status);
}

static void t_fae03(int *status)
/*
**  - - - - - - - -
**   t _ f a e 0 3
**  - - - - - - - -
**
**  Test iauFae03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFae03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFae03(0.80), 1.744713738913081846, 1e-12,
       "iauFae03", "", status);
}

static void t_faf03(int *status)
/*
**  - - - - - - - -
**   t _ f a f 0 3
**  - - - - - - - -
**
**  Test iauFaf03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFaf03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFaf03(0.80), 0.2597711366745499518, 1e-12,
       "iauFaf03", "", status);
}

static void t_faju03(int *status)
/*
**  - - - - - - - - -
**   t _ f a j u 0 3
**  - - - - - - - - -
**
**  Test iauFaju03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFaju03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFaju03(0.80), 5.275711665202481138, 1e-12,
       "iauFaju03", "", status);
}

static void t_fal03(int *status)
/*
**  - - - - - - - -
**   t _ f a l 0 3
**  - - - - - - - -
**
**  Test iauFal03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFal03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFal03(0.80), 5.132369751108684150, 1e-12,
       "iauFal03", "", status);
}

static void t_falp03(int *status)
/*
**  - - - - - - - - -
**   t _ f a l p 0 3
**  - - - - - - - - -
**
**  Test iauFalp03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFalp03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFalp03(0.80), 6.226797973505507345, 1e-12,
      "iauFalp03", "", status);
}

static void t_fama03(int *status)
/*
**  - - - - - - - - -
**   t _ f a m a 0 3
**  - - - - - - - - -
**
**  Test iauFama03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFama03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFama03(0.80), 3.275506840277781492, 1e-12,
       "iauFama03", "", status);
}

static void t_fame03(int *status)
/*
**  - - - - - - - - -
**   t _ f a m e 0 3
**  - - - - - - - - -
**
**  Test iauFame03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFame03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFame03(0.80), 5.417338184297289661, 1e-12,
       "iauFame03", "", status);
}

static void t_fane03(int *status)
/*
**  - - - - - - - - -
**   t _ f a n e 0 3
**  - - - - - - - - -
**
**  Test iauFane03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFane03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFane03(0.80), 2.079343830860413523, 1e-12,
       "iauFane03", "", status);
}

static void t_faom03(int *status)
/*
**  - - - - - - - - -
**   t _ f a o m 0 3
**  - - - - - - - - -
**
**  Test iauFaom03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFaom03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFaom03(0.80), -5.973618440951302183, 1e-12,
       "iauFaom03", "", status);
}

static void t_fapa03(int *status)
/*
**  - - - - - - - - -
**   t _ f a p a 0 3
**  - - - - - - - - -
**
**  Test iauFapa03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFapa03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFapa03(0.80), 0.1950884762240000000e-1, 1e-12,
       "iauFapa03", "", status);
}

static void t_fasa03(int *status)
/*
**  - - - - - - - - -
**   t _ f a s a 0 3
**  - - - - - - - - -
**
**  Test iauFasa03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFasa03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFasa03(0.80), 5.371574539440827046, 1e-12,
       "iauFasa03", "", status);
}

static void t_faur03(int *status)
/*
**  - - - - - - - - -
**   t _ f a u r 0 3
**  - - - - - - - - -
**
**  Test iauFaur03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFaur03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFaur03(0.80), 5.180636450180413523, 1e-12,
       "iauFaur03", "", status);
}

static void t_fave03(int *status)
/*
**  - - - - - - - - -
**   t _ f a v e 0 3
**  - - - - - - - - -
**
**  Test iauFave03 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFave03, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauFave03(0.80), 3.424900460533758000, 1e-12,
       "iauFave03", "", status);
}

static void t_fk52h(int *status)
/*
**  - - - - - - - -
**   t _ f k 5 2 h
**  - - - - - - - -
**
**  Test iauFk52h function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFk52h, vvd
**
**  This revision:  2013 August 7
*/
{
   double r5, d5, dr5, dd5, px5, rv5, rh, dh, drh, ddh, pxh, rvh;


   r5  =  1.76779433;
   d5  = -0.2917517103;
   dr5 = -1.91851572e-7;
   dd5 = -5.8468475e-6;
   px5 =  0.379210;
   rv5 = -7.6;

   iauFk52h(r5, d5, dr5, dd5, px5, rv5,
            &rh, &dh, &drh, &ddh, &pxh, &rvh);

   vvd(rh, 1.767794226299947632, 1e-14,
       "iauFk52h", "ra", status);
   vvd(dh,  -0.2917516070530391757, 1e-14,
       "iauFk52h", "dec", status);
   vvd(drh, -0.19618741256057224e-6,1e-19,
       "iauFk52h", "dr5", status);
   vvd(ddh, -0.58459905176693911e-5, 1e-19,
       "iauFk52h", "dd5", status);
   vvd(pxh,  0.37921, 1e-14,
       "iauFk52h", "px", status);
   vvd(rvh, -7.6000000940000254, 1e-11,
       "iauFk52h", "rv", status);

}

static void t_fk5hip(int *status)
/*
**  - - - - - - - - -
**   t _ f k 5 h i p
**  - - - - - - - - -
**
**  Test iauFk5hip function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFk5hip, vvd
**
**  This revision:  2013 August 7
*/
{
   double r5h[3][3], s5h[3];


   iauFk5hip(r5h, s5h);

   vvd(r5h[0][0], 0.9999999999999928638, 1e-14,
       "iauFk5hip", "11", status);
   vvd(r5h[0][1], 0.1110223351022919694e-6, 1e-17,
       "iauFk5hip", "12", status);
   vvd(r5h[0][2], 0.4411803962536558154e-7, 1e-17,
       "iauFk5hip", "13", status);
   vvd(r5h[1][0], -0.1110223308458746430e-6, 1e-17,
       "iauFk5hip", "21", status);
   vvd(r5h[1][1], 0.9999999999999891830, 1e-14,
       "iauFk5hip", "22", status);
   vvd(r5h[1][2], -0.9647792498984142358e-7, 1e-17,
       "iauFk5hip", "23", status);
   vvd(r5h[2][0], -0.4411805033656962252e-7, 1e-17,
       "iauFk5hip", "31", status);
   vvd(r5h[2][1], 0.9647792009175314354e-7, 1e-17,
       "iauFk5hip", "32", status);
   vvd(r5h[2][2], 0.9999999999999943728, 1e-14,
       "iauFk5hip", "33", status);
   vvd(s5h[0], -0.1454441043328607981e-8, 1e-17,
       "iauFk5hip", "s1", status);
   vvd(s5h[1], 0.2908882086657215962e-8, 1e-17,
       "iauFk5hip", "s2", status);
   vvd(s5h[2], 0.3393695767766751955e-8, 1e-17,
       "iauFk5hip", "s3", status);

}

static void t_fk5hz(int *status)
/*
**  - - - - - - - -
**   t _ f k 5 h z
**  - - - - - - - -
**
**  Test iauFk5hz function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFk5hz, vvd
**
**  This revision:  2013 August 7
*/
{
   double r5, d5, rh, dh;


   r5 =  1.76779433;
   d5 = -0.2917517103;

   iauFk5hz(r5, d5, 2400000.5, 54479.0, &rh, &dh);

   vvd(rh,  1.767794191464423978, 1e-12, "iauFk5hz", "ra", status);
   vvd(dh, -0.2917516001679884419, 1e-12, "iauFk5hz", "dec", status);

}

static void t_fw2m(int *status)
/*
**  - - - - - - -
**   t _ f w 2 m
**  - - - - - - -
**
**  Test iauFw2m function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFw2m, vvd
**
**  This revision:  2013 August 7
*/
{
   double gamb, phib, psi, eps, r[3][3];


   gamb = -0.2243387670997992368e-5;
   phib =  0.4091014602391312982;
   psi  = -0.9501954178013015092e-3;
   eps  =  0.4091014316587367472;

   iauFw2m(gamb, phib, psi, eps, r);

   vvd(r[0][0], 0.9999995505176007047, 1e-12,
       "iauFw2m", "11", status);
   vvd(r[0][1], 0.8695404617348192957e-3, 1e-12,
       "iauFw2m", "12", status);
   vvd(r[0][2], 0.3779735201865582571e-3, 1e-12,
       "iauFw2m", "13", status);

   vvd(r[1][0], -0.8695404723772016038e-3, 1e-12,
       "iauFw2m", "21", status);
   vvd(r[1][1], 0.9999996219496027161, 1e-12,
       "iauFw2m", "22", status);
   vvd(r[1][2], -0.1361752496887100026e-6, 1e-12,
       "iauFw2m", "23", status);

   vvd(r[2][0], -0.3779734957034082790e-3, 1e-12,
       "iauFw2m", "31", status);
   vvd(r[2][1], -0.1924880848087615651e-6, 1e-12,
       "iauFw2m", "32", status);
   vvd(r[2][2], 0.9999999285679971958, 1e-12,
       "iauFw2m", "33", status);

}

static void t_fw2xy(int *status)
/*
**  - - - - - - - -
**   t _ f w 2 x y
**  - - - - - - - -
**
**  Test iauFw2xy function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauFw2xy, vvd
**
**  This revision:  2013 August 7
*/
{
   double gamb, phib, psi, eps, x, y;


   gamb = -0.2243387670997992368e-5;
   phib =  0.4091014602391312982;
   psi  = -0.9501954178013015092e-3;
   eps  =  0.4091014316587367472;

   iauFw2xy(gamb, phib, psi, eps, &x, &y);

   vvd(x, -0.3779734957034082790e-3, 1e-14, "iauFw2xy", "x", status);
   vvd(y, -0.1924880848087615651e-6, 1e-14, "iauFw2xy", "y", status);

}

static void t_g2icrs(int *status)
/*
**  - - - - - - - - -
**   t _ g 2 i c r s
**  - - - - - - - - -
**
**  Test iauG2icrs function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauG2icrs, vvd
**
**  This revision:  2015 January 30
*/
{
   double dl, db, dr, dd;


   dl =  5.5850536063818546461558105;
   db = -0.7853981633974483096156608;
   iauG2icrs (dl, db, &dr, &dd);
   vvd(dr,  5.9338074302227188048671, 1e-14, "iauG2icrs", "R", status);
   vvd(dd, -1.1784870613579944551541, 1e-14, "iauG2icrs", "D", status);
 }

static void t_gc2gd(int *status)
/*
**  - - - - - - - -
**   t _ g c 2 g d
**  - - - - - - - -
**
**  Test iauGc2gd function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauGc2gd, viv, vvd
**
**  This revision:  2013 August 7
*/
{
   int j;
   double xyz[] = {2e6, 3e6, 5.244e6};
   double e, p, h;

   j = iauGc2gd(0, xyz, &e, &p, &h);

   viv(j, -1, "iauGc2gd", "j0", status);

   j = iauGc2gd(WGS84, xyz, &e, &p, &h);

   viv(j, 0, "iauGc2gd", "j1", status);
   vvd(e, 0.98279372324732907, 1e-14, "iauGc2gd", "e1", status);
   vvd(p, 0.97160184819075459, 1e-14, "iauGc2gd", "p1", status);
   vvd(h, 331.41724614260599, 1e-8, "iauGc2gd", "h1", status);

   j = iauGc2gd(GRS80, xyz, &e, &p, &h);

   viv(j, 0, "iauGc2gd", "j2", status);
   vvd(e, 0.98279372324732907, 1e-14, "iauGc2gd", "e2", status);
   vvd(p, 0.97160184820607853, 1e-14, "iauGc2gd", "p2", status);
   vvd(h, 331.41731754844348, 1e-8, "iauGc2gd", "h2", status);

   j = iauGc2gd(WGS72, xyz, &e, &p, &h);

   viv(j, 0, "iauGc2gd", "j3", status);
   vvd(e, 0.98279372324732907, 1e-14, "iauGc2gd", "e3", status);
   vvd(p, 0.97160181811015119, 1e-14, "iauGc2gd", "p3", status);
   vvd(h, 333.27707261303181, 1e-8, "iauGc2gd", "h3", status);

   j = iauGc2gd(4, xyz, &e, &p, &h);

   viv(j, -1, "iauGc2gd", "j4", status);
}

static void t_gc2gde(int *status)
/*
**  - - - - - - - - -
**   t _ g c 2 g d e
**  - - - - - - - - -
**
**  Test iauGc2gde function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauGc2gde, viv, vvd
**
**  This revision:  2013 August 7
*/
{
   int j;
   double a = 6378136.0, f = 0.0033528;
   double xyz[] = {2e6, 3e6, 5.244e6};
   double e, p, h;

   j = iauGc2gde(a, f, xyz, &e, &p, &h);

   viv(j, 0, "iauGc2gde", "j", status);
   vvd(e, 0.98279372324732907, 1e-14, "iauGc2gde", "e", status);
   vvd(p, 0.97160183775704115, 1e-14, "iauGc2gde", "p", status);
   vvd(h, 332.36862495764397, 1e-8, "iauGc2gde", "h", status);
}

static void t_gd2gc(int *status)
/*
**  - - - - - - - -
**   t _ g d 2 g c
**  - - - - - - - -
**
**  Test iauGd2gc function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauGd2gc, viv, vvd
**
**  This revision:  2013 August 7
*/
{
   int j;
   double e = 3.1, p = -0.5, h = 2500.0;
   double xyz[3];

   j = iauGd2gc(0, e, p, h, xyz);

   viv(j, -1, "iauGd2gc", "j0", status);

   j = iauGd2gc(WGS84, e, p, h, xyz);

   viv(j, 0, "iauGd2gc", "j1", status);
   vvd(xyz[0], -5599000.5577049947, 1e-7, "iauGd2gc", "0/1", status);
   vvd(xyz[1], 233011.67223479203, 1e-7, "iauGd2gc", "1/1", status);
   vvd(xyz[2], -3040909.4706983363, 1e-7, "iauGd2gc", "2/1", status);

   j = iauGd2gc(GRS80, e, p, h, xyz);

   viv(j, 0, "iauGd2gc", "j2", status);
   vvd(xyz[0], -5599000.5577260984, 1e-7, "iauGd2gc", "0/2", status);
   vvd(xyz[1], 233011.6722356703, 1e-7, "iauGd2gc", "1/2", status);
   vvd(xyz[2], -3040909.4706095476, 1e-7, "iauGd2gc", "2/2", status);

   j = iauGd2gc(WGS72, e, p, h, xyz);

   viv(j, 0, "iauGd2gc", "j3", status);
   vvd(xyz[0], -5598998.7626301490, 1e-7, "iauGd2gc", "0/3", status);
   vvd(xyz[1], 233011.5975297822, 1e-7, "iauGd2gc", "1/3", status);
   vvd(xyz[2], -3040908.6861467111, 1e-7, "iauGd2gc", "2/3", status);

   j = iauGd2gc(4, e, p, h, xyz);

   viv(j, -1, "iauGd2gc", "j4", status);
}

static void t_gd2gce(int *status)
/*
**  - - - - - - - - -
**   t _ g d 2 g c e
**  - - - - - - - - -
**
**  Test iauGd2gce function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauGd2gce, viv, vvd
**
**  This revision:  2013 August 7
*/
{
   int j;
   double a = 6378136.0, f = 0.0033528;
   double e = 3.1, p = -0.5, h = 2500.0;
   double xyz[3];

   j = iauGd2gce(a, f, e, p, h, xyz);

   viv(j, 0, "iauGd2gce", "j", status);
   vvd(xyz[0], -5598999.6665116328, 1e-7, "iauGd2gce", "0", status);
   vvd(xyz[1], 233011.63514630572, 1e-7, "iauGd2gce", "1", status);
   vvd(xyz[2], -3040909.0517314132, 1e-7, "iauGd2gce", "2", status);
}

static void t_gmst00(int *status)
/*
**  - - - - - - - - -
**   t _ g m s t 0 0
**  - - - - - - - - -
**
**  Test iauGmst00 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauGmst00, vvd
**
**  This revision:  2013 August 7
*/
{
   double theta;


   theta = iauGmst00(2400000.5, 53736.0, 2400000.5, 53736.0);

   vvd(theta, 1.754174972210740592, 1e-12, "iauGmst00", "", status);

}

static void t_gmst06(int *status)
/*
**  - - - - - - - - -
**   t _ g m s t 0 6
**  - - - - - - - - -
**
**  Test iauGmst06 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauGmst06, vvd
**
**  This revision:  2013 August 7
*/
{
   double theta;


   theta = iauGmst06(2400000.5, 53736.0, 2400000.5, 53736.0);

   vvd(theta, 1.754174971870091203, 1e-12, "iauGmst06", "", status);

}

static void t_gmst82(int *status)
/*
**  - - - - - - - - -
**   t _ g m s t 8 2
**  - - - - - - - - -
**
**  Test iauGmst82 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauGmst82, vvd
**
**  This revision:  2013 August 7
*/
{
   double theta;


   theta = iauGmst82(2400000.5, 53736.0);

   vvd(theta, 1.754174981860675096, 1e-12, "iauGmst82", "", status);

}

static void t_gst00a(int *status)
/*
**  - - - - - - - - -
**   t _ g s t 0 0 a
**  - - - - - - - - -
**
**  Test iauGst00a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauGst00a, vvd
**
**  This revision:  2013 August 7
*/
{
   double theta;


   theta = iauGst00a(2400000.5, 53736.0, 2400000.5, 53736.0);

   vvd(theta, 1.754166138018281369, 1e-12, "iauGst00a", "", status);

}

static void t_gst00b(int *status)
/*
**  - - - - - - - - -
**   t _ g s t 0 0 b
**  - - - - - - - - -
**
**  Test iauGst00b function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauGst00b, vvd
**
**  This revision:  2013 August 7
*/
{
   double theta;


   theta = iauGst00b(2400000.5, 53736.0);

   vvd(theta, 1.754166136510680589, 1e-12, "iauGst00b", "", status);

}

static void t_gst06(int *status)
/*
**  - - - - - - - -
**   t _ g s t 0 6
**  - - - - - - - -
**
**  Test iauGst06 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauGst06, vvd
**
**  This revision:  2013 August 7
*/
{
   double rnpb[3][3], theta;


   rnpb[0][0] =  0.9999989440476103608;
   rnpb[0][1] = -0.1332881761240011518e-2;
   rnpb[0][2] = -0.5790767434730085097e-3;

   rnpb[1][0] =  0.1332858254308954453e-2;
   rnpb[1][1] =  0.9999991109044505944;
   rnpb[1][2] = -0.4097782710401555759e-4;

   rnpb[2][0] =  0.5791308472168153320e-3;
   rnpb[2][1] =  0.4020595661593994396e-4;
   rnpb[2][2] =  0.9999998314954572365;

   theta = iauGst06(2400000.5, 53736.0, 2400000.5, 53736.0, rnpb);

   vvd(theta, 1.754166138018167568, 1e-12, "iauGst06", "", status);

}

static void t_gst06a(int *status)
/*
**  - - - - - - - - -
**   t _ g s t 0 6 a
**  - - - - - - - - -
**
**  Test iauGst06a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauGst06a, vvd
**
**  This revision:  2013 August 7
*/
{
   double theta;


   theta = iauGst06a(2400000.5, 53736.0, 2400000.5, 53736.0);

   vvd(theta, 1.754166137675019159, 1e-12, "iauGst06a", "", status);

}

static void t_gst94(int *status)
/*
**  - - - - - - - -
**   t _ g s t 9 4
**  - - - - - - - -
**
**  Test iauGst94 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauGst94, vvd
**
**  This revision:  2013 August 7
*/
{
   double theta;


   theta = iauGst94(2400000.5, 53736.0);

   vvd(theta, 1.754166136020645203, 1e-12, "iauGst94", "", status);

}

static void t_icrs2g(int *status)
/*
**  - - - - - - - - -
**   t _ i c r s 2 g
**  - - - - - - - - -
**
**  Test iauIcrs2g function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauIcrs2g, vvd
**
**  This revision:  2015 January 30
*/
{
   double dr, dd, dl, db;

   dr =  5.9338074302227188048671087;
   dd = -1.1784870613579944551540570;
   iauIcrs2g (dr, dd, &dl, &db);
   vvd(dl,  5.5850536063818546461558, 1e-14, "iauIcrs2g", "L", status);
   vvd(db, -0.7853981633974483096157, 1e-14, "iauIcrs2g", "B", status);
 }

static void t_h2fk5(int *status)
/*
**  - - - - - - - -
**   t _ h 2 f k 5
**  - - - - - - - -
**
**  Test iauH2fk5 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauH2fk5, vvd
**
**  This revision:  2013 August 7
*/
{
   double rh, dh, drh, ddh, pxh, rvh, r5, d5, dr5, dd5, px5, rv5;


   rh  =  1.767794352;
   dh  = -0.2917512594;
   drh = -2.76413026e-6;
   ddh = -5.92994449e-6;
   pxh =  0.379210;
   rvh = -7.6;

   iauH2fk5(rh, dh, drh, ddh, pxh, rvh,
            &r5, &d5, &dr5, &dd5, &px5, &rv5);

   vvd(r5, 1.767794455700065506, 1e-13,
       "iauH2fk5", "ra", status);
   vvd(d5, -0.2917513626469638890, 1e-13,
       "iauH2fk5", "dec", status);
   vvd(dr5, -0.27597945024511204e-5, 1e-18,
       "iauH2fk5", "dr5", status);
   vvd(dd5, -0.59308014093262838e-5, 1e-18,
       "iauH2fk5", "dd5", status);
   vvd(px5, 0.37921, 1e-13,
       "iauH2fk5", "px", status);
   vvd(rv5, -7.6000001309071126, 1e-10,
       "iauH2fk5", "rv", status);

}

static void t_hfk5z(int *status)
/*
**  - - - - - - - -
**   t _ h f k 5 z
**  - - - - - - - -
**
**  Test iauHfk5z function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauHfk5z, vvd
**
**  This revision:  2013 August 7
*/
{
   double rh, dh, r5, d5, dr5, dd5;



   rh =  1.767794352;
   dh = -0.2917512594;

   iauHfk5z(rh, dh, 2400000.5, 54479.0, &r5, &d5, &dr5, &dd5);

   vvd(r5, 1.767794490535581026, 1e-13,
       "iauHfk5z", "ra", status);
   vvd(d5, -0.2917513695320114258, 1e-14,
       "iauHfk5z", "dec", status);
   vvd(dr5, 0.4335890983539243029e-8, 1e-22,
       "iauHfk5z", "dr5", status);
   vvd(dd5, -0.8569648841237745902e-9, 1e-23,
       "iauHfk5z", "dd5", status);

}

static void t_ir(int *status)
/*
**  - - - - -
**   t _ i r
**  - - - - -
**
**  Test iauIr function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauIr, vvd
**
**  This revision:  2013 August 7
*/
{
   double r[3][3];


   r[0][0] = 2.0;
   r[0][1] = 3.0;
   r[0][2] = 2.0;

   r[1][0] = 3.0;
   r[1][1] = 2.0;
   r[1][2] = 3.0;

   r[2][0] = 3.0;
   r[2][1] = 4.0;
   r[2][2] = 5.0;

   iauIr(r);

   vvd(r[0][0], 1.0, 0.0, "iauIr", "11", status);
   vvd(r[0][1], 0.0, 0.0, "iauIr", "12", status);
   vvd(r[0][2], 0.0, 0.0, "iauIr", "13", status);

   vvd(r[1][0], 0.0, 0.0, "iauIr", "21", status);
   vvd(r[1][1], 1.0, 0.0, "iauIr", "22", status);
   vvd(r[1][2], 0.0, 0.0, "iauIr", "23", status);

   vvd(r[2][0], 0.0, 0.0, "iauIr", "31", status);
   vvd(r[2][1], 0.0, 0.0, "iauIr", "32", status);
   vvd(r[2][2], 1.0, 0.0, "iauIr", "33", status);

}

static void t_jd2cal(int *status)
/*
**  - - - - - - - - -
**   t _ j d 2 c a l
**  - - - - - - - - -
**
**  Test iauJd2cal function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauJd2cal, viv, vvd
**
**  This revision:  2013 August 7
*/
{
   double dj1, dj2, fd;
   int iy, im, id, j;


   dj1 = 2400000.5;
   dj2 = 50123.9999;

   j = iauJd2cal(dj1, dj2, &iy, &im, &id, &fd);

   viv(iy, 1996, "iauJd2cal", "y", status);
   viv(im, 2, "iauJd2cal", "m", status);
   viv(id, 10, "iauJd2cal", "d", status);
   vvd(fd, 0.9999, 1e-7, "iauJd2cal", "fd", status);
   viv(j, 0, "iauJd2cal", "j", status);

}

static void t_jdcalf(int *status)
/*
**  - - - - - - - - -
**   t _ j d c a l f
**  - - - - - - - - -
**
**  Test iauJdcalf function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauJdcalf, viv
**
**  This revision:  2013 August 7
*/
{
   double dj1, dj2;
   int iydmf[4], j;


   dj1 = 2400000.5;
   dj2 = 50123.9999;

   j = iauJdcalf(4, dj1, dj2, iydmf);

   viv(iydmf[0], 1996, "iauJdcalf", "y", status);
   viv(iydmf[1], 2, "iauJdcalf", "m", status);
   viv(iydmf[2], 10, "iauJdcalf", "d", status);
   viv(iydmf[3], 9999, "iauJdcalf", "f", status);

   viv(j, 0, "iauJdcalf", "j", status);

}

static void t_ld(int *status)
/*
**  - - - - -
**   t _ l d
**  - - - - -
**
**  Test iauLd function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauLd, vvd
*
**  This revision:  2013 October 2
*/
{
   double bm, p[3], q[3], e[3], em, dlim, p1[3];


   bm = 0.00028574;
   p[0] = -0.763276255;
   p[1] = -0.608633767;
   p[2] = -0.216735543;
   q[0] = -0.763276255;
   q[1] = -0.608633767;
   q[2] = -0.216735543;
   e[0] = 0.76700421;
   e[1] = 0.605629598;
   e[2] = 0.211937094;
   em = 8.91276983;
   dlim = 3e-10;

   iauLd(bm, p, q, e, em, dlim, p1);

   vvd(p1[0], -0.7632762548968159627, 1e-12,
               "iauLd", "1", status);
   vvd(p1[1], -0.6086337670823762701, 1e-12,
               "iauLd", "2", status);
   vvd(p1[2], -0.2167355431320546947, 1e-12,
               "iauLd", "3", status);

}

static void t_ldn(int *status)
/*
**  - - - - - -
**   t _ l d n
**  - - - - - -
**
**  Test iauLdn function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauLdn, vvd
**
**  This revision:  2013 October 2
*/
{
   int n;
   iauLDBODY b[3];
   double ob[3], sc[3], sn[3];


   n = 3;
   b[0].bm = 0.00028574;
   b[0].dl = 3e-10;
   b[0].pv[0][0] = -7.81014427;
   b[0].pv[0][1] = -5.60956681;
   b[0].pv[0][2] = -1.98079819;
   b[0].pv[1][0] =  0.0030723249;
   b[0].pv[1][1] = -0.00406995477;
   b[0].pv[1][2] = -0.00181335842;
   b[1].bm = 0.00095435;
   b[1].dl = 3e-9;
   b[1].pv[0][0] =  0.738098796;
   b[1].pv[0][1] =  4.63658692;
   b[1].pv[0][2] =  1.9693136;
   b[1].pv[1][0] = -0.00755816922;
   b[1].pv[1][1] =  0.00126913722;
   b[1].pv[1][2] =  0.000727999001;
   b[2].bm = 1.0;
   b[2].dl = 6e-6;
   b[2].pv[0][0] = -0.000712174377;
   b[2].pv[0][1] = -0.00230478303;
   b[2].pv[0][2] = -0.00105865966;
   b[2].pv[1][0] =  6.29235213e-6;
   b[2].pv[1][1] = -3.30888387e-7;
   b[2].pv[1][2] = -2.96486623e-7;
   ob[0] =  -0.974170437;
   ob[1] =  -0.2115201;
   ob[2] =  -0.0917583114;
   sc[0] =  -0.763276255;
   sc[1] =  -0.608633767;
   sc[2] =  -0.216735543;

   iauLdn(n, b, ob, sc, sn);

   vvd(sn[0], -0.7632762579693333866, 1e-12,
               "iauLdn", "1", status);
   vvd(sn[1], -0.6086337636093002660, 1e-12,
               "iauLdn", "2", status);
   vvd(sn[2], -0.2167355420646328159, 1e-12,
               "iauLdn", "3", status);

}

static void t_ldsun(int *status)
/*
**  - - - - - - - -
**   t _ l d s u n
**  - - - - - - - -
**
**  Test iauLdsun function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauLdsun, vvd
**
**  This revision:  2013 October 2
*/
{
   double p[3], e[3], em, p1[3];


   p[0] = -0.763276255;
   p[1] = -0.608633767;
   p[2] = -0.216735543;
   e[0] = -0.973644023;
   e[1] = -0.20925523;
   e[2] = -0.0907169552;
   em = 0.999809214;

   iauLdsun(p, e, em, p1);

   vvd(p1[0], -0.7632762580731413169, 1e-12,
               "iauLdsun", "1", status);
   vvd(p1[1], -0.6086337635262647900, 1e-12,
               "iauLdsun", "2", status);
   vvd(p1[2], -0.2167355419322321302, 1e-12,
               "iauLdsun", "3", status);

}

static void t_num00a(int *status)
/*
**  - - - - - - - - -
**   t _ n u m 0 0 a
**  - - - - - - - - -
**
**  Test iauNum00a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauNum00a, vvd
**
**  This revision:  2013 August 7
*/
{
   double rmatn[3][3];


   iauNum00a(2400000.5, 53736.0, rmatn);

   vvd(rmatn[0][0], 0.9999999999536227949, 1e-12,
       "iauNum00a", "11", status);
   vvd(rmatn[0][1], 0.8836238544090873336e-5, 1e-12,
       "iauNum00a", "12", status);
   vvd(rmatn[0][2], 0.3830835237722400669e-5, 1e-12,
       "iauNum00a", "13", status);

   vvd(rmatn[1][0], -0.8836082880798569274e-5, 1e-12,
       "iauNum00a", "21", status);
   vvd(rmatn[1][1], 0.9999999991354655028, 1e-12,
       "iauNum00a", "22", status);
   vvd(rmatn[1][2], -0.4063240865362499850e-4, 1e-12,
       "iauNum00a", "23", status);

   vvd(rmatn[2][0], -0.3831194272065995866e-5, 1e-12,
       "iauNum00a", "31", status);
   vvd(rmatn[2][1], 0.4063237480216291775e-4, 1e-12,
       "iauNum00a", "32", status);
   vvd(rmatn[2][2], 0.9999999991671660338, 1e-12,
       "iauNum00a", "33", status);

}

static void t_num00b(int *status)
/*
**  - - - - - - - - -
**   t _ n u m 0 0 b
**  - - - - - - - - -
**
**  Test iauNum00b function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauNum00b, vvd
**
**  This revision:  2013 August 7
*/
{
    double rmatn[3][3];

    iauNum00b(2400000.5, 53736, rmatn);

   vvd(rmatn[0][0], 0.9999999999536069682, 1e-12,
       "iauNum00b", "11", status);
   vvd(rmatn[0][1], 0.8837746144871248011e-5, 1e-12,
       "iauNum00b", "12", status);
   vvd(rmatn[0][2], 0.3831488838252202945e-5, 1e-12,
       "iauNum00b", "13", status);

   vvd(rmatn[1][0], -0.8837590456632304720e-5, 1e-12,
       "iauNum00b", "21", status);
   vvd(rmatn[1][1], 0.9999999991354692733, 1e-12,
       "iauNum00b", "22", status);
   vvd(rmatn[1][2], -0.4063198798559591654e-4, 1e-12,
       "iauNum00b", "23", status);

   vvd(rmatn[2][0], -0.3831847930134941271e-5, 1e-12,
       "iauNum00b", "31", status);
   vvd(rmatn[2][1], 0.4063195412258168380e-4, 1e-12,
       "iauNum00b", "32", status);
   vvd(rmatn[2][2], 0.9999999991671806225, 1e-12,
       "iauNum00b", "33", status);

}

static void t_num06a(int *status)
/*
**  - - - - - - - - -
**   t _ n u m 0 6 a
**  - - - - - - - - -
**
**  Test iauNum06a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauNum06a, vvd
**
**  This revision:  2013 August 7
*/
{
    double rmatn[3][3];

    iauNum06a(2400000.5, 53736, rmatn);

   vvd(rmatn[0][0], 0.9999999999536227668, 1e-12,
       "iauNum06a", "11", status);
   vvd(rmatn[0][1], 0.8836241998111535233e-5, 1e-12,
       "iauNum06a", "12", status);
   vvd(rmatn[0][2], 0.3830834608415287707e-5, 1e-12,
       "iauNum06a", "13", status);

   vvd(rmatn[1][0], -0.8836086334870740138e-5, 1e-12,
       "iauNum06a", "21", status);
   vvd(rmatn[1][1], 0.9999999991354657474, 1e-12,
       "iauNum06a", "22", status);
   vvd(rmatn[1][2], -0.4063240188248455065e-4, 1e-12,
       "iauNum06a", "23", status);

   vvd(rmatn[2][0], -0.3831193642839398128e-5, 1e-12,
       "iauNum06a", "31", status);
   vvd(rmatn[2][1], 0.4063236803101479770e-4, 1e-12,
       "iauNum06a", "32", status);
   vvd(rmatn[2][2], 0.9999999991671663114, 1e-12,
       "iauNum06a", "33", status);

}

static void t_numat(int *status)
/*
**  - - - - - - - -
**   t _ n u m a t
**  - - - - - - - -
**
**  Test iauNumat function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauNumat, vvd
**
**  This revision:  2013 August 7
*/
{
   double epsa, dpsi, deps, rmatn[3][3];


   epsa =  0.4090789763356509900;
   dpsi = -0.9630909107115582393e-5;
   deps =  0.4063239174001678826e-4;

   iauNumat(epsa, dpsi, deps, rmatn);

   vvd(rmatn[0][0], 0.9999999999536227949, 1e-12,
       "iauNumat", "11", status);
   vvd(rmatn[0][1], 0.8836239320236250577e-5, 1e-12,
       "iauNumat", "12", status);
   vvd(rmatn[0][2], 0.3830833447458251908e-5, 1e-12,
       "iauNumat", "13", status);

   vvd(rmatn[1][0], -0.8836083657016688588e-5, 1e-12,
       "iauNumat", "21", status);
   vvd(rmatn[1][1], 0.9999999991354654959, 1e-12,
       "iauNumat", "22", status);
   vvd(rmatn[1][2], -0.4063240865361857698e-4, 1e-12,
       "iauNumat", "23", status);

   vvd(rmatn[2][0], -0.3831192481833385226e-5, 1e-12,
       "iauNumat", "31", status);
   vvd(rmatn[2][1], 0.4063237480216934159e-4, 1e-12,
       "iauNumat", "32", status);
   vvd(rmatn[2][2], 0.9999999991671660407, 1e-12,
       "iauNumat", "33", status);

}

static void t_nut00a(int *status)
/*
**  - - - - - - - - -
**   t _ n u t 0 0 a
**  - - - - - - - - -
**
**  Test iauNut00a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauNut00a, vvd
**
**  This revision:  2013 August 7
*/
{
   double dpsi, deps;


   iauNut00a(2400000.5, 53736.0, &dpsi, &deps);

   vvd(dpsi, -0.9630909107115518431e-5, 1e-13,
       "iauNut00a", "dpsi", status);
   vvd(deps,  0.4063239174001678710e-4, 1e-13,
       "iauNut00a", "deps", status);

}

static void t_nut00b(int *status)
/*
**  - - - - - - - - -
**   t _ n u t 0 0 b
**  - - - - - - - - -
**
**  Test iauNut00b function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauNut00b, vvd
**
**  This revision:  2013 August 7
*/
{
   double dpsi, deps;


   iauNut00b(2400000.5, 53736.0, &dpsi, &deps);

   vvd(dpsi, -0.9632552291148362783e-5, 1e-13,
       "iauNut00b", "dpsi", status);
   vvd(deps,  0.4063197106621159367e-4, 1e-13,
       "iauNut00b", "deps", status);

}

static void t_nut06a(int *status)
/*
**  - - - - - - - - -
**   t _ n u t 0 6 a
**  - - - - - - - - -
**
**  Test iauNut06a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauNut06a, vvd
**
**  This revision:  2013 August 7
*/
{
   double dpsi, deps;


   iauNut06a(2400000.5, 53736.0, &dpsi, &deps);

   vvd(dpsi, -0.9630912025820308797e-5, 1e-13,
       "iauNut06a", "dpsi", status);
   vvd(deps,  0.4063238496887249798e-4, 1e-13,
       "iauNut06a", "deps", status);

}

static void t_nut80(int *status)
/*
**  - - - - - - - -
**   t _ n u t 8 0
**  - - - - - - - -
**
**  Test iauNut80 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauNut80, vvd
**
**  This revision:  2013 August 7
*/
{
   double dpsi, deps;


   iauNut80(2400000.5, 53736.0, &dpsi, &deps);

   vvd(dpsi, -0.9643658353226563966e-5, 1e-13,
       "iauNut80", "dpsi", status);
   vvd(deps,  0.4060051006879713322e-4, 1e-13,
       "iauNut80", "deps", status);

}

static void t_nutm80(int *status)
/*
**  - - - - - - - - -
**   t _ n u t m 8 0
**  - - - - - - - - -
**
**  Test iauNutm80 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauNutm80, vvd
**
**  This revision:  2013 August 7
*/
{
   double rmatn[3][3];


   iauNutm80(2400000.5, 53736.0, rmatn);

   vvd(rmatn[0][0], 0.9999999999534999268, 1e-12,
      "iauNutm80", "11", status);
   vvd(rmatn[0][1], 0.8847935789636432161e-5, 1e-12,
      "iauNutm80", "12", status);
   vvd(rmatn[0][2], 0.3835906502164019142e-5, 1e-12,
      "iauNutm80", "13", status);

   vvd(rmatn[1][0], -0.8847780042583435924e-5, 1e-12,
      "iauNutm80", "21", status);
   vvd(rmatn[1][1], 0.9999999991366569963, 1e-12,
      "iauNutm80", "22", status);
   vvd(rmatn[1][2], -0.4060052702727130809e-4, 1e-12,
      "iauNutm80", "23", status);

   vvd(rmatn[2][0], -0.3836265729708478796e-5, 1e-12,
      "iauNutm80", "31", status);
   vvd(rmatn[2][1], 0.4060049308612638555e-4, 1e-12,
      "iauNutm80", "32", status);
   vvd(rmatn[2][2], 0.9999999991684415129, 1e-12,
      "iauNutm80", "33", status);

}

static void t_obl06(int *status)
/*
**  - - - - - - - -
**   t _ o b l 0 6
**  - - - - - - - -
**
**  Test iauObl06 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauObl06, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauObl06(2400000.5, 54388.0), 0.4090749229387258204, 1e-14,
       "iauObl06", "", status);
}

static void t_obl80(int *status)
/*
**  - - - - - - - -
**   t _ o b l 8 0
**  - - - - - - - -
**
**  Test iauObl80 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauObl80, vvd
**
**  This revision:  2013 August 7
*/
{
   double eps0;


   eps0 = iauObl80(2400000.5, 54388.0);

   vvd(eps0, 0.4090751347643816218, 1e-14, "iauObl80", "", status);

}

static void t_p06e(int *status)
/*
**  - - - - - - -
**   t _ p 0 6 e
**  - - - - - - -
**
**  Test iauP06e function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauP06e, vvd
**
**  This revision:  2013 August 7
*/
{
    double eps0, psia, oma, bpa, bqa, pia, bpia,
           epsa, chia, za, zetaa, thetaa, pa, gam, phi, psi;


   iauP06e(2400000.5, 52541.0, &eps0, &psia, &oma, &bpa,
           &bqa, &pia, &bpia, &epsa, &chia, &za,
           &zetaa, &thetaa, &pa, &gam, &phi, &psi);

   vvd(eps0, 0.4090926006005828715, 1e-14,
       "iauP06e", "eps0", status);
   vvd(psia, 0.6664369630191613431e-3, 1e-14,
       "iauP06e", "psia", status);
   vvd(oma , 0.4090925973783255982, 1e-14,
       "iauP06e", "oma", status);
   vvd(bpa, 0.5561149371265209445e-6, 1e-14,
       "iauP06e", "bpa", status);
   vvd(bqa, -0.6191517193290621270e-5, 1e-14,
       "iauP06e", "bqa", status);
   vvd(pia, 0.6216441751884382923e-5, 1e-14,
       "iauP06e", "pia", status);
   vvd(bpia, 3.052014180023779882, 1e-14,
       "iauP06e", "bpia", status);
   vvd(epsa, 0.4090864054922431688, 1e-14,
       "iauP06e", "epsa", status);
   vvd(chia, 0.1387703379530915364e-5, 1e-14,
       "iauP06e", "chia", status);
   vvd(za, 0.2921789846651790546e-3, 1e-14,
       "iauP06e", "za", status);
   vvd(zetaa, 0.3178773290332009310e-3, 1e-14,
       "iauP06e", "zetaa", status);
   vvd(thetaa, 0.2650932701657497181e-3, 1e-14,
       "iauP06e", "thetaa", status);
   vvd(pa, 0.6651637681381016344e-3, 1e-14,
       "iauP06e", "pa", status);
   vvd(gam, 0.1398077115963754987e-5, 1e-14,
       "iauP06e", "gam", status);
   vvd(phi, 0.4090864090837462602, 1e-14,
       "iauP06e", "phi", status);
   vvd(psi, 0.6664464807480920325e-3, 1e-14,
       "iauP06e", "psi", status);

}

static void t_p2pv(int *status)
/*
**  - - - - - - -
**   t _ p 2 p v
**  - - - - - - -
**
**  Test iauP2pv function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauP2pv, vvd
**
**  This revision:  2013 August 7
*/
{
   double p[3], pv[2][3];


   p[0] = 0.25;
   p[1] = 1.2;
   p[2] = 3.0;

   pv[0][0] =  0.3;
   pv[0][1] =  1.2;
   pv[0][2] = -2.5;

   pv[1][0] = -0.5;
   pv[1][1] =  3.1;
   pv[1][2] =  0.9;

   iauP2pv(p, pv);

   vvd(pv[0][0], 0.25, 0.0, "iauP2pv", "p1", status);
   vvd(pv[0][1], 1.2,  0.0, "iauP2pv", "p2", status);
   vvd(pv[0][2], 3.0,  0.0, "iauP2pv", "p3", status);

   vvd(pv[1][0], 0.0,  0.0, "iauP2pv", "v1", status);
   vvd(pv[1][1], 0.0,  0.0, "iauP2pv", "v2", status);
   vvd(pv[1][2], 0.0,  0.0, "iauP2pv", "v3", status);

}

static void t_p2s(int *status)
/*
**  - - - - - -
**   t _ p 2 s
**  - - - - - -
**
**  Test iauP2s function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauP2s, vvd
**
**  This revision:  2013 August 7
*/
{
   double p[3], theta, phi, r;


   p[0] = 100.0;
   p[1] = -50.0;
   p[2] =  25.0;

   iauP2s(p, &theta, &phi, &r);

   vvd(theta, -0.4636476090008061162, 1e-12, "iauP2s", "theta", status);
   vvd(phi, 0.2199879773954594463, 1e-12, "iauP2s", "phi", status);
   vvd(r, 114.5643923738960002, 1e-9, "iauP2s", "r", status);

}

static void t_pap(int *status)
/*
**  - - - - - -
**   t _ p a p
**  - - - - - -
**
**  Test iauPap function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPap, vvd
**
**  This revision:  2013 August 7
*/
{
   double a[3], b[3], theta;


   a[0] =  1.0;
   a[1] =  0.1;
   a[2] =  0.2;

   b[0] = -3.0;
   b[1] = 1e-3;
   b[2] =  0.2;

   theta = iauPap(a, b);

   vvd(theta, 0.3671514267841113674, 1e-12, "iauPap", "", status);

}

static void t_pas(int *status)
/*
**  - - - - - -
**   t _ p a s
**  - - - - - -
**
**  Test iauPas function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPas, vvd
**
**  This revision:  2013 August 7
*/
{
   double al, ap, bl, bp, theta;


   al =  1.0;
   ap =  0.1;
   bl =  0.2;
   bp = -1.0;

   theta = iauPas(al, ap, bl, bp);

   vvd(theta, -2.724544922932270424, 1e-12, "iauPas", "", status);

}

static void t_pb06(int *status)
/*
**  - - - - - - -
**   t _ p b 0 6
**  - - - - - - -
**
**  Test iauPb06 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPb06, vvd
**
**  This revision:  2013 August 7
*/
{
   double bzeta, bz, btheta;


   iauPb06(2400000.5, 50123.9999, &bzeta, &bz, &btheta);

   vvd(bzeta, -0.5092634016326478238e-3, 1e-12,
       "iauPb06", "bzeta", status);
   vvd(bz, -0.3602772060566044413e-3, 1e-12,
       "iauPb06", "bz", status);
   vvd(btheta, -0.3779735537167811177e-3, 1e-12,
       "iauPb06", "btheta", status);

}

static void t_pdp(int *status)
/*
**  - - - - - -
**   t _ p d p
**  - - - - - -
**
**  Test iauPdp function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPdp, vvd
**
**  This revision:  2013 August 7
*/
{
   double a[3], b[3], adb;


   a[0] = 2.0;
   a[1] = 2.0;
   a[2] = 3.0;

   b[0] = 1.0;
   b[1] = 3.0;
   b[2] = 4.0;

   adb = iauPdp(a, b);

   vvd(adb, 20, 1e-12, "iauPdp", "", status);

}

static void t_pfw06(int *status)
/*
**  - - - - - - - -
**   t _ p f w 0 6
**  - - - - - - - -
**
**  Test iauPfw06 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPfw06, vvd
**
**  This revision:  2013 August 7
*/
{
   double gamb, phib, psib, epsa;


   iauPfw06(2400000.5, 50123.9999, &gamb, &phib, &psib, &epsa);

   vvd(gamb, -0.2243387670997995690e-5, 1e-16,
       "iauPfw06", "gamb", status);
   vvd(phib,  0.4091014602391312808, 1e-12,
       "iauPfw06", "phib", status);
   vvd(psib, -0.9501954178013031895e-3, 1e-14,
       "iauPfw06", "psib", status);
   vvd(epsa,  0.4091014316587367491, 1e-12,
       "iauPfw06", "epsa", status);

}

static void t_plan94(int *status)
/*
**  - - - - - - - - -
**   t _ p l a n 9 4
**  - - - - - - - - -
**
**  Test iauPlan94 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPlan94, vvd, viv
**
**  This revision:  2013 October 2
*/
{
   double pv[2][3];
   int j;


   j = iauPlan94(2400000.5, 1e6, 0, pv);

   vvd(pv[0][0], 0.0, 0.0, "iauPlan94", "x 1", status);
   vvd(pv[0][1], 0.0, 0.0, "iauPlan94", "y 1", status);
   vvd(pv[0][2], 0.0, 0.0, "iauPlan94", "z 1", status);

   vvd(pv[1][0], 0.0, 0.0, "iauPlan94", "xd 1", status);
   vvd(pv[1][1], 0.0, 0.0, "iauPlan94", "yd 1", status);
   vvd(pv[1][2], 0.0, 0.0, "iauPlan94", "zd 1", status);

   viv(j, -1, "iauPlan94", "j 1", status);

   j = iauPlan94(2400000.5, 1e6, 10, pv);

   viv(j, -1, "iauPlan94", "j 2", status);

   j = iauPlan94(2400000.5, -320000, 3, pv);

   vvd(pv[0][0], 0.9308038666832975759, 1e-11,
       "iauPlan94", "x 3", status);
   vvd(pv[0][1], 0.3258319040261346000, 1e-11,
       "iauPlan94", "y 3", status);
   vvd(pv[0][2], 0.1422794544481140560, 1e-11,
       "iauPlan94", "z 3", status);

   vvd(pv[1][0], -0.6429458958255170006e-2, 1e-11,
       "iauPlan94", "xd 3", status);
   vvd(pv[1][1], 0.1468570657704237764e-1, 1e-11,
       "iauPlan94", "yd 3", status);
   vvd(pv[1][2], 0.6406996426270981189e-2, 1e-11,
       "iauPlan94", "zd 3", status);

   viv(j, 1, "iauPlan94", "j 3", status);

   j = iauPlan94(2400000.5, 43999.9, 1, pv);

   vvd(pv[0][0], 0.2945293959257430832, 1e-11,
       "iauPlan94", "x 4", status);
   vvd(pv[0][1], -0.2452204176601049596, 1e-11,
       "iauPlan94", "y 4", status);
   vvd(pv[0][2], -0.1615427700571978153, 1e-11,
       "iauPlan94", "z 4", status);

   vvd(pv[1][0], 0.1413867871404614441e-1, 1e-11,
       "iauPlan94", "xd 4", status);
   vvd(pv[1][1], 0.1946548301104706582e-1, 1e-11,
       "iauPlan94", "yd 4", status);
   vvd(pv[1][2], 0.8929809783898904786e-2, 1e-11,
       "iauPlan94", "zd 4", status);

   viv(j, 0, "iauPlan94", "j 4", status);

}

static void t_pmat00(int *status)
/*
**  - - - - - - - - -
**   t _ p m a t 0 0
**  - - - - - - - - -
**
**  Test iauPmat00 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPmat00, vvd
**
**  This revision:  2013 August 7
*/
{
   double rbp[3][3];


   iauPmat00(2400000.5, 50123.9999, rbp);

   vvd(rbp[0][0], 0.9999995505175087260, 1e-12,
       "iauPmat00", "11", status);
   vvd(rbp[0][1], 0.8695405883617884705e-3, 1e-14,
       "iauPmat00", "12", status);
   vvd(rbp[0][2], 0.3779734722239007105e-3, 1e-14,
       "iauPmat00", "13", status);

   vvd(rbp[1][0], -0.8695405990410863719e-3, 1e-14,
       "iauPmat00", "21", status);
   vvd(rbp[1][1], 0.9999996219494925900, 1e-12,
       "iauPmat00", "22", status);
   vvd(rbp[1][2], -0.1360775820404982209e-6, 1e-14,
       "iauPmat00", "23", status);

   vvd(rbp[2][0], -0.3779734476558184991e-3, 1e-14,
       "iauPmat00", "31", status);
   vvd(rbp[2][1], -0.1925857585832024058e-6, 1e-14,
       "iauPmat00", "32", status);
   vvd(rbp[2][2], 0.9999999285680153377, 1e-12,
       "iauPmat00", "33", status);

}

static void t_pmat06(int *status)
/*
**  - - - - - - - - -
**   t _ p m a t 0 6
**  - - - - - - - - -
**
**  Test iauPmat06 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPmat06, vvd
**
**  This revision:  2013 August 7
*/
{
   double rbp[3][3];


   iauPmat06(2400000.5, 50123.9999, rbp);

   vvd(rbp[0][0], 0.9999995505176007047, 1e-12,
       "iauPmat06", "11", status);
   vvd(rbp[0][1], 0.8695404617348208406e-3, 1e-14,
       "iauPmat06", "12", status);
   vvd(rbp[0][2], 0.3779735201865589104e-3, 1e-14,
       "iauPmat06", "13", status);

   vvd(rbp[1][0], -0.8695404723772031414e-3, 1e-14,
       "iauPmat06", "21", status);
   vvd(rbp[1][1], 0.9999996219496027161, 1e-12,
       "iauPmat06", "22", status);
   vvd(rbp[1][2], -0.1361752497080270143e-6, 1e-14,
       "iauPmat06", "23", status);

   vvd(rbp[2][0], -0.3779734957034089490e-3, 1e-14,
       "iauPmat06", "31", status);
   vvd(rbp[2][1], -0.1924880847894457113e-6, 1e-14,
       "iauPmat06", "32", status);
   vvd(rbp[2][2], 0.9999999285679971958, 1e-12,
       "iauPmat06", "33", status);

}

static void t_pmat76(int *status)
/*
**  - - - - - - - - -
**   t _ p m a t 7 6
**  - - - - - - - - -
**
**  Test iauPmat76 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPmat76, vvd
**
**  This revision:  2013 August 7
*/
{
   double rmatp[3][3];


   iauPmat76(2400000.5, 50123.9999, rmatp);

   vvd(rmatp[0][0], 0.9999995504328350733, 1e-12,
       "iauPmat76", "11", status);
   vvd(rmatp[0][1], 0.8696632209480960785e-3, 1e-14,
       "iauPmat76", "12", status);
   vvd(rmatp[0][2], 0.3779153474959888345e-3, 1e-14,
       "iauPmat76", "13", status);

   vvd(rmatp[1][0], -0.8696632209485112192e-3, 1e-14,
       "iauPmat76", "21", status);
   vvd(rmatp[1][1], 0.9999996218428560614, 1e-12,
       "iauPmat76", "22", status);
   vvd(rmatp[1][2], -0.1643284776111886407e-6, 1e-14,
       "iauPmat76", "23", status);

   vvd(rmatp[2][0], -0.3779153474950335077e-3, 1e-14,
       "iauPmat76", "31", status);
   vvd(rmatp[2][1], -0.1643306746147366896e-6, 1e-14,
       "iauPmat76", "32", status);
   vvd(rmatp[2][2], 0.9999999285899790119, 1e-12,
       "iauPmat76", "33", status);

}

static void t_pm(int *status)
/*
**  - - - - -
**   t _ p m
**  - - - - -
**
**  Test iauPm function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPm, vvd
**
**  This revision:  2013 August 7
*/
{
   double p[3], r;


   p[0] =  0.3;
   p[1] =  1.2;
   p[2] = -2.5;

   r = iauPm(p);

   vvd(r, 2.789265136196270604, 1e-12, "iauPm", "", status);

}

static void t_pmp(int *status)
/*
**  - - - - - -
**   t _ p m p
**  - - - - - -
**
**  Test iauPmp function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPmp, vvd
**
**  This revision:  2013 August 7
*/
{
   double a[3], b[3], amb[3];


   a[0] = 2.0;
   a[1] = 2.0;
   a[2] = 3.0;

   b[0] = 1.0;
   b[1] = 3.0;
   b[2] = 4.0;

   iauPmp(a, b, amb);

   vvd(amb[0],  1.0, 1e-12, "iauPmp", "0", status);
   vvd(amb[1], -1.0, 1e-12, "iauPmp", "1", status);
   vvd(amb[2], -1.0, 1e-12, "iauPmp", "2", status);

}

static void t_pmpx(int *status)
/*
**  - - - - - - -
**   t _ p m p x
**  - - - - - - -
**
**  Test iauPmpx function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPmpx, vvd
**
**  This revision:  2013 October 2
*/
{
   double rc, dc, pr, pd, px, rv, pmt, pob[3], pco[3];


   rc = 1.234;
   dc = 0.789;
   pr = 1e-5;
   pd = -2e-5;
   px = 1e-2;
   rv = 10.0;
   pmt = 8.75;
   pob[0] = 0.9;
   pob[1] = 0.4;
   pob[2] = 0.1;

   iauPmpx(rc, dc, pr, pd, px, rv, pmt, pob, pco);

   vvd(pco[0], 0.2328137623960308440, 1e-12,
               "iauPmpx", "1", status);
   vvd(pco[1], 0.6651097085397855317, 1e-12,
               "iauPmpx", "2", status);
   vvd(pco[2], 0.7095257765896359847, 1e-12,
               "iauPmpx", "3", status);

}

static void t_pmsafe(int *status)
/*
**  - - - - - - - - -
**   t _ p m s a f e
**  - - - - - - - - -
**
**  Test iauPmsafe function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPmsafe, vvd, viv
**
**  This revision:  2013 October 2
*/
{
   int j;
   double ra1, dec1, pmr1, pmd1, px1, rv1, ep1a, ep1b, ep2a, ep2b,
          ra2, dec2, pmr2, pmd2, px2, rv2;


   ra1 = 1.234;
   dec1 = 0.789;
   pmr1 = 1e-5;
   pmd1 = -2e-5;
   px1 = 1e-2;
   rv1 = 10.0;
   ep1a = 2400000.5;
   ep1b = 48348.5625;
   ep2a = 2400000.5;
   ep2b = 51544.5;

   j = iauPmsafe(ra1, dec1, pmr1, pmd1, px1, rv1,
                 ep1a, ep1b, ep2a, ep2b,
                 &ra2, &dec2, &pmr2, &pmd2, &px2, &rv2);

   vvd(ra2, 1.234087484501017061, 1e-12,
            "iauPmsafe", "ra2", status);
   vvd(dec2, 0.7888249982450468574, 1e-12,
            "iauPmsafe", "dec2", status);
   vvd(pmr2, 0.9996457663586073988e-5, 1e-12,
             "iauPmsafe", "pmr2", status);
   vvd(pmd2, -0.2000040085106737816e-4, 1e-16,
             "iauPmsafe", "pmd2", status);
   vvd(px2, 0.9999997295356765185e-2, 1e-12,
            "iauPmsafe", "px2", status);
   vvd(rv2, 10.38468380113917014, 1e-10,
            "iauPmsafe", "rv2", status);
   viv ( j, 0, "iauPmsafe", "j", status);

}

static void t_pn(int *status)
/*
**  - - - - -
**   t _ p n
**  - - - - -
**
**  Test iauPn function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPn, vvd
**
**  This revision:  2013 August 7
*/
{
   double p[3], r, u[3];


   p[0] =  0.3;
   p[1] =  1.2;
   p[2] = -2.5;

   iauPn(p, &r, u);

   vvd(r, 2.789265136196270604, 1e-12, "iauPn", "r", status);

   vvd(u[0], 0.1075552109073112058, 1e-12, "iauPn", "u1", status);
   vvd(u[1], 0.4302208436292448232, 1e-12, "iauPn", "u2", status);
   vvd(u[2], -0.8962934242275933816, 1e-12, "iauPn", "u3", status);

}

static void t_pn00(int *status)
/*
**  - - - - - - -
**   t _ p n 0 0
**  - - - - - - -
**
**  Test iauPn00 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPn00, vvd
**
**  This revision:  2013 August 7
*/
{
   double dpsi, deps, epsa,
          rb[3][3], rp[3][3], rbp[3][3], rn[3][3], rbpn[3][3];


   dpsi = -0.9632552291149335877e-5;
   deps =  0.4063197106621141414e-4;

   iauPn00(2400000.5, 53736.0, dpsi, deps,
           &epsa, rb, rp, rbp, rn, rbpn);

   vvd(epsa, 0.4090791789404229916, 1e-12, "iauPn00", "epsa", status);

   vvd(rb[0][0], 0.9999999999999942498, 1e-12,
       "iauPn00", "rb11", status);
   vvd(rb[0][1], -0.7078279744199196626e-7, 1e-18,
       "iauPn00", "rb12", status);
   vvd(rb[0][2], 0.8056217146976134152e-7, 1e-18,
       "iauPn00", "rb13", status);

   vvd(rb[1][0], 0.7078279477857337206e-7, 1e-18,
       "iauPn00", "rb21", status);
   vvd(rb[1][1], 0.9999999999999969484, 1e-12,
       "iauPn00", "rb22", status);
   vvd(rb[1][2], 0.3306041454222136517e-7, 1e-18,
       "iauPn00", "rb23", status);

   vvd(rb[2][0], -0.8056217380986972157e-7, 1e-18,
       "iauPn00", "rb31", status);
   vvd(rb[2][1], -0.3306040883980552500e-7, 1e-18,
       "iauPn00", "rb32", status);
   vvd(rb[2][2], 0.9999999999999962084, 1e-12,
       "iauPn00", "rb33", status);

   vvd(rp[0][0], 0.9999989300532289018, 1e-12,
       "iauPn00", "rp11", status);
   vvd(rp[0][1], -0.1341647226791824349e-2, 1e-14,
       "iauPn00", "rp12", status);
   vvd(rp[0][2], -0.5829880927190296547e-3, 1e-14,
       "iauPn00", "rp13", status);

   vvd(rp[1][0], 0.1341647231069759008e-2, 1e-14,
       "iauPn00", "rp21", status);
   vvd(rp[1][1], 0.9999990999908750433, 1e-12,
       "iauPn00", "rp22", status);
   vvd(rp[1][2], -0.3837444441583715468e-6, 1e-14,
       "iauPn00", "rp23", status);

   vvd(rp[2][0], 0.5829880828740957684e-3, 1e-14,
       "iauPn00", "rp31", status);
   vvd(rp[2][1], -0.3984203267708834759e-6, 1e-14,
       "iauPn00", "rp32", status);
   vvd(rp[2][2], 0.9999998300623538046, 1e-12,
       "iauPn00", "rp33", status);

   vvd(rbp[0][0], 0.9999989300052243993, 1e-12,
       "iauPn00", "rbp11", status);
   vvd(rbp[0][1], -0.1341717990239703727e-2, 1e-14,
       "iauPn00", "rbp12", status);
   vvd(rbp[0][2], -0.5829075749891684053e-3, 1e-14,
       "iauPn00", "rbp13", status);

   vvd(rbp[1][0], 0.1341718013831739992e-2, 1e-14,
       "iauPn00", "rbp21", status);
   vvd(rbp[1][1], 0.9999990998959191343, 1e-12,
       "iauPn00", "rbp22", status);
   vvd(rbp[1][2], -0.3505759733565421170e-6, 1e-14,
       "iauPn00", "rbp23", status);

   vvd(rbp[2][0], 0.5829075206857717883e-3, 1e-14,
       "iauPn00", "rbp31", status);
   vvd(rbp[2][1], -0.4315219955198608970e-6, 1e-14,
       "iauPn00", "rbp32", status);
   vvd(rbp[2][2], 0.9999998301093036269, 1e-12,
       "iauPn00", "rbp33", status);

   vvd(rn[0][0], 0.9999999999536069682, 1e-12,
       "iauPn00", "rn11", status);
   vvd(rn[0][1], 0.8837746144872140812e-5, 1e-16,
       "iauPn00", "rn12", status);
   vvd(rn[0][2], 0.3831488838252590008e-5, 1e-16,
       "iauPn00", "rn13", status);

   vvd(rn[1][0], -0.8837590456633197506e-5, 1e-16,
       "iauPn00", "rn21", status);
   vvd(rn[1][1], 0.9999999991354692733, 1e-12,
       "iauPn00", "rn22", status);
   vvd(rn[1][2], -0.4063198798559573702e-4, 1e-16,
       "iauPn00", "rn23", status);

   vvd(rn[2][0], -0.3831847930135328368e-5, 1e-16,
       "iauPn00", "rn31", status);
   vvd(rn[2][1], 0.4063195412258150427e-4, 1e-16,
       "iauPn00", "rn32", status);
   vvd(rn[2][2], 0.9999999991671806225, 1e-12,
       "iauPn00", "rn33", status);

   vvd(rbpn[0][0], 0.9999989440499982806, 1e-12,
       "iauPn00", "rbpn11", status);
   vvd(rbpn[0][1], -0.1332880253640848301e-2, 1e-14,
       "iauPn00", "rbpn12", status);
   vvd(rbpn[0][2], -0.5790760898731087295e-3, 1e-14,
       "iauPn00", "rbpn13", status);

   vvd(rbpn[1][0], 0.1332856746979948745e-2, 1e-14,
       "iauPn00", "rbpn21", status);
   vvd(rbpn[1][1], 0.9999991109064768883, 1e-12,
       "iauPn00", "rbpn22", status);
   vvd(rbpn[1][2], -0.4097740555723063806e-4, 1e-14,
       "iauPn00", "rbpn23", status);

   vvd(rbpn[2][0], 0.5791301929950205000e-3, 1e-14,
       "iauPn00", "rbpn31", status);
   vvd(rbpn[2][1], 0.4020553681373702931e-4, 1e-14,
       "iauPn00", "rbpn32", status);
   vvd(rbpn[2][2], 0.9999998314958529887, 1e-12,
       "iauPn00", "rbpn33", status);

}

static void t_pn00a(int *status)
/*
**  - - - - - - - -
**   t _ p n 0 0 a
**  - - - - - - - -
**
**  Test iauPn00a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPn00a, vvd
**
**  This revision:  2013 August 7
*/
{
   double dpsi, deps, epsa,
          rb[3][3], rp[3][3], rbp[3][3], rn[3][3], rbpn[3][3];


   iauPn00a(2400000.5, 53736.0,
            &dpsi, &deps, &epsa, rb, rp, rbp, rn, rbpn);

   vvd(dpsi, -0.9630909107115518431e-5, 1e-12,
       "iauPn00a", "dpsi", status);
   vvd(deps,  0.4063239174001678710e-4, 1e-12,
       "iauPn00a", "deps", status);
   vvd(epsa,  0.4090791789404229916, 1e-12, "iauPn00a", "epsa", status);

   vvd(rb[0][0], 0.9999999999999942498, 1e-12,
       "iauPn00a", "rb11", status);
   vvd(rb[0][1], -0.7078279744199196626e-7, 1e-16,
       "iauPn00a", "rb12", status);
   vvd(rb[0][2], 0.8056217146976134152e-7, 1e-16,
       "iauPn00a", "rb13", status);

   vvd(rb[1][0], 0.7078279477857337206e-7, 1e-16,
       "iauPn00a", "rb21", status);
   vvd(rb[1][1], 0.9999999999999969484, 1e-12,
       "iauPn00a", "rb22", status);
   vvd(rb[1][2], 0.3306041454222136517e-7, 1e-16,
       "iauPn00a", "rb23", status);

   vvd(rb[2][0], -0.8056217380986972157e-7, 1e-16,
       "iauPn00a", "rb31", status);
   vvd(rb[2][1], -0.3306040883980552500e-7, 1e-16,
       "iauPn00a", "rb32", status);
   vvd(rb[2][2], 0.9999999999999962084, 1e-12,
       "iauPn00a", "rb33", status);

   vvd(rp[0][0], 0.9999989300532289018, 1e-12,
       "iauPn00a", "rp11", status);
   vvd(rp[0][1], -0.1341647226791824349e-2, 1e-14,
       "iauPn00a", "rp12", status);
   vvd(rp[0][2], -0.5829880927190296547e-3, 1e-14,
       "iauPn00a", "rp13", status);

   vvd(rp[1][0], 0.1341647231069759008e-2, 1e-14,
       "iauPn00a", "rp21", status);
   vvd(rp[1][1], 0.9999990999908750433, 1e-12,
       "iauPn00a", "rp22", status);
   vvd(rp[1][2], -0.3837444441583715468e-6, 1e-14,
       "iauPn00a", "rp23", status);

   vvd(rp[2][0], 0.5829880828740957684e-3, 1e-14,
       "iauPn00a", "rp31", status);
   vvd(rp[2][1], -0.3984203267708834759e-6, 1e-14,
       "iauPn00a", "rp32", status);
   vvd(rp[2][2], 0.9999998300623538046, 1e-12,
       "iauPn00a", "rp33", status);

   vvd(rbp[0][0], 0.9999989300052243993, 1e-12,
       "iauPn00a", "rbp11", status);
   vvd(rbp[0][1], -0.1341717990239703727e-2, 1e-14,
       "iauPn00a", "rbp12", status);
   vvd(rbp[0][2], -0.5829075749891684053e-3, 1e-14,
       "iauPn00a", "rbp13", status);

   vvd(rbp[1][0], 0.1341718013831739992e-2, 1e-14,
       "iauPn00a", "rbp21", status);
   vvd(rbp[1][1], 0.9999990998959191343, 1e-12,
       "iauPn00a", "rbp22", status);
   vvd(rbp[1][2], -0.3505759733565421170e-6, 1e-14,
       "iauPn00a", "rbp23", status);

   vvd(rbp[2][0], 0.5829075206857717883e-3, 1e-14,
       "iauPn00a", "rbp31", status);
   vvd(rbp[2][1], -0.4315219955198608970e-6, 1e-14,
       "iauPn00a", "rbp32", status);
   vvd(rbp[2][2], 0.9999998301093036269, 1e-12,
       "iauPn00a", "rbp33", status);

   vvd(rn[0][0], 0.9999999999536227949, 1e-12,
       "iauPn00a", "rn11", status);
   vvd(rn[0][1], 0.8836238544090873336e-5, 1e-14,
       "iauPn00a", "rn12", status);
   vvd(rn[0][2], 0.3830835237722400669e-5, 1e-14,
       "iauPn00a", "rn13", status);

   vvd(rn[1][0], -0.8836082880798569274e-5, 1e-14,
       "iauPn00a", "rn21", status);
   vvd(rn[1][1], 0.9999999991354655028, 1e-12,
       "iauPn00a", "rn22", status);
   vvd(rn[1][2], -0.4063240865362499850e-4, 1e-14,
       "iauPn00a", "rn23", status);

   vvd(rn[2][0], -0.3831194272065995866e-5, 1e-14,
       "iauPn00a", "rn31", status);
   vvd(rn[2][1], 0.4063237480216291775e-4, 1e-14,
       "iauPn00a", "rn32", status);
   vvd(rn[2][2], 0.9999999991671660338, 1e-12,
       "iauPn00a", "rn33", status);

   vvd(rbpn[0][0], 0.9999989440476103435, 1e-12,
       "iauPn00a", "rbpn11", status);
   vvd(rbpn[0][1], -0.1332881761240011763e-2, 1e-14,
       "iauPn00a", "rbpn12", status);
   vvd(rbpn[0][2], -0.5790767434730085751e-3, 1e-14,
       "iauPn00a", "rbpn13", status);

   vvd(rbpn[1][0], 0.1332858254308954658e-2, 1e-14,
       "iauPn00a", "rbpn21", status);
   vvd(rbpn[1][1], 0.9999991109044505577, 1e-12,
       "iauPn00a", "rbpn22", status);
   vvd(rbpn[1][2], -0.4097782710396580452e-4, 1e-14,
       "iauPn00a", "rbpn23", status);

   vvd(rbpn[2][0], 0.5791308472168152904e-3, 1e-14,
       "iauPn00a", "rbpn31", status);
   vvd(rbpn[2][1], 0.4020595661591500259e-4, 1e-14,
       "iauPn00a", "rbpn32", status);
   vvd(rbpn[2][2], 0.9999998314954572304, 1e-12,
       "iauPn00a", "rbpn33", status);

}

static void t_pn00b(int *status)
/*
**  - - - - - - - -
**   t _ p n 0 0 b
**  - - - - - - - -
**
**  Test iauPn00b function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPn00b, vvd
**
**  This revision:  2013 August 7
*/
{
   double dpsi, deps, epsa,
          rb[3][3], rp[3][3], rbp[3][3], rn[3][3], rbpn[3][3];


   iauPn00b(2400000.5, 53736.0, &dpsi, &deps, &epsa,
            rb, rp, rbp, rn, rbpn);

   vvd(dpsi, -0.9632552291148362783e-5, 1e-12,
       "iauPn00b", "dpsi", status);
   vvd(deps,  0.4063197106621159367e-4, 1e-12,
       "iauPn00b", "deps", status);
   vvd(epsa,  0.4090791789404229916, 1e-12, "iauPn00b", "epsa", status);

   vvd(rb[0][0], 0.9999999999999942498, 1e-12,
      "iauPn00b", "rb11", status);
   vvd(rb[0][1], -0.7078279744199196626e-7, 1e-16,
      "iauPn00b", "rb12", status);
   vvd(rb[0][2], 0.8056217146976134152e-7, 1e-16,
      "iauPn00b", "rb13", status);

   vvd(rb[1][0], 0.7078279477857337206e-7, 1e-16,
      "iauPn00b", "rb21", status);
   vvd(rb[1][1], 0.9999999999999969484, 1e-12,
      "iauPn00b", "rb22", status);
   vvd(rb[1][2], 0.3306041454222136517e-7, 1e-16,
      "iauPn00b", "rb23", status);

   vvd(rb[2][0], -0.8056217380986972157e-7, 1e-16,
      "iauPn00b", "rb31", status);
   vvd(rb[2][1], -0.3306040883980552500e-7, 1e-16,
      "iauPn00b", "rb32", status);
   vvd(rb[2][2], 0.9999999999999962084, 1e-12,
      "iauPn00b", "rb33", status);

   vvd(rp[0][0], 0.9999989300532289018, 1e-12,
      "iauPn00b", "rp11", status);
   vvd(rp[0][1], -0.1341647226791824349e-2, 1e-14,
      "iauPn00b", "rp12", status);
   vvd(rp[0][2], -0.5829880927190296547e-3, 1e-14,
      "iauPn00b", "rp13", status);

   vvd(rp[1][0], 0.1341647231069759008e-2, 1e-14,
      "iauPn00b", "rp21", status);
   vvd(rp[1][1], 0.9999990999908750433, 1e-12,
      "iauPn00b", "rp22", status);
   vvd(rp[1][2], -0.3837444441583715468e-6, 1e-14,
      "iauPn00b", "rp23", status);

   vvd(rp[2][0], 0.5829880828740957684e-3, 1e-14,
      "iauPn00b", "rp31", status);
   vvd(rp[2][1], -0.3984203267708834759e-6, 1e-14,
      "iauPn00b", "rp32", status);
   vvd(rp[2][2], 0.9999998300623538046, 1e-12,
      "iauPn00b", "rp33", status);

   vvd(rbp[0][0], 0.9999989300052243993, 1e-12,
      "iauPn00b", "rbp11", status);
   vvd(rbp[0][1], -0.1341717990239703727e-2, 1e-14,
      "iauPn00b", "rbp12", status);
   vvd(rbp[0][2], -0.5829075749891684053e-3, 1e-14,
      "iauPn00b", "rbp13", status);

   vvd(rbp[1][0], 0.1341718013831739992e-2, 1e-14,
      "iauPn00b", "rbp21", status);
   vvd(rbp[1][1], 0.9999990998959191343, 1e-12,
      "iauPn00b", "rbp22", status);
   vvd(rbp[1][2], -0.3505759733565421170e-6, 1e-14,
      "iauPn00b", "rbp23", status);

   vvd(rbp[2][0], 0.5829075206857717883e-3, 1e-14,
      "iauPn00b", "rbp31", status);
   vvd(rbp[2][1], -0.4315219955198608970e-6, 1e-14,
      "iauPn00b", "rbp32", status);
   vvd(rbp[2][2], 0.9999998301093036269, 1e-12,
      "iauPn00b", "rbp33", status);

   vvd(rn[0][0], 0.9999999999536069682, 1e-12,
      "iauPn00b", "rn11", status);
   vvd(rn[0][1], 0.8837746144871248011e-5, 1e-14,
      "iauPn00b", "rn12", status);
   vvd(rn[0][2], 0.3831488838252202945e-5, 1e-14,
      "iauPn00b", "rn13", status);

   vvd(rn[1][0], -0.8837590456632304720e-5, 1e-14,
      "iauPn00b", "rn21", status);
   vvd(rn[1][1], 0.9999999991354692733, 1e-12,
      "iauPn00b", "rn22", status);
   vvd(rn[1][2], -0.4063198798559591654e-4, 1e-14,
      "iauPn00b", "rn23", status);

   vvd(rn[2][0], -0.3831847930134941271e-5, 1e-14,
      "iauPn00b", "rn31", status);
   vvd(rn[2][1], 0.4063195412258168380e-4, 1e-14,
      "iauPn00b", "rn32", status);
   vvd(rn[2][2], 0.9999999991671806225, 1e-12,
      "iauPn00b", "rn33", status);

   vvd(rbpn[0][0], 0.9999989440499982806, 1e-12,
      "iauPn00b", "rbpn11", status);
   vvd(rbpn[0][1], -0.1332880253640849194e-2, 1e-14,
      "iauPn00b", "rbpn12", status);
   vvd(rbpn[0][2], -0.5790760898731091166e-3, 1e-14,
      "iauPn00b", "rbpn13", status);

   vvd(rbpn[1][0], 0.1332856746979949638e-2, 1e-14,
      "iauPn00b", "rbpn21", status);
   vvd(rbpn[1][1], 0.9999991109064768883, 1e-12,
      "iauPn00b", "rbpn22", status);
   vvd(rbpn[1][2], -0.4097740555723081811e-4, 1e-14,
      "iauPn00b", "rbpn23", status);

   vvd(rbpn[2][0], 0.5791301929950208873e-3, 1e-14,
      "iauPn00b", "rbpn31", status);
   vvd(rbpn[2][1], 0.4020553681373720832e-4, 1e-14,
      "iauPn00b", "rbpn32", status);
   vvd(rbpn[2][2], 0.9999998314958529887, 1e-12,
      "iauPn00b", "rbpn33", status);

}

static void t_pn06a(int *status)
/*
**  - - - - - - - -
**   t _ p n 0 6 a
**  - - - - - - - -
**
**  Test iauPn06a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPn06a, vvd
**
**  This revision:  2013 August 7
*/
{
   double dpsi, deps, epsa;
   double rb[3][3], rp[3][3], rbp[3][3], rn[3][3], rbpn[3][3];


   iauPn06a(2400000.5, 53736.0, &dpsi, &deps, &epsa,
            rb, rp, rbp, rn, rbpn);

   vvd(dpsi, -0.9630912025820308797e-5, 1e-12,
       "iauPn06a", "dpsi", status);
   vvd(deps,  0.4063238496887249798e-4, 1e-12,
       "iauPn06a", "deps", status);
   vvd(epsa,  0.4090789763356509926, 1e-12, "iauPn06a", "epsa", status);

   vvd(rb[0][0], 0.9999999999999942497, 1e-12,
       "iauPn06a", "rb11", status);
   vvd(rb[0][1], -0.7078368960971557145e-7, 1e-14,
       "iauPn06a", "rb12", status);
   vvd(rb[0][2], 0.8056213977613185606e-7, 1e-14,
       "iauPn06a", "rb13", status);

   vvd(rb[1][0], 0.7078368694637674333e-7, 1e-14,
       "iauPn06a", "rb21", status);
   vvd(rb[1][1], 0.9999999999999969484, 1e-12,
       "iauPn06a", "rb22", status);
   vvd(rb[1][2], 0.3305943742989134124e-7, 1e-14,
       "iauPn06a", "rb23", status);

   vvd(rb[2][0], -0.8056214211620056792e-7, 1e-14,
       "iauPn06a", "rb31", status);
   vvd(rb[2][1], -0.3305943172740586950e-7, 1e-14,
       "iauPn06a", "rb32", status);
   vvd(rb[2][2], 0.9999999999999962084, 1e-12,
       "iauPn06a", "rb33", status);

   vvd(rp[0][0], 0.9999989300536854831, 1e-12,
       "iauPn06a", "rp11", status);
   vvd(rp[0][1], -0.1341646886204443795e-2, 1e-14,
       "iauPn06a", "rp12", status);
   vvd(rp[0][2], -0.5829880933488627759e-3, 1e-14,
       "iauPn06a", "rp13", status);

   vvd(rp[1][0], 0.1341646890569782183e-2, 1e-14,
       "iauPn06a", "rp21", status);
   vvd(rp[1][1], 0.9999990999913319321, 1e-12,
       "iauPn06a", "rp22", status);
   vvd(rp[1][2], -0.3835944216374477457e-6, 1e-14,
       "iauPn06a", "rp23", status);

   vvd(rp[2][0], 0.5829880833027867368e-3, 1e-14,
       "iauPn06a", "rp31", status);
   vvd(rp[2][1], -0.3985701514686976112e-6, 1e-14,
       "iauPn06a", "rp32", status);
   vvd(rp[2][2], 0.9999998300623534950, 1e-12,
       "iauPn06a", "rp33", status);

   vvd(rbp[0][0], 0.9999989300056797893, 1e-12,
       "iauPn06a", "rbp11", status);
   vvd(rbp[0][1], -0.1341717650545059598e-2, 1e-14,
       "iauPn06a", "rbp12", status);
   vvd(rbp[0][2], -0.5829075756493728856e-3, 1e-14,
       "iauPn06a", "rbp13", status);

   vvd(rbp[1][0], 0.1341717674223918101e-2, 1e-14,
       "iauPn06a", "rbp21", status);
   vvd(rbp[1][1], 0.9999990998963748448, 1e-12,
       "iauPn06a", "rbp22", status);
   vvd(rbp[1][2], -0.3504269280170069029e-6, 1e-14,
       "iauPn06a", "rbp23", status);

   vvd(rbp[2][0], 0.5829075211461454599e-3, 1e-14,
       "iauPn06a", "rbp31", status);
   vvd(rbp[2][1], -0.4316708436255949093e-6, 1e-14,
       "iauPn06a", "rbp32", status);
   vvd(rbp[2][2], 0.9999998301093032943, 1e-12,
       "iauPn06a", "rbp33", status);

   vvd(rn[0][0], 0.9999999999536227668, 1e-12,
       "iauPn06a", "rn11", status);
   vvd(rn[0][1], 0.8836241998111535233e-5, 1e-14,
       "iauPn06a", "rn12", status);
   vvd(rn[0][2], 0.3830834608415287707e-5, 1e-14,
       "iauPn06a", "rn13", status);

   vvd(rn[1][0], -0.8836086334870740138e-5, 1e-14,
       "iauPn06a", "rn21", status);
   vvd(rn[1][1], 0.9999999991354657474, 1e-12,
       "iauPn06a", "rn22", status);
   vvd(rn[1][2], -0.4063240188248455065e-4, 1e-14,
       "iauPn06a", "rn23", status);

   vvd(rn[2][0], -0.3831193642839398128e-5, 1e-14,
       "iauPn06a", "rn31", status);
   vvd(rn[2][1], 0.4063236803101479770e-4, 1e-14,
       "iauPn06a", "rn32", status);
   vvd(rn[2][2], 0.9999999991671663114, 1e-12,
       "iauPn06a", "rn33", status);

   vvd(rbpn[0][0], 0.9999989440480669738, 1e-12,
       "iauPn06a", "rbpn11", status);
   vvd(rbpn[0][1], -0.1332881418091915973e-2, 1e-14,
       "iauPn06a", "rbpn12", status);
   vvd(rbpn[0][2], -0.5790767447612042565e-3, 1e-14,
       "iauPn06a", "rbpn13", status);

   vvd(rbpn[1][0], 0.1332857911250989133e-2, 1e-14,
       "iauPn06a", "rbpn21", status);
   vvd(rbpn[1][1], 0.9999991109049141908, 1e-12,
       "iauPn06a", "rbpn22", status);
   vvd(rbpn[1][2], -0.4097767128546784878e-4, 1e-14,
       "iauPn06a", "rbpn23", status);

   vvd(rbpn[2][0], 0.5791308482835292617e-3, 1e-14,
       "iauPn06a", "rbpn31", status);
   vvd(rbpn[2][1], 0.4020580099454020310e-4, 1e-14,
       "iauPn06a", "rbpn32", status);
   vvd(rbpn[2][2], 0.9999998314954628695, 1e-12,
       "iauPn06a", "rbpn33", status);

}

static void t_pn06(int *status)
/*
**  - - - - - - -
**   t _ p n 0 6
**  - - - - - - -
**
**  Test iauPn06 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPn06, vvd
**
**  This revision:  2013 August 7
*/
{
   double dpsi, deps, epsa,
          rb[3][3], rp[3][3], rbp[3][3], rn[3][3], rbpn[3][3];


   dpsi = -0.9632552291149335877e-5;
   deps =  0.4063197106621141414e-4;

   iauPn06(2400000.5, 53736.0, dpsi, deps,
           &epsa, rb, rp, rbp, rn, rbpn);

   vvd(epsa, 0.4090789763356509926, 1e-12, "iauPn06", "epsa", status);

   vvd(rb[0][0], 0.9999999999999942497, 1e-12,
       "iauPn06", "rb11", status);
   vvd(rb[0][1], -0.7078368960971557145e-7, 1e-14,
       "iauPn06", "rb12", status);
   vvd(rb[0][2], 0.8056213977613185606e-7, 1e-14,
       "iauPn06", "rb13", status);

   vvd(rb[1][0], 0.7078368694637674333e-7, 1e-14,
       "iauPn06", "rb21", status);
   vvd(rb[1][1], 0.9999999999999969484, 1e-12,
       "iauPn06", "rb22", status);
   vvd(rb[1][2], 0.3305943742989134124e-7, 1e-14,
       "iauPn06", "rb23", status);

   vvd(rb[2][0], -0.8056214211620056792e-7, 1e-14,
       "iauPn06", "rb31", status);
   vvd(rb[2][1], -0.3305943172740586950e-7, 1e-14,
       "iauPn06", "rb32", status);
   vvd(rb[2][2], 0.9999999999999962084, 1e-12,
       "iauPn06", "rb33", status);

   vvd(rp[0][0], 0.9999989300536854831, 1e-12,
       "iauPn06", "rp11", status);
   vvd(rp[0][1], -0.1341646886204443795e-2, 1e-14,
       "iauPn06", "rp12", status);
   vvd(rp[0][2], -0.5829880933488627759e-3, 1e-14,
       "iauPn06", "rp13", status);

   vvd(rp[1][0], 0.1341646890569782183e-2, 1e-14,
       "iauPn06", "rp21", status);
   vvd(rp[1][1], 0.9999990999913319321, 1e-12,
       "iauPn06", "rp22", status);
   vvd(rp[1][2], -0.3835944216374477457e-6, 1e-14,
       "iauPn06", "rp23", status);

   vvd(rp[2][0], 0.5829880833027867368e-3, 1e-14,
       "iauPn06", "rp31", status);
   vvd(rp[2][1], -0.3985701514686976112e-6, 1e-14,
       "iauPn06", "rp32", status);
   vvd(rp[2][2], 0.9999998300623534950, 1e-12,
       "iauPn06", "rp33", status);

   vvd(rbp[0][0], 0.9999989300056797893, 1e-12,
       "iauPn06", "rbp11", status);
   vvd(rbp[0][1], -0.1341717650545059598e-2, 1e-14,
       "iauPn06", "rbp12", status);
   vvd(rbp[0][2], -0.5829075756493728856e-3, 1e-14,
       "iauPn06", "rbp13", status);

   vvd(rbp[1][0], 0.1341717674223918101e-2, 1e-14,
       "iauPn06", "rbp21", status);
   vvd(rbp[1][1], 0.9999990998963748448, 1e-12,
       "iauPn06", "rbp22", status);
   vvd(rbp[1][2], -0.3504269280170069029e-6, 1e-14,
       "iauPn06", "rbp23", status);

   vvd(rbp[2][0], 0.5829075211461454599e-3, 1e-14,
       "iauPn06", "rbp31", status);
   vvd(rbp[2][1], -0.4316708436255949093e-6, 1e-14,
       "iauPn06", "rbp32", status);
   vvd(rbp[2][2], 0.9999998301093032943, 1e-12,
       "iauPn06", "rbp33", status);

   vvd(rn[0][0], 0.9999999999536069682, 1e-12,
       "iauPn06", "rn11", status);
   vvd(rn[0][1], 0.8837746921149881914e-5, 1e-14,
       "iauPn06", "rn12", status);
   vvd(rn[0][2], 0.3831487047682968703e-5, 1e-14,
       "iauPn06", "rn13", status);

   vvd(rn[1][0], -0.8837591232983692340e-5, 1e-14,
       "iauPn06", "rn21", status);
   vvd(rn[1][1], 0.9999999991354692664, 1e-12,
       "iauPn06", "rn22", status);
   vvd(rn[1][2], -0.4063198798558931215e-4, 1e-14,
       "iauPn06", "rn23", status);

   vvd(rn[2][0], -0.3831846139597250235e-5, 1e-14,
       "iauPn06", "rn31", status);
   vvd(rn[2][1], 0.4063195412258792914e-4, 1e-14,
       "iauPn06", "rn32", status);
   vvd(rn[2][2], 0.9999999991671806293, 1e-12,
       "iauPn06", "rn33", status);

   vvd(rbpn[0][0], 0.9999989440504506688, 1e-12,
       "iauPn06", "rbpn11", status);
   vvd(rbpn[0][1], -0.1332879913170492655e-2, 1e-14,
       "iauPn06", "rbpn12", status);
   vvd(rbpn[0][2], -0.5790760923225655753e-3, 1e-14,
       "iauPn06", "rbpn13", status);

   vvd(rbpn[1][0], 0.1332856406595754748e-2, 1e-14,
       "iauPn06", "rbpn21", status);
   vvd(rbpn[1][1], 0.9999991109069366795, 1e-12,
       "iauPn06", "rbpn22", status);
   vvd(rbpn[1][2], -0.4097725651142641812e-4, 1e-14,
       "iauPn06", "rbpn23", status);

   vvd(rbpn[2][0], 0.5791301952321296716e-3, 1e-14,
       "iauPn06", "rbpn31", status);
   vvd(rbpn[2][1], 0.4020538796195230577e-4, 1e-14,
       "iauPn06", "rbpn32", status);
   vvd(rbpn[2][2], 0.9999998314958576778, 1e-12,
       "iauPn06", "rbpn33", status);

}

static void t_pnm00a(int *status)
/*
**  - - - - - - - - -
**   t _ p n m 0 0 a
**  - - - - - - - - -
**
**  Test iauPnm00a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPnm00a, vvd
**
**  This revision:  2013 August 7
*/
{
   double rbpn[3][3];


   iauPnm00a(2400000.5, 50123.9999, rbpn);

   vvd(rbpn[0][0], 0.9999995832793134257, 1e-12,
       "iauPnm00a", "11", status);
   vvd(rbpn[0][1], 0.8372384254137809439e-3, 1e-14,
       "iauPnm00a", "12", status);
   vvd(rbpn[0][2], 0.3639684306407150645e-3, 1e-14,
       "iauPnm00a", "13", status);

   vvd(rbpn[1][0], -0.8372535226570394543e-3, 1e-14,
       "iauPnm00a", "21", status);
   vvd(rbpn[1][1], 0.9999996486491582471, 1e-12,
       "iauPnm00a", "22", status);
   vvd(rbpn[1][2], 0.4132915262664072381e-4, 1e-14,
       "iauPnm00a", "23", status);

   vvd(rbpn[2][0], -0.3639337004054317729e-3, 1e-14,
       "iauPnm00a", "31", status);
   vvd(rbpn[2][1], -0.4163386925461775873e-4, 1e-14,
       "iauPnm00a", "32", status);
   vvd(rbpn[2][2], 0.9999999329094390695, 1e-12,
       "iauPnm00a", "33", status);

}

static void t_pnm00b(int *status)
/*
**  - - - - - - - - -
**   t _ p n m 0 0 b
**  - - - - - - - - -
**
**  Test iauPnm00b function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPnm00b, vvd
**
**  This revision:  2013 August 7
*/
{
   double rbpn[3][3];


   iauPnm00b(2400000.5, 50123.9999, rbpn);

   vvd(rbpn[0][0], 0.9999995832776208280, 1e-12,
       "iauPnm00b", "11", status);
   vvd(rbpn[0][1], 0.8372401264429654837e-3, 1e-14,
       "iauPnm00b", "12", status);
   vvd(rbpn[0][2], 0.3639691681450271771e-3, 1e-14,
       "iauPnm00b", "13", status);

   vvd(rbpn[1][0], -0.8372552234147137424e-3, 1e-14,
       "iauPnm00b", "21", status);
   vvd(rbpn[1][1], 0.9999996486477686123, 1e-12,
       "iauPnm00b", "22", status);
   vvd(rbpn[1][2], 0.4132832190946052890e-4, 1e-14,
       "iauPnm00b", "23", status);

   vvd(rbpn[2][0], -0.3639344385341866407e-3, 1e-14,
       "iauPnm00b", "31", status);
   vvd(rbpn[2][1], -0.4163303977421522785e-4, 1e-14,
       "iauPnm00b", "32", status);
   vvd(rbpn[2][2], 0.9999999329092049734, 1e-12,
       "iauPnm00b", "33", status);

}

static void t_pnm06a(int *status)
/*
**  - - - - - - - - -
**   t _ p n m 0 6 a
**  - - - - - - - - -
**
**  Test iauPnm06a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPnm06a, vvd
**
**  This revision:  2013 August 7
*/
{
   double rbpn[3][3];


   iauPnm06a(2400000.5, 50123.9999, rbpn);

   vvd(rbpn[0][0], 0.9999995832794205484, 1e-12,
       "iauPnm06a", "11", status);
   vvd(rbpn[0][1], 0.8372382772630962111e-3, 1e-14,
       "iauPnm06a", "12", status);
   vvd(rbpn[0][2], 0.3639684771140623099e-3, 1e-14,
       "iauPnm06a", "13", status);

   vvd(rbpn[1][0], -0.8372533744743683605e-3, 1e-14,
       "iauPnm06a", "21", status);
   vvd(rbpn[1][1], 0.9999996486492861646, 1e-12,
       "iauPnm06a", "22", status);
   vvd(rbpn[1][2], 0.4132905944611019498e-4, 1e-14,
       "iauPnm06a", "23", status);

   vvd(rbpn[2][0], -0.3639337469629464969e-3, 1e-14,
       "iauPnm06a", "31", status);
   vvd(rbpn[2][1], -0.4163377605910663999e-4, 1e-14,
       "iauPnm06a", "32", status);
   vvd(rbpn[2][2], 0.9999999329094260057, 1e-12,
       "iauPnm06a", "33", status);

}

static void t_pnm80(int *status)
/*
**  - - - - - - - -
**   t _ p n m 8 0
**  - - - - - - - -
**
**  Test iauPnm80 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPnm80, vvd
**
**  This revision:  2013 August 7
*/
{
   double rmatpn[3][3];


   iauPnm80(2400000.5, 50123.9999, rmatpn);

   vvd(rmatpn[0][0], 0.9999995831934611169, 1e-12,
       "iauPnm80", "11", status);
   vvd(rmatpn[0][1], 0.8373654045728124011e-3, 1e-14,
       "iauPnm80", "12", status);
   vvd(rmatpn[0][2], 0.3639121916933106191e-3, 1e-14,
       "iauPnm80", "13", status);

   vvd(rmatpn[1][0], -0.8373804896118301316e-3, 1e-14,
       "iauPnm80", "21", status);
   vvd(rmatpn[1][1], 0.9999996485439674092, 1e-12,
       "iauPnm80", "22", status);
   vvd(rmatpn[1][2], 0.4130202510421549752e-4, 1e-14,
       "iauPnm80", "23", status);

   vvd(rmatpn[2][0], -0.3638774789072144473e-3, 1e-14,
       "iauPnm80", "31", status);
   vvd(rmatpn[2][1], -0.4160674085851722359e-4, 1e-14,
       "iauPnm80", "32", status);
   vvd(rmatpn[2][2], 0.9999999329310274805, 1e-12,
       "iauPnm80", "33", status);

}

static void t_pom00(int *status)
/*
**  - - - - - - - -
**   t _ p o m 0 0
**  - - - - - - - -
**
**  Test iauPom00 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPom00, vvd
**
**  This revision:  2013 August 7
*/
{
   double xp, yp, sp, rpom[3][3];


   xp =  2.55060238e-7;
   yp =  1.860359247e-6;
   sp = -0.1367174580728891460e-10;

   iauPom00(xp, yp, sp, rpom);

   vvd(rpom[0][0], 0.9999999999999674721, 1e-12,
       "iauPom00", "11", status);
   vvd(rpom[0][1], -0.1367174580728846989e-10, 1e-16,
       "iauPom00", "12", status);
   vvd(rpom[0][2], 0.2550602379999972345e-6, 1e-16,
       "iauPom00", "13", status);

   vvd(rpom[1][0], 0.1414624947957029801e-10, 1e-16,
       "iauPom00", "21", status);
   vvd(rpom[1][1], 0.9999999999982695317, 1e-12,
       "iauPom00", "22", status);
   vvd(rpom[1][2], -0.1860359246998866389e-5, 1e-16,
       "iauPom00", "23", status);

   vvd(rpom[2][0], -0.2550602379741215021e-6, 1e-16,
       "iauPom00", "31", status);
   vvd(rpom[2][1], 0.1860359247002414021e-5, 1e-16,
       "iauPom00", "32", status);
   vvd(rpom[2][2], 0.9999999999982370039, 1e-12,
       "iauPom00", "33", status);

}

static void t_ppp(int *status)
/*
**  - - - - - -
**   t _ p p p
**  - - - - - -
**
**  Test iauPpp function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPpp, vvd
**
**  This revision:  2013 August 7
*/
{
   double a[3], b[3], apb[3];


   a[0] = 2.0;
   a[1] = 2.0;
   a[2] = 3.0;

   b[0] = 1.0;
   b[1] = 3.0;
   b[2] = 4.0;

   iauPpp(a, b, apb);

   vvd(apb[0], 3.0, 1e-12, "iauPpp", "0", status);
   vvd(apb[1], 5.0, 1e-12, "iauPpp", "1", status);
   vvd(apb[2], 7.0, 1e-12, "iauPpp", "2", status);

}

static void t_ppsp(int *status)
/*
**  - - - - - - -
**   t _ p p s p
**  - - - - - - -
**
**  Test iauPpsp function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPpsp, vvd
**
**  This revision:  2013 August 7
*/
{
   double a[3], s, b[3], apsb[3];


   a[0] = 2.0;
   a[1] = 2.0;
   a[2] = 3.0;

   s = 5.0;

   b[0] = 1.0;
   b[1] = 3.0;
   b[2] = 4.0;

   iauPpsp(a, s, b, apsb);

   vvd(apsb[0], 7.0, 1e-12, "iauPpsp", "0", status);
   vvd(apsb[1], 17.0, 1e-12, "iauPpsp", "1", status);
   vvd(apsb[2], 23.0, 1e-12, "iauPpsp", "2", status);

}

static void t_pr00(int *status)
/*
**  - - - - - - -
**   t _ p r 0 0
**  - - - - - - -
**
**  Test iauPr00 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPr00, vvd
**
**  This revision:  2013 August 7
*/
{
   double dpsipr, depspr;

   iauPr00(2400000.5, 53736, &dpsipr, &depspr);

   vvd(dpsipr, -0.8716465172668347629e-7, 1e-22,
      "iauPr00", "dpsipr", status);
   vvd(depspr, -0.7342018386722813087e-8, 1e-22,
      "iauPr00", "depspr", status);

}

static void t_prec76(int *status)
/*
**  - - - - - - - - -
**   t _ p r e c 7 6
**  - - - - - - - - -
**
**  Test iauPrec76 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPrec76, vvd
**
**  This revision:  2013 August 7
*/
{
   double ep01, ep02, ep11, ep12, zeta, z, theta;


   ep01 = 2400000.5;
   ep02 = 33282.0;
   ep11 = 2400000.5;
   ep12 = 51544.0;

   iauPrec76(ep01, ep02, ep11, ep12, &zeta, &z, &theta);

   vvd(zeta,  0.5588961642000161243e-2, 1e-12,
       "iauPrec76", "zeta",  status);
   vvd(z,     0.5589922365870680624e-2, 1e-12,
       "iauPrec76", "z",     status);
   vvd(theta, 0.4858945471687296760e-2, 1e-12,
       "iauPrec76", "theta", status);

}

static void t_pv2p(int *status)
/*
**  - - - - - - -
**   t _ p v 2 p
**  - - - - - - -
**
**  Test iauPv2p function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPv2p, vvd
**
**  This revision:  2013 August 7
*/
{
   double pv[2][3], p[3];


   pv[0][0] =  0.3;
   pv[0][1] =  1.2;
   pv[0][2] = -2.5;

   pv[1][0] = -0.5;
   pv[1][1] =  3.1;
   pv[1][2] =  0.9;

   iauPv2p(pv, p);

   vvd(p[0],  0.3, 0.0, "iauPv2p", "1", status);
   vvd(p[1],  1.2, 0.0, "iauPv2p", "2", status);
   vvd(p[2], -2.5, 0.0, "iauPv2p", "3", status);

}

static void t_pv2s(int *status)
/*
**  - - - - - - -
**   t _ p v 2 s
**  - - - - - - -
**
**  Test iauPv2s function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPv2s, vvd
**
**  This revision:  2013 August 7
*/
{
   double pv[2][3], theta, phi, r, td, pd, rd;


   pv[0][0] = -0.4514964673880165;
   pv[0][1] =  0.03093394277342585;
   pv[0][2] =  0.05594668105108779;

   pv[1][0] =  1.292270850663260e-5;
   pv[1][1] =  2.652814182060692e-6;
   pv[1][2] =  2.568431853930293e-6;

   iauPv2s(pv, &theta, &phi, &r, &td, &pd, &rd);

   vvd(theta, 3.073185307179586515, 1e-12, "iauPv2s", "theta", status);
   vvd(phi, 0.1229999999999999992, 1e-12, "iauPv2s", "phi", status);
   vvd(r, 0.4559999999999999757, 1e-12, "iauPv2s", "r", status);
   vvd(td, -0.7800000000000000364e-5, 1e-16, "iauPv2s", "td", status);
   vvd(pd, 0.9010000000000001639e-5, 1e-16, "iauPv2s", "pd", status);
   vvd(rd, -0.1229999999999999832e-4, 1e-16, "iauPv2s", "rd", status);

}

static void t_pvdpv(int *status)
/*
**  - - - - - - - -
**   t _ p v d p v
**  - - - - - - - -
**
**  Test iauPvdpv function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPvdpv, vvd
**
**  This revision:  2013 August 7
*/
{
   double a[2][3], b[2][3], adb[2];


   a[0][0] = 2.0;
   a[0][1] = 2.0;
   a[0][2] = 3.0;

   a[1][0] = 6.0;
   a[1][1] = 0.0;
   a[1][2] = 4.0;

   b[0][0] = 1.0;
   b[0][1] = 3.0;
   b[0][2] = 4.0;

   b[1][0] = 0.0;
   b[1][1] = 2.0;
   b[1][2] = 8.0;

   iauPvdpv(a, b, adb);

   vvd(adb[0], 20.0, 1e-12, "iauPvdpv", "1", status);
   vvd(adb[1], 50.0, 1e-12, "iauPvdpv", "2", status);

}

static void t_pvm(int *status)
/*
**  - - - - - -
**   t _ p v m
**  - - - - - -
**
**  Test iauPvm function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPvm, vvd
**
**  This revision:  2013 August 7
*/
{
   double pv[2][3], r, s;


   pv[0][0] =  0.3;
   pv[0][1] =  1.2;
   pv[0][2] = -2.5;

   pv[1][0] =  0.45;
   pv[1][1] = -0.25;
   pv[1][2] =  1.1;

   iauPvm(pv, &r, &s);

   vvd(r, 2.789265136196270604, 1e-12, "iauPvm", "r", status);
   vvd(s, 1.214495780149111922, 1e-12, "iauPvm", "s", status);

}

static void t_pvmpv(int *status)
/*
**  - - - - - - - -
**   t _ p v m p v
**  - - - - - - - -
**
**  Test iauPvmpv function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPvmpv, vvd
**
**  This revision:  2013 August 7
*/
{
   double a[2][3], b[2][3], amb[2][3];


   a[0][0] = 2.0;
   a[0][1] = 2.0;
   a[0][2] = 3.0;

   a[1][0] = 5.0;
   a[1][1] = 6.0;
   a[1][2] = 3.0;

   b[0][0] = 1.0;
   b[0][1] = 3.0;
   b[0][2] = 4.0;

   b[1][0] = 3.0;
   b[1][1] = 2.0;
   b[1][2] = 1.0;

   iauPvmpv(a, b, amb);

   vvd(amb[0][0],  1.0, 1e-12, "iauPvmpv", "11", status);
   vvd(amb[0][1], -1.0, 1e-12, "iauPvmpv", "21", status);
   vvd(amb[0][2], -1.0, 1e-12, "iauPvmpv", "31", status);

   vvd(amb[1][0],  2.0, 1e-12, "iauPvmpv", "12", status);
   vvd(amb[1][1],  4.0, 1e-12, "iauPvmpv", "22", status);
   vvd(amb[1][2],  2.0, 1e-12, "iauPvmpv", "32", status);

}

static void t_pvppv(int *status)
/*
**  - - - - - - - -
**   t _ p v p p v
**  - - - - - - - -
**
**  Test iauPvppv function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPvppv, vvd
**
**  This revision:  2013 August 7
*/
{
   double a[2][3], b[2][3], apb[2][3];


   a[0][0] = 2.0;
   a[0][1] = 2.0;
   a[0][2] = 3.0;

   a[1][0] = 5.0;
   a[1][1] = 6.0;
   a[1][2] = 3.0;

   b[0][0] = 1.0;
   b[0][1] = 3.0;
   b[0][2] = 4.0;

   b[1][0] = 3.0;
   b[1][1] = 2.0;
   b[1][2] = 1.0;

   iauPvppv(a, b, apb);

   vvd(apb[0][0], 3.0, 1e-12, "iauPvppv", "p1", status);
   vvd(apb[0][1], 5.0, 1e-12, "iauPvppv", "p2", status);
   vvd(apb[0][2], 7.0, 1e-12, "iauPvppv", "p3", status);

   vvd(apb[1][0], 8.0, 1e-12, "iauPvppv", "v1", status);
   vvd(apb[1][1], 8.0, 1e-12, "iauPvppv", "v2", status);
   vvd(apb[1][2], 4.0, 1e-12, "iauPvppv", "v3", status);

}

static void t_pvstar(int *status)
/*
**  - - - - - - - - -
**   t _ p v s t a r
**  - - - - - - - - -
**
**  Test iauPvstar function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPvstar, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double pv[2][3], ra, dec, pmr, pmd, px, rv;
   int j;


   pv[0][0] =  126668.5912743160601;
   pv[0][1] =  2136.792716839935195;
   pv[0][2] = -245251.2339876830091;

   pv[1][0] = -0.4051854035740712739e-2;
   pv[1][1] = -0.6253919754866173866e-2;
   pv[1][2] =  0.1189353719774107189e-1;

   j = iauPvstar(pv, &ra, &dec, &pmr, &pmd, &px, &rv);

   vvd(ra, 0.1686756e-1, 1e-12, "iauPvstar", "ra", status);
   vvd(dec, -1.093989828, 1e-12, "iauPvstar", "dec", status);
   vvd(pmr, -0.178323516e-4, 1e-16, "iauPvstar", "pmr", status);
   vvd(pmd, 0.2336024047e-5, 1e-16, "iauPvstar", "pmd", status);
   vvd(px, 0.74723, 1e-12, "iauPvstar", "px", status);
   vvd(rv, -21.6, 1e-11, "iauPvstar", "rv", status);

   viv(j, 0, "iauPvstar", "j", status);

}

static void t_pvtob(int *status)
/*
**  - - - - - - - -
**   t _ p v t o b
**  - - - - - - - -
**
**  Test iauPvtob function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPvtob, vvd
**
**  This revision:  2013 October 2
*/
{
   double elong, phi, hm, xp, yp, sp, theta, pv[2][3];


   elong = 2.0;
   phi = 0.5;
   hm = 3000.0;
   xp = 1e-6;
   yp = -0.5e-6;
   sp = 1e-8;
   theta = 5.0;

   iauPvtob(elong, phi, hm, xp, yp, sp, theta, pv);

   vvd(pv[0][0], 4225081.367071159207, 1e-5,
                 "iauPvtob", "p(1)", status);
   vvd(pv[0][1], 3681943.215856198144, 1e-5,
                 "iauPvtob", "p(2)", status);
   vvd(pv[0][2], 3041149.399241260785, 1e-5,
                 "iauPvtob", "p(3)", status);
   vvd(pv[1][0], -268.4915389365998787, 1e-9,
                 "iauPvtob", "v(1)", status);
   vvd(pv[1][1], 308.0977983288903123, 1e-9,
                 "iauPvtob", "v(2)", status);
   vvd(pv[1][2], 0, 0,
                 "iauPvtob", "v(3)", status);

}

static void t_pvu(int *status)
/*
**  - - - - - -
**   t _ p v u
**  - - - - - -
**
**  Test iauPvu function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPvu, vvd
**
**  This revision:  2013 August 7
*/
{
   double pv[2][3], upv[2][3];


   pv[0][0] =  126668.5912743160734;
   pv[0][1] =  2136.792716839935565;
   pv[0][2] = -245251.2339876830229;

   pv[1][0] = -0.4051854035740713039e-2;
   pv[1][1] = -0.6253919754866175788e-2;
   pv[1][2] =  0.1189353719774107615e-1;

   iauPvu(2920.0, pv, upv);

   vvd(upv[0][0], 126656.7598605317105, 1e-12,
       "iauPvu", "p1", status);
   vvd(upv[0][1], 2118.531271155726332, 1e-12,
       "iauPvu", "p2", status);
   vvd(upv[0][2], -245216.5048590656190, 1e-12,
       "iauPvu", "p3", status);

   vvd(upv[1][0], -0.4051854035740713039e-2, 1e-12,
       "iauPvu", "v1", status);
   vvd(upv[1][1], -0.6253919754866175788e-2, 1e-12,
       "iauPvu", "v2", status);
   vvd(upv[1][2], 0.1189353719774107615e-1, 1e-12,
       "iauPvu", "v3", status);

}

static void t_pvup(int *status)
/*
**  - - - - - - -
**   t _ p v u p
**  - - - - - - -
**
**  Test iauPvup function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPvup, vvd
**
**  This revision:  2013 August 7
*/
{
   double pv[2][3], p[3];


   pv[0][0] =  126668.5912743160734;
   pv[0][1] =  2136.792716839935565;
   pv[0][2] = -245251.2339876830229;

   pv[1][0] = -0.4051854035740713039e-2;
   pv[1][1] = -0.6253919754866175788e-2;
   pv[1][2] =  0.1189353719774107615e-1;

   iauPvup(2920.0, pv, p);

   vvd(p[0],  126656.7598605317105,   1e-12, "iauPvup", "1", status);
   vvd(p[1],    2118.531271155726332, 1e-12, "iauPvup", "2", status);
   vvd(p[2], -245216.5048590656190,   1e-12, "iauPvup", "3", status);

}

static void t_pvxpv(int *status)
/*
**  - - - - - - - -
**   t _ p v x p v
**  - - - - - - - -
**
**  Test iauPvxpv function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPvxpv, vvd
**
**  This revision:  2013 August 7
*/
{
   double a[2][3], b[2][3], axb[2][3];


   a[0][0] = 2.0;
   a[0][1] = 2.0;
   a[0][2] = 3.0;

   a[1][0] = 6.0;
   a[1][1] = 0.0;
   a[1][2] = 4.0;

   b[0][0] = 1.0;
   b[0][1] = 3.0;
   b[0][2] = 4.0;

   b[1][0] = 0.0;
   b[1][1] = 2.0;
   b[1][2] = 8.0;

   iauPvxpv(a, b, axb);

   vvd(axb[0][0],  -1.0, 1e-12, "iauPvxpv", "p1", status);
   vvd(axb[0][1],  -5.0, 1e-12, "iauPvxpv", "p2", status);
   vvd(axb[0][2],   4.0, 1e-12, "iauPvxpv", "p3", status);

   vvd(axb[1][0],  -2.0, 1e-12, "iauPvxpv", "v1", status);
   vvd(axb[1][1], -36.0, 1e-12, "iauPvxpv", "v2", status);
   vvd(axb[1][2],  22.0, 1e-12, "iauPvxpv", "v3", status);

}

static void t_pxp(int *status)
/*
**  - - - - - -
**   t _ p x p
**  - - - - - -
**
**  Test iauPxp function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauPxp, vvd
**
**  This revision:  2013 August 7
*/
{
   double a[3], b[3], axb[3];


   a[0] = 2.0;
   a[1] = 2.0;
   a[2] = 3.0;

   b[0] = 1.0;
   b[1] = 3.0;
   b[2] = 4.0;

   iauPxp(a, b, axb);

   vvd(axb[0], -1.0, 1e-12, "iauPxp", "1", status);
   vvd(axb[1], -5.0, 1e-12, "iauPxp", "2", status);
   vvd(axb[2],  4.0, 1e-12, "iauPxp", "3", status);

}

static void t_refco(int *status)
/*
**  - - - - - - - -
**   t _ r e f c o
**  - - - - - - - -
**
**  Test iauRefco function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauRefco, vvd
**
**  This revision:  2013 October 2
*/
{
   double phpa, tc, rh, wl, refa, refb;


   phpa = 800.0;
   tc = 10.0;
   rh = 0.9;
   wl = 0.4;

   iauRefco(phpa, tc, rh, wl, &refa, &refb);

   vvd(refa, 0.2264949956241415009e-3, 1e-15,
             "iauRefco", "refa", status);
   vvd(refb, -0.2598658261729343970e-6, 1e-18,
             "iauRefco", "refb", status);

}

static void t_rm2v(int *status)
/*
**  - - - - - - -
**   t _ r m 2 v
**  - - - - - - -
**
**  Test iauRm2v function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauRm2v, vvd
**
**  This revision:  2013 August 7
*/
{
   double r[3][3], w[3];


   r[0][0] =  0.00;
   r[0][1] = -0.80;
   r[0][2] = -0.60;

   r[1][0] =  0.80;
   r[1][1] = -0.36;
   r[1][2] =  0.48;

   r[2][0] =  0.60;
   r[2][1] =  0.48;
   r[2][2] = -0.64;

   iauRm2v(r, w);

   vvd(w[0],  0.0,                  1e-12, "iauRm2v", "1", status);
   vvd(w[1],  1.413716694115406957, 1e-12, "iauRm2v", "2", status);
   vvd(w[2], -1.884955592153875943, 1e-12, "iauRm2v", "3", status);

}

static void t_rv2m(int *status)
/*
**  - - - - - - -
**   t _ r v 2 m
**  - - - - - - -
**
**  Test iauRv2m function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauRv2m, vvd
**
**  This revision:  2013 August 7
*/
{
   double w[3], r[3][3];


   w[0] =  0.0;
   w[1] =  1.41371669;
   w[2] = -1.88495559;

   iauRv2m(w, r);

   vvd(r[0][0], -0.7071067782221119905, 1e-14, "iauRv2m", "11", status);
   vvd(r[0][1], -0.5656854276809129651, 1e-14, "iauRv2m", "12", status);
   vvd(r[0][2], -0.4242640700104211225, 1e-14, "iauRv2m", "13", status);

   vvd(r[1][0],  0.5656854276809129651, 1e-14, "iauRv2m", "21", status);
   vvd(r[1][1], -0.0925483394532274246, 1e-14, "iauRv2m", "22", status);
   vvd(r[1][2], -0.8194112531408833269, 1e-14, "iauRv2m", "23", status);

   vvd(r[2][0],  0.4242640700104211225, 1e-14, "iauRv2m", "31", status);
   vvd(r[2][1], -0.8194112531408833269, 1e-14, "iauRv2m", "32", status);
   vvd(r[2][2],  0.3854415612311154341, 1e-14, "iauRv2m", "33", status);

}

static void t_rx(int *status)
/*
**  - - - - -
**   t _ r x
**  - - - - -
**
**  Test iauRx function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauRx, vvd
**
**  This revision:  2013 August 7
*/
{
   double phi, r[3][3];


   phi = 0.3456789;

   r[0][0] = 2.0;
   r[0][1] = 3.0;
   r[0][2] = 2.0;

   r[1][0] = 3.0;
   r[1][1] = 2.0;
   r[1][2] = 3.0;

   r[2][0] = 3.0;
   r[2][1] = 4.0;
   r[2][2] = 5.0;

   iauRx(phi, r);

   vvd(r[0][0], 2.0, 0.0, "iauRx", "11", status);
   vvd(r[0][1], 3.0, 0.0, "iauRx", "12", status);
   vvd(r[0][2], 2.0, 0.0, "iauRx", "13", status);

   vvd(r[1][0], 3.839043388235612460, 1e-12, "iauRx", "21", status);
   vvd(r[1][1], 3.237033249594111899, 1e-12, "iauRx", "22", status);
   vvd(r[1][2], 4.516714379005982719, 1e-12, "iauRx", "23", status);

   vvd(r[2][0], 1.806030415924501684, 1e-12, "iauRx", "31", status);
   vvd(r[2][1], 3.085711545336372503, 1e-12, "iauRx", "32", status);
   vvd(r[2][2], 3.687721683977873065, 1e-12, "iauRx", "33", status);

}

static void t_rxp(int *status)
/*
**  - - - - - -
**   t _ r x p
**  - - - - - -
**
**  Test iauRxp function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauRxp, vvd
**
**  This revision:  2013 August 7
*/
{
   double r[3][3], p[3], rp[3];


   r[0][0] = 2.0;
   r[0][1] = 3.0;
   r[0][2] = 2.0;

   r[1][0] = 3.0;
   r[1][1] = 2.0;
   r[1][2] = 3.0;

   r[2][0] = 3.0;
   r[2][1] = 4.0;
   r[2][2] = 5.0;

   p[0] = 0.2;
   p[1] = 1.5;
   p[2] = 0.1;

   iauRxp(r, p, rp);

   vvd(rp[0], 5.1, 1e-12, "iauRxp", "1", status);
   vvd(rp[1], 3.9, 1e-12, "iauRxp", "2", status);
   vvd(rp[2], 7.1, 1e-12, "iauRxp", "3", status);

}

static void t_rxpv(int *status)
/*
**  - - - - - - -
**   t _ r x p v
**  - - - - - - -
**
**  Test iauRxpv function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauRxpv, vvd
**
**  This revision:  2013 August 7
*/
{
   double r[3][3], pv[2][3], rpv[2][3];


   r[0][0] = 2.0;
   r[0][1] = 3.0;
   r[0][2] = 2.0;

   r[1][0] = 3.0;
   r[1][1] = 2.0;
   r[1][2] = 3.0;

   r[2][0] = 3.0;
   r[2][1] = 4.0;
   r[2][2] = 5.0;

   pv[0][0] = 0.2;
   pv[0][1] = 1.5;
   pv[0][2] = 0.1;

   pv[1][0] = 1.5;
   pv[1][1] = 0.2;
   pv[1][2] = 0.1;

   iauRxpv(r, pv, rpv);

   vvd(rpv[0][0], 5.1, 1e-12, "iauRxpv", "11", status);
   vvd(rpv[1][0], 3.8, 1e-12, "iauRxpv", "12", status);

   vvd(rpv[0][1], 3.9, 1e-12, "iauRxpv", "21", status);
   vvd(rpv[1][1], 5.2, 1e-12, "iauRxpv", "22", status);

   vvd(rpv[0][2], 7.1, 1e-12, "iauRxpv", "31", status);
   vvd(rpv[1][2], 5.8, 1e-12, "iauRxpv", "32", status);

}

static void t_rxr(int *status)
/*
**  - - - - - -
**   t _ r x r
**  - - - - - -
**
**  Test iauRxr function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauRxr, vvd
**
**  This revision:  2013 August 7
*/
{
   double a[3][3], b[3][3], atb[3][3];


   a[0][0] = 2.0;
   a[0][1] = 3.0;
   a[0][2] = 2.0;

   a[1][0] = 3.0;
   a[1][1] = 2.0;
   a[1][2] = 3.0;

   a[2][0] = 3.0;
   a[2][1] = 4.0;
   a[2][2] = 5.0;

   b[0][0] = 1.0;
   b[0][1] = 2.0;
   b[0][2] = 2.0;

   b[1][0] = 4.0;
   b[1][1] = 1.0;
   b[1][2] = 1.0;

   b[2][0] = 3.0;
   b[2][1] = 0.0;
   b[2][2] = 1.0;

   iauRxr(a, b, atb);

   vvd(atb[0][0], 20.0, 1e-12, "iauRxr", "11", status);
   vvd(atb[0][1],  7.0, 1e-12, "iauRxr", "12", status);
   vvd(atb[0][2],  9.0, 1e-12, "iauRxr", "13", status);

   vvd(atb[1][0], 20.0, 1e-12, "iauRxr", "21", status);
   vvd(atb[1][1],  8.0, 1e-12, "iauRxr", "22", status);
   vvd(atb[1][2], 11.0, 1e-12, "iauRxr", "23", status);

   vvd(atb[2][0], 34.0, 1e-12, "iauRxr", "31", status);
   vvd(atb[2][1], 10.0, 1e-12, "iauRxr", "32", status);
   vvd(atb[2][2], 15.0, 1e-12, "iauRxr", "33", status);

}

static void t_ry(int *status)
/*
**  - - - - -
**   t _ r y
**  - - - - -
**
**  Test iauRy function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauRy, vvd
**
**  This revision:  2013 August 7
*/
{
   double theta, r[3][3];


   theta = 0.3456789;

   r[0][0] = 2.0;
   r[0][1] = 3.0;
   r[0][2] = 2.0;

   r[1][0] = 3.0;
   r[1][1] = 2.0;
   r[1][2] = 3.0;

   r[2][0] = 3.0;
   r[2][1] = 4.0;
   r[2][2] = 5.0;

   iauRy(theta, r);

   vvd(r[0][0], 0.8651847818978159930, 1e-12, "iauRy", "11", status);
   vvd(r[0][1], 1.467194920539316554, 1e-12, "iauRy", "12", status);
   vvd(r[0][2], 0.1875137911274457342, 1e-12, "iauRy", "13", status);

   vvd(r[1][0], 3, 1e-12, "iauRy", "21", status);
   vvd(r[1][1], 2, 1e-12, "iauRy", "22", status);
   vvd(r[1][2], 3, 1e-12, "iauRy", "23", status);

   vvd(r[2][0], 3.500207892850427330, 1e-12, "iauRy", "31", status);
   vvd(r[2][1], 4.779889022262298150, 1e-12, "iauRy", "32", status);
   vvd(r[2][2], 5.381899160903798712, 1e-12, "iauRy", "33", status);

}

static void t_rz(int *status)
/*
**  - - - - -
**   t _ r z
**  - - - - -
**
**  Test iauRz function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauRz, vvd
**
**  This revision:  2013 August 7
*/
{
   double psi, r[3][3];


   psi = 0.3456789;

   r[0][0] = 2.0;
   r[0][1] = 3.0;
   r[0][2] = 2.0;

   r[1][0] = 3.0;
   r[1][1] = 2.0;
   r[1][2] = 3.0;

   r[2][0] = 3.0;
   r[2][1] = 4.0;
   r[2][2] = 5.0;

   iauRz(psi, r);

   vvd(r[0][0], 2.898197754208926769, 1e-12, "iauRz", "11", status);
   vvd(r[0][1], 3.500207892850427330, 1e-12, "iauRz", "12", status);
   vvd(r[0][2], 2.898197754208926769, 1e-12, "iauRz", "13", status);

   vvd(r[1][0], 2.144865911309686813, 1e-12, "iauRz", "21", status);
   vvd(r[1][1], 0.865184781897815993, 1e-12, "iauRz", "22", status);
   vvd(r[1][2], 2.144865911309686813, 1e-12, "iauRz", "23", status);

   vvd(r[2][0], 3.0, 1e-12, "iauRz", "31", status);
   vvd(r[2][1], 4.0, 1e-12, "iauRz", "32", status);
   vvd(r[2][2], 5.0, 1e-12, "iauRz", "33", status);

}

static void t_s00a(int *status)
/*
**  - - - - - - -
**   t _ s 0 0 a
**  - - - - - - -
**
**  Test iauS00a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauS00a, vvd
**
**  This revision:  2013 August 7
*/
{
   double s;


   s = iauS00a(2400000.5, 52541.0);

   vvd(s, -0.1340684448919163584e-7, 1e-18, "iauS00a", "", status);

}

static void t_s00b(int *status)
/*
**  - - - - - - -
**   t _ s 0 0 b
**  - - - - - - -
**
**  Test iauS00b function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauS00b, vvd
**
**  This revision:  2013 August 7
*/
{
   double s;


   s = iauS00b(2400000.5, 52541.0);

   vvd(s, -0.1340695782951026584e-7, 1e-18, "iauS00b", "", status);

}

static void t_s00(int *status)
/*
**  - - - - - -
**   t _ s 0 0
**  - - - - - -
**
**  Test iauS00 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauS00, vvd
**
**  This revision:  2013 August 7
*/
{
   double x, y, s;


   x = 0.5791308486706011000e-3;
   y = 0.4020579816732961219e-4;

   s = iauS00(2400000.5, 53736.0, x, y);

   vvd(s, -0.1220036263270905693e-7, 1e-18, "iauS00", "", status);

}

static void t_s06a(int *status)
/*
**  - - - - - - -
**   t _ s 0 6 a
**  - - - - - - -
**
**  Test iauS06a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauS06a, vvd
**
**  This revision:  2013 August 7
*/
{
   double s;


   s = iauS06a(2400000.5, 52541.0);

   vvd(s, -0.1340680437291812383e-7, 1e-18, "iauS06a", "", status);

}

static void t_s06(int *status)
/*
**  - - - - - -
**   t _ s 0 6
**  - - - - - -
**
**  Test iauS06 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauS06, vvd
**
**  This revision:  2013 August 7
*/
{
   double x, y, s;


   x = 0.5791308486706011000e-3;
   y = 0.4020579816732961219e-4;

   s = iauS06(2400000.5, 53736.0, x, y);

   vvd(s, -0.1220032213076463117e-7, 1e-18, "iauS06", "", status);

}

static void t_s2c(int *status)
/*
**  - - - - - -
**   t _ s 2 c
**  - - - - - -
**
**  Test iauS2c function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauS2c, vvd
**
**  This revision:  2013 August 7
*/
{
   double c[3];


   iauS2c(3.0123, -0.999, c);

   vvd(c[0], -0.5366267667260523906, 1e-12, "iauS2c", "1", status);
   vvd(c[1],  0.0697711109765145365, 1e-12, "iauS2c", "2", status);
   vvd(c[2], -0.8409302618566214041, 1e-12, "iauS2c", "3", status);

}

static void t_s2p(int *status)
/*
**  - - - - - -
**   t _ s 2 p
**  - - - - - -
**
**  Test iauS2p function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauS2p, vvd
**
**  This revision:  2013 August 7
*/
{
   double p[3];


   iauS2p(-3.21, 0.123, 0.456, p);

   vvd(p[0], -0.4514964673880165228, 1e-12, "iauS2p", "x", status);
   vvd(p[1],  0.0309339427734258688, 1e-12, "iauS2p", "y", status);
   vvd(p[2],  0.0559466810510877933, 1e-12, "iauS2p", "z", status);

}

static void t_s2pv(int *status)
/*
**  - - - - - - -
**   t _ s 2 p v
**  - - - - - - -
**
**  Test iauS2pv function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauS2pv, vvd
**
**  This revision:  2013 August 7
*/
{
   double pv[2][3];


   iauS2pv(-3.21, 0.123, 0.456, -7.8e-6, 9.01e-6, -1.23e-5, pv);

   vvd(pv[0][0], -0.4514964673880165228, 1e-12, "iauS2pv", "x", status);
   vvd(pv[0][1],  0.0309339427734258688, 1e-12, "iauS2pv", "y", status);
   vvd(pv[0][2],  0.0559466810510877933, 1e-12, "iauS2pv", "z", status);

   vvd(pv[1][0],  0.1292270850663260170e-4, 1e-16,
       "iauS2pv", "vx", status);
   vvd(pv[1][1],  0.2652814182060691422e-5, 1e-16,
       "iauS2pv", "vy", status);
   vvd(pv[1][2],  0.2568431853930292259e-5, 1e-16,
       "iauS2pv", "vz", status);

}

static void t_s2xpv(int *status)
/*
**  - - - - - - - -
**   t _ s 2 x p v
**  - - - - - - - -
**
**  Test iauS2xpv function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauS2xpv, vvd
**
**  This revision:  2013 August 7
*/
{
   double s1, s2, pv[2][3], spv[2][3];


   s1 = 2.0;
   s2 = 3.0;

   pv[0][0] =  0.3;
   pv[0][1] =  1.2;
   pv[0][2] = -2.5;

   pv[1][0] =  0.5;
   pv[1][1] =  2.3;
   pv[1][2] = -0.4;

   iauS2xpv(s1, s2, pv, spv);

   vvd(spv[0][0],  0.6, 1e-12, "iauS2xpv", "p1", status);
   vvd(spv[0][1],  2.4, 1e-12, "iauS2xpv", "p2", status);
   vvd(spv[0][2], -5.0, 1e-12, "iauS2xpv", "p3", status);

   vvd(spv[1][0],  1.5, 1e-12, "iauS2xpv", "v1", status);
   vvd(spv[1][1],  6.9, 1e-12, "iauS2xpv", "v2", status);
   vvd(spv[1][2], -1.2, 1e-12, "iauS2xpv", "v3", status);

}

static void t_sepp(int *status)
/*
**  - - - - - - -
**   t _ s e p p
**  - - - - - - -
**
**  Test iauSepp function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauSepp, vvd
**
**  This revision:  2013 August 7
*/
{
   double a[3], b[3], s;


   a[0] =  1.0;
   a[1] =  0.1;
   a[2] =  0.2;

   b[0] = -3.0;
   b[1] =  1e-3;
   b[2] =  0.2;

   s = iauSepp(a, b);

   vvd(s, 2.860391919024660768, 1e-12, "iauSepp", "", status);

}

static void t_seps(int *status)
/*
**  - - - - - - -
**   t _ s e p s
**  - - - - - - -
**
**  Test iauSeps function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauSeps, vvd
**
**  This revision:  2013 August 7
*/
{
   double al, ap, bl, bp, s;


   al =  1.0;
   ap =  0.1;

   bl =  0.2;
   bp = -3.0;

   s = iauSeps(al, ap, bl, bp);

   vvd(s, 2.346722016996998842, 1e-14, "iauSeps", "", status);

}

static void t_sp00(int *status)
/*
**  - - - - - - -
**   t _ s p 0 0
**  - - - - - - -
**
**  Test iauSp00 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauSp00, vvd
**
**  This revision:  2013 August 7
*/
{
   vvd(iauSp00(2400000.5, 52541.0),
       -0.6216698469981019309e-11, 1e-12, "iauSp00", "", status);

}

static void t_starpm(int *status)
/*
**  - - - - - - - - -
**   t _ s t a r p m
**  - - - - - - - - -
**
**  Test iauStarpm function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauStarpm, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double ra1, dec1, pmr1, pmd1, px1, rv1;
   double ra2, dec2, pmr2, pmd2, px2, rv2;
   int j;


   ra1 =   0.01686756;
   dec1 = -1.093989828;
   pmr1 = -1.78323516e-5;
   pmd1 =  2.336024047e-6;
   px1 =   0.74723;
   rv1 = -21.6;

   j = iauStarpm(ra1, dec1, pmr1, pmd1, px1, rv1,
                 2400000.5, 50083.0, 2400000.5, 53736.0,
                 &ra2, &dec2, &pmr2, &pmd2, &px2, &rv2);

   vvd(ra2, 0.01668919069414242368, 1e-13,
       "iauStarpm", "ra", status);
   vvd(dec2, -1.093966454217127879, 1e-13,
       "iauStarpm", "dec", status);
   vvd(pmr2, -0.1783662682155932702e-4, 1e-17,
       "iauStarpm", "pmr", status);
   vvd(pmd2, 0.2338092915987603664e-5, 1e-17,
       "iauStarpm", "pmd", status);
   vvd(px2, 0.7473533835323493644, 1e-13,
       "iauStarpm", "px", status);
   vvd(rv2, -21.59905170476860786, 1e-11,
       "iauStarpm", "rv", status);

   viv(j, 0, "iauStarpm", "j", status);

}

static void t_starpv(int *status)
/*
**  - - - - - - - - -
**   t _ s t a r p v
**  - - - - - - - - -
**
**  Test iauStarpv function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauStarpv, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double ra, dec, pmr, pmd, px, rv, pv[2][3];
   int j;


   ra =   0.01686756;
   dec = -1.093989828;
   pmr = -1.78323516e-5;
   pmd =  2.336024047e-6;
   px =   0.74723;
   rv = -21.6;

   j = iauStarpv(ra, dec, pmr, pmd, px, rv, pv);

   vvd(pv[0][0], 126668.5912743160601, 1e-10,
       "iauStarpv", "11", status);
   vvd(pv[0][1], 2136.792716839935195, 1e-12,
       "iauStarpv", "12", status);
   vvd(pv[0][2], -245251.2339876830091, 1e-10,
       "iauStarpv", "13", status);

   vvd(pv[1][0], -0.4051854035740712739e-2, 1e-13,
       "iauStarpv", "21", status);
   vvd(pv[1][1], -0.6253919754866173866e-2, 1e-15,
       "iauStarpv", "22", status);
   vvd(pv[1][2], 0.1189353719774107189e-1, 1e-13,
       "iauStarpv", "23", status);

   viv(j, 0, "iauStarpv", "j", status);

}

static void t_sxp(int *status)
/*
**  - - - - - -
**   t _ s x p
**  - - - - - -
**
**  Test iauSxp function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauSxp, vvd
**
**  This revision:  2013 August 7
*/
{
   double s, p[3], sp[3];


   s = 2.0;

   p[0] =  0.3;
   p[1] =  1.2;
   p[2] = -2.5;

   iauSxp(s, p, sp);

   vvd(sp[0],  0.6, 0.0, "iauSxp", "1", status);
   vvd(sp[1],  2.4, 0.0, "iauSxp", "2", status);
   vvd(sp[2], -5.0, 0.0, "iauSxp", "3", status);

}


static void t_sxpv(int *status)
/*
**  - - - - - - -
**   t _ s x p v
**  - - - - - - -
**
**  Test iauSxpv function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauSxpv, vvd
**
**  This revision:  2013 August 7
*/
{
   double s, pv[2][3], spv[2][3];


   s = 2.0;

   pv[0][0] =  0.3;
   pv[0][1] =  1.2;
   pv[0][2] = -2.5;

   pv[1][0] =  0.5;
   pv[1][1] =  3.2;
   pv[1][2] = -0.7;

   iauSxpv(s, pv, spv);

   vvd(spv[0][0],  0.6, 0.0, "iauSxpv", "p1", status);
   vvd(spv[0][1],  2.4, 0.0, "iauSxpv", "p2", status);
   vvd(spv[0][2], -5.0, 0.0, "iauSxpv", "p3", status);

   vvd(spv[1][0],  1.0, 0.0, "iauSxpv", "v1", status);
   vvd(spv[1][1],  6.4, 0.0, "iauSxpv", "v2", status);
   vvd(spv[1][2], -1.4, 0.0, "iauSxpv", "v3", status);

}

static void t_taitt(int *status)
/*
**  - - - - - - - -
**   t _ t a i t t
**  - - - - - - - -
**
**  Test iauTaitt function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTaitt, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double t1, t2;
   int j;


   j = iauTaitt(2453750.5, 0.892482639, &t1, &t2);

   vvd(t1, 2453750.5, 1e-6, "iauTaitt", "t1", status);
   vvd(t2, 0.892855139, 1e-12, "iauTaitt", "t2", status);
   viv(j, 0, "iauTaitt", "j", status);

}

static void t_taiut1(int *status)
/*
**  - - - - - - - - -
**   t _ t a i u t 1
**  - - - - - - - - -
**
**  Test iauTaiut1 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTaiut1, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double u1, u2;
   int j;


   j = iauTaiut1(2453750.5, 0.892482639, -32.6659, &u1, &u2);

   vvd(u1, 2453750.5, 1e-6, "iauTaiut1", "u1", status);
   vvd(u2, 0.8921045614537037037, 1e-12, "iauTaiut1", "u2", status);
   viv(j, 0, "iauTaiut1", "j", status);

}

static void t_taiutc(int *status)
/*
**  - - - - - - - - -
**   t _ t a i u t c
**  - - - - - - - - -
**
**  Test iauTaiutc function.
**
**  Returned:
**     status    LOGICAL     TRUE = success, FALSE = fail
**
**  Called:  iauTaiutc, vvd, viv
**
**  This revision:  2013 October 3
*/
{
   double u1, u2;
   int j;


   j = iauTaiutc(2453750.5, 0.892482639, &u1, &u2);

   vvd(u1, 2453750.5, 1e-6, "iauTaiutc", "u1", status);
   vvd(u2, 0.8921006945555555556, 1e-12, "iauTaiutc", "u2", status);
   viv(j, 0, "iauTaiutc", "j", status);

}

static void t_tcbtdb(int *status)
/*
**  - - - - - - - - -
**   t _ t c b t d b
**  - - - - - - - - -
**
**  Test iauTcbtdb function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTcbtdb, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double b1, b2;
   int j;


   j = iauTcbtdb(2453750.5, 0.893019599, &b1, &b2);

   vvd(b1, 2453750.5, 1e-6, "iauTcbtdb", "b1", status);
   vvd(b2, 0.8928551362746343397, 1e-12, "iauTcbtdb", "b2", status);
   viv(j, 0, "iauTcbtdb", "j", status);

}

static void t_tcgtt(int *status)
/*
**  - - - - - - - -
**   t _ t c g t t
**  - - - - - - - -
**
**  Test iauTcgtt function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTcgtt, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double t1, t2;
   int j;


   j = iauTcgtt(2453750.5, 0.892862531, &t1, &t2);

   vvd(t1, 2453750.5, 1e-6, "iauTcgtt", "t1", status);
   vvd(t2, 0.8928551387488816828, 1e-12, "iauTcgtt", "t2", status);
   viv(j, 0, "iauTcgtt", "j", status);

}

static void t_tdbtcb(int *status)
/*
**  - - - - - - - - -
**   t _ t d b t c b
**  - - - - - - - - -
**
**  Test iauTdbtcb function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTdbtcb, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double b1, b2;
   int j;


   j = iauTdbtcb(2453750.5, 0.892855137, &b1, &b2);

   vvd( b1, 2453750.5, 1e-6, "iauTdbtcb", "b1", status);
   vvd( b2, 0.8930195997253656716, 1e-12, "iauTdbtcb", "b2", status);
   viv(j, 0, "iauTdbtcb", "j", status);

}

static void t_tdbtt(int *status)
/*
**  - - - - - - - -
**   t _ t d b t t
**  - - - - - - - -
**
**  Test iauTdbtt function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTdbtt, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double t1, t2;
   int j;


   j = iauTdbtt(2453750.5, 0.892855137, -0.000201, &t1, &t2);

   vvd(t1, 2453750.5, 1e-6, "iauTdbtt", "t1", status);
   vvd(t2, 0.8928551393263888889, 1e-12, "iauTdbtt", "t2", status);
   viv(j, 0, "iauTdbtt", "j", status);

}

static void t_tf2a(int *status)
/*
**  - - - - - - -
**   t _ t f 2 a
**  - - - - - - -
**
**  Test iauTf2a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTf2a, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double a;
   int j;


   j = iauTf2a('+', 4, 58, 20.2, &a);

   vvd(a, 1.301739278189537429, 1e-12, "iauTf2a", "a", status);
   viv(j, 0, "iauTf2a", "j", status);

}

static void t_tf2d(int *status)
/*
**  - - - - - - -
**   t _ t f 2 d
**  - - - - - - -
**
**  Test iauTf2d function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTf2d, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double d;
   int j;


   j = iauTf2d(' ', 23, 55, 10.9, &d);

   vvd(d, 0.9966539351851851852, 1e-12, "iauTf2d", "d", status);
   viv(j, 0, "iauTf2d", "j", status);

}

static void t_tr(int *status)
/*
**  - - - - -
**   t _ t r
**  - - - - -
**
**  Test iauTr function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTr, vvd
**
**  This revision:  2013 August 7
*/
{
   double r[3][3], rt[3][3];


   r[0][0] = 2.0;
   r[0][1] = 3.0;
   r[0][2] = 2.0;

   r[1][0] = 3.0;
   r[1][1] = 2.0;
   r[1][2] = 3.0;

   r[2][0] = 3.0;
   r[2][1] = 4.0;
   r[2][2] = 5.0;

   iauTr(r, rt);

   vvd(rt[0][0], 2.0, 0.0, "iauTr", "11", status);
   vvd(rt[0][1], 3.0, 0.0, "iauTr", "12", status);
   vvd(rt[0][2], 3.0, 0.0, "iauTr", "13", status);

   vvd(rt[1][0], 3.0, 0.0, "iauTr", "21", status);
   vvd(rt[1][1], 2.0, 0.0, "iauTr", "22", status);
   vvd(rt[1][2], 4.0, 0.0, "iauTr", "23", status);

   vvd(rt[2][0], 2.0, 0.0, "iauTr", "31", status);
   vvd(rt[2][1], 3.0, 0.0, "iauTr", "32", status);
   vvd(rt[2][2], 5.0, 0.0, "iauTr", "33", status);

}

static void t_trxp(int *status)
/*
**  - - - - - - -
**   t _ t r x p
**  - - - - - - -
**
**  Test iauTrxp function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTrxp, vvd
**
**  This revision:  2013 August 7
*/
{
   double r[3][3], p[3], trp[3];


   r[0][0] = 2.0;
   r[0][1] = 3.0;
   r[0][2] = 2.0;

   r[1][0] = 3.0;
   r[1][1] = 2.0;
   r[1][2] = 3.0;

   r[2][0] = 3.0;
   r[2][1] = 4.0;
   r[2][2] = 5.0;

   p[0] = 0.2;
   p[1] = 1.5;
   p[2] = 0.1;

   iauTrxp(r, p, trp);

   vvd(trp[0], 5.2, 1e-12, "iauTrxp", "1", status);
   vvd(trp[1], 4.0, 1e-12, "iauTrxp", "2", status);
   vvd(trp[2], 5.4, 1e-12, "iauTrxp", "3", status);

}

static void t_trxpv(int *status)
/*
**  - - - - - - - -
**   t _ t r x p v
**  - - - - - - - -
**
**  Test iauTrxpv function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTrxpv, vvd
**
**  This revision:  2013 August 7
*/
{
   double r[3][3], pv[2][3], trpv[2][3];


   r[0][0] = 2.0;
   r[0][1] = 3.0;
   r[0][2] = 2.0;

   r[1][0] = 3.0;
   r[1][1] = 2.0;
   r[1][2] = 3.0;

   r[2][0] = 3.0;
   r[2][1] = 4.0;
   r[2][2] = 5.0;

   pv[0][0] = 0.2;
   pv[0][1] = 1.5;
   pv[0][2] = 0.1;

   pv[1][0] = 1.5;
   pv[1][1] = 0.2;
   pv[1][2] = 0.1;

   iauTrxpv(r, pv, trpv);

   vvd(trpv[0][0], 5.2, 1e-12, "iauTrxpv", "p1", status);
   vvd(trpv[0][1], 4.0, 1e-12, "iauTrxpv", "p1", status);
   vvd(trpv[0][2], 5.4, 1e-12, "iauTrxpv", "p1", status);

   vvd(trpv[1][0], 3.9, 1e-12, "iauTrxpv", "v1", status);
   vvd(trpv[1][1], 5.3, 1e-12, "iauTrxpv", "v2", status);
   vvd(trpv[1][2], 4.1, 1e-12, "iauTrxpv", "v3", status);

}

static void t_tttai(int *status)
/*
**  - - - - - - - -
**   t _ t t t a i
**  - - - - - - - -
**
**  Test iauTttai function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTttai, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double a1, a2;
   int j;


   j = iauTttai(2453750.5, 0.892482639, &a1, &a2);

   vvd(a1, 2453750.5, 1e-6, "iauTttai", "a1", status);
   vvd(a2, 0.892110139, 1e-12, "iauTttai", "a2", status);
   viv(j, 0, "iauTttai", "j", status);

}

static void t_tttcg(int *status)
/*
**  - - - - - - - -
**   t _ t t t c g
**  - - - - - - - -
**
**  Test iauTttcg function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTttcg, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double g1, g2;
   int j;


   j = iauTttcg(2453750.5, 0.892482639, &g1, &g2);

   vvd( g1, 2453750.5, 1e-6, "iauTttcg", "g1", status);
   vvd( g2, 0.8924900312508587113, 1e-12, "iauTttcg", "g2", status);
   viv(j, 0, "iauTttcg", "j", status);

}

static void t_tttdb(int *status)
/*
**  - - - - - - - -
**   t _ t t t d b
**  - - - - - - - -
**
**  Test iauTttdb function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTttdb, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double b1, b2;
   int j;


   j = iauTttdb(2453750.5, 0.892855139, -0.000201, &b1, &b2);

   vvd(b1, 2453750.5, 1e-6, "iauTttdb", "b1", status);
   vvd(b2, 0.8928551366736111111, 1e-12, "iauTttdb", "b2", status);
   viv(j, 0, "iauTttdb", "j", status);

}

static void t_ttut1(int *status)
/*
**  - - - - - - - -
**   t _ t t u t 1
**  - - - - - - - -
**
**  Test iauTtut1 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauTtut1, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double u1, u2;
   int j;


   j = iauTtut1(2453750.5, 0.892855139, 64.8499, &u1, &u2);

   vvd(u1, 2453750.5, 1e-6, "iauTtut1", "u1", status);
   vvd(u2, 0.8921045614537037037, 1e-12, "iauTtut1", "u2", status);
   viv(j, 0, "iauTtut1", "j", status);

}

static void t_ut1tai(int *status)
/*
**  - - - - - - - - -
**   t _ u t 1 t a i
**  - - - - - - - - -
**
**  Test iauUt1tai function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauUt1tai, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double a1, a2;
   int j;


   j = iauUt1tai(2453750.5, 0.892104561, -32.6659, &a1, &a2);

   vvd(a1, 2453750.5, 1e-6, "iauUt1tai", "a1", status);
   vvd(a2, 0.8924826385462962963, 1e-12, "iauUt1tai", "a2", status);
   viv(j, 0, "iauUt1tai", "j", status);

}

static void t_ut1tt(int *status)
/*
**  - - - - - - - -
**   t _ u t 1 t t
**  - - - - - - - -
**
**  Test iauUt1tt function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauUt1tt, vvd, viv
**
**  This revision:  2013 October 3
*/
{
   double t1, t2;
   int j;


   j = iauUt1tt(2453750.5, 0.892104561, 64.8499, &t1, &t2);

   vvd(t1, 2453750.5, 1e-6, "iauUt1tt", "t1", status);
   vvd(t2, 0.8928551385462962963, 1e-12, "iauUt1tt", "t2", status);
   viv(j, 0, "iauUt1tt", "j", status);

}

static void t_ut1utc(int *status)
/*
**  - - - - - - - - -
**   t _ u t 1 u t c
**  - - - - - - - - -
**
**  Test iauUt1utc function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauUt1utc, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double u1, u2;
   int j;


   j = iauUt1utc(2453750.5, 0.892104561, 0.3341, &u1, &u2);

   vvd(u1, 2453750.5, 1e-6, "iauUt1utc", "u1", status);
   vvd(u2, 0.8921006941018518519, 1e-12, "iauUt1utc", "u2", status);
   viv(j, 0, "iauUt1utc", "j", status);

}

static void t_utctai(int *status)
/*
**  - - - - - - - - -
**   t _ u t c t a i
**  - - - - - - - - -
**
**  Test iauUtctai function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauUtctai, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double u1, u2;
   int j;


   j = iauUtctai(2453750.5, 0.892100694, &u1, &u2);

   vvd(u1, 2453750.5, 1e-6, "iauUtctai", "u1", status);
   vvd(u2, 0.8924826384444444444, 1e-12, "iauUtctai", "u2", status);
   viv(j, 0, "iauUtctai", "j", status);

}

static void t_utcut1(int *status)
/*
**  - - - - - - - - -
**   t _ u t c u t 1
**  - - - - - - - - -
**
**  Test iauUtcut1 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauUtcut1, vvd, viv
**
**  This revision:  2013 August 7
*/
{
   double u1, u2;
   int j;


   j = iauUtcut1(2453750.5, 0.892100694, 0.3341, &u1, &u2);

   vvd(u1, 2453750.5, 1e-6, "iauUtcut1", "u1", status);
   vvd(u2, 0.8921045608981481481, 1e-12, "iauUtcut1", "u2", status);
   viv(j, 0, "iauUtcut1", "j", status);

}

static void t_xy06(int *status)
/*
**  - - - - - - -
**   t _ x y 0 6
**  - - - - - - -
**
**  Test iauXy06 function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauXy06, vvd
**
**  This revision:  2013 August 7
*/
{
   double x, y;


   iauXy06(2400000.5, 53736.0, &x, &y);

   vvd(x, 0.5791308486706010975e-3, 1e-15, "iauXy06", "x", status);
   vvd(y, 0.4020579816732958141e-4, 1e-16, "iauXy06", "y", status);

}

static void t_xys00a(int *status)
/*
**  - - - - - - - - -
**   t _ x y s 0 0 a
**  - - - - - - - - -
**
**  Test iauXys00a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauXys00a, vvd
**
**  This revision:  2013 August 7
*/
{
   double x, y, s;


   iauXys00a(2400000.5, 53736.0, &x, &y, &s);

   vvd(x,  0.5791308472168152904e-3, 1e-14, "iauXys00a", "x", status);
   vvd(y,  0.4020595661591500259e-4, 1e-15, "iauXys00a", "y", status);
   vvd(s, -0.1220040848471549623e-7, 1e-18, "iauXys00a", "s", status);

}

static void t_xys00b(int *status)
/*
**  - - - - - - - - -
**   t _ x y s 0 0 b
**  - - - - - - - - -
**
**  Test iauXys00b function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauXys00b, vvd
**
**  This revision:  2013 August 7
*/
{
   double x, y, s;


   iauXys00b(2400000.5, 53736.0, &x, &y, &s);

   vvd(x,  0.5791301929950208873e-3, 1e-14, "iauXys00b", "x", status);
   vvd(y,  0.4020553681373720832e-4, 1e-15, "iauXys00b", "y", status);
   vvd(s, -0.1220027377285083189e-7, 1e-18, "iauXys00b", "s", status);

}

static void t_xys06a(int *status)
/*
**  - - - - - - - - -
**   t _ x y s 0 6 a
**  - - - - - - - - -
**
**  Test iauXys06a function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauXys06a, vvd
**
**  This revision:  2013 August 7
*/
{
   double x, y, s;


   iauXys06a(2400000.5, 53736.0, &x, &y, &s);

   vvd(x,  0.5791308482835292617e-3, 1e-14, "iauXys06a", "x", status);
   vvd(y,  0.4020580099454020310e-4, 1e-15, "iauXys06a", "y", status);
   vvd(s, -0.1220032294164579896e-7, 1e-18, "iauXys06a", "s", status);

}

static void t_zp(int *status)
/*
**  - - - - -
**   t _ z p
**  - - - - -
**
**  Test iauZp function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauZp, vvd
**
**  This revision:  2013 August 7
*/
{
   double p[3];


   p[0] =  0.3;
   p[1] =  1.2;
   p[2] = -2.5;

   iauZp(p);

   vvd(p[0], 0.0, 0.0, "iauZp", "1", status);
   vvd(p[1], 0.0, 0.0, "iauZp", "2", status);
   vvd(p[2], 0.0, 0.0, "iauZp", "3", status);

}

static void t_zpv(int *status)
/*
**  - - - - - -
**   t _ z p v
**  - - - - - -
**
**  Test iauZpv function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauZpv, vvd
**
**  This revision:  2013 August 7
*/
{
   double pv[2][3];


   pv[0][0] =  0.3;
   pv[0][1] =  1.2;
   pv[0][2] = -2.5;

   pv[1][0] = -0.5;
   pv[1][1] =  3.1;
   pv[1][2] =  0.9;

   iauZpv(pv);

   vvd(pv[0][0], 0.0, 0.0, "iauZpv", "p1", status);
   vvd(pv[0][1], 0.0, 0.0, "iauZpv", "p2", status);
   vvd(pv[0][2], 0.0, 0.0, "iauZpv", "p3", status);

   vvd(pv[1][0], 0.0, 0.0, "iauZpv", "v1", status);
   vvd(pv[1][1], 0.0, 0.0, "iauZpv", "v2", status);
   vvd(pv[1][2], 0.0, 0.0, "iauZpv", "v3", status);

}

static void t_zr(int *status)
/*
**  - - - - -
**   t _ z r
**  - - - - -
**
**  Test iauZr function.
**
**  Returned:
**     status    int         FALSE = success, TRUE = fail
**
**  Called:  iauZr, vvd
**
**  This revision:  2013 August 7
*/
{
   double r[3][3];


   r[0][0] = 2.0;
   r[1][0] = 3.0;
   r[2][0] = 2.0;

   r[0][1] = 3.0;
   r[1][1] = 2.0;
   r[2][1] = 3.0;

   r[0][2] = 3.0;
   r[1][2] = 4.0;
   r[2][2] = 5.0;

   iauZr(r);

   vvd(r[0][0], 0.0, 0.0, "iauZr", "00", status);
   vvd(r[1][0], 0.0, 0.0, "iauZr", "01", status);
   vvd(r[2][0], 0.0, 0.0, "iauZr", "02", status);

   vvd(r[0][1], 0.0, 0.0, "iauZr", "10", status);
   vvd(r[1][1], 0.0, 0.0, "iauZr", "11", status);
   vvd(r[2][1], 0.0, 0.0, "iauZr", "12", status);

   vvd(r[0][2], 0.0, 0.0, "iauZr", "20", status);
   vvd(r[1][2], 0.0, 0.0, "iauZr", "21", status);
   vvd(r[2][2], 0.0, 0.0, "iauZr", "22", status);

}

int main(int argc, char *argv[])
/*
**  - - - - -
**   m a i n
**  - - - - -
**
**  This revision:  2013 October 3
*/
{
   int status;


/* If any command-line argument, switch to verbose reporting. */
   if (argc > 1) {
      verbose = 1;
      argv[0][0] += 0;    /* to avoid compiler warnings */
   }

/* Preset the &status to FALSE = success. */
   status = 0;

/* Test all of the SOFA functions. */
   t_a2af(&status);
   t_a2tf(&status);
   t_ab(&status);
   t_af2a(&status);
   t_anp(&status);
   t_anpm(&status);
   t_apcg(&status);
   t_apcg13(&status);
   t_apci(&status);
   t_apci13(&status);
   t_apco(&status);
   t_apco13(&status);
   t_apcs(&status);
   t_apcs13(&status);
   t_aper(&status);
   t_aper13(&status);
   t_apio(&status);
   t_apio13(&status);
   t_atci13(&status);
   t_atciq(&status);
   t_atciqn(&status);
   t_atciqz(&status);
   t_atco13(&status);
   t_atic13(&status);
   t_aticq(&status);
   t_aticqn(&status);
   t_atio13(&status);
   t_atioq(&status);
   t_atoc13(&status);
   t_atoi13(&status);
   t_atoiq(&status);
   t_bi00(&status);
   t_bp00(&status);
   t_bp06(&status);
   t_bpn2xy(&status);
   t_c2i00a(&status);
   t_c2i00b(&status);
   t_c2i06a(&status);
   t_c2ibpn(&status);
   t_c2ixy(&status);
   t_c2ixys(&status);
   t_c2s(&status);
   t_c2t00a(&status);
   t_c2t00b(&status);
   t_c2t06a(&status);
   t_c2tcio(&status);
   t_c2teqx(&status);
   t_c2tpe(&status);
   t_c2txy(&status);
   t_cal2jd(&status);
   t_cp(&status);
   t_cpv(&status);
   t_cr(&status);
   t_d2dtf(&status);
   t_d2tf(&status);
   t_dat(&status);
   t_dtdb(&status);
   t_dtf2d(&status);
   t_ee00(&status);
   t_ee00a(&status);
   t_ee00b(&status);
   t_ee06a(&status);
   t_eect00(&status);
   t_eform(&status);
   t_eo06a(&status);
   t_eors(&status);
   t_epb(&status);
   t_epb2jd(&status);
   t_epj(&status);
   t_epj2jd(&status);
   t_epv00(&status);
   t_eqeq94(&status);
   t_era00(&status);
   t_fad03(&status);
   t_fae03(&status);
   t_faf03(&status);
   t_faju03(&status);
   t_fal03(&status);
   t_falp03(&status);
   t_fama03(&status);
   t_fame03(&status);
   t_fane03(&status);
   t_faom03(&status);
   t_fapa03(&status);
   t_fasa03(&status);
   t_faur03(&status);
   t_fave03(&status);
   t_fk52h(&status);
   t_fk5hip(&status);
   t_fk5hz(&status);
   t_fw2m(&status);
   t_fw2xy(&status);
   t_g2icrs(&status);
   t_gc2gd(&status);
   t_gc2gde(&status);
   t_gd2gc(&status);
   t_gd2gce(&status);
   t_gmst00(&status);
   t_gmst06(&status);
   t_gmst82(&status);
   t_gst00a(&status);
   t_gst00b(&status);
   t_gst06(&status);
   t_gst06a(&status);
   t_gst94(&status);
   t_h2fk5(&status);
   t_hfk5z(&status);
   t_icrs2g(&status);
   t_ir(&status);
   t_jd2cal(&status);
   t_jdcalf(&status);
   t_ld(&status);
   t_ldn(&status);
   t_ldsun(&status);
   t_num00a(&status);
   t_num00b(&status);
   t_num06a(&status);
   t_numat(&status);
   t_nut00a(&status);
   t_nut00b(&status);
   t_nut06a(&status);
   t_nut80(&status);
   t_nutm80(&status);
   t_obl06(&status);
   t_obl80(&status);
   t_p06e(&status);
   t_p2pv(&status);
   t_p2s(&status);
   t_pap(&status);
   t_pas(&status);
   t_pb06(&status);
   t_pdp(&status);
   t_pfw06(&status);
   t_plan94(&status);
   t_pmat00(&status);
   t_pmat06(&status);
   t_pmat76(&status);
   t_pm(&status);
   t_pmp(&status);
   t_pmpx(&status);
   t_pmsafe(&status);
   t_pn(&status);
   t_pn00(&status);
   t_pn00a(&status);
   t_pn00b(&status);
   t_pn06a(&status);
   t_pn06(&status);
   t_pnm00a(&status);
   t_pnm00b(&status);
   t_pnm06a(&status);
   t_pnm80(&status);
   t_pom00(&status);
   t_ppp(&status);
   t_ppsp(&status);
   t_pr00(&status);
   t_prec76(&status);
   t_pv2p(&status);
   t_pv2s(&status);
   t_pvdpv(&status);
   t_pvm(&status);
   t_pvmpv(&status);
   t_pvppv(&status);
   t_pvstar(&status);
   t_pvtob(&status);
   t_pvu(&status);
   t_pvup(&status);
   t_pvxpv(&status);
   t_pxp(&status);
   t_refco(&status);
   t_rm2v(&status);
   t_rv2m(&status);
   t_rx(&status);
   t_rxp(&status);
   t_rxpv(&status);
   t_rxr(&status);
   t_ry(&status);
   t_rz(&status);
   t_s00a(&status);
   t_s00b(&status);
   t_s00(&status);
   t_s06a(&status);
   t_s06(&status);
   t_s2c(&status);
   t_s2p(&status);
   t_s2pv(&status);
   t_s2xpv(&status);
   t_sepp(&status);
   t_seps(&status);
   t_sp00(&status);
   t_starpm(&status);
   t_starpv(&status);
   t_sxp(&status);
   t_sxpv(&status);
   t_taitt(&status);
   t_taiut1(&status);
   t_taiutc(&status);
   t_tcbtdb(&status);
   t_tcgtt(&status);
   t_tdbtcb(&status);
   t_tdbtt(&status);
   t_tf2a(&status);
   t_tf2d(&status);
   t_tr(&status);
   t_trxp(&status);
   t_trxpv(&status);
   t_tttai(&status);
   t_tttcg(&status);
   t_tttdb(&status);
   t_ttut1(&status);
   t_ut1tai(&status);
   t_ut1tt(&status) ;
   t_ut1utc(&status);
   t_utctai(&status);
   t_utcut1(&status);
   t_xy06(&status);
   t_xys00a(&status);
   t_xys00b(&status);
   t_xys06a(&status);
   t_zp(&status);
   t_zpv(&status);
   t_zr(&status);

/* Report, set up an appropriate exit status, and finish. */
   if (status) {
      printf("t_sofa_c validation failed!\n");
   } else {
      printf("t_sofa_c validation successful\n");
   }
   return status;
}
