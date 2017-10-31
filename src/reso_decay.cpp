// ------------ resonance decays. adapted from azhydro -----------------
//
// These functions were adapted by Björn Schenke from the public version of the 
// resonance decay calculation using the output 
// generated by the hydrodynamical code azhydro0p2.  The majority of the code was 
// developed by Josef Sollfrank and Peter Kolb.  Additional work and comments 
// were added by Evan Frodermann, September 2005.
// Please refer to the papers 
// J. Sollfrank, P. Koch, and U. Heinz, Phys. Lett B 252 (1990) and 
// J. Sollfrank, P. Koch, and U. Heinz, Z. Phys. C 52 (1991) 
// for a description of the formalism utilized in this program.

#include "./freeze.h"
#include "./int.h"

/*************************************************
*
*   Edndp3
*
* 
**************************************************/
// This function interpolates the needed spectra for a given y, pt and phi.
double Freeze::Edndp3(double yr, double ptr, double phirin, int res_num) {
// if pseudofreeze flag is set, yr is the *pseudorapidity* of the resonance
    if (phirin < 0.0) {
        printf("ERROR: phir %15.8le < 0 !!! \n", phirin);
        exit(0);
    }
    if(phirin > 2.0*PI) {
        printf("ERROR: phir %15.8le > 2PI !!! \n", phirin);
        exit(0);
    }
    double phir = phirin;
    int pn = partid[MHALF + res_num];

    // If pseudofreeze flag is set,
    // dNdydptdphi is on a fixed grid in pseudorapidity. 
    // Set yr to the *pseudorapidity* of the resonance,
    // and then interpolate the yield at that value.
    if (pseudofreeze) {
        double yrtemp = yr;
        yr = PseudoRap(yrtemp, ptr, particleList[pn].mass);
    }
  
    if (ptr > particleList[pn].pt[particleList[pn].npt - 1]) {
        return 0.;
    }

    if (fabs(yr) > particleList[pn].ymax && !boost_invariant) {
        return 0.;
    }

    int nphi = 1; 
    while ((phir > phiArray[nphi]) && (nphi<(particleList[pn].nphi-1))) {
        nphi++;
    }
    int npt = 1; 
    while (ptr > particleList[pn].pt[npt] && npt<(particleList[pn].npt - 1)) {
        npt++;
    }
    int ny = 1; 
    while (yr > particleList[pn].y[ny] && ny < (particleList[pn].ny - 1)) {
        ny++;
    }

    /* phi interpolation */
    double f1 = util->lin_int(
            phiArray[nphi-1], phiArray[nphi], 
            particleList[pn].dNdydptdphi[ny-1][npt-1][nphi-1], 
            particleList[pn].dNdydptdphi[ny-1][npt-1][nphi], phir);
    double f2 = util->lin_int(
            phiArray[nphi-1], phiArray[nphi], 
            particleList[pn].dNdydptdphi[ny-1][npt][nphi-1], 
            particleList[pn].dNdydptdphi[ny-1][npt][nphi], phir);

    // security: if for some reason we got a negative number of particles
    // (happened in the viscous code at large eta sometimes)
    if (f1 < 0.)
        f1 = 1e-30;
    if (f2 < 0.)
        f2 = 1e-30;
    
    double f1s = f1;
    double f2s = f2;

    if (ptr > PTCHANGE && f1s > 0 && f2s > 0) {
        f1 = log(f1); 
        f2 = log(f2);
    }
    double val1 = util->lin_int(particleList[pn].pt[npt-1],
                                particleList[pn].pt[npt], f1, f2, ptr);

    if (ptr > PTCHANGE && f1s > 0 && f2s > 0) {
        val1 = exp(val1);
    }
  
    if (isnan(val1)) {
        fprintf(stderr,"\n number=%d\n\n",res_num);
        fprintf(stderr,"val1=%f\n",val1);
        fprintf(stderr,"f1=%f\n",f1);
        fprintf(stderr,"f2=%f\n",f2);
        fprintf(stderr,"f1s=%f\n",f1s);
        fprintf(stderr,"f2s=%f\n",f2s);
      
        fprintf(stderr,"pn=%d\n",pn);
        fprintf(stderr,"ny=%d\n",ny);
        fprintf(stderr,"npt=%d\n",npt);
        fprintf(stderr,"nphi=%d\n",nphi);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny-1][npt-1][nphi-1]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny][npt-1][nphi-1]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny-1][npt][nphi-1]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny-1][npt-1][nphi]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny][npt][nphi-1]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny-1][npt][nphi]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny][npt-1][nphi]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny][npt][nphi]);
        fprintf(stderr,"phi1=%f\n",phiArray[nphi-1]);
        fprintf(stderr,"phi2=%f\n",phiArray[nphi]);
        fprintf(stderr,"pt1=%f\n",particleList[pn].pt[npt-1]);
        fprintf(stderr,"pt2=%f\n",particleList[pn].pt[npt]);
        fprintf(stderr,"y1=%f\n",particleList[pn].y[ny-1]);
        fprintf(stderr,"y2=%f\n",particleList[pn].y[ny]);
    }

    f1 = util->lin_int(phiArray[nphi-1], phiArray[nphi], 
                       particleList[pn].dNdydptdphi[ny][npt-1][nphi-1], 
                       particleList[pn].dNdydptdphi[ny][npt-1][nphi], phir);
    f2 = util->lin_int(phiArray[nphi-1], phiArray[nphi], 
                       particleList[pn].dNdydptdphi[ny][npt][nphi-1], 
                       particleList[pn].dNdydptdphi[ny][npt][nphi], phir);
  
    // security: if for some reason we got a negative number of particles
    // (happened in the viscous code at large eta sometimes)
    if (f1 < 0.)
        f1 = 1e-30;
    if (f2 < 0.)
        f2 = 1e-30;
    
    f1s = f1;
    f2s = f2;

    if (ptr > PTCHANGE && f1s > 0 && f2s > 0){
        f1 = log(f1); 
        f2 = log(f2);
    }
    double val2 = util->lin_int(particleList[pn].pt[npt-1],
                                particleList[pn].pt[npt], f1, f2, ptr);

    if (ptr > PTCHANGE && f1s > 0 && f2s > 0){
        val2 = exp(val2);
    }
  
    double val = util->lin_int(particleList[pn].y[ny-1],
                               particleList[pn].y[ny], val1, val2, yr);

    if (isnan(val)) {
        fprintf(stderr,"val=%f\n",val);
        fprintf(stderr,"val1=%f\n",val1);
        fprintf(stderr,"val2=%f\n",val2);
        fprintf(stderr,"f1=%f\n",f1);
        fprintf(stderr,"f2=%f\n",f2);
        fprintf(stderr,"f1s=%f\n",f1s);
        fprintf(stderr,"f2s=%f\n",f2s);
        fprintf(stderr,"ny=%d\n",ny);
        fprintf(stderr,"npt=%d\n",npt);
        fprintf(stderr,"nphi=%d\n",nphi);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny-1][npt-1][nphi-1]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny][npt-1][nphi-1]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny-1][npt][nphi-1]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny-1][npt-1][nphi]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny][npt][nphi-1]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny-1][npt][nphi]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny][npt-1][nphi]);
        fprintf(stderr,"dN..=%e\n",particleList[pn].dNdydptdphi[ny][npt][nphi]);
        fprintf(stderr,"phi1=%f\n",phiArray[nphi-1]);
        fprintf(stderr,"phi2=%f\n",phiArray[nphi]);
        fprintf(stderr,"pt1=%f\n",particleList[pn].pt[npt-1]);
        fprintf(stderr,"pt2=%f\n",particleList[pn].pt[npt]);
        fprintf(stderr,"y1=%f\n",particleList[pn].y[ny-1]);
        fprintf(stderr,"y2=%f\n",particleList[pn].y[ny]);
        fprintf(stderr,"yR=%f\n",yr);
    }
  
    return val;
}


double Freeze::dnpir2N (double phi, void *para1)    
{
  pblockN *para = (pblockN *) para1;
  double D;
  double eR, plR, ptR, yR, phiR, sume, jac;
  double cphiR, sphiR;
  double dnr;           /* dn/mtdmt of resonance */
  
  sume = para->e + para->e0;

  D = para->e * para->e0 + para->pl * para->p0 * para->costh +
    para->pt * para->p0 * para->sinth * cos (phi) + para->m1 * para->m1;

  eR = para->mr * (sume * sume / D - 1.0);
  jac = para->mr + eR;
  plR = para->mr * sume * (para->pl - para->p0 * para->costh) / D;
  ptR = (eR * eR - plR * plR - para->mr * para->mr);

  if (ptR < 0.0)
    ptR = 0.0;

  else
    ptR = sqrt (ptR);

  yR = 0.5 * log ((eR + plR) / (eR - plR));
  cphiR = -jac * (para->p0 * para->sinth * cos (phi + para->phi)
          - para->pt * cos (para->phi)) / (sume * ptR);
  sphiR = -jac * (para->p0 * para->sinth * sin (phi + para->phi)
          - para->pt * sin (para->phi)) / (sume * ptR);

  if ((fabs (cphiR) > 1.000) || (fabs (sphiR) > 1.000))
    {
      if ((fabs (cphiR) > 1.01) || (fabs (sphiR) > 1.01))
    {
      //  printf ("  |phir| = %15.8lf  > 1 ! \n", phiR);
      printf (" phi %15.8le D %15.8le \n", phi, D);
      printf (" eR %15.8le plR %15.8le \n", eR, plR);
      printf (" ptR %15.8le jac %15.8le \n", ptR, jac);
      printf (" sume %15.8le costh %15.8le \n", sume, para->costh);

      printf (" pt %15.8le \n", para->pt);
      printf (" mt  %15.8le \n", para->mt);
      printf (" y %15.8le \n", para->y);
      printf (" e %15.8le \n", para->e);
      printf (" e0 %15.8le \n", para->e0);
      printf (" p0 %15.8le \n", para->p0);
      printf (" pl %15.8le \n", para->pl);
      printf (" phi %15.8le \n", para->phi);

      printf (" m1 %15.8le \n", para->m1);
      printf (" m2 %15.8le \n", para->m2);
      printf (" m3 %15.8le \n", para->m3);
      printf (" mr %15.8le \n", para->mr);
      if (cphiR > 1.0)
        cphiR = 1.0;
      if (cphiR < -1.0)
        cphiR = -1.0;
      //exit (0);
    }
      else
    {
      if (cphiR > 1.0)
        cphiR = 1.0;
      if (cphiR < -1.0)
        cphiR = -1.0;
    }
    }

  phiR = acos (cphiR);
  if (sphiR < 0.0)
    phiR = 2.0 * PI - phiR;

  dnr = Edndp3 (yR, ptR, phiR, para->res_num);

  /*printf(" phir = %15.8lf  ! ", phiR);
     printf(" ptR %15.8le jac %15.8le ", ptR, jac );
     printf(" dnr %15.8le \n", dnr); */

  return dnr * jac * jac / (2.0 * sume * sume);
}

double Freeze::norm3int (double x, void *paranorm) // this computes "Q(m_R,m_1,m_2,m_3)"
{
  nblock *tmp = (nblock *) paranorm;
  double res = sqrt ((tmp->a - x) * (tmp->b - x)
             * (x - tmp->c) * (x - tmp->d)) / x;
  return res;
}

double Freeze::dnpir1N (double costh, void* para1)         
{
  pblockN *para = (pblockN *) para1;
  double r;
  para->costh = costh;
  para->sinth = sqrt (1.0 - para->costh * para->costh);
  r = gauss (PTN2, &Freeze::dnpir2N, 0.0, 2.0 * PI, para); //Integrates the "dnpir2N" kernel over phi using gaussian integration
  return r;
}

double Freeze::dn2ptN (double w2, void* para1)
{
  pblockN *para = (pblockN *) para1;
  para->e0 = (para->mr * para->mr + para->m1 * para->m1 - w2) / (2 * para->mr); //particle one energy in resonance rest frame
  para->p0 = sqrt (para->e0 * para->e0 - para->m1 * para->m1); // particle one absolute value of three momentum on resonance rest frame
  return gauss (PTN1, &Freeze::dnpir1N, -1.0, 1.0, para); //Integrate the "dnpir1N" kernel over cos(theta) using gaussian integration
}

double Freeze::dn3ptN (double x, void* para1)  //The integration kernel for "W" in 3-body decays. x=invariant mass of other particles squared
{
  pblockN *para = (pblockN *) para1;
  double e0 =(para->mr * para->mr + para->m1 * para->m1 - x) / (2 * para->mr);
  double p0 = sqrt (e0 * e0 - para->m1 * para->m1);
  double a = (para->m2 + para->m3) * (para->m2 + para->m3);
  double b = (para->m2 - para->m3) * (para->m2 - para->m3);
  double re = p0 * sqrt ((x - a) * (x - b)) / x * dn2ptN (x, para);
  return re;
}

double Freeze::gauss(int n, double (Freeze::*f)(double, void *),
                     double xlo, double xhi, void *optvec) {
    double  xoffs, xdiff; 
    int ix;
    double  s;      /* summing up */
    double  *p, *w;     /* pointing to active list */

    switch (n) {
        case 4:     p= gaulep4; w= gaulew4; break;
        case 8:     p= gaulep8; w= gaulew8; break;
        case 10:    p=gaulep10; w=gaulew10; break;
        case 12:    p=gaulep12; w=gaulew12; break;
        case 16:    p=gaulep16; w=gaulew16; break;
        case 20:    p=gaulep20; w=gaulew20; break;
        case 48:    p=gaulep48; w=gaulew48; break;
        default:    printf("\ngauss():%d points not in list\n",n);
                exit(0);
        }
    xoffs = 0.5 * ( xlo + xhi );
    xdiff = 0.5 * ( xhi - xlo );
    s = 0;
    for( ix=0; ix<n/2; ix++ )   /* n is even */
      s += w[ix] * ( (this->*f)(xoffs+xdiff*p[ix],optvec)
                 + (this->*f)(xoffs-xdiff*p[ix],optvec) );
    return( s * xdiff );
}
    
    
/********************************************************************
*
*   Edndp3_2bodyN()
*
* transverse momentum spectrum in GeV^-2 from pions out of resonances
*********************************************************************/
double Freeze::Edndp3_2bodyN (double y, double pt, double phi, double m1, double m2, double mr, int res_num)
/*      /\* in units of GeV^-2,includes phasespace and volume, */
/*         does not include degeneracy factors  *\/ */
/*      double y;           /\* rapidity of particle 1       *\/ */
/*      double pt;          /\* transverse momentum of particle 1    *\/ */
/*      double phi;     /\* phi angle of particle 1      *\/ */
/*      double m1, m2;      /\* restmasses of decay particles in MeV *\/ */
/*      double mr;          /\* restmass of resonance MeV            *\/ */
/*      int res_num;        /\* Montecarlo number of the Resonance   */ 

{
  double mt = sqrt (pt * pt + m1 * m1);
  double norm2;         /* 2-body normalization         */
  pblockN para;
  double res2;

  para.pt = pt;
  para.mt = mt;
  para.e = mt * cosh (y);
  para.pl = mt * sinh (y);
  para.y = y;
  para.phi = phi;
  para.m1 = m1;
  para.m2 = m2;
  para.mr = mr;

  para.res_num = res_num;

  norm2 = 1.0 / (2.0 * PI);
  res2 = norm2 * dn2ptN (m2 * m2, &para); //Calls the integration routines for 2-body
  if (res2<0.) res2=0.;
  return res2;          /* like Ed3ndp3_2body() */
}


/********************************************************************
*
*   Edndp3_3bodyN()
*
* transverse momentum spectrum in GeV^-2 from pions out of resonances
*********************************************************************/
double Freeze::Edndp3_3bodyN (double y, double pt, double phi, double m1, double m2,
              double m3, double mr, double norm3, int res_num)
        /* in units of GeV^-2,includes phasespace and volume,
           does not include degeneracy factors  */
{
  double mt = sqrt (pt * pt + m1 * m1);
  pblockN para;
  double wmin, wmax;
  double res3;
//   double slope;          /* slope of resonance for high mt */
//   int pn;

  para.pt = pt;
  para.mt = mt;
  para.y = y;
  para.e = mt * cosh (y);
  para.pl = mt * sinh (y);
  para.phi = phi;

  para.m1 = m1;
  para.m2 = m2;
  para.m3 = m3;
  para.mr = mr;

//   pn = partid[MHALF + res_num];

  para.res_num = res_num;

  wmin = (m2 + m3) * (m2 + m3); 
  wmax = (mr - m1) * (mr - m1);
  res3 = 2.0 * norm3 * gauss (PTS4, &Freeze::dn3ptN, wmin, wmax, &para) / mr;  //Integrates "W" using gaussian 
  if (res3<0.) res3=0.; 
  return res3;
}


//! computes the pt, mt distribution including resonance decays
void Freeze::add_reso(int pn, int pnR, int k, int j) {
    nblock paranorm;      /* for 3body normalization integral */
    double y;
    double m1, m2, m3, mr;
    double norm3;         /* normalisation of 3-body integral */
    int pn2, pn3, pn4;        /* internal numbers for resonances */

    // this variable stores number of points in pseudorapidity
    int ny = particleList[pn].ny;
    int npt = particleList[pn].npt;
    int nphi = particleList[pn].nphi;
    double deltaphi = 2*PI/nphi;
  

    // Determine the number of particles involved in the decay with the switch
    switch (abs(decay[j].numpart)) {
        case 1:
        {
            // Only 1 particle, if it gets here, by accident,
            // this prevents any integration for 1 particle chains
            break;
        }

        case 2:  // 2-body decay 
        {
            if (k == 0) {
                pn2 = partid[MHALF + decay[j].part[1]];
            } else {
                pn2 = partid[MHALF + decay[j].part[0]];
            }
            // printf ("case 2:  i %3i j %3i k %3i \n", pn, j, k);
            m1 = particleList[pn].mass;
            m2 = particleList[pn2].mass;
            mr = particleList[pnR].mass;

            while ((m1 + m2) > mr) {
                mr += 0.25*particleList[pnR].width;
                m1 -= 0.5*particleList[pn].width;
                m2 -= 0.5*particleList[pn2].width;
            }
            // fprintf(stderr,"mr=%f\n",mr);
            // fprintf(stderr,"m1=%f\n",m1);
            // fprintf(stderr,"m2=%f\n",m2);

            if (boost_invariant) {
                y = 0.0;
                #pragma omp parallel for collapse(2)
                for (int l = 0; l < npt; l++) {
                    for (int i = 0; i < nphi; i++) {
                        if (pseudofreeze) {
                            y = Rap(y, particleList[pn].pt[l], m1);
                        }
                        double phi = 0.0;
                        if (pseudofreeze) {
                            phi = i*deltaphi;
                        } else {
                            phi = phiArray[i];
                        }
                        double spectrum = Edndp3_2bodyN(
                                y, particleList[pn].pt[l], phi,
                                m1, m2, mr,
                                particleList[pnR].number);
                        if (isnan(spectrum)) {
                            fprintf(stderr, "2 pt=%f\n",
                                    particleList[pn].pt[l]);
                            fprintf(stderr, "2 number=%d\n",
                                    particleList[pnR].number);
                            fprintf(stderr, "2 Edn..=%f\n", spectrum);
                        } else {
                            // Call the 2-body decay integral and
                            // add its contribution to the daughter
                            // particle of interest
                            for (int n = 0; n < ny; n++) {
                                particleList[pn].dNdydptdphi[n][l][i] += (
                                                decay[j].branch*spectrum);
                                //if (n == ny/2 && i==0) {
                                //    particleList[pn].resCont[n][l][i] += (
                                //                decay[j].branch*spectrum);
                                //}
                            }
                        }
                    }
                }
            } else {
                #pragma omp parallel for collapse(3)
                for (int n = 0; n < ny; n++) {
                    for (int l = 0; l < npt; l++) {
                        for (int i = 0; i < nphi; i++) {
                            y = particleList[pn].y[n];
                            if (pseudofreeze) {
                                y = Rap(particleList[pn].y[n],
                                        particleList[pn].pt[l], m1);
                            }
                            double phi = 0.0;
                            if (pseudofreeze) {
                                phi = i*deltaphi;
                            } else {
                                phi = phiArray[i];
                            }
                            double spectrum = Edndp3_2bodyN(
                                    y, particleList[pn].pt[l], phi,
                                    m1, m2, mr,
                                    particleList[pnR].number);
                            if (isnan(spectrum)) {
                                fprintf(stderr, "2 pt=%f\n",
                                        particleList[pn].pt[l]);
                                fprintf(stderr, "2 number=%d\n",
                                        particleList[pnR].number);
                                fprintf(stderr, "2 Edn..=%f\n", spectrum);
                            } else {
                                // Call the 2-body decay integral and
                                // add its contribution to the daughter
                                // particle of interest
                                particleList[pn].dNdydptdphi[n][l][i] += (
                                                    decay[j].branch*spectrum);
                                //if (n == ny/2 && i==0) {
                                //    particleList[pn].resCont[n][l][i] += (
                                //                    decay[j].branch*spectrum);
                                //}
                            }
                        }
                    }
                }
            }
            break;
        }

        case 3:  // 3-body decay
        {
            if (k == 0) {
                pn2 = partid[MHALF + decay[j].part[1]];
                pn3 = partid[MHALF + decay[j].part[2]];
            } else {
                if (k == 1) {
                    pn2 = partid[MHALF + decay[j].part[0]];
                    pn3 = partid[MHALF + decay[j].part[2]];
                } else {
                    pn2 = partid[MHALF + decay[j].part[0]];
                    pn3 = partid[MHALF + decay[j].part[1]];
                }
            }

            m1 = particleList[pn].mass;
            m2 = particleList[pn2].mass;
            m3 = particleList[pn3].mass;
            mr = particleList[pnR].mass;
            paranorm.a = (mr + m1)*(mr + m1);
            paranorm.b = (mr - m1)*(mr - m1);
            paranorm.c = (m2 + m3)*(m2 + m3);
            paranorm.d = (m2 - m3)*(m2 - m3);
            norm3 = mr*mr/(2*PI*gauss(PTS3, &Freeze::norm3int, paranorm.c,
                                      paranorm.b, &paranorm));
    
            if (boost_invariant) {
                y = 0.0;
                #pragma omp parallel for collapse(2)
                for (int l = 0; l < npt; l++) {
                    for (int i = 0; i < nphi; i++) {
                        if (pseudofreeze) {
                            y = Rap(y, particleList[pn].pt[l], m1);
                        }
                        double phi;
                        if (pseudofreeze) {
                            phi = i*deltaphi;
                        } else {
                            phi = phiArray[i];
                        }
                        double spectrum = Edndp3_3bodyN(
                                y, particleList[pn].pt[l], phi,
                                m1, m2, m3, mr, norm3, 
                                particleList[pnR].number);
                        if (isnan(spectrum)) {
                            fprintf(stderr,"3 number=%d\n",
                                    particleList[pnR].number);
                            fprintf(stderr,"3 Edn..=%f\n", spectrum);
                        } else {
                            // Call the 3-body decay integral and
                            // add its contribution to the daughter
                            // particle of interest 
                            for (int n = 0; n < ny; n++) {
                                particleList[pn].dNdydptdphi[n][l][i] += (
                                                decay[j].branch*spectrum);
                                //if (n == ny/2 && i==0) {
                                //    particleList[pn].resCont[n][l][i]+= (
                                //                    decay[j].branch*spectrum);
                   
                                //}
                            }
                        }
                    }
                }
            } else {
                #pragma omp parallel for collapse(3)
                for (int n = 0; n < ny; n++) {
                    for (int l = 0; l < npt; l++) {
                        for (int i = 0; i < nphi; i++) {
                            y = particleList[pn].y[n];
                            if (pseudofreeze) {
                                y = Rap(particleList[pn].y[n],
                                        particleList[pn].pt[l], m1);
                            }
                            double phi;
                            if (pseudofreeze) {
                                phi = i*deltaphi;
                            } else {
                                phi = phiArray[i];
                            }
                            double spectrum = Edndp3_3bodyN(
                                    y, particleList[pn].pt[l], phi,
                                    m1, m2, m3, mr, norm3, 
                                    particleList[pnR].number);
                            if (isnan(spectrum)) {
                                fprintf(stderr,"3 number=%d\n",
                                        particleList[pnR].number);
                                fprintf(stderr,"3 Edn..=%f\n", spectrum);
                            } else {
                                // Call the 3-body decay integral and
                                // add its contribution to the daughter
                                // particle of interest 
                                particleList[pn].dNdydptdphi[n][l][i] += (
                                                    decay[j].branch*spectrum);
                            }
                
                            //if (n == ny/2 && i==0) {
                            //    particleList[pn].resCont[n][l][i]+= (
                            //                        decay[j].branch*spectrum);
                   
                            //}
                        }
                    }
                }
            }
            break;
        }

        case 4:  // 4-body decay (rare and low contribution)
        {
            if (k == 0) {
                pn2 = partid[MHALF + decay[j].part[1]];
                pn3 = partid[MHALF + decay[j].part[2]];
                pn4 = partid[MHALF + decay[j].part[3]];
            } else if (k == 1) {
                pn2 = partid[MHALF + decay[j].part[0]];
                pn3 = partid[MHALF + decay[j].part[2]];
                pn4 = partid[MHALF + decay[j].part[3]];
            } else if (k == 2) {
                pn2 = partid[MHALF + decay[j].part[0]];
                pn3 = partid[MHALF + decay[j].part[1]];
                pn4 = partid[MHALF + decay[j].part[3]];
            } else {
                pn2 = partid[MHALF + decay[j].part[0]];
                pn3 = partid[MHALF + decay[j].part[1]];
                pn4 = partid[MHALF + decay[j].part[2]];
            }
            // approximate the 4-body with a 3-body decay with the 4th particle
            // being the center of mass of 2 particles.
            m1 = particleList[pn].mass;
            m2 = particleList[pn2].mass;
            mr = particleList[pnR].mass;
            m3 = 0.5*(particleList[pn3].mass + particleList[pn4].mass
                      + mr - m1 - m2);
            paranorm.a = (mr + m1)*(mr + m1);
            paranorm.b = (mr - m1)*(mr - m1);
            paranorm.c = (m2 + m3)*(m2 + m3);
            paranorm.d = (m2 - m3)*(m2 - m3);
            norm3 = mr*mr/(2*PI*gauss(PTS3, &Freeze::norm3int, paranorm.c,
                                      paranorm.b, &paranorm));
    
            if (boost_invariant) {
                y = 0.0;
                #pragma omp parallel for collapse(2)
                for (int i = 0; i < nphi; i++) {
                    for (int l = 0; l < npt; l++) {
                        double phi;
                        if (pseudofreeze) {
                            phi = i*deltaphi;
                        } else {
                            phi = phiArray[i];
                        }
                        if (pseudofreeze) {
                            y = Rap(y, particleList[pn].pt[l], m1);
                        }
                        double spectrum = Edndp3_3bodyN(
                                y, particleList[pn].pt[l], phi,
                                m1, m2, m3, mr, norm3,
                                particleList[pnR].number);
                        if (isnan(spectrum)) {
                            fprintf(stderr, "3 number=%d\n",
                                    particleList[pnR].number);
                        } else {
                            for (int n = 0; n < ny; n++) {
                                particleList[pn].dNdydptdphi[n][l][i] += (
                                        decay[j].branch*spectrum);
                            }
                        }
                    }
                }
            } else {
                #pragma omp parallel for collapse(3)
                for (int n = 0; n < ny; n++) {
                    for (int i = 0; i < nphi; i++) {
                        for (int l = 0; l < npt; l++) {
                            y = particleList[pn].y[n];
                            double phi;
                            if (pseudofreeze) {
                                phi = i*deltaphi;
                            } else {
                                phi = phiArray[i];
                            }
                            if (pseudofreeze) {
                                y = Rap(particleList[pn].y[n],
                                        particleList[pn].pt[l], m1);
                            }
                            double spectrum = Edndp3_3bodyN(
                                    y, particleList[pn].pt[l], phi,
                                    m1, m2, m3, mr, norm3,
                                    particleList[pnR].number);
                            if (isnan(spectrum)) {
                                fprintf(stderr, "3 number=%d\n",
                                        particleList[pnR].number);
                            } else {
                                particleList[pn].dNdydptdphi[n][l][i] += (
                                        decay[j].branch*spectrum);
                            }
                        }
                    }
                }
            }
            break;
        }
      
        default:
        {
            printf ("ERROR in add_reso! \n");
            printf ("%i decay not implemented ! \n", abs (decay[j].numpart));
            exit (0);
        }
    }
}

void Freeze::cal_reso_decays(int maxpart, int maxdecay, int bound) {
    music_message << "CALCULATE RESONANCE DECAYS (as fast as I can) ...";
    music_message.flush("info");
    int pn = partid[MHALF + bound];
    int ny = particleList[pn].ny;
    int npt = particleList[pn].npt;
    int nphi = particleList[pn].nphi;
  
    for (int i = maxpart-1; i > pn - 1; i--) {
        // Cycle the particles known from the particle.dat input
        //for (int n1 = 0; n1 < ny; n1++) {
        //    for (int n2 = 0; n2 < npt; n2++) {
        //        for (int n3 = 0; n3 < nphi; n3++) {
        //            particleList[pn].resCont[n1][n2][n3] =0.;
        //        }
        //    }
        //}

        int part = particleList[i].number;
        music_message << "Calculating the decays with "
                      << particleList[i].name;
        music_message.flush("info");
        switch (particleList[i].baryon) {
            // Check the particle is baryon, anti-baryon or meson
            case 1:  // Baryon
            {
                for (int j = 0; j < maxdecay; j++) {
                    // Cycle through every decay channel known
                    // to see if the particle was a daughter particle
                    // in a decay channel
                    int pnR = partid[MHALF + decay[j].reso];
                    for (int k = 0; k < abs(decay[j].numpart); k++) {
                        if ((part == decay[j].part[k])
                                && (decay[j].numpart != 1)) {
                            // Make sure that the decay channel isn't trivial
                            // and contains the daughter particle
                            music_message << "Partid is " << pnR << ". "
                                          << particleList[pnR].name << " into "
                                          << particleList[i].name;
                            music_message.flush("info");
                            add_reso(i, pnR, k, j);
                        }
                    }
                }
                break;
            }

            case -1:  // Anti-Baryon
            {
                for (int j = 0; j < maxdecay; j++) {
                    // Cycle through every decay channel known
                    // to see if the particle was a daughter particle
                    // in a decay channel
                    int pnaR = partid[MHALF - decay[j].reso];
                    for (int k = 0; k < abs(decay[j].numpart); k++) {
                        if ((-part == decay[j].part[k])
                                && (decay[j].numpart != 1)) {
                            // Make sure that the decay channel isn't trivial
                            // and contains the daughter particle
                            music_message << "Partid is " << pnaR << ". "
                                          << particleList[pnaR].name
                                          << " into "
                                          << particleList[i].name;
                            music_message.flush("info");
                            add_reso(i, pnaR, k, j);
                        }
                    }
                }
                break;
            }

            case 0:  // Meson
            {
                for (int j = 0; j < maxdecay; j++) {
                    int pnR = partid[MHALF + decay[j].reso];
                    for (int k = 0; k < abs (decay[j].numpart); k++) {
                        if (particleList[pnR].baryon == 1) {
                            int pnaR = partid[MHALF - decay[j].reso];
                            if ((particleList[i].charge == 0)
                                 && (particleList[i].strange == 0)) {
                                if ((part == decay[j].part[k])
                                    && (decay[j].numpart != 1)) {
                                    music_message << "Partid is " << pnR
                                                  << ". "
                                                  << particleList[pnR].name
                                                  << " into "
                                                  << particleList[i].name;
                                    music_message.flush("info");
                                    music_message << "Partid is " << pnaR
                                                  << ". "
                                                  << particleList[pnaR].name
                                                  << " into "
                                                  << particleList[i].name;
                                    music_message.flush("info");
                                    add_reso(i, pnR, k, j);
                                    add_reso(i, pnaR, k, j);
                                }
                            } else {
                                if ((part == decay[j].part[k])
                                    && (decay[j].numpart != 1)) {
                                    music_message << "Partid is " << pnR
                                                  << ". "
                                                  << particleList[pnR].name
                                                  << " into "
                                                  << particleList[i].name;
                                    music_message.flush("info");
                                    add_reso (i, pnR, k, j);
                                }
                                if ((-part == decay[j].part[k])
                                    && (decay[j].numpart != 1)) {
                                    music_message << "Partid is " << pnaR
                                                  << ". "
                                                  << particleList[pnaR].name
                                                  << " into "
                                                  << particleList[i].name;
                                    music_message.flush("info");
                                    add_reso(i, pnaR, k, j);
                                }
                            }
                        } else {
                            if ((part == decay[j].part[k])
                                && (decay[j].numpart != 1)) {
                                music_message << "Partid is " << pnR
                                              << ". "
                                              << particleList[pnR].name
                                              << " into "
                                              << particleList[i].name;
                                music_message.flush("info");
                                add_reso(i, pnR, k, j);
                            }
                        }
                    }
                }
                break;
            }
            default:
                music_message << "Error in switch in func partden_wdecay";
                music_message.flush("error");
                exit(1);
        }
    }
}

