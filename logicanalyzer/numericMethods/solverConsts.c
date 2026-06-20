#include "solver.h"
#include <stdint.h>
#include <stdlib.h>
#include "solverConsts.h"
#include <math.h>

/*
	Runge-Kutta-verfahren fünfte ordnung
	"Diese Differentialgleichungsmethode ist scheisse"
*/

const double LA_SOLVER_RK5_A_DATA[LA_SOLVER_RK5_STAGES * LA_SOLVER_RK5_STAGES] = {
	0.0,			0.0,			0.0,			0.0,			0.0,		0.0,
	1.0/3.0,		0.0,			0.0,			0.0,			0.0,		0.0,
	4.0/25.0,		6.0/25.0,		0.0,			0.0,			0.0,		0.0,
	1.0/4.0,		-3.0,			15.0/4.0,		0.0,			0.0,		0.0,
	2.0/27.0,		10.0/9.0,		-50.0/81.0,		8.0/81.0,		0.0,		0.0,
	2.0/25.0,		12.0/25.0,		2.0/15.0,		8.0/75.0,		0.0,		0.0
};

const double LA_SOLVER_RK5_B_DATA[LA_SOLVER_RK5_STAGES] = {
	0.0,	1.0/3.0,	2.0/5.0,	1.0,	2.0/5.0,	4.0/5.0
};

const double LA_SOLVER_RK5_C_DATA[LA_SOLVER_RK5_STAGES] = {
	23.0/192.0,		0.0,	125.0/192.0,	0.0,	-27.0/64.0,		125.0/192.0
};

/*
	Runge-Kutta-verfahren sechste ordnung
*/

const double LA_SOLVER_RK6_A_DATA[LA_SOLVER_RK6_STAGES * LA_SOLVER_RK6_STAGES] = {
		0.0,		0.0,		0.0,		0.0,		0.0,		0.0,		0.0,
		1.0/3.0,	0.0,		0.0,		0.0,		0.0,		0.0,		0.0,
		0.0,		2.0/3.0,	0.0,		0.0,		0.0,		0.0,		0.0,
		1.0/12.0,	1.0/3.0,	-1.0/12.0,	0.0,		0.0,		0.0,		0.0,
		25.0/48.0,	-55.0/24.0,	35.0/48.0,	15.0/8.0,	0.0,		0.0,		0.0,
		3.0/20.0,	-1.0/20.0,	-1.0/8.0,	1.0/2.0,	1.0/10.0,	0.0,		0.0,
		-261.0/260.0,33.0/13.0,	43.0/156.0,	-118.0/39.0,32.0/195.0,	80.0/39.0,	0.0
};

const double LA_SOLVER_RK6_B_DATA[LA_SOLVER_RK6_STAGES] = {
	0.0, 1.0/3.0, 2.0/3.0, 1.0/3.0, 5.0/6.0, 1.0/6.0, 1.0
};

const double LA_SOLVER_RK6_C_DATA[LA_SOLVER_RK6_STAGES] = {
	13.0/200.0, 0.0, 11.0/40.0, 11.0/40.0, 4.0/25.0, 4.0/25.0, 13.0/200.0
};

/*
	Runge-Kutta-verfahren achte ordnung
*/

const double LA_SOLVER_RK8_A_DATA[LA_SOLVER_RK8_STAGES * LA_SOLVER_RK8_STAGES] = {
                   0,                  0,                  0,                  0,                  0,                  0,                  0,                  0,                  0,                  0,			0,
   0.500000000000000,                  0,                  0,                  0,                  0,                  0,                  0,                  0,                  0,                  0,			0,
   0.250000000000000,  0.250000000000000,                  0,                  0,                  0,                  0,                  0,                  0,                  0,                  0,			0,
   0.142857142857143, -0.211711500865995,  0.896181193362841,                  0,                  0,                  0,                  0,                  0,                  0,                  0,			0,
   0.185506853511379,                  0,  0.290957186981323,  0.065148509147001,                  0,                  0,                  0,                  0,                  0,                  0,			0,
   0.199636993644913,                  0,  0.377293769304329, -0.463455389640606,  0.386524626691364,                  0,                  0,                  0,                  0,                  0,			0,
   0.128986292977242,                  0, -0.033025511314485, -0.349705286317741,  0.328517213141737,  0.097900456159260,                  0,                  0,                  0,                  0,			0,
  -0.071428571428571,                  0,  0.195800912318519, -0.232662610208074, -0.002002165993115,  0.183932228431733,  0.111111111111111,                  0,                  0,                  0,			0,
   0.031250000000000,                  0,                  0, -0.000000000000000, -0.009086961100821,  0.152777777777778, -0.632546160695910,  0.957605344018952,                  0,                  0,			0,
   0.071428571428571,                  0,                  0,                  0,  0.111111111111111, -0.636933105911264,  2.031083139166862, -1.810863082937754,  1.062498446770463,                  0,			0,
                   0,                  0,                  0, -0.000000000000000, -0.551220563072729,  2.451380432416967, -7.164951553231381,  7.553840442120270, -2.229158210194745,  0.940109451961618, 			0
};

const double LA_SOLVER_RK8_B_DATA[LA_SOLVER_RK8_STAGES] = {
                   0,  0.500000000000000,  0.500000000000000,  0.827326835353989,  0.827326835353989,  0.500000000000000,  0.172673164646011,  0.172673164646011,  0.500000000000000,  0.827326835353989,  1.000000000000000
};

const double LA_SOLVER_RK8_C_DATA[LA_SOLVER_RK8_STAGES] = {
   0.050000000000000,                  0,                  0,                  0,                  0,                  0,                  0,  0.272222222222222,  0.355555555555556,  0.272222222222222,  0.050000000000000
};

/*
 *	Runge Kutta Fehlberg 4(5)
 */

const double LA_SOLVER_RKF45_A_DATA[LA_SOLVER_RKF45_STAGES*LA_SOLVER_RKF45_STAGES] = {
		      0.0,			 0.0,			 0.0,		  0.0,		  0.0,		0.0,
		  2.0/9.0,			 0.0,			 0.0,		  0.0,		  0.0,		0.0,
		  1.0/12.0,		 1.0/4.0,			 0.0,		  0.0,		  0.0,		0.0,
		 69.0/128.0,  -243.0/128.0,	   135.0/64.0,	      0.0,		  0.0,		0.0,
		-17.0/12.0,		27.0/4.0,	   -27.0/5.0,	16.0/15.0,		  0.0,		0.0,	
		 65.0/432.0,	-5.0/16.0,		13.0/16.0,	 4.0/27.0,	5.0/144.0,		0.0
};

const double LA_SOLVER_RKF45_B_DATA[LA_SOLVER_RKF45_STAGES]  = {
	0.0,	    2.0/9.0,	 1.0/3.0,	 3.0/4.0,	1.0,	    5.0/6.0
};

const double LA_SOLVER_RKF45_C_DATA[LA_SOLVER_RKF45_STAGES*2] = {
	 1.0/9.0,	0.0,	 9.0/20.0,	16.0/45.0,	1.0/12.0, 	     0.0,
	47.0/450.0,	0.0,	12.0/25.0,	32.0/225.0,	1.0/30.0, 	6.0/25.0
};

/*
 *	Dormand Prince 4(5)
 */

const double LA_SOLVER_DOP45_A_DATA[LA_SOLVER_DOP45_STAGES*LA_SOLVER_DOP45_STAGES] = {
		      0.0,			      0.0,			    0.0,		  		   0.0,					  0.0,			  0.0,		0.0,
		  1.0/5.0,			      0.0,			    0.0,		  		   0.0,		 			  0.0,			  0.0,		0.0,
		  3.0/40.0,	         9.0/40.0,		        0.0,		  		   0.0,		 			  0.0,			  0.0,		0.0,
		 44.0/45.0,	  	   -56.0/15.0,	       32.0/9.0,		 		   0.0,		  			  0.0,			  0.0,		0.0,
	  19372.0/6561.0,	-25360.0/2187.0,	64448.0/6561.0,		  -212.0/729.0,		 			  0.0,			  0.0,		0.0,
	   9017.0/3168.0,	  -355.0/33.0,	    46732.0/5247.0,		    49.0/176.0,		  -5103.0/18656.0,			  0.0,		0.0,
	     35.0/384.0,	  		  0.0,	      500.0/1113.0,		   125.0/192.0,		  -2187.0/6784.0,		11.0/84.0,		0.0,
};

const double LA_SOLVER_DOP45_B_DATA[LA_SOLVER_DOP45_STAGES]  = {
	0.0,	    1.0/5.0,	 3.0/10.0,	 4.0/5.0,	8.0/9.0,	    1.0,		1.0
};

const double LA_SOLVER_DOP45_C_DATA[LA_SOLVER_DOP45_STAGES*2] = {
	35.0/384.0,		0.0,	500.0/1113.0,	125.0/192.0,	-2187.0/6784.0,     11.0/84.0,		     0.0,
  5179.0/57600.0,	0.0,   7571.0/16995.0,	393.0/640.0,   -92097.0/339200.0,  187.0/2100.0,	1.0/40.0
};

/*
 *	Bogacki–Shampine 2(3)
 */

const double LA_SOLVER_BS23_A_DATA[LA_SOLVER_BS23_STAGES*LA_SOLVER_BS23_STAGES] = {
		      0.0,			      0.0,			    0.0,		  		   0.0,
		  1.0/2.0,			      0.0,			    0.0,		  		   0.0,
		  	  0.0,	          3.0/4.0,		        0.0,		  		   0.0,
		  2.0/9.0,	  	      1.0/3.0,	        4.0/9.0,		 		   0.0,
};

const double LA_SOLVER_BS23_B_DATA[LA_SOLVER_BS23_STAGES]  = {
	0.0,	    1.0/2.0,	 3.0/4.0,	 	1.0,
};

const double LA_SOLVER_BS23_C_DATA[LA_SOLVER_BS23_STAGES*2] = {
 	 2.0/9.0,		1.0/3.0,	4.0/9.0,	     0.0,
     7.0/24.0,	    1.0/4.0,    1.0/3.0,	 1.0/8.0,
};

/*
 *	Cash-Karp 4(5)
 */

const double LA_SOLVER_RKCK45_A_DATA[LA_SOLVER_RKCK45_STAGES*LA_SOLVER_RKCK45_STAGES] = {
		      0.0,			      0.0,			    0.0,		  		   0.0,					0.0,				0.0,
    	  1.0/5.0,			      0.0,			    0.0,		  		   0.0,					0.0,				0.0,
          3.0/40.0,			 9.0/40.0,			    0.0,		  		   0.0,					0.0,				0.0,
          3.0/10.0,			-9.0/10.0,			6.0/5.0,		  		   0.0,					0.0,				0.0,
        -11.0/54.0,			 5.0/2.0,		  -70.0/27.0,		     35.0/27.0,					0.0,				0.0,
       1631.0/55296.0,	   175.0/512.0,		  575.0/13824.0,      44275.0/110592.0,    253.0/4096.0,				0.0,
};

const double LA_SOLVER_RKCK45_B_DATA[LA_SOLVER_RKCK45_STAGES]  = {
	0.0,	    1.0/5.0,	 3.0/10.0,	 	3.0/5.0,	1.0,	7.0/8.0
};

const double LA_SOLVER_RKCK45_C_DATA[LA_SOLVER_RKCK45_STAGES*2] = {
 	    37.0/378.0,		    0.0,	  250.0/621.0,	       125.0/594.0,				  0.0,		512.0/1771.0,
     28257.0/27648.0,	    0.0,    18575.0/48384.0,	 13525.0/55296.0,	277.0/14336.0,		  1.0/4.0
};

/*
 *	GDGL 1(2) (gewöhnliche Differentialgleichungen)
 */

const double LA_SOLVER_ODE12_A_DATA[LA_SOLVER_ODE12_STAGES*LA_SOLVER_ODE12_STAGES] = {
		      0.0,			      0.0,
		      1.0,			      0.0,
};

const double LA_SOLVER_ODE12_B_DATA[LA_SOLVER_ODE12_STAGES]  = {
	0.0,	    1.0
};

const double LA_SOLVER_ODE12_C_DATA[LA_SOLVER_ODE12_STAGES*2] = {
	1.0/2.0,	    1.0/2.0,
	1.0,			0.0
};

/*
 *	Runge-Kutta Fehlberg 7(8)
 */

const double LA_SOLVER_RKF78_A_DATA[LA_SOLVER_RKF78_STAGES*LA_SOLVER_RKF78_STAGES] = {
		      0.0,			 0.0,			 0.0,		    0.0,		    0.0,			   0.0,				0.0,			 0.0,			0.0,			0.0,		0.0,		0.0,		0.0,
		 2.0/27.0,			 0.0,			 0.0,		    0.0,		    0.0,			   0.0,				0.0,			 0.0,			0.0,			0.0,		0.0,		0.0,		0.0,
		 1.0/36.0,	    1.0/12.0,			 0.0,	  	    0.0,		    0.0,			   0.0,				0.0,			 0.0,			0.0,			0.0,		0.0,		0.0,		0.0,
		 1.0/24.0,	    	 0.0,		 1.0/8.0,	   	    0.0,		    0.0,			   0.0,				0.0,			 0.0,			0.0,			0.0,		0.0,		0.0,		0.0,
		 5.0/12.0,	    	 0.0,	   -25.0/16.0,	   25.0/16.0,		    0.0,			   0.0,				0.0,			 0.0,			0.0,			0.0,		0.0,		0.0,		0.0,
		 1.0/20.0,	    	 0.0,	   		 0.0,	 	1.0/4.0,	     1.0/5.0,			   0.0,				0.0,			 0.0,			0.0,			0.0,		0.0,		0.0,		0.0,
	   -25.0/108.0,	    	 0.0,	   		 0.0,  	  125.0/108.0,	   -65.0/27.0,		125.0/54.0,				0.0,			 0.0,			0.0,			0.0,		0.0,		0.0,		0.0,
	    31.0/300.0,	    	 0.0,	   		 0.0,  	  		0.0,	    61.0/225.0,	     -2.0/9.0,		 13.0/900.0,			 0.0,			0.0,			0.0,		0.0,		0.0,		0.0,
	           2.0,	    	 0.0,	   		 0.0,  	  -53.0/6.0,	   704.0/45.0,	   -107.0/9.0,		 67.0/90.0,		 	     3.0,			0.0,			0.0,		0.0,		0.0,		0.0,
	   -91.0/108.0,	    	 0.0,	   		 0.0,  	   23.0/108.0,	  -976.0/135.0,	    311.0/54.0,	    -19.0/60.0,	   		17.0/6.0,	  -1.0/12.0,			0.0,		0.0,		0.0,		0.0,
	 -2383.0/4100.0,	   	 0.0,	   		 0.0,  	 -341.0/164.0,	  4496.0/1025.0,   -301.0/82.0,	   2133.0/4100.0,	   	45.0/82.0,	  45.0/164.0,	  18.0/41.0,		0.0,		0.0,		0.0,
	     3.0/205.0,	   	 	 0.0,	   		 0.0,  	        0.0,	  		 0.0,        -6.0/41.0,	     -3.0/205.0,	   	-3.0/41.0,	   3.0/41.0,	   6.0/41.0,		0.0,		0.0,		0.0,
	 -1777.0/4100.0,	   	 0.0,	   		 0.0,  	 -341.0/164.0,	  4496.0/1025.0,   -289.0/82.0,	   2193.0/4100.0,	   	51.0/82.0,	  33.0/164.0,	  12.0/41.0,		0.0,		1.0,		0.0,
};

const double LA_SOLVER_RKF78_B_DATA[LA_SOLVER_RKF78_STAGES]  = {
	0.0,	    2.0/27.0,	 1.0/9.0,	 1.0/6.0,	5.0/12.0,	    1.0/2.0,		5.0/6.0,		1.0/6.0,		2.0/3.0,		1.0/3.0,		1.0,		0.0,		1.0
};

const double LA_SOLVER_RKF78_C_DATA[LA_SOLVER_RKF78_STAGES*2] = {
	41.0/840.0,		0.0,	0.0,	0.0,	0.0,	34.0/105.0,		9.0/35.0,	9.0/35.0,	9.0/280.0,		9.0/280.0,		41.0/840.0,		0.0,		0.0,
	0.0,			0.0,	0.0,	0.0,	0.0,	34.0/105.0,		9.0/35.0,	9.0/35.0,	9.0/280.0,		9.0/280.0,		0.0,			41.0/840.0,	41.0/480.0
};

/*
	Gauss-Legendre vierte ordnung
*/

const double LA_SOLVER_GL4_A_DATA[LA_SOLVER_GL4_STAGES * LA_SOLVER_GL4_STAGES] = {
		1.0/4.0,							(1.0/4.0) - (1.0/6.0)*sqrt(3.0),
		(1.0/4.0) + (1.0/6.0)*sqrt(3.0),	1.0/4.0
};

const double LA_SOLVER_GL4_B_DATA[LA_SOLVER_GL4_STAGES] = {
		(1.0/2.0) - (1.0/6.0)*sqrt(3.0),	(1.0/2.0) + (1.0/6.0)*sqrt(3.0),		
};

const double LA_SOLVER_GL4_C_DATA[LA_SOLVER_GL4_STAGES] = {
		1.0/2.0, 							1.0/2.0
};

/*
	Gauss-Legendre sechste ordnung
*/

const double LA_SOLVER_GL6_A_DATA[LA_SOLVER_GL6_STAGES * LA_SOLVER_GL6_STAGES] = {
		(5.0/36.0),								(2.0/9.0) - (1.0/15.0)*sqrt(15.0),		(5.0/36.0) - (1.0/30.0)*sqrt(15.0),
		(5.0/36.0) + (1.0/24.0)*sqrt(15.0),		2.0/9.0,								(5.0/36.0) - (1.0/24.0)*sqrt(15.0),
		(5.0/36.0) + (1.0/30.0)*sqrt(15.0),		(2.0/9.0) + (1.0/15.0)*sqrt(15.0),		(5.0/36.0)
};

const double LA_SOLVER_GL6_B_DATA[LA_SOLVER_GL6_STAGES] = {
		(1.0/2.0) - (1.0/10.0)*sqrt(15.0),		(1.0/2.0),								(1.0/2.0) + (1.0/10.0)*sqrt(15.0)
};

const double LA_SOLVER_GL6_C_DATA[LA_SOLVER_GL6_STAGES] = {
		5.0/18.0,								4.0/9.0,								5.0/18.0
};

/*
	Radau IA dritte ordnung
*/

const double LA_SOLVER_R1A3_A_DATA[LA_SOLVER_R1A3_STAGES * LA_SOLVER_R1A3_STAGES] = {
		1.0/4.0,		-1.0/4.0,
		1.0/4.0,		5.0/12.0,
};

const double LA_SOLVER_R1A3_B_DATA[LA_SOLVER_R1A3_STAGES] = {
		0.0,			2.0/3.0
};

const double LA_SOLVER_R1A3_C_DATA[LA_SOLVER_R1A3_STAGES] = {
		1.0/4.0,		3.0/4.0
};

/*
	Radau IA fünfte ordnung
*/

const double LA_SOLVER_R1A5_A_DATA[LA_SOLVER_R1A5_STAGES * LA_SOLVER_R1A5_STAGES] = {
		1.0/9.0,		(-1.0 - sqrt(6.0))/18.0,				(-1.0 + sqrt(6.0))/18.0,
		1.0/9.0,		(11.0/45.0) + (7.0*sqrt(6.0))/360.0,	(11.0/45.0) - (43.0*sqrt(6.0))/360.0,
		1.0/9.0,		(11.0/45.0) + ((43.0*sqrt(6.0)))/360.0,	(11.0/45.0) - (7.0*sqrt(6.0))/360.0
};

const double LA_SOLVER_R1A5_B_DATA[LA_SOLVER_R1A5_STAGES] = {
		0.0,			(3.0/5.0) - (sqrt(6.0)/10.0),			(3.0/5.0) + (sqrt(6.0)/10.0)
};

const double LA_SOLVER_R1A5_C_DATA[LA_SOLVER_R1A5_STAGES] = {
		1.0/9.0,		(4.0/9.0) + sqrt(6.0)/36.0,				(4.0/9.0) - (sqrt(6.0)/36.0)
};

/*
	Radau IIA dritte ordnung
*/

const double LA_SOLVER_R2A3_A_DATA[LA_SOLVER_R2A3_STAGES * LA_SOLVER_R2A3_STAGES] = {
		5.0/12.0,		-1.0/12.0,
		3.0/4.0,		1.0/4.0
};

const double LA_SOLVER_R2A3_B_DATA[LA_SOLVER_R2A3_STAGES] = {
		1.0/3.0,		1.0
};

const double LA_SOLVER_R2A3_C_DATA[LA_SOLVER_R2A3_STAGES] = {
		3.0/4.0,		1.0/4.0
};

/*
	Radau IA fünfte ordnung
*/

const double LA_SOLVER_R2A5_A_DATA[LA_SOLVER_R2A5_STAGES * LA_SOLVER_R2A5_STAGES] = {
		(11.0/45.0) - (7.0*sqrt(6.0))/360.0,		(37.0/225.0) - ((169.0*sqrt(6.0)))/1800.0,			-(2.0/225.0) + (sqrt(6.0)/75.0),
		(37.0/225.0) + (169.0*sqrt(6.0))/1800.0,	(11.0/45.0) + ((7.0*sqrt(6.0)))/360.0,				-(2.0/225.0) - (sqrt(6.0)/75.0),
		(4.0/9.0) - (sqrt(6.0))/36.0,				(4.0/9.0) + ((sqrt(6.0)))/36.0,						1.0/9.0
};

const double LA_SOLVER_R2A5_B_DATA[LA_SOLVER_R2A5_STAGES] = {
		(2.0/5.0) - (sqrt(6.0)/10.0),		(2.0/5.0) + (sqrt(6.0)/10.0),			1.0,
};

const double LA_SOLVER_R2A5_C_DATA[LA_SOLVER_R2A5_STAGES] = {
		(4.0/9.0) - (sqrt(6.0))/36.0,				(4.0/9.0) + ((sqrt(6.0)))/36.0,						1.0/9.0
};

/*
	Lobatto IIIA zweite ordnung
*/

const double LA_SOLVER_L3A2_A_DATA[LA_SOLVER_L3A2_STAGES * LA_SOLVER_L3A2_STAGES] = {
		0.0,		0.0,
		1.0/2.0,	1.0/2.0
};

const double LA_SOLVER_L3A2_B_DATA[LA_SOLVER_L3A2_STAGES] = {
		0.0,		1.0
};

const double LA_SOLVER_L3A2_C_DATA[LA_SOLVER_L3A2_STAGES] = {
		1.0/2.0,	1.0/2.0
};

/*
	Lobatto IIIA vierte ordnung
*/

const double LA_SOLVER_L3A4_A_DATA[LA_SOLVER_L3A4_STAGES * LA_SOLVER_L3A4_STAGES] = {
		0.0,		0.0,		0.0,
		5.0/24.0,	1.0/3.0,	-1.0/24.0,
		1.0/6.0,	2.0/3.0,	1.0/6.0
};

const double LA_SOLVER_L3A4_B_DATA[LA_SOLVER_L3A4_STAGES] = {
		0.0,		1.0/2.0,	1.0
};

const double LA_SOLVER_L3A4_C_DATA[LA_SOLVER_L3A4_STAGES] = {
		1.0/6.0,	2.0/3.0,	1.0/6.0
};

/*
	Lobatto IIIA sechste ordnung
*/

const double LA_SOLVER_L3A6_A_DATA[LA_SOLVER_L3A6_STAGES * LA_SOLVER_L3A6_STAGES] = {
		0.0,							0.0,							0.0,								0.0,
		(11.0 + sqrt(5.0))/120.0,		(25.0 - sqrt(5.0))/120.0,		(25.0 - 13.0*sqrt(5.0))/120.0,		(-1.0 + sqrt(5.0))/120.0,	
		(11.0 - sqrt(5.0))/120.0,		(25.0 + 13.0*sqrt(5.0))/120.0,	(25.0 + sqrt(5.0))/120.0,			(-1.0 - sqrt(5.0))/120.0,	
		1.0/12.0,						5.0/12.0,						5.0/12.0,							1.0/12.0
};

const double LA_SOLVER_L3A6_B_DATA[LA_SOLVER_L3A6_STAGES] = {
		0.0,							(1.0/2.0) - sqrt(5.0)/10.0, 	(1.0/2.0) + sqrt(5.0)/10.0,			1.0
};

const double LA_SOLVER_L3A6_C_DATA[LA_SOLVER_L3A6_STAGES] = {
		1.0/12.0,						5.0/12.0,						5.0/12.0,							1.0/12.0
};

/*
	Lobatto IIIA achte ordnung
*/

const double LA_SOLVER_L3A8_A_DATA[LA_SOLVER_L3A8_STAGES * LA_SOLVER_L3A8_STAGES] = {
		0.0,								0.0,								0.0,								0.0,								0.0,
		(119.0 + 3.0*sqrt(21.0))/1960.0,	(343.0 - 9.0*sqrt(21.0))/2520.0,	(392.0 - 96.0*sqrt(21.0))/2205.0,	(343.0 - 69.0*sqrt(21.0))/2520.0,	(-21.0 + 3.0*sqrt(21.0))/1960.0,
		13.0/320.0,							(392.0 + 105.0*sqrt(21.0))/2880.0,	8.0/45.0,							(392.0 - 105.0*sqrt(21.0))/2880.0,	3.0/320.0,
		(119.0 - 3.0*sqrt(21.0))/1960.0,	(343.0 + 69.0*sqrt(21.0))/2520.0,	(392.0 + 96.0*sqrt(21.0))/2205.0,	(343.0 + 9.0*sqrt(21.0))/2520.0,	(-21.0 - 3.0*sqrt(21.0))/1960.0,
		1.0/20.0,							49.0/180.0,							16.0/45.0,							49.0/180.0,							1.0/20.0
};

const double LA_SOLVER_L3A8_B_DATA[LA_SOLVER_L3A8_STAGES] = {
		0.0,							(1.0/2.0) - sqrt(21.0)/14.0, 	1.0/2.0,							(1.0/2.0) + sqrt(21.0)/14.0,			1.0
};

const double LA_SOLVER_L3A8_C_DATA[LA_SOLVER_L3A8_STAGES] = {
		1.0/20.0,						49.0/180.0,						16.0/45.0,							49.0/180.0,							1.0/20.0
};

/*
	Lobatto IIIB zweite ordnung
*/

const double LA_SOLVER_L3B2_A_DATA[LA_SOLVER_L3B2_STAGES * LA_SOLVER_L3B2_STAGES] = {
		1.0/2.0,	0.0,
		1.0/2.0,	0.0
};

const double LA_SOLVER_L3B2_B_DATA[LA_SOLVER_L3B2_STAGES] = {
		0.0,		1.0
};

const double LA_SOLVER_L3B2_C_DATA[LA_SOLVER_L3B2_STAGES] = {
		1.0/2.0,	1.0/2.0
};

/*
	Lobatto IIIB vierte ordnung
*/

const double LA_SOLVER_L3B4_A_DATA[LA_SOLVER_L3B4_STAGES * LA_SOLVER_L3B4_STAGES] = {
		1.0/6.0,	-1.0/6.0,	0.0,
		1.0/6.0,	1.0/3.0,	0.0,
		1.0/6.0,	5.0/6.0,	0.0
};

const double LA_SOLVER_L3B4_B_DATA[LA_SOLVER_L3B4_STAGES] = {
		0.0,		1.0/2.0,	1.0
};

const double LA_SOLVER_L3B4_C_DATA[LA_SOLVER_L3B4_STAGES] = {
		1.0/6.0,	2.0/3.0,	1.0/6.0
};

/*
	Lobatto IIIB sechste ordnung
*/

const double LA_SOLVER_L3B6_A_DATA[LA_SOLVER_L3B6_STAGES * LA_SOLVER_L3B6_STAGES] = {
		1.0/12.0,						(-1.0 - sqrt(5.0))/24.0,		(-1.0 + sqrt(5.0))/24.0,			0.0,
		1.0/12.0,						(25.0 + sqrt(5.0))/120.0,		(25.0 - 13.0*sqrt(5.0))/24.0,		0.0,
		1.0/12.0,						(25.0 + 13.0*sqrt(5.0))/120.0,	(25.0 - sqrt(5.0))/24.0,			0.0,
		1.0/12.0,						(11.0 - sqrt(5.0))/24.0,		(11.0 + sqrt(5.0))/24.0,			0.0,
};

const double LA_SOLVER_L3B6_B_DATA[LA_SOLVER_L3B6_STAGES] = {
		0.0,							(1.0/2.0) - sqrt(5.0)/10.0, 	(1.0/2.0) + sqrt(5.0)/10.0,			1.0
};

const double LA_SOLVER_L3B6_C_DATA[LA_SOLVER_L3B6_STAGES] = {
		1.0/12.0,						5.0/12.0,						5.0/12.0,							1.0/12.0
};

/*
	Lobatto IIIB achte ordnung
*/

const double LA_SOLVER_L3B8_A_DATA[LA_SOLVER_L3B8_STAGES * LA_SOLVER_L3B8_STAGES] = {
		1.0/20.0,						(-7.0 - sqrt(21.0))/120.0,			1.0/15.0,							(-7.0 + sqrt(21.0))/120.0,			0.0,
		1.0/20.0,						(343.0 + 9.0*sqrt(21.0))/2520.0,	(56.0 - 15.0*sqrt(21.0))/315.0,		(343.0 - 69.0*sqrt(21.0))/2520.0,	0.0,
		1.0/20.0,						(49.0 + 12.0*sqrt(21.0))/360.0,		8.0/45.0,							(49.0 - 12.0*sqrt(21.0))/360.0,		0.0,
		1.0/20.0,						(343.0 + 69.0*sqrt(21.0))/2520.0,	(56.0 + 15.0*sqrt(21.0))/315.0,		(343.0 + 9.0*sqrt(21.0))/2520.0,	0.0,
		1.0/20.0,						(119.0 - 3.0*sqrt(21.0))/360.0,		13.0/45.0,							(119.0 + 3.0*sqrt(21.0))/360.0,		0.0,
};

const double LA_SOLVER_L3B8_B_DATA[LA_SOLVER_L3B8_STAGES] = {
		0.0,							(1.0/2.0) - sqrt(21.0)/14.0, 	1.0/2.0,							(1.0/2.0) + sqrt(21.0)/14.0,			1.0
};

const double LA_SOLVER_L3B8_C_DATA[LA_SOLVER_L3B8_STAGES] = {
		1.0/20.0,						49.0/180.0,						16.0/45.0,							49.0/180.0,							1.0/20.0
};

/*
	Lobatto IIIC zweite ordnung
*/

const double LA_SOLVER_L3C2_A_DATA[LA_SOLVER_L3C2_STAGES * LA_SOLVER_L3C2_STAGES] = {
		1.0/2.0,	-1.0/2.0,
		1.0/2.0,	1.0/2.0
};

const double LA_SOLVER_L3C2_B_DATA[LA_SOLVER_L3C2_STAGES] = {
		0.0,		1.0
};

const double LA_SOLVER_L3C2_C_DATA[LA_SOLVER_L3C2_STAGES] = {
		1.0/2.0,	1.0/2.0
};

/*
	Lobatto IIIC vierte ordnung
*/

const double LA_SOLVER_L3C4_A_DATA[LA_SOLVER_L3C4_STAGES * LA_SOLVER_L3C4_STAGES] = {
		1.0/6.0,	-1.0/3.0,	1.0/6.0,
		1.0/6.0,	5.0/12.0,	-1.0/12.0,
		1.0/6.0,	2.0/3.0,	1.0/6.0
};

const double LA_SOLVER_L3C4_B_DATA[LA_SOLVER_L3C4_STAGES] = {
		0.0,		1.0/2.0,	1.0
};

const double LA_SOLVER_L3C4_C_DATA[LA_SOLVER_L3C4_STAGES] = {
		1.0/6.0,	2.0/3.0,	1.0/6.0
};

/*
	Lobatto IIIB sechste ordnung
*/

const double LA_SOLVER_L3C6_A_DATA[LA_SOLVER_L3C6_STAGES * LA_SOLVER_L3C6_STAGES] = {
		1.0/12.0,						-sqrt(5.0)/12.0,				sqrt(5.0)/12.0,						-1.0/12.0,
		1.0/12.0,						1.0/4.0,						(10.0 - 7.0*sqrt(5.0))/60.0,		sqrt(5.0)/60.0,
		1.0/12.0,						(10.0 + 7.0*sqrt(5.0))/60.0,	1.0/4.0,							-sqrt(5.0)/60.0,
		1.0/12.0,						5.0/12.0,						5.0/12.0,							1.0/12.0
};

const double LA_SOLVER_L3C6_B_DATA[LA_SOLVER_L3C6_STAGES] = {
		0.0,							(1.0/2.0) - sqrt(5.0)/10.0, 	(1.0/2.0) + sqrt(5.0)/10.0,			1.0
};

const double LA_SOLVER_L3C6_C_DATA[LA_SOLVER_L3C6_STAGES] = {
		1.0/12.0,						5.0/12.0,						5.0/12.0,							1.0/12.0
};

/*
	Lobatto IIIC achte ordnung
*/

const double LA_SOLVER_L3C8_A_DATA[LA_SOLVER_L3C8_STAGES * LA_SOLVER_L3C8_STAGES] = {
		1.0/20.0,						-(7.0/60.0),						2.0/15.0,							-(7.0/60.0),						1.0/20.0,
		1.0/20.0,						29.0/180.0,							(47.0 - 15.0*sqrt(21.0))/315.0,		(203.0 - 30.0*sqrt(21.0))/1260.0,	-(3.0/140.0),
		1.0/20.0,						(329.0 + 105.0*sqrt(21.0))/2880.0,	73.0/360.0,							(329.0 - 105.0*sqrt(21.0))/2880.0,	3.0/160.0,
		1.0/20.0,						(203.0 + 30.0*sqrt(21.0))/1260.0,	(47.0 + 15.0*sqrt(21.0))/315.0,		29.0/180.0,							-(3.0/140.0),
		1.0/20.0,						49.0/180.0,							16.0/45.0,							49.0/180.0,							1.0/20.0
};

const double LA_SOLVER_L3C8_B_DATA[LA_SOLVER_L3C8_STAGES] = {
		0.0,							(1.0/2.0) - sqrt(21.0)/14.0, 	1.0/2.0,							(1.0/2.0) + sqrt(21.0)/14.0,			1.0
};

const double LA_SOLVER_L3C8_C_DATA[LA_SOLVER_L3C8_STAGES] = {
		1.0/20.0,						49.0/180.0,						16.0/45.0,							49.0/180.0,							1.0/20.0
};
