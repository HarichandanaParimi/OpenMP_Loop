#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif



int main (int argc, char* argv[]) {
	//forces openmp to create the threads beforehand
	#pragma omp parallel
	{
		int fd = open (argv[0], O_RDONLY);
		if (fd != -1) {
			close (fd);
		}
		else {
			std::cerr<<"something is amiss"<<std::endl;
		}
	}

	if (argc < 9) {
		std::cerr<<"Usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <scheduling> <granularity>"<<std::endl;
		return -1;
	}
	int function_id = atoi(argv[1]);
	float a = atof(argv[2]);
	float b = atof(argv[3]);
	int n = atoi(argv[4]);
	int intensity = atoi(argv[5]);
	int nbthreads = atoi(argv[6]);
	std::string scheduling = argv[7];
	int granularity = atoi(argv[8]);
  float temp = ((b-a)/n);
	float NIntegration = 0;
  omp_set_num_threads(nbthreads);
  // start clock
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  //set the  omp scheduling method
	if(scheduling.compare("static") == 0)
	{
		omp_set_schedule(omp_sched_static,granularity);
	}
	else if(scheduling.compare("dynamic") == 0)
	{
		omp_set_schedule(omp_sched_dynamic , granularity);
	}
	else if(scheduling.compare("guided") == 0)
	{
		omp_set_schedule(omp_sched_guided , granularity);
	}
	omp_lock_t mut;
	omp_init_lock(&mut);
#pragma omp parallel for schedule(runtime)
	for(int i = 0;i<n;i++)
	{
		float j = 0;
		j = a+((i+0.5)*temp);
		omp_set_lock(&mut);
		switch(function_id)
		{
			case 1 : {
					NIntegration += f1(j,intensity);
					break;
				}
			case 2 : {
					NIntegration += f2(j,intensity);
					break;
				}
			case 3 : {
					NIntegration += f3(j,intensity);
					break;
				}
			case 4 : {
					NIntegration += f4(j,intensity);
					break;
				}
			default:break;
		}
		omp_unset_lock(&mut);
	}

	NIntegration = NIntegration*temp;

	omp_destroy_lock(&mut);

	// end clock after computing result
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapased_seconds = end-start;

	// printing NIntegration result and time taken to calculate
	std::cout<<NIntegration;
	std::cerr<<elapased_seconds.count();
	return 0;
}
