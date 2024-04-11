#pragma once

#include "error.h"

#include <stdbool.h>
#include <stdio.h>

struct db_manager {
	FILE *db_file;
	size_t entry_size;
};

struct db_manager create_database(const char *db_name, size_t entry_size);
void close_database(struct db_manager db_mgr);
enum status append_entry(struct db_manager db_mgr, const void *entry);
enum status update_entries(struct db_manager db_mgr, const void *criteria,
						   bool (*should_update)(const void *, const void *),
						   const void *update_val,
						   void (*update)(void *, const void *));
enum status remove_unique_entry(struct db_manager db_mgr, const void *criteria,
								bool (*matches_crit)(const void *,
													 const void *));
void dump_database(struct db_manager db_mgr, void (*print_entry)(const void *));
