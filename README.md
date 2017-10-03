# logideco2neo4j

## Abstract
logideco2neo4j is an application that inserts, updates and deletes into Neo 4j based on JSON format logical WAL.
It depends on json-c and libcurl.

## How to use

* Exsample

```
logideco2neo4j conninfo slot_name neo4j_url
```

* argv[1] PostgreSQL conninfo 
* argv[2] Logical replication slot name (default: 'neo4_slot')
* argv[3] Neo4j Server URL (default: 'http://localhost:7474/db/data/cypher')

# Author

nuko-yokohama (@nuko_yokohama)
