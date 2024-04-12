#pragma once

#include "error.h"

#include <stdbool.h>
#include <stdio.h>

struct db_manager {
	FILE *db_file;
	size_t entry_size;
};

/*
 * @brief Create a new database.
 * @param db_name The name of the database.
 * @param entry_size The size of each entry in the database.
 * @return The database manager.
 */
struct db_manager create_database(const char *db_name, size_t entry_size);

/*
 * @brief Open an existing database.
 * @param db_name The name of the database.
 * @param entry_size The size of each entry in the database.
 * @return The database manager.
 */
struct db_manager open_database(const char *db_name, size_t entry_size);

/*
 * @brief Close the database.
 * @param db_mgr The database manager.
 */
void close_database(struct db_manager db_mgr);

/*
 * @brief Append an entry to the database.
 * @param db_mgr The database manager.
 * @param entry The entry to append.
 * @return The status of the operation.
 */
enum status append_entry(struct db_manager db_mgr, const void *entry);

/*
 * @brief Update all entries in the database that match the criteria.
 * @param db_mgr The database manager.
 * @param criteria The criteria to match.
 * @param should_update A function that determines if an entry should be updated
 * based on the criteria.
 * The function should have the following signature:
 * bool should_update(const void *entry_to_check, const void *criteria);
 * @param update_val The value used to update the entry.
 * @param update A function that updates the entry using information from
 * update_val.
 * The function should have the following signature:
 * void update(void *entry_to_update, const void *update_val);
 * @return The status of the operation.
 */
enum status update_entries(struct db_manager db_mgr, const void *criteria,
						   bool (*should_update)(const void *, const void *),
						   const void *update_val,
						   void (*update)(void *, const void *));

/*
 * @brief Remove the first entry in the database that matches the criteria.
 * @param db_mgr The database manager.
 * @param criteria The criteria to match.
 * @param matches_crit A function that determines if an entry matches the
 * criteria.
 * @return The status of the operation.
 *
 */
enum status remove_unique_entry(struct db_manager db_mgr, const void *criteria,
								bool (*matches_crit)(const void *,
													 const void *));
void dump_database(struct db_manager db_mgr,
				   void (*dump_entry)(const void *, FILE *),
				   const void *criteria,
				   bool (*matches_crit)(const void *, const void *), FILE *out);
