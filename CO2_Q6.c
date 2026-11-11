
#include <stdio.h>

#define P 5
#define R 3

int allocation[P][R] =
{
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 2},
    {2, 1, 1},
    {0, 0, 2}
};

int maximum[P][R] =
{
    {7, 5, 3},
    {3, 2, 2},
    {9, 0, 2},
    {2, 2, 2},
    {4, 3, 3}
};

int available[R] = {3, 3, 2};
int need[P][R];

void calculateNeed(void)
{
    int i, j;

    for (i = 0; i < P; i++)
    {
        for (j = 0; j < R; j++)
        {
            need[i][j] =
                maximum[i][j] - allocation[i][j];
        }
    }
}

int safetyCheck(int sequence[])
{
    int work[R];
    int finish[P] = {0};
    int i, j;
    int count = 0;
    int found;

    for (j = 0; j < R; j++)
        work[j] = available[j];

    while (count < P)
    {
        found = 0;

        for (i = 0; i < P; i++)
        {
            if (finish[i] == 0)
            {
                int possible = 1;

                for (j = 0; j < R; j++)
                {
                    if (need[i][j] > work[j])
                    {
                        possible = 0;
                        break;
                    }
                }

                if (possible)
                {
                    for (j = 0; j < R; j++)
                    {
                        work[j] += allocation[i][j];
                    }

                    sequence[count] = i;
                    count++;
                    finish[i] = 1;
                    found = 1;
                }
            }
        }

        if (!found)
            return 0;
    }

    return 1;
}

void printSequence(int sequence[])
{
    int i;

    for (i = 0; i < P; i++)
    {
        printf("P%d", sequence[i]);

        if (i < P - 1)
            printf(" -> ");
    }

    printf("\n");
}

int requestResources(int process, int request[])
{
    int j;
    int sequence[P];

    /* Check whether request is within the remaining need */
    for (j = 0; j < R; j++)
    {
        if (request[j] > need[process][j])
            return 0;
    }

    /* Check whether resources are currently available */
    for (j = 0; j < R; j++)
    {
        if (request[j] > available[j])
            return -1;
    }

    /* Temporarily allocate requested resources */
    for (j = 0; j < R; j++)
    {
        available[j] -= request[j];
        allocation[process][j] += request[j];
        need[process][j] -= request[j];
    }

    /* Check whether the new state is safe */
    if (safetyCheck(sequence))
        return 1;

    /* Roll back if the state is unsafe */
    for (j = 0; j < R; j++)
    {
        available[j] += request[j];
        allocation[process][j] -= request[j];
        need[process][j] += request[j];
    }

    return 0;
}

int main(void)
{
    int sequence[P];
    int request1[R] = {1, 0, 2};
    int request2[R] = {3, 3, 0};
    int request3[R] = {0, 2, 0};
    int result;

    calculateNeed();

    printf("BANKER'S ALGORITHM\n");
    printf("------------------\n");

    printf("\nInitial Available Resources:\n");
    printf("%d %d %d\n",
           available[0], available[1], available[2]);

    if (safetyCheck(sequence))
    {
        printf("\nInitial System State: SAFE\n");
        printf("Safe Sequence: ");
        printSequence(sequence);
    }
    else
    {
        printf("\nInitial System State: UNSAFE\n");
        return 0;
    }

    printf("\nRequest 1: P1 requests (1, 0, 2)\n");

    result = requestResources(1, request1);

    if (result == 1)
    {
        printf("Request granted.\n");

        safetyCheck(sequence);
        printf("Safe Sequence: ");
        printSequence(sequence);
    }
    else
    {
        printf("Request denied or must wait.\n");
    }

    printf("\nRequest 2: P4 requests (3, 3, 0)\n");

    result = requestResources(4, request2);

    if (result == 1)
        printf("Request granted.\n");
    else if (result == -1)
        printf("Request must wait: insufficient available resources.\n");
    else
        printf("Request denied: unsafe state or invalid request.\n");

    printf("\nRequest 3: P0 requests (0, 2, 0)\n");

    result = requestResources(0, request3);

    if (result == 1)
        printf("Request granted.\n");
    else if (result == -1)
        printf("Request must wait: insufficient available resources.\n");
    else
        printf("Request denied: unsafe state.\n");

    return 0;
}

