#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

// Function to evaluate the curve (y = f(x))
float f(float x) {
    return x * x ; // Example: y = x^2
}

// Function to compute the area of a trapezoid
float trapezoid_area(float a, float b, float d) { 
    float area = 0;
    for (float x = a; x < b; x+=d) {
        area += f(x) + f(x+d);
    }
    
    return area * d / 2.0f;
}

int main(int argc, char** argv) {
    int rank, size;
    float a = 0.0f, b = 1.0f;  // Limits of integration
    int n;
    float start, end, local_area, total_area;
    
    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get number of processes

    if (rank == 0) {
        // Get the number of intervals from the user
        printf("Enter the number of intervals: ");
        scanf("%d", &n);
    }
    
    // Broadcast the number of intervals to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Calculate the interval size for each process
    float d = (b - a) / n; // delta
    float region = (b - a)/ size;
    
    // Calculate local bounds for each process
    start = a + rank * region;
    end = start + region;
    
    double time1=MPI_Wtime();
    
    // Each process calculates the area of its subinterval
    local_area = trapezoid_area(start, end, d);
    
    // Reduce all local areas to the total area on the root process
    MPI_Reduce(&local_area, &total_area, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    double time2=MPI_Wtime();
    double Time=time2-time1;


    
    if (rank == 0) {
        printf("The total area under the curve is: %f\n", total_area);
        printf("Time taken with %d processors: %f sec\n", size , Time);
    }
    
    MPI_Finalize(); // Finalize MPI
    return 0;
}
