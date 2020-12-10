/* simple MPI program to exchange values between two processes
   this illustrates the use of send and receive

*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

int main(int argc, char *argv[]) {
   int myid, otherid, size;
   int length = 1, tag = 1;
   int myvalue, othervalue;
   MPI_Status status;
 
   /* initialize MPI and get own id (rank) */
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);
   MPI_Comm_size(MPI_COMM_WORLD, &size); 
   if (size!=2) {
     printf("use exactly two processes\n");
      MPI_Finalize(); 
      return 0;
   }

   if (myid == 0) {
      otherid = 1; myvalue = 10;
   } else {
      otherid = 0; myvalue = 20;
   }
   if (myid == 0){
     //printf("Aguardando mensagem de %d \n",otherid);fflush(stdout); 
     MPI_Recv(&othervalue, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
            MPI_COMM_WORLD, &status);  
     
     struct timespec spec1;
     clock_gettime(CLOCK_REALTIME, &spec1);
     printf("time: %ld, process %d received a %d\n", spec1.tv_nsec, myid, othervalue);fflush(stdout);
   }

    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    printf("time: %ld, process %d sending %d to process %d\n", spec.tv_nsec, myid, myvalue, otherid);fflush(stdout);
    MPI_Send(&myvalue, 1, MPI_INT, otherid, tag, MPI_COMM_WORLD);

   //printf("Aguardando mensagem de %d \n",otherid);
   if(myid == 1){
      MPI_Recv(&othervalue, 1, MPI_INT, MPI_ANY_SOURCE, 
                 MPI_ANY_TAG, MPI_COMM_WORLD, &status);  
   
     struct timespec spec1;
     clock_gettime(CLOCK_REALTIME, &spec1);
     printf("time: %ld, process %d received a %d\n", spec1.tv_nsec, myid, othervalue);fflush(stdout);
   }
  
   

   MPI_Finalize();
   return 0;
}