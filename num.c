#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

int main(int argc, char **argv)
{
	uint8_t counter;
	uint8_t asc;
	uint8_t inc;
	uint8_t max;
	int i;

	if (3 != argc) {
		fprintf(stderr, "Usage: %s max inc\n", argv[0]);
		exit(1);
	}

	max = (uint8_t) atoi(argv[1]);
	inc = (uint8_t) atoi(argv[2]);

	counter = 1;
	asc = 1;
	i = 0;

	while (counter) {
		uint8_t newcounter;

		//printf("---\n");

		if (counter >= max) {
			asc = 0;
		}
		if (asc) {
			if (counter < 10) {
				//printf("increment by inc\n");
				newcounter = counter + inc;
			} else {
				//printf("increment by inc*counter/10\n");
				newcounter = counter + 2 * inc * (counter/10);
			}
			if (newcounter < counter) {
				asc = 0;
				newcounter = counter;
			}
		} else {
			if (counter < inc) {
				newcounter = 0;
			} else if (counter < 10) {
				//printf("dec by inc\n");
				newcounter = counter - inc;
			} else {
				//printf("dec by inc*counter/10\n");
				newcounter = counter - inc * (counter/10);
			}
			if (newcounter >= counter) {
				newcounter = counter - inc;
			}
		}
		//printf("asc=%u; counter=%u; newcounter=%u\n", asc, counter, newcounter);
		counter = newcounter;
		printf("%u\n", counter);
		i++;
	}
	printf("i=%d\n", i);
	return 0;
}
