#include "store_manager.h"

#include "database.h"
#include "error.h"

#include <string.h>

enum status add_item(struct db_manager db_mgr, const struct store_item *item)
{
	return append_entry(db_mgr, (void *)item);
}

bool matches_barcode(const void *entry, const void *barcode)
{
	return ((const struct store_item *)entry)->barcode ==
		   *(const int64_t *)barcode;
}

void update_price(void *entry, const void *price)
{
	((struct store_item *)entry)->price = *(const float *)price;
}

// enum status update_price_by_id(struct db_manager db_mgr, int64_t barcode,
// 							   float price)
// {
// 	return update_entries(db_mgr, &barcode, matches_barcode, &price,
// 						  update_price);
// }

void update_quantity(void *entry, const void *quantity)
{
	((struct store_item *)entry)->quantity = *(const int *)quantity;
}

// enum status update_quantity_by_id(struct db_manager db_mgr, int64_t barcode,
// 								  int quantity)
// {
// 	return update_entries(db_mgr, &barcode, matches_barcode, &quantity,
// 						  update_quantity);
// }

void update_expiry_date(void *entry, const void *expiry_date)
{
	memmove(&((struct store_item *)entry)->expiry_date, expiry_date,
			sizeof(struct date));
}

// enum status update_expiry_date_by_id(struct db_manager db_mgr, int64_t
// barcode, 									 const char *expiry_date)
// {
// 	return update_entries(db_mgr, &barcode, matches_barcode, expiry_date,
// 						  update_expiry_date);
// }

bool matches_category(const void *entry, const void *category)
{
	return strcmp(((const struct store_item *)entry)->category,
				  (const char *)category) == 0;
}
