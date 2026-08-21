#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

static inline void	print_usage_and_exit(void)
{
	printf("Usage: ping [qvV?] [-c NUMBER] [--count=NUMBER] [-i NUMBER] [--interval=NUMBER] [-s NUMBER] [--size=NUMBER] [--verbose] [--quiet] [--help] [--usage] [--version] HOST ...\n");
	exit(EXIT_SUCCESS);
}

static inline void	print_version_and_exit(void)
{
	printf("ping (GNU inetutils reimplementation) 2.5\n\
License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
\n\
Written by Victor Groux\n");
	exit(EXIT_SUCCESS);
}

static inline void	print_help_and_exit(int exit_code)
{
	printf("\
Usage\n\
  ping [OPTIONS...] HOST ...\n\
\n\
Options:\n\
  -v, --verbose        Verbose output\n\
  -q, --quiet          Quiet output\n\
  -c, --count=NUMBER   Stop after sending NUMBER packets\n\
  -i, --interval=N     Wait N seconds between to packet\n\
  -s, --size=NUMBER    Send NUMBER data octets\n\
      --usage          Print usage\n\
  -?, --help           Print this help message\n");
	exit(exit_code);
}

static inline void	print_invalid_and_exit(char* arg, char *err)
{
	fprintf(stderr, "ping: invalid value (`%s' near `%s')\n", arg, err);
	printf("Try 'ping --help' or 'ping --usage' for more information.\n");
	exit(EXIT_FAILURE);
}

static inline void	print_no_argument_and_exit(char *option)
{
	fprintf(stderr, "ping: option '%s' requires an argument\n", option);
	printf("Try 'ping --help' or 'ping --usage' for more information.\n");
	exit(EXIT_FAILURE);
}

static inline long	str2int(char *str)
{
	errno = 0;
	char	*err;
	long	res = strtol(str, &err, 10);

	if (errno || err[0] != '\0')
		print_invalid_and_exit(str, err);
	else if (res < 0)
	{
		fprintf(stderr, "ping: option value too big: %ld\n", res);
		exit(EXIT_FAILURE);
	}

	return res;
}

static inline uint8_t	checking_for_int(char *option, char *arg)
{
	if (arg[0] == '\0')
		print_no_argument_and_exit(option);

	return str2int(arg);
}

t_config	checking_arguments(int argc, char **argv, char **host)
{
	if (argc == 1)
	{
		fprintf(stderr, "ping: usage error: Destination address required\n");
		exit(EXIT_FAILURE);
	}

	int			nb_host = 0;
	t_config	flags = {
		.verbose = 0,
		.quiet = 0,
		.identifier = (uint16_t)getpid(),
		.payload_size = 56,
		.count = 0,
		.interval = 0
	};

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
			flags.verbose = 1;
		else if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quiet") == 0)
			flags.quiet = 1;
		else if (strcmp(argv[i], "-i") == 0)
		{
			int	j = i;
			if (argv[++i] == NULL)
				print_no_argument_and_exit("-i");
			flags.interval = checking_for_int(argv[j], argv[i]);
		}
		else if (strncmp(argv[i], "--interval=", 11) == 0)
			flags.interval = checking_for_int(argv[i], (argv[i]) + 11);
		else if (strncmp(argv[i], "--interval", 10) == 0)
		{
			int	j = i;
			if (argv[++i] == NULL)
				print_no_argument_and_exit("--interval");
			flags.interval = checking_for_int(argv[j], argv[i]);
		}
		else if (strcmp(argv[i], "-s") == 0)
		{
			int	j = i;
			if (argv[++i] == NULL)
				print_no_argument_and_exit("-s");
			flags.payload_size = checking_for_int(argv[j], argv[i]);
		}
		else if (strncmp(argv[i], "--size=", 7) == 0)
			flags.payload_size = checking_for_int(argv[i], (argv[i]) + 7);
		else if (strncmp(argv[i], "--size", 6) == 0)
		{
			int	j = i;
			if (argv[++i] == NULL)
				print_no_argument_and_exit("--size");
			flags.payload_size = checking_for_int(argv[j], argv[i]);
		}
		else if (strcmp(argv[i], "-c") == 0)
		{
			int	j = i;
			if (argv[++i] == NULL)
				print_no_argument_and_exit("-c");
			flags.count = checking_for_int(argv[j], argv[i]);
		}
		else if (strncmp(argv[i], "--count=", 8) == 0)
			flags.count = atoi((argv[i]) + 8);
		else if (strncmp(argv[i], "--count", 7) == 0)
		{
			int	j = i;
			if (argv[++i] == NULL)
				print_no_argument_and_exit("--count");
			flags.count = checking_for_int(argv[j], argv[i]);
		}
		else if (strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--version") == 0)
			print_version_and_exit();
		else if (strcmp(argv[i], "-?") == 0 || strcmp(argv[i], "--help") == 0)
			print_help_and_exit(EXIT_SUCCESS);
		else if (strcmp(argv[i], "--usage") == 0)
			print_usage_and_exit();
		else if (strncmp(argv[i], "-", 1) == 0)
		{
			fprintf(stderr, "ping: invalid option: %s is not supported\n\n", argv[i]);
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
		fprintf(stderr, "ping: usage error: Destination address required\n");
		exit(EXIT_FAILURE);
	}

	return flags;
}