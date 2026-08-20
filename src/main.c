#include <stdlib.h>

#include "ping.h"
#include "config.h"
#include "check.h"

int	main(int argc, char **argv)
{
	char*		host = NULL;
	t_config	flags = checking_arguments(argc, argv, &host);

	ping(host, flags);

	return EXIT_SUCCESS;
}
