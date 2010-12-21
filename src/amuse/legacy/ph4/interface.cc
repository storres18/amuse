#include "interface.h"

// A stub of this file is machine generated, but the content is
// hand-coded.  SAVE A COPY (here interface.cc.1) to avoid accidental
// overwriting!

#include "src/stdinc.h"
#include "src/jdata.h"
#include "src/idata.h"
#include "src/scheduler.h"

static jdata *jd = NULL;
static idata *id = NULL;
static scheduler *s = NULL;

// Setup and parameters.

int initialize_code()
{
    // Begin the initialization by creating the jdata data structure.

    jd = new jdata;
    jd->setup_mpi(MPI::COMM_WORLD);
    if (jd->mpi_rank == 0) {
	cout << "initialize_code: ";
	PRL(jd->mpi_size);
    }
    jd->system_time = 0;		// TBD
    return 0;
}

int set_eps2(double epsilon_squared)
{
    jd->eps2 = epsilon_squared;
    return 0;
}

int get_eps2(double * epsilon_squared)
{
    *epsilon_squared = jd->eps2;
    return 0;
}

int set_eta(double timestep_parameter)
{
    jd->eta = timestep_parameter;
    return 0;
}

int get_eta(double * timestep_parameter)
{
    *timestep_parameter = jd->eta;
    return 0;
}

int set_gpu(int gpu)
{
    jd->use_gpu = jd->have_gpu && (gpu == 1);
    return 0;
}

int get_gpu(int * gpu)
{
    *gpu = jd->use_gpu;
    return 0;
}

int set_time(double sys_time)
{
    jd->system_time = sys_time;
    return 0;
}

int get_time(double * sys_time)
{
    *sys_time = jd->system_time;
    return 0;
}

int commit_parameters()
{
    // Perform any needed setup after initial code parameters have been set.

    if (jd->mpi_rank == 0) {
	cout << "commit_parameters: ";
	PRC(jd->have_gpu); PRL(jd->use_gpu);
    }

    return 0;
}

int recommit_parameters()
{
    // Perform any needed changes after code parameters have been reset.

    return 0;
}

int commit_particles()
{
    // Complete the initialization, after all particles have been loaded.

    jd->initialize_arrays();
    id = new idata(jd);	  // set up idata data structures (sets acc and jerk)
    jd->set_initial_timestep();		// set timesteps (needs acc and jerk)
    s = new scheduler(jd);
    return 0;
}

int recommit_particles()
{
    // Reinitialize/reset the system after particles have been added
    // or removed.  The system must be synchronized at some reasonable
    // system_time, so we just need to recompute forces and update the
    // GPU and scheduler.  Note that we don't resize the jdata or
    // idata arrays.  To resize idata, just delete and create a new
    // one.  Resizing jdata is more complicated -- defer for now.

    if (!jd->use_gpu)
	jd->predict_all(jd->system_time, true);	// set pred quantities
    else
	jd->initialize_gpu(true);		// reload the GPU
    id->setup(*jd);				// compute acc and jerk
    s->initialize();				// reconstruct the scheduler
    return 0;
}

int cleanup_code()
{
    // Clean up at the end of the calculation.

    jd->cleanup();
    return 0;
}

// Setters and getters for individual particles.

int new_particle(int * index_of_the_particle,
		 double mass, double radius, 
		 double x, double y, double z,
		 double vx, double vy, double vz)
{
    // Add a particle to the system.  Let the system set the id.

    *index_of_the_particle = jd->add_particle(mass, radius,
					      vec(x,y,z), vec(vx,vy,vz));
    return 0;
}

int delete_particle(int index_of_the_particle)
{
    // Remove a particle from the system.

    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    jd->remove_particle(j);
    return 0;
}

int get_index_of_next_particle(int index_of_the_particle, 
			       int * index_of_the_next_particle)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    else if (j >= jd->get_nj()-1) return 1;
    else *index_of_the_next_particle = jd->id[j+1];
    return 0;
}

int set_state(int index_of_the_particle,
	      double mass, double radius, 
	      double x, double y, double z,
	      double vx, double vy, double vz)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    jd->mass[j] = mass;
    jd->radius[j] = radius;
    jd->pos[j][0] = x;
    jd->pos[j][1] = y;
    jd->pos[j][2] = z;
    jd->vel[j][0] = vx;
    jd->vel[j][1] = vy;
    jd->vel[j][2] = vz;
    return 0;
}

int get_state(int index_of_the_particle,
	      double * mass, double * radius, 
	      double * x, double * y, double * z,
	      double * vx, double * vy, double * vz)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    *mass = jd->mass[j];
    *radius = jd->radius[j];
    *x = jd->pos[j][0];
    *y = jd->pos[j][1];
    *z = jd->pos[j][2];
    *vx = jd->vel[j][0];
    *vy = jd->vel[j][1];
    *vz = jd->vel[j][2];
    return 0;
}

int set_mass(int index_of_the_particle, double mass)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    jd->mass[j] = mass;
    return 0;
}

int get_mass(int index_of_the_particle, double * mass)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    *mass = jd->mass[j];
    return 0;
}

int set_radius(int index_of_the_particle, double radius)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    jd->radius[j] = radius;
    return 0;
}

int get_radius(int index_of_the_particle, double * radius)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    *radius = jd->radius[j];
    return 0;
}

int set_position(int index_of_the_particle,
		 double x, double y, double z)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    jd->pos[j][0] = x;
    jd->pos[j][1] = y;
    jd->pos[j][2] = z;
    return 0;
}

int get_position(int index_of_the_particle,
		 double * x, double * y, double * z)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    *x = jd->pos[j][0];
    *y = jd->pos[j][1];
    *z = jd->pos[j][2];
    return 0;
}

int set_velocity(int index_of_the_particle,
		 double vx, double vy, double vz)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    jd->vel[j][0] = vx;
    jd->vel[j][1] = vy;
    jd->vel[j][2] = vz;
    return 0;
}

int get_velocity(int index_of_the_particle,
		 double * vx, double * vy, double * vz)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    *vx = jd->vel[j][0];
    *vy = jd->vel[j][1];
    *vz = jd->vel[j][2];
    return 0;
}

int set_acceleration(int index_of_the_particle,
		     double ax, double ay, double az)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    jd->acc[j][0] = ax;
    jd->acc[j][1] = ay;
    jd->acc[j][2] = az;
    return 0;
}

int get_acceleration(int index_of_the_particle,
		     double * ax, double * ay, double * az)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    *ax = jd->acc[j][0];
    *ay = jd->acc[j][1];
    *az = jd->acc[j][2];
    return 0;
}

int get_potential(int index_of_the_particle, double * pot)
{
    int j = jd->get_inverse_id(index_of_the_particle);
    if (j < 0) return -1;
    *pot = jd->pot[j];
    return 0;
}


// System-wide operations.

int evolve(double time)
{
    // On return, system_time will be greater than or equal to the
    // specified time.  All particles j will have time[j] <=
    // system_time < time[j] + timestep[j].  If synchronization is
    // needed, do it with synchronize_model().

    while (jd->system_time < time) jd->advance(*id, *s);
    return 0;
}

int synchronize_model()
{
    // Synchronize all particles at the current system time.  The
    // default is not to reinitialize the scheduler, as this will be
    // handled later, in recommit_particles().

    jd->synchronize_all(*id, s);
    return 0;
}

int get_time_step(double * time_step)
{
    *time_step = 0;			// not relevant here
    return -1;
}

int get_index_of_first_particle(int * index_of_the_particle)
{
    *index_of_the_particle = jd->id[0];
    return 0;
}

int get_indices_of_colliding_particles(int * index_of_particle1, 
				       int * index_of_particle2)
{
    *index_of_particle1 = jd->coll1;
    *index_of_particle2 = jd->coll2;
    return 0;
}

int get_number_of_particles(int * number_of_particles)
{
    *number_of_particles = jd->get_nj();
    return 0;
}

int get_total_mass(double * mass)
{
    real mtot = 0;
    for (int j = 0; j < jd->get_nj(); j++) mtot += jd->mass[j];
    *mass = mtot;
    return 0;
}

int get_potential_energy(double * potential_energy)
{
    *potential_energy = jd->get_pot(id);
    return 0;
}

int get_kinetic_energy(double * kinetic_energy)
{
    *kinetic_energy = jd->get_kin(id);	// NB should call get_kin after get_pot
    return 0;
}

int get_center_of_mass_position(double * x, double * y, double * z)
{
    real mtot = 0;
    vec cmx(0,0,0);
    for (int j = 0; j < jd->get_nj(); j++) {
	mtot += jd->mass[j];
	for (int k = 0; k < 3; k++) cmx[k] += jd->mass[j]*jd->pos[j][k];
    }
    *x = cmx[0]/mtot;
    *y = cmx[1]/mtot;
    *z = cmx[2]/mtot;
    return 0;
}

int get_center_of_mass_velocity(double * vx, double * vy, double * vz)
{
    real mtot = 0;
    vec cmv(0,0,0);
    for (int j = 0; j < jd->get_nj(); j++) {
	mtot += jd->mass[j];
	for (int k = 0; k < 3; k++) cmv[k] += jd->mass[j]*jd->vel[j][k];
    }
    *vx = cmv[0]/mtot;
    *vy = cmv[1]/mtot;
    *vz = cmv[2]/mtot;
    return 0;
}

int get_total_radius(double * radius)
{
    real r2max = 0;
    vec pos;
    get_center_of_mass_position(&pos[0], &pos[1], &pos[2]);
    for (int j = 0; j < jd->get_nj(); j++)  {
	real r2 = 0;
	for (int k = 0; k < 3; k++) r2 += pow(jd->pos[j][k]-pos[k], 2);
	if (r2 > r2max) r2max = r2;
    }
    *radius = sqrt(r2max);
    return 0;
}

// Optional (not implemented):

int get_potential_at_point(double eps,
			   double x, double y, double z, 
			   double * phi)
{
    return -1;
}

int get_gravity_at_point(double eps, double x, double y, double z, 
			 double * forcex, double * forcey, double * forcez)
{
    return -1;
}
