#include <stdio.h>

#define MAX_BUCKET_SIZE 10 // Maximum size of the leaky bucket in bytes
#define OUTGOING_RATE 1    // Outgoing rate in bytes per second
#define PACKET_SIZE 4      // Size of each packet in bytes

// Function to simulate the leaky bucket algorithm
void leakyBucket(int arrivalTimes[], int numPackets)
{
    int bucket = 0;   // Current bucket size (in bytes)
    int lastTime = 0; // Last time when packet was transmitted
    int conformingCount = 0, nonConformingCount = 0;

    printf("Time\tAction\t\tBucket Size\tConforming/Nonconforming\n");
    printf("---------------------------------------------------------------\n");

    for (int i = 0; i < numPackets; i++)
    {
        int arrivalTime = arrivalTimes[i];

        // Update the bucket by simulating the outgoing process
        int timePassed = arrivalTime - lastTime;

        // Subtract outgoing packets (1 byte per second)
        bucket -= timePassed * OUTGOING_RATE;
        if (bucket < 0)
        {
            bucket = 0;
        }

        // Try to add the incoming packet
        if (bucket + PACKET_SIZE <= MAX_BUCKET_SIZE)
        {
            bucket += PACKET_SIZE;
            printf("%-8d%-10s%-14d%-25s\n", arrivalTime, "Added", bucket, "Conforming");
            conformingCount++;
        }
        else
        {
            printf("%-8d%-10s%-14d%-25s\n", arrivalTime, "Dropped", bucket, "Nonconforming");
            nonConformingCount++;
        }

        lastTime = arrivalTime; // Update last time
    }

    printf("\n---------------------------------------------------------------\n");
    printf("Total Conforming Packets: %d\n", conformingCount);
    printf("Total Nonconforming Packets: %d\n", nonConformingCount);
}

int main()
{
    // Array containing arrival times of packets (in seconds)
    int arrivalTimes[] = {1, 2, 3, 5, 6, 8, 11, 12, 15, 16, 19};
    int numPackets = sizeof(arrivalTimes) / sizeof(arrivalTimes[0]);

    // Call leakyBucket function to simulate the process
    leakyBucket(arrivalTimes, numPackets);

    return 0;
}
