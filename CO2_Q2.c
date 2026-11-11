
#include <stdio.h>

#define N 4
#define QUANTUM 3
#define MAX_QUEUE 100

int main(void)
{
    int arrival[N] = {0, 1, 2, 3};
    int burst[N] = {8, 4, 9, 5};
    int remaining[N] = {8, 4, 9, 5};

    int completion[N] = {0};
    int turnaround[N] = {0};
    int waiting[N] = {0};

    int queue[MAX_QUEUE];
    int head = 0;
    int tail = 0;
    int count = 0;

    int finished = 0;
    int time = 0;
    int i;
    int current;
    int execution;

    float totalTAT = 0;
    float totalWT = 0;

    printf("ROUND ROBIN CPU SCHEDULING\n");
    printf("--------------------------\n");
    printf("Time Quantum = %d ms\n\n", QUANTUM);

    /* Add processes that arrive at time zero */
    for (i = 0; i < N; i++)
    {
        if (arrival[i] == 0)
        {
            queue[tail] = i;
            tail = (tail + 1) % MAX_QUEUE;
            count++;
        }
    }

    while (finished < N)
    {
        /* If ready queue is empty, advance time */
        if (count == 0)
        {
            time++;

            for (i = 0; i < N; i++)
            {
                if (arrival[i] == time && remaining[i] > 0)
                {
                    queue[tail] = i;
                    tail = (tail + 1) % MAX_QUEUE;
                    count++;
                }
            }

            continue;
        }

        /* Remove the next process from the queue */
        current = queue[head];
        head = (head + 1) % MAX_QUEUE;
        count--;

        /* Execute process for one time quantum */
        if (remaining[current] > QUANTUM)
            execution = QUANTUM;
        else
            execution = remaining[current];

        printf("P%d executes from %d to %d\n",
               current + 1, time, time + execution);

        time = time + execution;
        remaining[current] = remaining[current] - execution;

        /* Add newly arrived processes to ready queue */
        for (i = 0; i < N; i++)
        {
            if (arrival[i] > 0 &&
                arrival[i] <= time &&
                remaining[i] == burst[i])
            {
                queue[tail] = i;
                tail = (tail + 1) % MAX_QUEUE;
                count++;

                /* Mark as already added */
                remaining[i] = remaining[i] + 1000;
            }
        }

        /* Restore the remaining burst time of newly arrived jobs */
        for (i = 0; i < N; i++)
        {
            if (remaining[i] > 1000)
                remaining[i] = remaining[i] - 1000;
        }

        /* Re-add the current process if it is not finished */
        if (remaining[current] > 0)
        {
            queue[tail] = current;
            tail = (tail + 1) % MAX_QUEUE;
            count++;
        }
        else
        {
            completion[current] = time;
            finished++;
        }
    }

    printf("\nPROCESS SCHEDULING TABLE\n");
    printf("Process\tAT\tBT\tCT\tTAT\tWT\n");

    for (i = 0; i < N; i++)
    {
        turnaround[i] = completion[i] - arrival[i];
        waiting[i] = turnaround[i] - burst[i];

        totalTAT = totalTAT + turnaround[i];
        totalWT = totalWT + waiting[i];

        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               i + 1,
               arrival[i],
               burst[i],
               completion[i],
               turnaround[i],
               waiting[i]);
    }

    printf("\nAverage Turnaround Time = %.2f ms\n",
           totalTAT / N);

    printf("Average Waiting Time = %.2f ms\n",
           totalWT / N);

    return 0;
}

