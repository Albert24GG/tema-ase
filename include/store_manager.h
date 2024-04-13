#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define ITEM_NAME_MAX_LEN 64
#define ITEM_CATEGORY_MAX_LEN 64

struct date {
	int8_t day;
	int8_t month;
	int32_t year;
};

struct store_item {
	float price;
	int64_t barcode;
	size_t quantity;
	struct date expiry_date;
	char name[ITEM_NAME_MAX_LEN];
	char category[ITEM_CATEGORY_MAX_LEN];
};

/*
 * @brief check if the barcode of the entry matches the reference barcode
 * @param entry the entry to check
 * @param barcode the barcode to check against
 * @return true if the barcodes match, false otherwise
 */
bool matches_barcode(const void *entry, const void *barcode);

/*
 * @brief check if the category of the entry matches the reference category
 * @param entry the entry to check
 * @param category the category to check against
 * @return true if the categories match, false otherwise
 */
bool matches_category(const void *entry, const void *category);

/*
 * @brief check if the name of the entry matches the reference name
 * @param entry the entry to check
 * @param name the name to check against
 * @return true if the names match, false otherwise
 */
bool matches_name(const void *entry, const void *name);

/*
 * @brief check if the price of the entry matches the reference price
 * @param entry the entry to check
 * @param price the price to check against
 * @return true if the prices match, false otherwise
 */
void update_price(void *entry, const void *price);

/*
 * @brief check if the quantity of the entry matches the reference quantity
 * @param entry the entry to check
 * @param quantity the quantity to check against
 * @return true if the quantities match, false otherwise
 */
void update_quantity(void *entry, const void *quantity);

/*
 * @brief check if the expiry date of the entry matches the reference expiry
 * date
 * @param entry the entry to check
 * @param expiry_date the expiry date to check against
 * @return true if the expiry dates match, false otherwise
 */
void update_expiry_date(void *entry, const void *expiry_date);

/*
 * @brief apply a discount to the price of the entry using the discount value
 * provided
 * @param entry the entry to apply the discount to
 * @param discount the discount value to apply
 * discount is a float value between 0 and 1
 */
void discount_price(void *entry, const void *discount);

/*
 * @brief dump the information of the store item to the output stream
 * @param entry the entry to dump
 * @param out the output stream to dump the entry to
 */
void dump_store_item_info(const void *entry, FILE *out);
