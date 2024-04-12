#include "database.h"

#include "error.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define _POSIX1_SOURCE 2
#include <unistd.h>

struct db_manager create_database(const char *db_name, size_t entry_size)
{
	printf("name: %s\n", db_name);
	FILE *db = fopen(db_name, "w+b");
	DIE(db == NULL, "Error opening database");

	return (struct db_manager){ .db_file = db, .entry_size = entry_size };
}

struct db_manager open_database(const char *db_name, size_t entry_size)
{
	FILE *db = fopen(db_name, "r+b");
	DIE(db == NULL, "Error opening database");

	return (struct db_manager){ .db_file = db, .entry_size = entry_size };
}

void close_database(struct db_manager db_mgr)
{
	if (db_mgr.db_file != NULL)
		(void)fclose(db_mgr.db_file);
}

static enum status write_entry(struct db_manager db_mgr, const void *entry)
{
	size_t written = fwrite(entry, db_mgr.entry_size, 1, db_mgr.db_file);

	return written == 1 ? STATUS_OK : STATUS_ERROR;
}

/*
 * @brief Find the index of the entry in the database
 * @param db_mgr - the database manager
 * @param criteria - the criteria to match
 * @param matches_crit - the function to match the criteria
 * @return the index of the entry in the database or -1 if the entry is not
 * found
 */
static int64_t find_entry_idx(struct db_manager db_mgr, const void *criteria,
							  bool (*matches_crit)(const void *, const void *))
{
	FILE *db = db_mgr.db_file;
	size_t entry_size = db_mgr.entry_size;
	// (void)fseek(db, 0, SEEK_SET);

	int64_t idx = 0;

	char *buffer = malloc(entry_size);
	DIE(buffer == NULL, "Error allocating buffer");

	while (fread(buffer, entry_size, 1, db) == 1) {
		if (matches_crit(buffer, criteria)) {
			free(buffer);
			return idx;
		}
		++idx;
	}

	free(buffer);
	return -1;
}

enum status append_entry(struct db_manager db_mgr, const void *entry)
{
	// set the file pointer to the end of the file
	fseek(db_mgr.db_file, 0, SEEK_END);
	return write_entry(db_mgr, entry);
}

enum status update_entries(struct db_manager db_mgr, const void *criteria,
						   bool (*should_update)(const void *, const void *),
						   const void *update_val,
						   void (*update)(void *, const void *))
{
	FILE *db = db_mgr.db_file;
	size_t entry_size = db_mgr.entry_size;
	(void)fseek(db, 0, SEEK_SET);

	char *buffer = malloc(entry_size);
	DIE(buffer == NULL, "Error allocating buffer");

	while (fread(buffer, entry_size, 1, db) == 1) {
		if (should_update(buffer, criteria)) {
			update(buffer, update_val);

			fseek(db, -entry_size, SEEK_CUR);
			write_entry(db_mgr, buffer);
		}
	}

	free(buffer);
	return STATUS_OK;
}

enum status remove_unique_entry(struct db_manager db_mgr, const void *criteria,
								bool (*matches_crit)(const void *,
													 const void *))
{
	FILE *db = db_mgr.db_file;
	size_t entry_size = db_mgr.entry_size;
	(void)fseek(db, 0, SEEK_SET);

	int64_t idx = find_entry_idx(db_mgr, criteria, matches_crit);

	if (idx == -1) {
		return STATUS_ERROR;
	}

	// calculate the size of the remaining entries
	fseek(db, 0, SEEK_END);
	long int db_size = ftell(db);

	(void)fseek(db, idx * (long)entry_size, SEEK_SET);
	long int remaining_size = db_size - (idx + 1) * (long int)entry_size;

	if (remaining_size <= 0) {
		int ret_val = ftruncate(fileno(db), db_size - (long)entry_size);
		fflush(db);
		return ret_val == 0 ? STATUS_OK : STATUS_ERROR;
	}

	// read the remaining entries
	(void)fseek(db, (idx + 1) * (long)entry_size, SEEK_SET);

	char *remaining_data = malloc(remaining_size);
	DIE(remaining_data == NULL, "Error allocating buffer");

	if (fread(remaining_data, 1, remaining_size, db) != remaining_size) {
		free(remaining_data);
		return STATUS_ERROR;
	}

	// write the remaining entries one position back

	(void)fseek(db, idx * (long)entry_size, SEEK_SET);
	if (fwrite(remaining_data, 1, remaining_size, db) != remaining_size) {
		free(remaining_data);
		return STATUS_ERROR;
	}

	// truncate the file to the new size
	if (ftruncate(fileno(db), db_size - (long)entry_size) != 0) {
		free(remaining_data);
		return STATUS_ERROR;
	}

	fflush(db);
	free(remaining_data);
	return STATUS_OK;
}

void dump_database(struct db_manager db_mgr,
				   void (*dump_entry)(const void *, FILE *),
				   const void *criteria,
				   bool (*matches_crit)(const void *, const void *), FILE *out)
{
	FILE *db = db_mgr.db_file;
	size_t entry_size = db_mgr.entry_size;
	(void)fseek(db, 0, SEEK_SET);

	char *buffer = malloc(entry_size);
	DIE(buffer == NULL, "Error allocating buffer");

	while (fread(buffer, entry_size, 1, db) == 1) {
		if (matches_crit == NULL || matches_crit(buffer, criteria)) {
			dump_entry(buffer, out);
		}
	}

	free(buffer);
}
