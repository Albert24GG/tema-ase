#pragma once

#include "database.h"
#include "error.h"

struct cli_program {
	struct db_manager db_mgr;
	char *cmd_buffer;
};

/*
 * @brief Create a new CLI program.
 * @return The CLI program.
 */
struct cli_program *create_cli_program(void);

/*
 * @brief Destroy a CLI program.
 * @param cli_prog The CLI program.
 */
void destroy_cli_program(struct cli_program *cli_prog);

/*
 * @brief Process the next operation in the CLI program.
 * @param cli_prog The CLI program.
 * @return The status of the operation.
 */
enum status cli_process_next_op(struct cli_program *cli_prog);
