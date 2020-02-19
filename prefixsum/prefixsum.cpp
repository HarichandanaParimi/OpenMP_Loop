#include <chrono>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <omp.h>

#ifdef __cplusplus
extern "C" {
#endif

  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);


#ifdef __cplusplus
}
#endif

int main (int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr<<"Usage: "<<argv[0]<<" <n>"<<std::endl;
		return -1;
	}

	int n = atoi(argv[1]);
  int * arr = new int [n];
	int nbthreads = atoi(argv[2]);
	omp_set_num_threads(nbthreads);
	int* pr = new int [n+1];
	generatePrefixSumData (arr, n);
  //start the system_clock
 std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
 pr[0] = arr[0];
	int *partial_prefix_sum;
	#pragma omp parallel
	{
		int threadid = omp_get_thread_num();

		#pragma omp single
		{
			partial_prefix_sum = new int[nbthreads+1];
			partial_prefix_sum[0] = 0;
		}
		int prefix_sum = 0;
		#pragma omp for schedule(auto) nowait
		for(int i=0; i<n; i++) {
			prefix_sum += arr[i];
			pr[i+1] = prefix_sum;
		}
		partial_prefix_sum[threadid+1] = prefix_sum;

		#pragma omp barrier
		int partial_sum = 0;
		for(int i=0; i<(threadid+1); i++) {
			partial_sum+= partial_prefix_sum[i];
		}

		#pragma omp for schedule(auto)
		for(int i=0; i<n; i++) {
			pr[i+1] += partial_sum;
		}
	}
//end the system_clock
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end-start;

	std::cerr<<elapsed_seconds.count()<<std::endl;

	checkPrefixSumResult(pr, n);

	delete[] arr;
	return 0;
}
