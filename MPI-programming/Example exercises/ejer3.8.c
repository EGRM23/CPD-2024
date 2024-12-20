#include <stdio.h>
#include <mpi.h>

void Parallel_vector_sum(double local_x[], double local_y[], double local_z[], int local_n) {
    for (int local_i = 0; local_i < local_n; local_i++) {
        local_z[local_i] = local_x[local_i] + local_y[local_i];
    }
}

int main(void) {
    int my_rank, comm_sz;
    int n = 8;
    int local_n;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if (n % comm_sz != 0) {
        if (my_rank == 0) {
            printf("El tamaño del vector (n = %d) debe ser divisible por el número de procesos.\n", n);
        }
        MPI_Finalize();
        return -1;
    }

    local_n = n / comm_sz;
    double local_x[local_n], local_y[local_n], local_z[local_n];

    double x[n], y[n];
    if (my_rank == 0) {
        for (int i = 0; i < n; i++) {
            x[i] = i * 1.0;
            y[i] = i * 2.0;
        }
    }

    MPI_Scatter(x, local_n, MPI_DOUBLE, local_x, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(y, local_n, MPI_DOUBLE, local_y, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    Parallel_vector_sum(local_x, local_y, local_z, local_n);

    double z[n];
    MPI_Gather(local_z, local_n, MPI_DOUBLE, z, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("Vector resultante (suma de x y y):\n");
        for (int i = 0; i < n; i++) {
            printf("z[%d] = %.1f\n", i, z[i]);
        }
    }

    MPI_Finalize();
    return 0;
}
