#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>


void * doctor_function(void *idp);
void * patient_function(void *idp);
void service_patient();
void * make_patient_function();

/* Mutex*/
pthread_mutex_t srvCust;

/* Semaphores */
sem_t doctor_ready;
sem_t patient_ready;
sem_t modifySeats;

/* Inputs */
int chair_cnt;
int total_custs;

int available_seats;
int no_served_custs = 0;
time_t waiting_time_sum;

void * doctor_function(void *idp)
{
    int counter = 0;

    while(1)
    {

        sem_wait(&patient_ready);


        sem_wait(&modifySeats);


        available_seats++;


        sem_post(&modifySeats);


        sem_post(&doctor_ready);


        pthread_mutex_lock(&srvCust);


        service_patient();


        pthread_mutex_unlock(&srvCust);

        printf("Patient was Treated...\n");
        counter++;
        if(counter == (total_custs - no_served_custs))
            break;

    }
    pthread_exit(NULL);
}

void * patient_function(void *idp)
{
    struct timeval start, stop;


    sem_wait(&modifySeats);

    if(available_seats >= 1)
    {
        available_seats--;

        printf("Patient[pid = %lu] is waiting.\n", pthread_self());
        printf("Available seats: %d\n", available_seats);

        gettimeofday(&start, NULL);
        sem_post(&patient_ready);

        sem_post(&modifySeats);

        sem_wait(&doctor_ready);

        gettimeofday(&stop, NULL);

        double sec = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

        waiting_time_sum += 1000 * sec;
        printf("Patient[pid = %lu] is being treated. \n", pthread_self());
    }
    else
    {
        sem_post(&modifySeats);
        no_served_custs++;
        printf("A Patient left due to unavailability of chairs...\n");
    }

    pthread_exit(NULL);
}

void service_patient(){

    int s = rand() % 401; /* Random number between 0 and 400 (miliseconds)*/

    s = s * 1000; /* Convert miliseconds to microseconds*/
    usleep(s);
}

void * make_patient_function(void *){
    int tmp;
    int counter = 0;

    while(counter < total_custs)
    {

        pthread_t customer_thread;
        tmp = pthread_create(&customer_thread, NULL, patient_function, NULL);
        if(tmp)
            printf("Failed to create thread.");

        counter++;


        usleep(10000);
    }
}
void header()
{
    printf("CS558 : COMPUTER SYSTEM LAB ASSIGNMENT - 04\n");
    printf("Instructor : Dr. Manas Khatua and Dr. Moumita Patra\n");
    printf("TA's : Alakesh Kalita, Arunav Saikia, Meenu Rani Dey, Priya Undriwade, Vinay Kumar Gupta\n");
    printf("Sumbittted By : Himanshu Patel(204101029), Ketan Karnakota(204101030)\n");
}
int main(){

    header();
    printf("----------------------------------------------------------------------------------------\n");
    srand(time(NULL));


    pthread_t doctor_1;


    pthread_t patient_maker;

    int tmp;

    pthread_mutex_init(&srvCust, NULL);

    /* Initialize semaphores */
    sem_init(&patient_ready, 0, 0);
    sem_init(&doctor_ready, 0, 0);
    sem_init(&modifySeats, 0, 1);

    printf("Please enter the number of seats: \n");
    scanf("%d", &chair_cnt);

    printf("Please enter the total patients: \n");
    scanf("%d", &total_custs);

    available_seats = chair_cnt;


    tmp = pthread_create(&doctor_1, NULL, doctor_function, NULL);
    if(tmp)
        printf("Failed to create thread.");

    tmp = pthread_create(&patient_maker, NULL, make_patient_function, NULL);
    if(tmp)
        printf("Failed to create thread.");

    pthread_join(doctor_1, NULL);
    pthread_join(patient_maker, NULL);

    printf("\n------------------------------------------------\n");
    printf("Number of patients that were forced to leave: %d\n", no_served_custs);
    return 0;
}