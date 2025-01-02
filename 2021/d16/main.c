#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_BIT_COUNT 131072
#ifdef TEST
#define INPUT "input_test"
#else
#define INPUT "input"
#endif

int bits[MAX_BIT_COUNT];
int nb_bits = 0;
int version_sum = 0;

void parse_packet(int* start) {
    int packet_version = ((bits[*start] << 2) | (bits[*start + 1] << 1) | (bits[*start + 2]));
    *start += 3;
	version_sum += packet_version;

	int packet_type = ((bits[*start] << 2) | (bits[*start + 1] << 1) | (bits[*start + 2]));
    *start += 3;

    if (packet_type == 4) {
        // Literal
        while (bits[*start] == 1) {
			*start += 5;
		}
		*start += 5;
    } else {
        // parse subpackets

		int length_type;
		length_type = bits[*start];
		(*start)++;

		if (length_type == 0) {
			int subpackets_length = 0;
			for (int i = 0; i < 15; i++) {
				subpackets_length <<= 1;
				subpackets_length |= bits[*start];
				(*start)++;
			}
			int end = subpackets_length + (*start);
			while ((*start) < end) {
				parse_packet(start);
			}

		} else {
			int subpackets_number = 0;
			for (int i = 0; i < 11; i++) {
				subpackets_number <<= 1;
				subpackets_number |= bits[*start];
				(*start)++;
			}
			for (int i = 0; i < subpackets_number; i++) {
				parse_packet(start);
            }
		}
    }
}

unsigned long long evaluate(int* start) {
	unsigned long long res = 0;
    int packet_version = ((bits[*start] << 2) | (bits[*start + 1] << 1) | (bits[*start + 2]));
    *start += 3;
	version_sum += packet_version;

	int packet_type = ((bits[*start] << 2) | (bits[*start + 1] << 1) | (bits[*start + 2]));
    *start += 3;

    if (packet_type == 4) {
        unsigned long long val = 0;
        do {
            (*start)++;
            for (int i = 0; i < 4; i++) {
                val <<= 1;
                val |= bits[*start];
                (*start)++;
            }
        } while (bits[*start - 5] == 1);
        res = val;
    } else {
		int length_type;
		length_type = bits[*start];
		(*start)++;
		unsigned long long val;

		if (length_type == 0) {
			int subpackets_length = 0;
			for (int i = 0; i < 15; i++) {
				subpackets_length <<= 1;
				subpackets_length |= bits[*start];
				(*start)++;
			}
			int end = subpackets_length + (*start);
			int n = 0;
			while ((*start) < end) {
				val = evaluate(start);
				switch(packet_type) {
                case 0: res += val; break;
                case 1: if (n == 0) res = val; else res *= val; break;
                case 2: if ((res > val) || (n == 0)) res = val; break;
                case 3: if ((res < val) || (n == 0)) res = val; break;
                case 5: if (n == 0) { res = val; } else { res = (res > val); } break;
                case 6: if (n == 0) { res = val; } else { res = (res < val); } break;
                case 7: if (n == 0) { res = val; } else { res = (res == val); } break;
                default: break;
				}
				n++;
			}

		} else {
			int subpackets_number = 0;
			for (int n = 0; n < 11; n++) {
				subpackets_number <<= 1;
				subpackets_number |= bits[*start];
				(*start)++;
			}
			for (int n = 0; n < subpackets_number; n++) {
				val = evaluate(start);
				switch(packet_type) {
                case 0: res += val; break;
                case 1: if (n == 0) res = val; else res *= val; break;
                case 2: if ((res > val) || (n == 0)) res = val; break;
                case 3: if ((res < val) || (n == 0)) res = val; break;
                case 5: if (n == 0) { res = val; } else { res = (res > val); } break;
                case 6: if (n == 0) { res = val; } else { res = (res < val); } break;
                case 7: if (n == 0) { res = val; } else { res = (res == val); } break;
                default: break;
				}
			}
		}
    }

    return res;
}


int main() {
    FILE* f = fopen(INPUT, "r");

    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        for (size_t i = 0; i < strlen(p); i++) {
            int c = p[i];
            if (isupper(c)) {
                c -= 'A' - 10;
            } else if (isdigit(c)) {
                c -= '0';
            } else {
                continue;
            }
            bits[nb_bits++] = (c >> 3) & 0x1;
            bits[nb_bits++] = (c >> 2) & 0x1;
            bits[nb_bits++] = (c >> 1) & 0x1;
            bits[nb_bits++] = (c >> 0) & 0x1;
        }
    }

    int start = 0;
    parse_packet(&start);
    printf("part1: %d\n", version_sum);

    start = 0;
    printf("part2: %lld\n", evaluate(&start));

    fclose(f);
}
