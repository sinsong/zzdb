#pragma once

#include "zzdef.h"
#include <stdio.h>

typedef struct
{
	char *dbpath;
	char *filename_zzdb_tables;
	char *filename_zzdb_stat;
} zzConn;

zzConn *zzConnect(const char *path);
zzConn *zzInitDB(const char *path);
zzConn *zzClose(zzConn *conn);

const char *zzErrorMessage();

#include "zztb.h"
#include "zzcur.h"