#include <stdio.h>
#include <mpi.h>

void Get_input(int my_rank, int comm_sz, double *a_p, double *b_p, int *n_p) {
    if (my_rank == 0) {
        printf("Enter a, b, and n\n");
        scanf("%lf %lf %d", a_p, b_p, n_p);
    }
    // Broadcast the values of a, b, and n to all processes
    MPI_Bcast(a_p, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(b_p, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(n_p, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

int main(void) {
    int my_rank, comm_sz;
    double a, b, h;
    int n, local_n;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // Call the Get_input function to get values for a, b, and n
    Get_input(my_rank, comm_sz, &a, &b, &n);

    // Now all processes have the values of a, b, and n
    h = (b - a) / n;
    local_n = n / comm_sz;

    if (my_rank == 0) {
        printf("Values received: a = %f, b = %f, n = %d\n", a, b, n);
    }

    // The rest of your MPI program would go here...

    MPI_Finalize();
    return 0;
}
