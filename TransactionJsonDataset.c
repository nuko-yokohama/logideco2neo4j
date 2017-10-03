#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libpq-fe.h>

#include "TransactionJsonDataset.h"

static TransactionJsonDataset_t TransactionJsonDataset;

static void add_TransactionJsonDataset(char* value);

void
init_TransactionJsonDataset() {
	int num;
	TransactionJsonDataset.transactions = 0;
	for (num = 0; num < MAX_TRANSACTIONS; num++) {
		TransactionJsonDataset.json_data[num][0] = '\0';
	}
}

void
print_TransactionJsonDataset(TransactionJsonDataset_t* dataset) {
	int num;
	// printf("txs = %d\n", dataset->transactions); 
	for (num = 0; num < dataset->transactions; num++) {
		printf("tx[%d] %s\n", num, dataset->json_data[num]); 
	}
}

static void
add_TransactionJsonDataset(char* value) {
	strcat(
		TransactionJsonDataset.json_data[TransactionJsonDataset.transactions],
		value);
	if (!strcmp(value, "]}")) {
		// transaction terminate
		TransactionJsonDataset.transactions++;
	}
}

int
getTransactions_TransactionJsonDataset(TransactionJsonDataset_t* dataset) {
	return dataset->transactions;
}

char*
getJsonData_TransactionJsonDataset(TransactionJsonDataset_t* dataset, int idx) {
	if (idx < 0 || idx >= MAX_TRANSACTIONS)
		return NULL;

	return dataset->json_data[idx];
}

TransactionJsonDataset_t*
create_TransactionJsonDataset(char* conninfo, char* slot_name) {
	char *sql = "SELECT data FROM pg_logical_slot_get_changes($1, NULL, NULL);";
	const char *paramValues[1];

	PGconn *conn;
	PGresult *res;

	int tuples;
	int t;
	char *v;

	/* create postgres connection */
	conn = PQconnectdb(conninfo);

	init_TransactionJsonDataset();

	/* read logical replication slot. */
	paramValues[0] = slot_name;
	res = PQexecParams(conn, sql,
		1, 
		NULL,        // paramTypes
		paramValues, // paramValues
		NULL,        // paramLength
		NULL,        // paramFormats
		0            // resultFormat, text
	 );
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "PQexec is not PGRES_TUPLES_OK\n");
		exit(-1);
	}

	tuples = PQntuples(res);
	for (t = 0; t < tuples; t++) {
		/* get logical replication information(LRI) */
		v = PQgetvalue(res, t, 0);
		add_TransactionJsonDataset(v);
	}
	PQclear(res);
	PQfinish(conn);

	return &TransactionJsonDataset;
}


