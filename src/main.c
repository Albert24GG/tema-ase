#include "cli.h"

int main(void)
{
	struct cli_program *cli_prog = create_cli_program();
	while (cli_process_next_op(cli_prog) != STATUS_EXIT)
		;

	destroy_cli_program(cli_prog);
}
