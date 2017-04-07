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

sem_t bridge, not_full, empty, done;
int bridge_cars = 0; //-3 <= bridge_cars <= 3: + means going N, - means going S.
double time_elapsed = 0.0;

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
    printf("Time %.1f: Vehicle %2d (%c) entering\n", time_elapsed, v->id, v->dir);

    if ((bridge_cars > 0 && v->dir == 'N') || (bridge_cars < 0 && v->dir == 'S'))
    {
        printf("Waiting on not_full\n");
        sem_wait(&not_full); //Cars going in same direction as me? Then go when not full.
        sem_wait(&bridge);   //Acquire bridge after not_full: otherwise deadlock.
        if (v->dir == 'N')
            bridge_cars++;   //Add or subtract bridge_cars depending on direction.
        else
            bridge_cars--;

        if (bridge_cars != 3 && bridge_cars != -3)
            sem_post(&not_full); //If bridge is now full, want to hold not_full until
                                 //it isn't. If it isn't full, return not_full.
    }
    else
    {
        printf("Waiting on empty\n");
        sem_wait(&empty);    //Otherwise, they are going the opposite direction.
        sem_wait(&bridge);
        if (v->dir == 'N')   //Wait until the bridge is empty to go.
            bridge_cars++;
        else
            bridge_cars--;
    }
    sem_post(&bridge);
}

void * CrossBridge(vehicle_info *v)
{
    printf("Time %.1f: Vehicle %2d (%c) crossing\n", time_elapsed, v->id, v->dir);
    assert(bridge_cars <= 3 && bridge_cars >= -3);
    Spin(5);
}

void * ExitBridge(vehicle_info *v)
{
    sem_wait(&bridge);
    if (v->dir == 'N')
        bridge_cars--; //Done crossing, so modify # cars on bridge.
    else
        bridge_cars++;

    if (bridge_cars == 2 || bridge_cars == -2)
        sem_post(&not_full); //If it was full, but now isn't, return not_full.
    if (bridge_cars == 0)
        sem_post(&empty);    //If it wasn't empty but now is, return empty.

    printf("Time %.1f: Vehicle %2d (%c) exited\n", time_elapsed, v->id, v->dir);
    int value;
    sem_getvalue(&not_full, &value);
    printf("not_full: %d\n", value);
    sem_post(&bridge);
}

void * CountTimer()
{
    while (sem_trywait(&done) != 0)
    {
        Spin(1);
        time_elapsed += 1.0;
    }
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
    pthread_t timer;
    sem_init(&bridge, 0, 1);
    sem_init(&not_full, 0, 1);
    sem_init(&empty, 0, 1);
    sem_init(&done, 0, 1);
    sem_wait(&done);

    int i;
    pthread_create(&timer, 0, CountTimer, 0);
    for (i = 0; i < 13; i++)
    {
        pthread_create(&threads[i], 0, VehicleAction, &cars[i]);
        usleep(1000000); //wait between vehicles
    }

    for (i = 0; i < 13; i++)
    {
        pthread_join(threads[i], 0);
    }
    sem_post(&done);
    pthread_join(timer, 0);

    sem_destroy(&bridge);
    sem_destroy(&not_full);
    sem_destroy(&empty);
    return 0;
}
