
#pragma once

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define DIE(assertion, call_description)                       \
	do {                                                       \
		if (assertion) {                                       \
			fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__); \
			perror(call_description);                          \
			exit(errno);                                       \
		}                                                      \
	} while (0)

enum status { STATUS_OK, STATUS_ERROR, STATUS_NOT_FOUND };
