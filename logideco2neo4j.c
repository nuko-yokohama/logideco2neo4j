/*
 * logodeco2neo4j
 *
 * read wal2json logical decode, apply neo4j
 *
 * logideco2neo4j conninfo slot_name neo4j_url
 *
 * argv[1] PostgreSQL conninfo 
 * argv[2] Logical replication slot name (default: 'neo4_slot')
 * argv[3] Neo4j Server URL (default: 'http://localhost:7474/db/data/cypher')
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "TransactionJsonDataset.h"

#define SLEEP_TIME 1 // second.

int
main(int argc, char** argv) {
	TransactionJsonDataset_t* dataset;
	int i;
	int txs;

	setParams(argc, argv);
	printParams();
	// get Transacsion Json Dataset From Logical Replication Slot.
	while (1) {
		dataset = create_TransactionJsonDataset(getConninfo(), getSlotname());
		print_TransactionJsonDataset(dataset);
		txs = getTransactions_TransactionJsonDataset(dataset);
		// execute CypherQuery
		for (i=0; i < txs; i++) {
			executeCypherQuery(getJsonData_TransactionJsonDataset(dataset, i));
		}

		sleep(SLEEP_TIME);
	}
}
