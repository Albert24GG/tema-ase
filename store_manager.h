#pragma once

#include <stddef.h>
#include <stdint.h>

#define MAX_ITEM_NAME 64
#define MAX_CATEGORY_NAME 64

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
	char name[MAX_ITEM_NAME];
	char category[MAX_CATEGORY_NAME];
};
