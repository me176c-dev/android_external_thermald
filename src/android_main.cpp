/*
 * android_main.cpp: Thermal Daemon entry point tuned for Android
 *
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 or later as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 *
 * Author Name <Srinivas.Pandruvada@linux.intel.com>
 *
 * This is the main entry point for thermal daemon. This has main function
 * which parses command line arguments, setup logs and starts thermal
 * engine.
 */

#include "thermald.h"
#include "thd_preference.h"
#include "thd_engine.h"
#include "thd_engine_default.h"
#include "thd_parse.h"
#include <sys/file.h>

// for AID_* constants
#include <private/android_filesystem_config.h>

// poll mode
int thd_poll_interval = 4; //in seconds

#ifdef DETECT_THERMAL_ZONES
bool thd_ignore_default_control = false;
#endif

// SIGTERM & SIGINT handler
void sig_int_handler(int signum) {
	thd_engine->thd_engine_terminate();
	sleep(1);
	delete thd_engine;
	exit(EXIT_SUCCESS);
}

static void print_usage(FILE* stream, int exit_code) {
	fprintf(stream, "Usage:  thermald options [ ... ]\n");
	fprintf(stream, "  --help Display this usage information.\n"
			"  --version Print thermald version and exit\n"
			"  --test-mode Test Mode only: Allow non root user"
			"  --poll-interval Poll interval 0 to disable.\n"
			"  --exclusive-control Take over thermal control from kernel thermal driver.\n"
			"  --ignore-cpuid-check Ignore CPU ID check.\n"
			"  --config-file Configuration file to use other than the default config.\n");

	exit(exit_code);
}

int main(int argc, char *argv[]) {
	int c;
	int option_index = 0;
	bool test_mode = false;
	bool exclusive_control = false;
	bool ignore_cpuid_check = false;
	char *conf_file = NULL;

	const char* const short_options = "hvtp:eic:";
	static struct option long_options[] = {
			{ "help", no_argument, 0, 'h' },
			{ "version", no_argument, 0, 'v' },
			{ "test-mode", no_argument, 0, 't' },
			{ "poll-interval", required_argument, 0, 'p' },
			{ "exclusive-control", no_argument, 0, 'e' },
			{ "ignore-cpuid-check", no_argument, 0, 'i'},
			{ "config-file", required_argument, 0, 'c' },
			{ NULL, 0, NULL, 0 } };

	if (argc > 1) {
		while ((c = getopt_long(argc, argv, short_options, long_options,
				&option_index)) != -1) {
			switch (c) {
			case 'h':
				print_usage(stdout, 0);
				break;
			case 'v':
				fprintf(stdout, "1.7.1\n");
				exit(EXIT_SUCCESS);
				break;
			case 't':
				test_mode = true;
				break;
			case 'p':
				thd_poll_interval = atoi(optarg);
				break;
			case 'e':
				exclusive_control = true;
				break;
			case 'i':
				ignore_cpuid_check = true;
				break;
			case 'c':
				conf_file = optarg;
				break;
			case -1:
			case 0:
				break;
			default:
				break;
			}
		}
	}

	if (((getuid() != 0) && (getuid() != AID_SYSTEM)) && !test_mode) {
		thd_log_error("You do not have correct permissions to run thermald!\n");
		exit(EXIT_FAILURE);
	}

#ifdef GENERATE_CONFIG
	if (mkdir(TDRUNDIR, 0755) != 0) {
		if (errno != EEXIST) {
			fprintf(stderr, "Cannot create '%s': %s\n", TDRUNDIR,
					strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
#endif

	signal(SIGINT, sig_int_handler);
	signal(SIGTERM, sig_int_handler);

	thd_log_info(
			"Linux Thermal Daemon is starting: poll_interval %d :ex_control %d\n",
			thd_poll_interval, exclusive_control);

	if (thd_engine_create_default_engine(ignore_cpuid_check, exclusive_control,
			conf_file) != THD_SUCCESS) {
		exit(EXIT_FAILURE);
	}

#ifdef VALGRIND_TEST
	// lots of STL lib function don't free memory
	// when called with exit().
	// Here just run for some time and gracefully return.
	sleep(10);
	thd_engine->thd_engine_terminate();
	sleep(1);
	delete thd_engine;
#else
	pthread_exit(NULL);
#endif
	return 0;
}
