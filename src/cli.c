#include "cli.h"

#include "database.h"
#include "error.h"
#include "store_manager.h"

#include <ctype.h>
#include <inttypes.h>
#include <string.h>

#define CLI_MAX_CMD_LEN 128

// macro for parsing a string to a uintmax_t value in a given base
// should be only used fo parsing command arguments
#define CMD_PARSE_UINTMAX(arg, base)              \
	({                                            \
		const char *_arg = arg;                   \
		char *_endptr;                            \
		uintmax_t _val;                           \
		_val = strtoumax(_arg, &_endptr, base);   \
		if (_arg == _endptr || errno == ERANGE) { \
			return STATUS_ERROR;                  \
		}                                         \
		_val;                                     \
	})

// macro for parsing a string to a float value in a given base
// should be only used fo parsing command arguments
#define CMD_PARSE_FLOAT(arg)                      \
	({                                            \
		const char *_arg = arg;                   \
		char *_endptr;                            \
		float _val;                               \
		_val = strtof(_arg, &_endptr);            \
		if (_arg == _endptr || errno == ERANGE) { \
			return STATUS_ERROR;                  \
		}                                         \
		_val;                                     \
	})

struct cli_program *create_cli_program(void)
{
	struct cli_program *cli_prog = calloc(1, sizeof(struct cli_program));

	DIE(cli_prog == NULL, "Failed to allocate memory for cli_program");

	cli_prog->cmd_buffer = calloc(CLI_MAX_CMD_LEN, sizeof(char));

	DIE(cli_prog->cmd_buffer == NULL,
		"Failed to allocate memory for cmd_buffer");

	return cli_prog;
}

void destroy_cli_program(struct cli_program *cli_prog)
{
	if (cli_prog == NULL)
		return;
	close_database(cli_prog->db_mgr);
	free(cli_prog->cmd_buffer);
	free(cli_prog);
}

static char *ignore_leading_spaces(char *str)
{
	while (isspace(*str))
		++str;
	return str;
}

static char *strip(char *str)
{
	str[strcspn(str, "\n")] = '\0';
	return ignore_leading_spaces(str);
}

static char *get_filename(struct cli_program *cli_prog)
{
	printf("Introduceti numele fisierului: ");
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return NULL;
	return strip(cli_prog->cmd_buffer);
}

static enum status cli_create_db(struct cli_program *cli_prog)
{
	if (cli_prog->db_mgr.db_file != NULL) {
		fprintf(stderr, "Baza de date deja deschisa\n");
		return STATUS_ERROR;
	}
	char *filename = get_filename(cli_prog);
	if (filename == NULL) {
		fprintf(stderr, "Fisier invalid\n");
		return STATUS_ERROR;
	}

	cli_prog->db_mgr = create_database(filename, sizeof(struct store_item));

	return STATUS_OK;
}

static enum status cli_load_db(struct cli_program *cli_prog)
{
	if (cli_prog->db_mgr.db_file != NULL) {
		fprintf(stderr, "Baza de date deja deschisa\n");
		return STATUS_ERROR;
	}
	char *filename = get_filename(cli_prog);
	if (filename == NULL) {
		fprintf(stderr, "Fisier invalid\n");
		return STATUS_ERROR;
	}

	cli_prog->db_mgr = open_database(filename, sizeof(struct store_item));

	return STATUS_OK;
}

static bool is_valid_date(const struct date *date)
{
	return date->day > 0 && date->day <= 31 && date->month > 0 &&
		   date->month <= 12 && date->year > 0;
}

static enum status cli_add_prod(struct cli_program *cli_prog)
{
	struct store_item item = { 0 };
	printf("Cod produs: ");
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;
	item.barcode = CMD_PARSE_UINTMAX(cli_prog->cmd_buffer, 10);

	printf("Nume produs: ");
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;
	strncpy(item.name, strip(cli_prog->cmd_buffer), ITEM_NAME_MAX_LEN);

	printf("Categorie: ");
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;
	strncpy(item.category, strip(cli_prog->cmd_buffer), ITEM_CATEGORY_MAX_LEN);

	printf("Pret: ");
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;
	item.price = CMD_PARSE_FLOAT(cli_prog->cmd_buffer);

	printf("Cantitate: ");
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;
	item.quantity = CMD_PARSE_UINTMAX(cli_prog->cmd_buffer, 10);

	printf("Data de expirare(zi luna an): ");
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;

	char *tok = strtok(cli_prog->cmd_buffer, " ");
	item.expiry_date.day = CMD_PARSE_UINTMAX(tok, 10);

	tok = strtok(NULL, " ");
	item.expiry_date.month = CMD_PARSE_UINTMAX(tok, 10);

	tok = strtok(NULL, " ");
	item.expiry_date.year = CMD_PARSE_UINTMAX(tok, 10);

	if (!is_valid_date(&item.expiry_date)) {
		fprintf(stderr, "Data de expirare invalida\n");
		return STATUS_ERROR;
	}

	return append_entry(cli_prog->db_mgr, &item);
}

static enum status cli_update_prod(struct cli_program *cli_prog)
{
	printf("Introduceti codul produsului: ");
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;

	uintmax_t barcode = CMD_PARSE_UINTMAX(cli_prog->cmd_buffer, 10);

	printf("Alegeti ce doriti sa actualizati:\n"
		   "1. Pret\n"
		   "2. Cantitate\n"
		   "3. Data de expirare\n"
		   "Introduceti comanda: ");

	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;

	uintmax_t cmd = CMD_PARSE_UINTMAX(cli_prog->cmd_buffer, 10);

	switch (cmd) {
	case 1: {
		printf("Introduceti noul pret: ");
		if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
			return STATUS_ERROR;

		float price = CMD_PARSE_FLOAT(cli_prog->cmd_buffer);

		return update_entries(cli_prog->db_mgr, &barcode, matches_barcode,
							  &price, update_price);
	}
	case 2: {
		printf("Introduceti noua cantitate: ");
		if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
			return STATUS_ERROR;

		uintmax_t quantity = CMD_PARSE_UINTMAX(cli_prog->cmd_buffer, 10);

		return update_entries(cli_prog->db_mgr, &barcode, matches_barcode,
							  &quantity, update_quantity);
	}
	case 3: {
		printf("Introduceti noua data de expirare(zi luna an): ");
		if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
			return STATUS_ERROR;

		char *tok = strtok(cli_prog->cmd_buffer, " ");
		struct date expiry_date = { 0 };
		expiry_date.day = CMD_PARSE_UINTMAX(tok, 10);

		tok = strtok(NULL, " ");
		expiry_date.month = CMD_PARSE_UINTMAX(tok, 10);

		tok = strtok(NULL, " ");
		expiry_date.year = CMD_PARSE_UINTMAX(tok, 10);

		if (!is_valid_date(&expiry_date)) {
			fprintf(stderr, "Data de expirare invalida\n");
			return STATUS_ERROR;
		}

		return update_entries(cli_prog->db_mgr, &barcode, matches_barcode,
							  &expiry_date, update_expiry_date);
	}
	default:
		printf("Comanda invalida\n");
		return STATUS_ERROR;
	}

	return STATUS_OK;
}

static inline bool is_valid_percentage(float discount)
{
	return discount >= 0 && discount <= 100;
}

static enum status cli_update_cat(struct cli_program *cli_prog)
{
	printf("Introduceti categoria: ");
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;
	char category[ITEM_CATEGORY_MAX_LEN];
	strncpy(category, strip(cli_prog->cmd_buffer), ITEM_CATEGORY_MAX_LEN);

	printf("Introduceti discount-ul(%%): ");
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;
	float discount = CMD_PARSE_FLOAT(cli_prog->cmd_buffer);

	if (!is_valid_percentage(discount)) {
		printf("Discount invalid\n");
		return STATUS_ERROR;
	}

	discount /= 100;

	return update_entries(cli_prog->db_mgr, category, matches_category,
						  &discount, discount_price);
}

static enum status cli_delete_prod(struct cli_program *cli_prog)
{
	printf("Introduceti codul produsului: ");
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;

	uintmax_t barcode = CMD_PARSE_UINTMAX(cli_prog->cmd_buffer, 10);

	enum status status =
		remove_unique_entry(cli_prog->db_mgr, &barcode, matches_barcode);
	if (status == STATUS_ERROR)
		printf("Produsul nu a fost gasit\n");
	return status;
}

static enum status cli_gen_total_report(struct cli_program *cli_prog)
{
	char *filename = get_filename(cli_prog);
	FILE *out = fopen(filename, "w");
	if (out == NULL) {
		fprintf(stderr, "Eroare la deschiderea fisierului\n");
		return STATUS_ERROR;
	}

	dump_database(cli_prog->db_mgr, dump_store_item_info, NULL, NULL, out);
	return STATUS_OK;
}

static enum status cli_gen_category_report(struct cli_program *cli_prog)
{
	char *filename = get_filename(cli_prog);
	FILE *out = fopen(filename, "w");
	if (out == NULL) {
		fprintf(stderr, "Eroare la deschiderea fisierului\n");
		return STATUS_ERROR;
	}

	printf("Introduceti categoria: ");
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;
	char *category = strip(cli_prog->cmd_buffer);
	dump_database(cli_prog->db_mgr, dump_store_item_info, (void *)category,
				  matches_category, out);
	return STATUS_OK;
}

static enum status cli_find_prod(struct cli_program *cli_prog)
{
	printf("Introduceti numele produsului: ");
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;
	char *name = strip(cli_prog->cmd_buffer);
	dump_database(cli_prog->db_mgr, dump_store_item_info, (void *)name,
				  matches_name, stdout);
	return STATUS_OK;
}

static enum status cli_exit(struct cli_program *cli_prog)
{
	(void)cli_prog;
	return STATUS_EXIT;
}

enum cli_ops {
	CLI_CREATE_DB = 1,
	CLI_LOAD_DB,
	CLI_ADD_PRODUCT,
	CLI_UPDATE_PRODUCT,
	CLI_UPDATE_CATEGORY,
	CLI_DELETE_PRODUCT,
	CLI_GEN_TOTAL_REPORT,
	CLI_GEN_CATEGORY_REPORT,
	CLI_FIND_PRODUCT,
	CLI_EXIT,
	CLI_MAX_OPS
};

typedef enum status (*cli_op_func)(struct cli_program *);

typedef struct {
	const char *info;
	cli_op_func func;
} cli_op_t;

static const cli_op_t cli_ops[] = {
	[CLI_CREATE_DB] = { "Creaza baza de date(fisierul binar)", cli_create_db },
	[CLI_LOAD_DB] = { "Incarca o baza de date existenta(fisierul binar)",
					  cli_load_db },
	[CLI_ADD_PRODUCT] = { "Adauga un produs", cli_add_prod },
	[CLI_UPDATE_PRODUCT] = { "Actualizeaza un produs unic", cli_update_prod },
	[CLI_UPDATE_CATEGORY] = { "Aplicati discount unei categorii de produse",
							  cli_update_cat },
	[CLI_DELETE_PRODUCT] = { "Sterge un produs unic", cli_delete_prod },
	[CLI_GEN_TOTAL_REPORT] = { "Genereaza un raport total(fisier text)",
							   cli_gen_total_report },
	[CLI_GEN_CATEGORY_REPORT] = { "Genereaza un raport pentru o categorie(fisier text)",
								  cli_gen_category_report },
	[CLI_FIND_PRODUCT] = { "Gaseste un produs dupa nume(afisare pe ecran)",
						   cli_find_prod },
	[CLI_EXIT] = { "Iesire", cli_exit }
};

// static void clrscr(void)
// {
// 	printf("\033[2J\033[1;1H");
// }

static void cli_print_menu(void)
{
	// clrscr();
	printf("-----------------\n");
	for (int i = 1; i < CLI_MAX_OPS; ++i)
		printf("%d. %s\n", i, cli_ops[i].info);
	printf("-----------------\n");
}

enum status cli_process_next_op(struct cli_program *cli_prog)
{
	cli_print_menu();
	printf("Introduceti comanda: ");
	uintmax_t cmd;
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;

	cmd = CMD_PARSE_UINTMAX(cli_prog->cmd_buffer, 10);

	if (cmd != CLI_CREATE_DB && cmd != CLI_LOAD_DB &&
		cli_prog->db_mgr.db_file == NULL) {
		fprintf(stderr, "Nu exista o baza de date deschisa\n");
		return STATUS_ERROR;
	}

	if (cmd < 1 || cmd >= CLI_MAX_OPS) {
		fprintf(stderr, "Comanda invalida\n");
		return STATUS_ERROR;
	}

	return cli_ops[cmd].func(cli_prog);
}
