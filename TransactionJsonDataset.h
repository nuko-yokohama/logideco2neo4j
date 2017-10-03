#define MAX_TRANSACTIONS 64

typedef struct TransactionJsonDataset_T {
	int transactions;
	char json_data[MAX_TRANSACTIONS][8192];
} TransactionJsonDataset_t;

extern void init_TransactionJsonDataset();
extern void print_TransactionJsonDataset(TransactionJsonDataset_t* dataset);
extern int getTransactions_TransactionJsonDataset(TransactionJsonDataset_t* dataset);
extern char* getJsonData_TransactionJsonDataset(TransactionJsonDataset_t* dataset, int idx);
extern TransactionJsonDataset_t* create_TransactionJsonData(char* conninfo, char* slot_name);

