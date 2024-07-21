// For compilation you may need -lpthread
// This day was a real pain in the ***, especially for part 2.
// In part2, each seed becomes a range of seeds. I had initially modified
// my script to have a linked list of ranges of seeds, that is updated at
// each stage traversal. But the traversals became too complex and hard to debug
// even for the initial example. I ended out reusing part1's code by splitting
// the computation on a thread for each range of seed (fortunately there weren't too many of them!),
// it's a bit slow (~2min), but it works.
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

#define SEED_IS_IN_MAP_RANGE(seed, map) seed >= map->src && seed <= map->src + map->shift

typedef struct seed_s {
    long seed;
    long width; // not used in part 1
} seed_t;

typedef struct map_s {
    // parsed from: 'dst src shift'
    long dst;
    long src;
    long shift;
    struct map_s* next;
} map_t;

// 7 maps: seed to soil, to fertilizer, to water, to light, to temperature, to humidity, to location, each is a list of 3 elements arrays of int (d, s, width) 
static map_t* maps[7]; // zeroied by default
static volatile long min_loc = LONG_MAX; // XXX shouldn't we be using a mutex??

void parse_seeds(char *line, int *nb_seeds, seed_t *seeds) {
    *nb_seeds = 0;
    long seed, tmp;
    char* p = line;
    while (p) {
        sscanf(p, "%ld %ld", &seed, &tmp);
#ifndef PART2
        // For part 1 we parse seeds by pairs
        seeds[(*nb_seeds)].seed = seed;
        seeds[(*nb_seeds)].width = 0; // set to 0 to refactor code for part 2
        (*nb_seeds)++;
        seeds[(*nb_seeds)].seed = tmp;
        seeds[(*nb_seeds)].width = 0;
        (*nb_seeds)++;
#else
        seeds[*nb_seeds].seed = seed;
        seeds[*nb_seeds].width = tmp;
        (*nb_seeds)++;
#endif

        char* q = strchr(p, ' ') + 1;
        if (strchr(q, ' ') == NULL) {
            break;
        }
        p = strchr(q, ' ') + 1; // skip the intermediate space
    }
}

static void add_map(map_t** map, long dst, long src, long shift) {
    map_t* new_map = malloc(sizeof(map_t));
    new_map->dst = dst;
    new_map->src = src;
    new_map->shift = shift;
    new_map->next = NULL;

    if (!*map) {
        *map = new_map;
    } else {
        map_t* m = *map;
        while (m->next) {
            m = m->next;
        }
        m->next = new_map;
    }
}


// For a given seed, check the minimum location it outputs at the end of map transformations.
// This is particularly useful for part 2 where each seed is actually a range of seeds
void* check_min_location_for_seed(void* arg) {
    seed_t* seed = (seed_t* ) arg;
    long j = 0;
    do {
        long location = seed->seed + j;
        for (int m = 0; m < 7; m++) {
            map_t* map = maps[m];
            while (map) {
                if (SEED_IS_IN_MAP_RANGE(location, map)) {
                    location = map->dst + (location - map->src);
                    break;
                }
                map = map->next;
            }
        }
        if (location < min_loc) {
            // printf("[%ld] new min_loc: %ld\n", seed->width, location);
            min_loc = location;
        }

        j++;
    } while (j < seed->width);

    pthread_exit(EXIT_SUCCESS);
}

int main() {
   FILE *fp = fopen(INPUT, "r");
   if (!fp) {
      perror("File opening failed");
      return 1;
   }

   char line[250];
   int nb_seeds = 0;
   seed_t seeds[10];

   int current_map = -1;
   while (fgets(line, sizeof(line), fp)) {
       if (line[0] == '\n') {
           current_map++;
           continue;
       }
       line[strcspn(line, "\n")] = 0;

       if (strstr(line, "seeds")) {
           char* p = strchr(line, ':') + 2;
           parse_seeds(p, &nb_seeds, seeds);
           continue;
       }

       if (strstr(line, "to")) {
           continue;
       }


       long dst, src, width;
       sscanf(line, "%ld %ld %ld", &dst, &src, &width);
       add_map(&maps[current_map], dst, src, width);
   }

#ifdef PART2
   // For each (range of) seed(s), we create a thread that checks the potential final values
   // and updates min_loc global variable (works without a mutex \o/)
   pthread_t threads[nb_seeds];
   for (int i = 0; i < nb_seeds; i++) {
       pthread_create(&threads[i], NULL, check_min_location_for_seed, &seeds[i]);
   }
   for (int i = 0; i < nb_seeds; i++) {
       pthread_join(threads[i], NULL);
   }

#else
   // For some reason I failed to use threads for part1, at some point in the loop
   // I ended up having crappy data in my seeds array (especially non-null width values)
   // and I couldn't figure out why. So as a last resort I just used plain loops as a fallback.
   for (int i = 0; i < nb_seeds; ++i) {
       long location = seeds[i].seed;
       for (int m = 0; m < 7; m++) {
           map_t* map = maps[m];
           while (map) {
               if (SEED_IS_IN_MAP_RANGE(location, map)) {
                   location = map->dst + (location - map->src);
                   break;
               }
               map = map->next;
           }
       }
       if (location < min_loc) {
           min_loc = location;
       }
   }
#endif

   printf("min loc: %ld\n", min_loc);

   for (int i = 0; i < 7; ++i) {
       map_t* map = maps[i];
       while (map) {
           map_t* m = map;
           map = map->next;
           free(m);
       }
   }

   fclose(fp);
   return 0;
}
