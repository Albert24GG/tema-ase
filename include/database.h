#pragma once

#include "error.h"

#include <stdbool.h>
#include <stdio.h>

struct db_manager {
	FILE *db_file;
	size_t entry_size;
};

/*
 * @brief A function that determines if an entry matches the criteria.
 * @param entry The entry to check.
 * @param criteria The criteria to match.
 * @return True if the entry matches the criteria, false otherwise.
 */
typedef bool (*match_crit_func)(const void *, const void *);

/*
 * @brief A function that dumps the entry either to a file or to stdout.
 * @param entry The entry to dump.
 * @param out The file descriptor to dump the entry to.
 */
typedef void (*dump_entry_func)(const void *, FILE *);

/*
 * @brief A function that updates an entry in the database.
 * It is not mandatory for the second parameter to be of the same type as the
 * entry. The function can use the second parameter in any way to update the
 * entry.
 * @param entry The entry to update.
 * @param update_val The value used to update the entry.
 */
typedef void (*update_func)(void *, const void *);

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
void dump_database(struct db_manager db_mgr,
 * @param criteria The criteria to match.
 * @param should_update A function that determines if an entry should be updated
 * based on the criteria.
 * @param update_val The value used to update the entry.
 * @param update A function that updates the entry using information from
 * update_val.
 * @return The status of the operation.
 */
enum status update_entries(struct db_manager db_mgr, const void *criteria,
						   match_crit_func should_update,
						   const void *update_val, update_func update);

/*
 * @brief Remove the first entry in the database that matches the criteria.
 * @param db_mgr The database manager.
 * @param criteria The criteria to match.
 * @param matches_crit A function that determines if an entry matches the
 * criteria.
 * @return The status of the operation.
 */
enum status remove_unique_entry(struct db_manager db_mgr, const void *criteria,
								match_crit_func matches_crit);

/*
 * @brief Dump entries from the database that match some criteria.
 * @param db_mgr The database manager.
 * @param dump_entry A function that dumps the entry to a file.
 * @param criteria The criteria to match.
 * @param matches_crit A function that determines if an entry matches the
 * criteria.
 * @param out The file descriptor to dump the entries to.
 */
void dump_database(struct db_manager db_mgr, dump_entry_func dump_entry,
				   const void *criteria, match_crit_func matches_crit,
				   FILE *out);
