/*
 * main.c
 *
 *  Created on: Oct 14, 2017
 *      Author: gilles
 */

#ifdef WIN32
#define LOG2REPORTER_LIBRARY_IMPORTS
#endif

#include "log2reporter.h"

static unsigned char binData[] = {'D', 'A', 'T', 'A', '!'};
int main(int argc, char **argv) {
	for (int i = 0; i < 1000; i++)
		for (int j = 1; j < argc; j++) {
			LogText(0x00FF, j, true, argv[j]);
			LogVText(0x00FF, j, true, "(%d):'%s'", j, argv[j]);
			LogData(0xFF00, j, binData, sizeof(binData));
		}
	return 0;
}
