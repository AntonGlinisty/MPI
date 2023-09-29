#include <mpi.h>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <fstream>

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);

	int num_segments = strtol(argv[1], NULL, 10);
	double segment_lenght =  1 / static_cast<double>(num_segments);

	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int num_processes = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

	int limit = num_segments / num_processes;

	auto integrand = [](double x) -> double {
		return 4 / (1 + x * x);
	};

	auto trapeze = [&integrand](double left_bound, double right_bound) -> double {
		return ((integrand(right_bound) + integrand(left_bound)) / 2) * (right_bound - left_bound);
	};

	double result = 0;
	double start_point = 0;

	if (rank == 0) {
		// consistent section
		double consistent_result = 0;

		auto consistent_start = MPI_Wtime();
		for (int i = 0; i < num_segments; ++i) {
			consistent_result += trapeze(segment_lenght * i, segment_lenght * (i + 1));
		}
		auto consistent_end = MPI_Wtime();

		// parallel section
		double proc_result = 0;
		double parallel_result = 0;

		auto parallel_start = MPI_Wtime();
		for (int i = 0; i < num_processes - 1; ++i) {
			start_point = limit / static_cast<double>(num_segments) * (i + 1);
			MPI_Send(&start_point, 1, MPI_DOUBLE, i + 1, 0, MPI_COMM_WORLD);
		}

		for (int i = 0; i < limit; ++i) {
			result += trapeze(segment_lenght * i, segment_lenght * (i + 1));
		}

		parallel_result += result;

		for (int i = 0; i < num_processes - 1; ++i) {
			MPI_Recv(&proc_result, 1, MPI_DOUBLE, i + 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			parallel_result += proc_result;
		}
		auto parallel_end = MPI_Wtime();

		std::cout << "process rank = " << rank << " process result = " << result << std::endl;
		std::cout << "accumulative consistent result = " << std::setprecision(10) << consistent_result << std::endl;
		std::cout << "accumulative parallel result = " << std::setprecision(10) << parallel_result << std::endl;

		std::ofstream file;
		file.open("time.txt", std::ios::app);
		file << consistent_end - consistent_start + parallel_end - parallel_start << std::endl;
	} else {
		MPI_Recv(&start_point, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		for (int i = 0; i < limit; ++i) {
			result += trapeze(start_point + segment_lenght * i, start_point + segment_lenght * (i + 1));
		}

		if (rank == num_processes - 1) {
		  int iter_count = num_segments - num_processes * (num_segments / num_processes);
		  for (int i = 0; i < iter_count; ++i) {
		    result += trapeze(segment_lenght * (num_segments - iter_count + i),
		        segment_lenght * (num_segments - iter_count + i + 1));
		  }
		}

		MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		std::cout << "process rank = " << rank  << " process result = " << result << std::endl;
	}

	MPI_Finalize();
	return 0;
}
