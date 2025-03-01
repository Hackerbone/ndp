#include <stdio.h>

#define TOKEN_RATE 10       // KB per second (tokens replenished per second)
#define BUCKET_SIZE 50      // Maximum token bucket size (KB)
#define PACKET_SIZE 15      // Each packet size (KB)
#define PACKET_INTERVAL 0.5 // Packets arrive every 0.5 seconds
#define SIMULATION_TIME 10  // Run simulation for 10 seconds

void simulate_token_bucket()
{

    double tokens = BUCKET_SIZE; // Initially, bucket starts full (50 KB)

    double next_packet_time = 0.5; // First packet arrives at t = 0.5s

    double time;

    double ts;

    int queued_packets = 0; // Track number of queued packets

    printf("Time\tTokens Available\tAction\n");

    printf("-------------------------------------------\n");

    for (time = 0.5; time <= SIMULATION_TIME; time += 0.5)
    {

        // Refill tokens every 1 second (+10 KB)

        if (time - (int)time == 0)
        {

            tokens += TOKEN_RATE;

            if (tokens > BUCKET_SIZE)
            {

                tokens = BUCKET_SIZE; // Don't exceed max bucket size
            }
        }

        if (time == 1.5)
        {
            ts = tokens;
        }
        // If a packet arrives at this time

        if (time == next_packet_time)
        {

            printf("%.1f\t%.1f KB\t\t", time, tokens);

            if (tokens >= PACKET_SIZE)
            {

                tokens -= PACKET_SIZE; // Deduct tokens as packet is sent

                printf("Packet sent (15 KB)\n");
            }
            else
            {

                queued_packets++;

                printf("Packet queued (Insufficient tokens)\n");
            }

            next_packet_time += PACKET_INTERVAL; // Schedule next packet
        }
        else
        {

            printf("%.1f\t%.1f KB\n", time, tokens);
        }

        // Stop simulation if packets start queuing

        if (queued_packets > 0)
        {

            printf("\nPackets started queuing at t = %.1f seconds\n", time);

            break;
        }
    }

    // Answer Question (i): Tokens left after 1.5s

    printf("\n(i) Tokens left in the bucket after 1.5s: %.1f KB\n", ts);

    // Answer Question (ii): When packets start to queue

    printf("(ii) Packets started queuing at %.1f seconds\n", time);

    // Answer Question (iii): Max burst size with rate limit of 20 KBps

    int max_burst_packets = BUCKET_SIZE / PACKET_SIZE; // Max packets that fit in bucket

    printf("(iii) With a max rate of 20 KBps, the max burst size = %d packets (%.1f KB)\n",

           max_burst_packets, (double)(max_burst_packets * PACKET_SIZE));
}

int main()
{

    simulate_token_bucket();

    return 0;
}