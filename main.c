#include "database.h"

void print_int(const void *entry)
{
	printf("%d\n", *(const int *)entry);
}

bool should_remove(const void *entry, const void *val)
{
	return *(const int *)entry == *(const int *)val;
}

int main(void)
{
	struct db_manager db_mgr = create_database("test", sizeof(int));
	append_entry(db_mgr, &(int){ 42 });
	append_entry(db_mgr, &(int){ 43 });
	append_entry(db_mgr, &(int){ 44 });
	append_entry(db_mgr, &(int){ 61 });
	enum status st = remove_unique_entry(db_mgr, &(int){ 61 }, should_remove);

	dump_database(db_mgr, print_int);
	close_database(db_mgr);
}
