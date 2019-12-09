#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void  ClientProcess(int [], int[]);

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;
  
     int fd[2];
     pipe(fd);
  
     int numbers[10];

     if (argc != 11) {
          printf("Use: %s #1 #2 #3 #4 #5 #6 #7 #8 #9 #10\n", argv[0]);
          exit(1);
     }

     ShmID = shmget(IPC_PRIVATE, 10*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     printf("Server has received a shared memory of ten integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     printf("Server has attached the shared memory...\n");

     //Read the input numbers
     for(int i = 0; i < 10; i++){
       ShmPTR[i] = atoi(argv[i+1]);
     }
     printf("Server has filled all ten numbers in shared memory...\n");

     printf("Server is about to fork a child process...\n");
     pid = fork();
     if (pid < 0) {
          printf("*** fork error (server) ***\n");
          exit(1);
     }
     else if (pid == 0) {
          ClientProcess(ShmPTR, fd);
          exit(0);
     }

     wait(&status);
     printf("Server has detected the completion of its child...\n");
  
     //Read numbers from pip and print them
     close(fd[1]);
     printf("Reprinting from the server ");
     for(int i = 0; i<10; i++){
       read(fd[0], numbers, sizeof(int));
       printf("%d ", *numbers); 
     }
     printf("\n");
     close(fd[0]);
  
     shmdt((void *) ShmPTR);
     printf("Server has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Server has removed its shared memory...\n");
     printf("Server exits...\n");
     exit(0);
}

void  ClientProcess(int  SharedMem[], int fd[])
{
     printf("   Client process started\n");
     printf("Client found ");
     for (int j=0; j<10; j++){
       printf("%d ", SharedMem[j]);
     }
     printf("\n");
     //close read end and write to fd.
     close(fd[0]);
  
     for(int j = 0; j < 10; j++){
        write(fd[1], &SharedMem[j], sizeof(int));
     }
     printf("   Client is done writing to the pipe\n");
     printf("   Client is about to exit\n");
}