#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Params.h"

Params params;

void
setParams(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "parameter error.\n");
        exit(-1);
    }

    params.conninfo = strdup(argv[1]);
    params.slot_name = strdup(argv[2]);
    params.neo4jurl = strdup(argv[3]);

    return;
}

char*
getConninfo() {
	return params.conninfo;
}

char*
getSlotname() {
	return params.slot_name;
}

char*
getNeo4jurl() {
	return params.neo4jurl;
}

void
printParams() {
	printf("conninfo = %s\n", params.conninfo);
	printf("slot_name = %s\n", params.slot_name);
	printf("neo4jurl= %s\n", params.neo4jurl);
}
