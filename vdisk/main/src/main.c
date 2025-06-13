/*
 * Copyright (C) 2025  Aleksa Radomirovic
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "args.h"
#include "vdisk.h"

#include <errno.h>
#include <error.h>
#include <string.h>

disk_t disk = NULL;
int (*mode)() = NULL;

static const struct argp_option args_options[] = {
    { 0 }
};

static int args_parser(int key, char *arg, struct argp_state *state) {
    switch(key) {
        case ARGP_KEY_ARG:
            if(state->arg_num == 0) {
                disk = disk_open(arg);
                if(disk == NULL) {
                    argp_failure(state, errno, errno, "failed to open disk file");
                }
            } else if(state->arg_num == 1) {
                if(strcmp(arg, "part") == 0) {
                    mode = mode_part;
                } else {
                    argp_error(state, "unknown mode '%s'", arg);
                }
            } else {
                return ARGP_ERR_UNKNOWN;
            }
            break;
        case ARGP_KEY_END:
            if(disk == NULL || mode == NULL) {
                argp_error(state, "not enough arguments");
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static const struct argp_child args_children[] = {
    { &part_args_info, ARGP_IN_ORDER, "Partitioning options:", 1 },
    { 0 }
};

static const struct argp args_info = {
    .options  = args_options,
    .parser   = args_parser,
    .children = args_children,
};

int main(int argc, char **argv) {
    log_vdisk = stdlog;

    int status = argp_parse(&args_info, argc, argv, ARGP_IN_ORDER, NULL, NULL);
    if(status != 0) {
        return status;
    }

    status = mode();

    if(disk_close(disk) != 0) {
        error(errno, errno, "failed to close disk");
    }
    return status;
}
