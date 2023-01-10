#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#include "api.h"
#include "command.h"
#include "config.h"
#include "image.h"
#include "steammontage.h"
#include "version.h"
#include "whereami/whereami.h"

int
main (int argc, char *argv[])
{
	printf("steammontage %s - "
		"https://github.com/grizzlybearington/steammontage\n\n", SM_VERSION);

	struct options opts = {
		.cfg = CONFIG_DEFAULTS,
		.help = 0,
		.version = 0,
	};

	if (!parse_args(&opts, argc, argv)) {
		print_cli_failure(argv[0]);
		return 0;
	}

	if (opts.help) {
		print_help(argv[0]);
		return 1;
	}

	if (opts.version) {
		return 1;
	}

	struct runningdir runningdir = {
		.dirpath = NULL,
		.dirlen = 0
	};

	if (get_running_dir(&runningdir) == 0) {
		goto end;
	}

	if (parse_config(runningdir.dirpath, &opts.cfg) == -1) {
		goto end;
	}

	if (validate_input(&opts.cfg) == -1) {
		goto end;
	}

	init_curl();

	if (create_montage(&opts.cfg, runningdir) == -1) {
		goto end;
	}
	printf("Job's done!\n");

end:
	free_curl();
	free(runningdir.dirpath);
	return 1;
}
