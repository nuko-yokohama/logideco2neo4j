# contrib/json2neo4j/Makefile

PGFILEDESC = ""
PGAPPICON = win32

PROGRAM = json2neo4j
OBJS	= Params.o TransactionJsonDataset.o logideco2neo4j.o CypherQuery.o neo4j_accessor.o

PG_CPPFLAGS = -I$(libpq_srcdir)
LDLIBS += -L/usr/local/lib -lcurl -ljson-c 
PG_LIBS = $(libpq_pgport) $(PTHREAD_LIBS) -lcurl -ljson-c
SHLIB_LINK = -L/usr/local/lib 

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/json2neo4j
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

ifneq ($(PORTNAME), win32)
override CFLAGS += $(PTHREAD_CFLAGS)
endif
