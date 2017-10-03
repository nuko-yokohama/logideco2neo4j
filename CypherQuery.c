#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>

#include "CypherQuery.h"

static char* createQuery(json_object* jo);

static int getChangeNum(json_object* jo);
static json_object* getChangeItem(json_object* jo, int idx);
static char* getKind(json_object* jo) ;
static char* getTable(json_object* jo) ;

static void deleteQuery(char* query);

static
int getChangeNum(json_object* jo) {
	int res;
	json_object* change_obj;

	res  = json_object_object_get_ex(jo, "change", &change_obj);
	if (res == FALSE) {
		// invalid.
		return 0;
	}
	
	if (json_object_get_type(change_obj) != json_type_array) {
		// invalid.
		return 0;
	}

	return json_object_array_length(change_obj);
}

static
json_object* getChangeItem(json_object* jo, int idx) {
	int res;
	json_object* change_obj;
	json_object* change_item_obj;

	res  = json_object_object_get_ex(jo, "change", &change_obj);
	if (res == FALSE) {
		// invalid.
		return NULL;
	}

	if (json_object_get_type(change_obj) != json_type_array) {
		// not array, invalid.
		return NULL;
	}
	
	change_item_obj = json_object_array_get_idx(change_obj, idx);
	return change_item_obj;
}

char*
getKind(json_object* item) {
	int res;
	json_object* kind_obj;
	char* kind;

    res = json_object_object_get_ex(item, "kind", &kind_obj);
    if (res == FALSE)
        return NULL; // can't get

	kind = (char*) json_object_get_string(kind_obj);
	return kind;
}

char*
getTable(json_object* item) {
	int res;
	json_object* table_obj;
	char* table;

    res = json_object_object_get_ex(item, "table", &table_obj);
    if (res == FALSE)
        return NULL; // can't get

	table = (char*) json_object_get_string(table_obj);
	return table;
}

static
char* createInsertRelationshipQuery(json_object* item) {
	char* query ;
	int	res;

	json_object* name_array;
	json_object* value_array;

	int name_num;
	int value_num;

	res = json_object_object_get_ex(item, "columnnames", &name_array);
	if (res == FALSE) {
		return NULL;
	}
	if (json_object_get_type(name_array) != json_type_array) {
		// invalid
		return NULL;
	}

	res = json_object_object_get_ex(item, "columnvalues", &value_array);
	if (res == FALSE) {
		return NULL;
	}
	if (json_object_get_type(value_array) != json_type_array) {
		// invalid
		return NULL;
	}

	name_num = json_object_array_length(name_array);	
	value_num = json_object_array_length(value_array);	

	if (name_num != value_num) {
		// invalid
		return NULL;
	}

	query = malloc(QUERY_BUFFER);
	if (query == NULL) {
		return NULL;
	}

	if (name_num == 0) {
		// property not exist
		return NULL;
	}

	{
		// property exist
		int i;
		json_object* name_obj;
		json_object* value_obj;
		char* name;
		char* value;

		char rid[32];
		char f_label[32];
		char t_label[32];
		char f_uid[32];
		char t_uid[32];
		char type[32];

		rid[0] = '\0';
		f_uid[0] = '\0';
		t_uid[0] = '\0';
		type[0] = '\0';

		for (i=0; i < name_num; i++) {
			if (i != 0) {
				strcat(query, ",");
			}
			name_obj = json_object_array_get_idx(name_array, i);
			name = json_object_get_string(name_obj);
			value_obj = json_object_array_get_idx(value_array, i);
			value = json_object_get_string(value_obj);

			if (!strcmp(name, "rid")) {
				strcpy(rid, value);
			} else if (!strcmp(name, "f_label")) {
				strcpy(f_label, value);
			} else if (!strcmp(name, "t_label")) {
				strcpy(t_label, value);
			} else if (!strcmp(name, "f_uid")) {
				strcpy(f_uid, value);
			} else if (!strcmp(name, "t_uid")) {
				strcpy(t_uid, value);
			} else if (!strcmp(name, "type")) {
				strcpy(type, value);
			} else {
				// skip
			}
		}

		// Create MATCH Query 
		sprintf(query,
			"{\"query\":\"MATCH (f:%s {nid:\\\"%s\\\"}),(t:%s {nid:\\\"%s\\\"}) MERGE (f)-[e:%s {rid:\\\"%s\\\"}]->(t)\"}",
			f_label, f_uid, t_label, t_uid, type, rid);

	}
	return query;
}

static
char* createInsertNodeQuery(json_object* item, char* table) {
	char* query ;
	int	res;

	json_object* name_array;
	json_object* value_array;

	int name_num;
	int value_num;

	res = json_object_object_get_ex(item, "columnnames", &name_array);
	if (res == FALSE) {
		return NULL;
	}
	if (json_object_get_type(name_array) != json_type_array) {
		// invalid
		return NULL;
	}

	res = json_object_object_get_ex(item, "columnvalues", &value_array);
	if (res == FALSE) {
		return NULL;
	}
	if (json_object_get_type(value_array) != json_type_array) {
		// invalid
		return NULL;
	}

	name_num = json_object_array_length(name_array);	
	value_num = json_object_array_length(value_array);	

	if (name_num != value_num) {
		// invalid
		return NULL;
	}

	query = malloc(QUERY_BUFFER);
	if (query == NULL) {
		return NULL;
	}
	strcpy(query, "{\"query\":\"CREATE (n:");
	strcat(query, table);
	strcat(query, " { ");


	if (name_num == 0) {
		// property not exist
		strcat(query, "});");
		return query;
	} else {
		// property exist
		int i;
		json_object* name_obj;
		json_object* value_obj;
		char* name;
		char* value;

		for (i=0; i < name_num; i++) {
			if (i != 0) {
				strcat(query, ",");
			}
			name_obj = json_object_array_get_idx(name_array, i);
			name = json_object_get_string(name_obj);
			value_obj = json_object_array_get_idx(value_array, i);
			value = json_object_get_string(value_obj);

			strcat(query, "");
			strcat(query, name);
			strcat(query, ":\\\"");
			strcat(query, value);
			strcat(query, "\\\"");
		}
		strcat(query, " }) \"} ");
	}
	return query;
}

static
char* createDeleteNodeQuery(json_object* item) {
	char* query ;
	int	res;

	json_object* table;
	json_object* oldkeys;
	json_object* value_array;
	json_object* value_obj;
	char* value;
	char* table_name;

	int name_num;
	int value_num;
printf("createDeleteNodeQuery start\n");
	res = json_object_object_get_ex(item, "table", &table);
	if (res == FALSE) {
fprintf(stderr, "return null, %d\n", __LINE__);
		return NULL;
	}
	table_name = json_object_get_string(table);

	res = json_object_object_get_ex(item, "oldkeys", &oldkeys);
	if (res == FALSE) {
fprintf(stderr, "return null, %d\n", __LINE__);
		return NULL;
	}
	if (json_object_get_type(oldkeys) != json_type_object) {
		// invalid
fprintf(stderr, "return null, %d\n", __LINE__);
		return NULL;
	}

	res = json_object_object_get_ex(oldkeys, "keyvalues", &value_array);
	if (res == FALSE) {
fprintf(stderr, "return null, %d\n", __LINE__);
		return NULL;
	}
	if (json_object_get_type(value_array) != json_type_array) {
		// invalid
fprintf(stderr, "return null, %d\n", __LINE__);
		return NULL;
	}
	value_num = json_object_array_length(value_array);	
	if (value_num == 0) {
		// value not exist.
fprintf(stderr, "return null, %d\n", __LINE__);
		return NULL;
	}

	value_obj = json_object_array_get_idx(value_array, 0);
	value = json_object_get_string(value_obj);

	query = malloc(QUERY_BUFFER);
	if (query == NULL) {
		return NULL;
	}

	// Create MATCH DELETE Query 
	sprintf(query,
		"{\"query\":\"MATCH (n:%s) WHERE n.nid = '%s' DELETE n\"}", table_name, value);

	return query;
}

static
char* createDeleteRelationshipQuery(json_object* item) {
	char* query ;
	int	res;

	json_object* oldkeys;
	json_object* value_array;
	json_object* value_obj;
	char* value;

	int name_num;
	int value_num;

	res = json_object_object_get_ex(item, "oldkeys", &oldkeys);
	if (res == FALSE) {
		return NULL;
	}
	if (json_object_get_type(oldkeys) != json_type_object) {
		// invalid
		return NULL;
	}

	res = json_object_object_get_ex(oldkeys, "keyvalues", &value_array);
	if (res == FALSE) {
		return NULL;
	}
	if (json_object_get_type(value_array) != json_type_array) {
		// invalid
		return NULL;
	}
	value_num = json_object_array_length(value_array);	
	if (value_num == 0) {
		// value not exist.
		return NULL;
	}

	value_obj = json_object_array_get_idx(value_array, 0);
	value = json_object_get_string(value_obj);

	query = malloc(QUERY_BUFFER);
	if (query == NULL) {
		return NULL;
	}

	// Create MATCH DELETE Query 
	sprintf(query,
		"{\"query\":\"MATCH (f)-[r]-(t) WHERE r.rid = '%s' DELETE r\"}", value);

	return query;
}

static
char* createInsertQuery(json_object* item, char* table) {
	char* query;
	if (!strcmp(table, "person") || !strcmp(table, "company")) {
		// cretae node
		query = createInsertNodeQuery(item, table);
	} else if ("relationship") {
		// cretae relationship
		query = createInsertRelationshipQuery(item);
		return query;
	} else {
		// invalid table.
		return NULL;
	}
	return query;
}

static
char* createDeleteQuery(json_object* item, char* table) {
	char* query;
	if (!strcmp(table, "person") || !strcmp(table, "company")) {
		// delete node
		query = createDeleteNodeQuery(item);
	} else if ("relationship") {
		// cretae relationship
		query = createDeleteRelationshipQuery(item);
	} else {
		// invalid table.
		return NULL;
	}
	return query;
}

static
char* createQuery(json_object* item) {
	char* kind;
	char* table;
	char* query = NULL;

	kind = getKind(item);
	table = getTable(item);

	if ( !strcmp(kind, "insert") ) {
		query = createInsertQuery(item, table);
	} else if ( !strcmp(kind, "update") ) {
		// query = createUpdateQuery(item, table);
	} else if ( !strcmp(kind, "delete") ) {
		query = createDeleteQuery(item, table);
	} else {
		// invalid kind.
		fprintf(stderr, "createQuery: invalid kind = [%s]\n", kind);
		return NULL;
	}
	return query;
}

/*
 * execute queries in transaction.
 */
int
executeCypherQuery(char* json) {
	json_object* json_obj;
	json_object* item;	// one change item
	char* query;
	int num;
	int i;
	int executed = 0;

	// parse json string.
	json_obj = json_tokener_parse(json);
	if (json_obj == NULL) {
		// parse error, return NULL
		fprintf(stderr, "parse error\n");
		return NULL;
	}
	
	num = getChangeNum(json_obj);
	for (i=0; i < num; i++) {
		item = getChangeItem(json_obj, i);
		query = createQuery(item);

		if (query != NULL) {
			char* url;
			char* result;

			url = getNeo4jurl();
			// execute query
			// call neo4j_accesor
			result = neo4j_accessor(url, query);
			if (result != NULL) {
				printf("neo4j_rest_accessor success.\nurl=%s\nquery=%s\n",
					url, query);
				// printf("result = %s\n", result);
			} else {
				fprintf(stderr, "neo4j_rest_accessor error.\nerror url=%s\nerror query=%s\n", url, query);
			}
			deleteQuery(query);
			executed++;
		} else {
			fprintf(stderr, "not create query\n");
		}
	}
	return executed;	
}


void 
deleteQuery(char* query) {
	if (query != NULL)
		free(query);
}
