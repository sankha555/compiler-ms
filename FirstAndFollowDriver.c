# include <stdio.h>
# include "FirstAndFollow.h"

int main(int argc, char *argv[]) {
	// exactly one input file required
	if (argc != 2) {
		printf("ERROR: exactly one file argument expected!\n");
		return -1;
	}

	computeFirstAndFollowSets(argv[1]);

	return 0;
}
