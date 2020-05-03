#pragma once
#include <cmath>
#include <Eigen/Dense>

using namespace Eigen;



/***************************************************************/
/************************* STRUCT PART *************************/
/***************************************************************/

typedef struct Particle {
	double m; /* Mass */
	Vector3d x; /* Position */
	Vector3d v; /* Velocity */
	Vector3d f; /* Force */
};

typedef struct Spring {
	int a; /* Particle A */
	int b; /* Particle B */
	double r; /* Rest length */
	double s; /* Spring constant */
	double d; /* Damping constant */
};

typedef struct ControlledParticle {
	int i; /* Index */
	Vector3d m; /* Motion */
};

typedef struct ControlledSpring {

};



/********************************************************************/
/************************* MASS SPRING PART *************************/
/********************************************************************/

class MassSpring {
public:
	Particle* p; /* Array of particles */
	Spring* s; /* Array of springs */
	ControlledParticle* cp; /* Array of controlled particles */
	ControlledSpring* cs; /* Array of controlled springs */

	int np; /* Number of particles */
	int ns; /* Number of springs */
	int ncp; /* Number of controlled particles */
	int ncs; /* Number of controlled springs */

	double t; /* Simulation clock */



	/* Constructor */
	MassSpring() {
		p = NULL;
		s = NULL;
		cp = NULL;
		cs = NULL;

		np = 0;
		ns = 0;
		ncp = 0;
		ncs = 0;

		t = 0;
	}

	/* Gather vertex from the particles into dst */
	void particleGetVertex(double* dst) {
		for (int i = 0; i < np; i++) {
			*(dst++) = p[i].x[0];
			*(dst++) = p[i].x[1];
			*(dst++) = p[i].x[2];
		}
	}

	/* Gather element from the springs into dst */
	void particleGetElement(int* dst) {
		for (int i = 0; i < ns; i++) {
			*(dst++) = s[i].a;
			*(dst++) = s[i].b;
		}
	}

	/* Compute unary forces */
	void unaryForces() {
		Vector3d g = { 0, -10, 0 }; /* Gravity */
		double d = 0.001; /* Viscous drag */

		for (int i = 0; i < np; i++)
			p[i].f = p[i].m * g - d * p[i].v;
	}

	/* Compute n-ary forces */
	void n_aryForces() {
		for (int i = 0; i < ns; i++) {
			Vector3d x = p[s[i].a].x - p[s[i].b].x;
			Vector3d v = p[s[i].a].v - p[s[i].b].v;
			Vector3d f = -(s[i].s * (x.norm() - s[i].r) + s[i].d * v.dot(x.normalized())) * x.normalized();

			p[s[i].a].f += f;
			p[s[i].b].f -= f;
		}
	}

	/* Compute spatial interaction forces */
	void spatialInteraction() {
		/* Attraction */

		/* Repulsion */

	}

	/* Compute user interaction forces */
	void userInteraction() {
		/* Controlled particles */
		for (int i = 0; i < ncp; i++) 
			p[cp[i].i].f = cp[i].m;

		/* Structures */

		/* Mouse springs */

	}

	void eulerStep(double deltaT) {
		/* Compute forces */
		unaryForces();
		n_aryForces();
		spatialInteraction();
		userInteraction();

		/* Update state */
		for (int i = 0; i < np; i++) {
			p[i].x += p[i].v * deltaT;
			p[i].v += p[i].f / p[i].m * deltaT;
		}

		/* Update time */
		t += deltaT;
	}
};