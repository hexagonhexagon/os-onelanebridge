#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "spin.h"

typedef struct _vehicle_info
{
    int id;
    int dir;
    double inter_arrival_t;
} vehicle_info;

sem_t bridge, not_full, lock_n, lock_s;
int bridge_cars = 0;  //number of cars on bridge
double time_start;

void * VehicleAction(void *);
void * ArriveBridge(vehicle_info *);
void * CrossBridge(vehicle_info *);
void * ExitBridge(vehicle_info *);

void * VehicleAction(void *arg)
{
    vehicle_info *ti = (vehicle_info *) arg;
    ArriveBridge(ti);
    CrossBridge(ti);
    ExitBridge(ti);
}

void * ArriveBridge(vehicle_info *v)
{
    sem_wait(&not_full); //After get lock, wait until not full.
    sem_wait(&bridge);   //Acquire bridge after not_full: otherwise deadlock.
    bridge_cars++;
    if (bridge_cars != 3)
        sem_post(&not_full); //If bridge is now full, want to hold not_full until
                             //it isn't. If it isn't full, return not_full.
    sem_post(&bridge);
}

void * CrossBridge(vehicle_info *v)
{
    printf("Time %.1f: Vehicle %2d (%c) crossing\n", GetTime() - time_start, v->id, v->dir);
    Spin(5);
}

void * ExitBridge(vehicle_info *v)
{
    sem_wait(&bridge);
    bridge_cars--; //Done crossing, so modify # cars on bridge.

    if (bridge_cars == 2)
        sem_post(&not_full); //If it was full, but now isn't, return not_full.
    if (bridge_cars == 0)
    {
        if (v->dir == 'N')
            sem_post(&lock_s); //If bridge is now empty, allow other direction to go.
        else
            sem_post(&lock_n);
    }

    sem_post(&bridge);
}

int main()
{
    vehicle_info cars[13] = {
        {0, 'N', 0.0},
        {1, 'N', 1.0},
        {2, 'S', 2.0},
        {3, 'S', 3.0},
        {4, 'S', 4.0},
        {5, 'N', 5.0},
        {6, 'N', 6.0},
        {7, 'S', 7.0},
        {8, 'S', 8.0},
        {9, 'N', 9.0},
        {10, 'N', 10.0},
        {11, 'N', 11.0},
        {12, 'N', 12.0}
    };
    pthread_t threads[13];
    sem_init(&bridge, 0, 1);
    sem_init(&not_full, 0, 1);
    sem_init(&lock_n, 0, 1);
    sem_init(&lock_s, 0, 1);

    int i;
    time_start = GetTime();
    for (i = 0; i < 13; i++)
    {
        pthread_create(&threads[i], 0, VehicleAction, &cars[i]);
        usleep(1000000); //wait between vehicles
    }

    for (i = 0; i < 13; i++)
    {
        pthread_join(threads[i], 0);
    }

    sem_destroy(&bridge);
    sem_destroy(&not_full);
    sem_destroy(&lock_n);
    sem_destroy(&lock_s);
    return 0;
}
