typedef struct Params_t {
    char *conninfo;
    char *slot_name;
    char *neo4jurl;
} Params;

extern void setParams(int argc, char** argv);
extern void printParams();
extern char* getConninfo();
extern char* getSlotname();
extern char* getNeo4jurl();
