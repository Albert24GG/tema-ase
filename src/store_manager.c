#include "store_manager.h"

#include "database.h"
#include "error.h"

#include <string.h>
#include <strings.h>

enum status add_item(struct db_manager db_mgr, const struct store_item *item)
{
	return append_entry(db_mgr, (void *)item);
}

bool matches_barcode(const void *entry, const void *barcode)
{
	return ((const struct store_item *)entry)->barcode ==
		   *(const int64_t *)barcode;
}

bool matches_category(const void *entry, const void *category)
{
	return strcasecmp(((const struct store_item *)entry)->category,
					  (const char *)category) == 0;
}

bool matches_name(const void *entry, const void *name)
{
	return strcasecmp(((const struct store_item *)entry)->name,
					  (const char *)name) == 0;
}

void update_price(void *entry, const void *price)
{
	((struct store_item *)entry)->price = *(const float *)price;
}

void update_quantity(void *entry, const void *quantity)
{
	((struct store_item *)entry)->quantity = *(const int *)quantity;
}

void update_expiry_date(void *entry, const void *expiry_date)
{
	memmove(&((struct store_item *)entry)->expiry_date, expiry_date,
			sizeof(struct date));
}

void discount_price(void *entry, const void *discount)
{
	((struct store_item *)entry)->price *= 1 - *(const float *)discount;
}

void dump_store_item_info(const void *entry, FILE *out)
{
	(void)fprintf(out, "-----------------\n");
	const struct store_item *item = (const struct store_item *)entry;
	(void)fprintf(out, "Cod produs: %ld\n", item->barcode);
	(void)fprintf(out, "Nume produs: %s\n", item->name);
	(void)fprintf(out, "Categorie: %s\n", item->category);
	(void)fprintf(out, "Pret: %.2f\n", item->price);
	(void)fprintf(out, "Cantitate: %zu\n", item->quantity);
	(void)fprintf(out, "Data de expirare: %d/%d/%d\n", item->expiry_date.day,
				  item->expiry_date.month, item->expiry_date.year);
	(void)fprintf(out, "-----------------\n");
}
