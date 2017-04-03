#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "spin.h"

typedef struct _vehicle_info
{
    int id;
    int dir;
    double inter_arrival_t;
} vehicle_info;

void * ArriveBridge(vehicle_info *);
void * CrossBridge(vehicle_info *);
void * ExitBridge(vehicle_info *);

void * VehicleAction(void *arg)
{
    vehicle_info *ti = (vehicle_info *) arg;
//    ArriveBridge(ti);
    CrossBridge(ti);
//    ExitBridge(ti);
}

void * ArriveBridge(vehicle_info *v)
{
    ;
}

void * CrossBridge(vehicle_info *v)
{
    printf("Time %.1f: Vehicle %2d (%c) crossing\n", v->inter_arrival_t, v->id, v->dir);
    Spin(5);
}

void * ExitBridge(vehicle_info *v)
{
    ;
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
    double time_elapsed = 0.0;

    int i;
    double time_to_wait = 0.0;
    for (i = 0; i < 13; i++)
    {
        time_to_wait = cars[i].inter_arrival_t - time_elapsed;
        Spin(time_to_wait); //wait between vehicles
        time_elapsed += time_to_wait;
        pthread_create(&threads[i], 0, VehicleAction, &cars[i]);
    }

    for (i = 0; i < 13; i++)
    {
        pthread_join(threads[i], 0);
    }

    return 0;
}
