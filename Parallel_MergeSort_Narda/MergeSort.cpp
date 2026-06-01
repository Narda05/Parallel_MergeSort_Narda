/**
 * Challenge: Sort an array of random integers with merge sort
 */
#include <thread>
#include <random>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <memory>

//Threads and parallelism
const unsigned int L = 100000;


using namespace std;

/* declaration of merge helper function */
void merge_arrays(int* array, unsigned int left, unsigned int mid, unsigned int right);


/* sequential implementation of merge sort */
void sequential_merge_sort(int* array, unsigned int left, unsigned int right)
{
	if (left < right)
	{
		// if array is smaller that L use sort directly
		if (right - left < L)
		{
			std::sort(array + left, array + right + 1);
			return;
		}

		unsigned int mid = (left + right) / 2; // find the middle point
		sequential_merge_sort(array, left, mid); // sort the left half
		sequential_merge_sort(array, mid + 1, right); // sort the right half
		merge_arrays(array, left, mid, right); // merge the two sorted halves
	}
}

/* parallel implementation of merge sort */
void parallel_merge_sort(int* array, unsigned int left, unsigned int right)
{
	if (left < right)
	{
		//is array is smaller than L use sort directly
		if (right - left < L)
		{
			std::sort(array + left, array + right + 1);
			return;
		}

		unsigned int mind = (left + right) / 2; // find the middle point

		// create a thread for the left and right half 
		std::thread leftThread(parallel_merge_sort, array, left, mind);
		parallel_merge_sort(array, mind + 1, right);
		leftThread.join();

		// merge the two sorted halves
		merge_arrays(array, left, mind, right);
	}
}


//Sort hemper 
void swap(int& a, int& b)
{
	int t = a;
	a = b;
	b = t;
}

// Move the pivot element to its correct position smalles number to left and bigger number to right
int partition(int* array, unsigned int left, unsigned int right)
{
	int pivot = array[right];
	int i = left - 1;
	for (int j = left; j < right; j++)
	{
		if (array[j] < pivot)
		{
			i++;
			swap(array[i], array[j]);
		}
	}
	swap(array[i + 1], array[right]);
	return i + 1;
}


// If the smaller number L use sort and is bigger number divede by two 
void Parallel_quick_sort(int* array, unsigned int left, unsigned int right)
{
	if (left < right)
	{
		if (right - left < L)
		{
			std::sort(array + left, array + right + 1);
			return;
		}

		unsigned int pivot = partition(array, left, right);

		std::thread leftThread(Parallel_quick_sort, array, left, pivot - 1);
		Parallel_quick_sort(array, pivot + 1, right);
		leftThread.join();
	}
}

/* helper function to merge two sorted subarrays
   array[l..m] and array[m+1..r] into array */
void merge_arrays(int* arr, unsigned int left, unsigned int mid, unsigned int right)
{
	unsigned int num_left = mid - left + 1; // number of elements in left subarray
	unsigned int num_right = right - mid; // number of elements in right subarray

	// copy data into temporary left and right subarrays to be merged
	vector<int> array_left(num_left);
	vector<int> array_right(num_right);

	std::copy(&arr[left], &arr[mid + 1], array_left.begin());
	std::copy(&arr[mid + 1], &arr[right + 1], array_right.begin());

	// initialize indices for array_left, array_right, and input subarrays
	unsigned int index_left = 0;    // index to get elements from array_left
	unsigned int index_right = 0;    // index to get elements from array_right
	unsigned int index_insert = left; // index to insert elements into input array

	// merge temporary subarrays into original input array
	while ((index_left < num_left) || (index_right < num_right))
	{
		if ((index_left < num_left) && (index_right < num_right))
		{
			if (array_left[index_left] <= array_right[index_right])
			{
				arr[index_insert] = array_left[index_left];
				index_left++;
			}
			else
			{
				arr[index_insert] = array_right[index_right];
				index_right++;
			}
		}
		// copy any remain elements of array_left into array
		else if (index_left < num_left)
		{
			arr[index_insert] = array_left[index_left];
			index_left += 1;
		}
		// copy any remain elements of array_right into array
		else if (index_right < num_right)
		{
			arr[index_insert] = array_right[index_right];
			index_right += 1;
		}
		++index_insert;
	}
}

int main()
{
	mt19937 generator((unsigned int)chrono::system_clock::now().time_since_epoch().count());

	const int NUM_EVAL_RUNS = 2;
	const int N = 10000000; // number of elements to sort

	std::vector<int> original_array, sequential_result, parallel_result;
	original_array.reserve(N);
	sequential_result.reserve(N);
	parallel_result.reserve(N);

	for (int i = 0; i < N; i++)
	{
		original_array.push_back(generator() % 2000000 - 1000000);
	}

	cout << "Evaluating Sequential Implementation...\n";
	std::chrono::duration<double> sequential_time(0);
	sequential_result = original_array;
	//std::copy(&original_array[0], &original_array[N-1], sequential_result);
	sequential_merge_sort(&sequential_result[0], 0, N - 1); // "warm up"	
	for (int i = 0; i < NUM_EVAL_RUNS; i++)
	{
		sequential_result = original_array;
		//	std::copy(&original_array[0], &original_array[N-1], sequential_result); // reset result array
		auto start_time = std::chrono::high_resolution_clock::now();
		sequential_merge_sort(&sequential_result[0], 0, N - 1);
		sequential_time += std::chrono::high_resolution_clock::now() - start_time;
	}
	sequential_time /= NUM_EVAL_RUNS;

	printf("Evaluating Parallel Implementation...\n");
	std::chrono::duration<double> parallel_time(0);

	parallel_result = original_array;
	//	std::copy(&original_array[0], &original_array[N-1], parallel_result);
	parallel_merge_sort(&parallel_result[0], 0, N - 1); // "warm up"
	for (int i = 0; i < NUM_EVAL_RUNS; i++)
	{
		parallel_result = original_array;
		//	std::copy(&original_array[0], &original_array[N - 1], parallel_result); // reset result array
		auto start_time = std::chrono::high_resolution_clock::now();
		parallel_merge_sort(&parallel_result[0], 0, N - 1);
		parallel_time += std::chrono::high_resolution_clock::now() - start_time;
	}
	parallel_time /= NUM_EVAL_RUNS;


	//QUICK SORT EVALUATION
	cout << "Evaluating Parallel QuickSort Implementation...\n";
	std::chrono::duration<double> quicksort_time(0);
	vector<	int> quicksort_result;
	quicksort_result.reserve(N);

	quicksort_result = original_array;
	Parallel_quick_sort(&quicksort_result[0], 0, N - 1); // "warm up"
	for(int i = 0; i < NUM_EVAL_RUNS; i++)
	{
		quicksort_result = original_array;
		auto start_time = std::chrono::high_resolution_clock::now();
		Parallel_quick_sort(&quicksort_result[0], 0, N - 1);
		quicksort_time += std::chrono::high_resolution_clock::now() - start_time;
	}
	quicksort_time /= NUM_EVAL_RUNS;


	// verify sequential and parallel results are same
	for (int i = 0; i < 10; i++)
	{
		if (sequential_result[i] != parallel_result[i])
		{
			cout << "ERROR: Result mismatch at index " << i << endl;
		}
	}
	cout << "Average Sequential Time: " << sequential_time.count() * 1000 << " ms \n";
	cout << "  Average Parallel Time: " << parallel_time.count() * 1000 << " ms \n";
	cout << "Avarege Parallel QuickSort Time: " << quicksort_time.count() * 1000 << " ms \n";

	cout << "Speedup: " << sequential_time / parallel_time << " ms \n";
	cout << "Efficiency " << 100 * (sequential_time / parallel_time) / std::thread::hardware_concurrency() << " ms \n";
	cout << "L value used: " << L << "\n";

	return 0;
}