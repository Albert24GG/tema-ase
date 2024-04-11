#include "cli.h"

#include "database.h"
#include "error.h"
#include "store_manager.h"

#include <inttypes.h>

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

struct cli_program create_cli_program(void)
{
	struct cli_program *cli_prog = calloc(1, sizeof(struct cli_program));

	DIE(cli_prog == NULL, "Failed to allocate memory for cli_program");

	cli_prog->cmd_buffer = calloc(CLI_MAX_CMD_LEN, sizeof(char));

	DIE(cli_prog->cmd_buffer == NULL,
		"Failed to allocate memory for cmd_buffer");

	return *cli_prog;
}

void destroy_cli_program(struct cli_program *cli_prog)
{
	if (cli_prog == NULL)
		return;
	close_database(cli_prog->db_mgr);
	free(cli_prog->cmd_buffer);
	free(cli_prog);
}

static enum status cli_create_db(struct cli_program *cli_prog)
{
}

static enum status cli_load_db(struct cli_program *cli_prog)
{
}

static enum status cli_add_prod(struct cli_program *cli_prog)
{
}

static enum status cli_update_prod(struct cli_program *cli_prog)
{
}

static enum status cli_update_cat(struct cli_program *cli_prog)
{
}

static enum status cli_delete_prod(struct cli_program *cli_prog)
{
}

static enum status cli_gen_total_report(struct cli_program *cli_prog)
{
}

static enum status cli_gen_category_report(struct cli_program *cli_prog)
{
}

static enum status cli_find_prod(struct cli_program *cli_prog)
{
}

static enum status cli_exit(struct cli_program *cli_prog)
{
}

static enum cli_ops {
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

static const char *cli_menu[] = {
	[CLI_CREATE_DB] = "Creaza baza de date(fisierul binar)",
	[CLI_LOAD_DB] = "Incarca o baza de date existenat(fisierul binar)",
	[CLI_ADD_PRODUCT] = "Adauga un produs",
	[CLI_UPDATE_PRODUCT] = "Actualizeaza un produs",
	[CLI_UPDATE_CATEGORY] = "Actualizeaza o categorie de produse",
	[CLI_DELETE_PRODUCT] = "Sterge un produs",
	[CLI_GEN_TOTAL_REPORT] = "Genereaza un raport total",
	[CLI_GEN_CATEGORY_REPORT] = "Genereaza un raport pentru o categorie",
	[CLI_FIND_PRODUCT] = "Gaseste un produs",
	[CLI_EXIT] = "Iesire"
};

typedef enum status (*cli_op_func)(struct cli_program *);

static const cli_op_func cli_ops[] = {
	[CLI_CREATE_DB] = cli_create_db,
	[CLI_LOAD_DB] = cli_load_db,
	[CLI_ADD_PRODUCT] = cli_add_prod,
	[CLI_UPDATE_PRODUCT] = cli_update_prod,
	[CLI_UPDATE_CATEGORY] = cli_update_cat,
	[CLI_DELETE_PRODUCT] = cli_delete_prod,
	[CLI_GEN_TOTAL_REPORT] = cli_gen_total_report,
	[CLI_GEN_CATEGORY_REPORT] = cli_gen_category_report,
	[CLI_FIND_PRODUCT] = cli_find_prod,
	[CLI_EXIT] = cli_exit
};

static void cli_print_menu(void)
{
	for (int i = 1; i < CLI_MAX_OPS; ++i)
		printf("%d. %s\n", i, cli_menu[i]);
}

enum status cli_process_next_op(struct cli_program *cli_prog)
{
	cli_print_menu();
	printf("Introduceti comanda: ");
	uintmax_t cmd;
	if (fgets(cli_prog->cmd_buffer, CLI_MAX_CMD_LEN, stdin) == NULL)
		return STATUS_ERROR;

	cmd = CMD_PARSE_UINTMAX(cli_prog->cmd_buffer, 10);

	if (cmd < 1 || cmd >= CLI_MAX_OPS)
		return STATUS_ERROR;

	return cli_ops[cmd](cli_prog);
}
