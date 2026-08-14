#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ping.h"
#include "config.h"

static void	print_help_and_exit(int exit_code)
{
	printf("\
Usage\n\
  ping [options] <destination>\n\
\n\
Options:\n\
  -v            Verbose mode\n\
  -?, --help    Print this help message\n");
	exit(exit_code);
}

static t_config	checking_arguments(int argc, char **argv, char **host)
{
	if (argc == 1)
	{
		printf("ping: usage error: Destination address required\n");
		exit(EXIT_FAILURE);
	}

	int			nb_host = 0;
	t_config	flags = {
		.verbose = 0,
		.quiet = 0
	};
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-v") == 0)
			flags.verbose = 1;
		else if (strcmp(argv[i], "-q") == 0)
			flags.quiet = 1;
		else if (strcmp(argv[i], "-?") == 0 || strcmp(argv[i], "--help") == 0)
			print_help_and_exit(EXIT_SUCCESS);
		else if (strncmp(argv[i], "-", 1) == 0)
		{
			printf("ping: invalid option: %s is not supported\n\n", argv[i]);
			print_help_and_exit(EXIT_FAILURE);
		}
		else
		{
			*host = argv[i];
			nb_host++;
		}
	}

	if (nb_host == 0)
	{
		printf("ping: usage error: Destination address required\n");
		exit(EXIT_FAILURE);
	}
	else if (nb_host != 1)
	{
		printf("ping: usage error: More than one destination\n");
		exit(EXIT_FAILURE);
	}

	return flags;
}

int	main(int argc, char **argv)
{
	char*		host = NULL;
	t_config	flags = checking_arguments(argc, argv, &host);

	ping(host, flags);

	return 0;
}
