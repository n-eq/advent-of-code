#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef TEST
#define INPUT "input_test"
#define NB_RACES 3
#else
#define INPUT "input"
#define NB_RACES 4
#endif

typedef struct race_s {
    // longs are required for part 2
    long t; // max time allow for the race
    long d; // record distance
} race_t;

int main() {
   FILE *fp = fopen(INPUT, "r");
   if (!fp) {
      perror("File opening failed");
      return 1;
   }

   char line[250];

   race_t races[NB_RACES] = {0};

   int i = 0;
   while (fgets(line, sizeof(line), fp)) {
       if (line[0] == '\n') {
           continue;
       }
       line[strcspn(line, "\n")] = 0;

       char* p = strchr(line, ':') + 1;

       int j = 0;
       p = strtok (p, " ");
       while (p != NULL) {
           if (i == 0) {
               races[j].t = atol(p);
           } else {
               races[j].d = atol(p);
           }
           p = strtok(NULL, " ");
           j++;
       }

       i++;
   }

#ifdef PART2
   // For part 2, we'll redefine races array and limit it to only once race
   // This is a too long and ugly way to do it but sometimes it's ok.
   long new_time = 0;
   long new_distance = 0;
   int shift_time = 1;
   int shift_distance = 1;
   for (int i = 0; i < NB_RACES; ++i) {
       race_t r = races[NB_RACES - i - 1];
       long t = r.t;
       long d = r.d;

       long dshift_base, tshift_base;
       if (i == 0) {
           new_time = t;
           new_distance = d;
       } else {
           new_time += pow(10, shift_time) * t;
           new_distance += pow(10, shift_distance) * d;

       }

       shift_time = 1;
       shift_distance = 1;
       tshift_base = new_time;
       dshift_base = new_distance;

       while (tshift_base > 9) {
           tshift_base /= 10;
           shift_time++;
       }

       while (dshift_base > 9) {
           dshift_base /= 10;
           shift_distance++;
       }
   }
   races[0].d = new_distance;
   races[0].t = new_time;
#undef NB_RACES
#define NB_RACES 1
#endif

   // For each race we need to count the integer solutions of the quadractic inequality:
   // x^2 - tx + d < 0; where t is the race's time and d the record distance.
   // That is, the number of integers between x1 and x2 (the solution of the equation)
   // The input seems to be given to make sure only the range [x1, x2] is to be taken
   // into account, otherwise, there would be an infinity of possibilities!
   long res = 0;
   for (int i = 0; i < NB_RACES; ++i) {
       long t = races[i].t;
       long d = races[i].d;

       double x1 = (t - sqrt(pow(t, 2) - 4 * d)) / 2;
       double x2 = (t + sqrt(pow(t, 2) - 4 * d)) / 2;
       // printf("x1: %f x2: %f\n", x1, x2);

       long sol_count = ((long) x2 - (long) x1);
       res = (res == 0) ? sol_count : (res * sol_count);
   }

#ifndef PART2
   printf("part1 = %ld\n", res);
#else
   printf("part2 = %ld\n", res);
#endif

   // Here's how to find the inequality
   // we know that distance = speed(s) * time(t)
   // speed = hold time * 1m/s (we can skip the 1m/s)
   // so: distance = hold_time * remaining_time
   // distance = hold_time * (total_time - hold_time)
   // if we take: hold_time = x, distance = d, hold_time = t,; then:
   // d = x * (t - x)
   // we want our distance to surpass the record (d), so
   // xt - x^2 > d; or x^2 + d - tx < 0
   // if t^2 >= 4d; then:
   // x1 = (t - sqrt(t^2 - 4d)) / 2
   // x2 = (t + sqrt(t^2 - 4d)) / 2
}
