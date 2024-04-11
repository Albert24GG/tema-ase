#pragma once

#include "database.h"
#include "error.h"

struct cli_program {
	struct db_manager db_mgr;
	char *cmd_buffer;
};
