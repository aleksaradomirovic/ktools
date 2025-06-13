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

#include "vdisk/part.h"

#include <string.h>
#include <strings.h>

static enum part_mode {
    PART_NONE = 0,
    PART_FORMAT,
    PART_VERIFY,
    PART_IDENTIFY,
} part_mode = PART_NONE;

static part_table_type_t part_type = PART_TABLE_UNKNOWN;

static const struct { part_table_type_t type; char *name; } PART_TYPE_LOOKUP[] = {
    { PART_TABLE_GPT, "GPT" },
    { -1, NULL }
};

static const struct argp_option args_options[] = {
    { 0 }
};

static int args_parser(int key, char *arg, struct argp_state *state) {
    if(mode != mode_part) {
        return ARGP_ERR_UNKNOWN;
    }

    switch(key) {
        case ARGP_KEY_ARG:
            if(state->arg_num == 0) {
                if(strcmp(arg, "format") == 0) {
                    part_mode = PART_FORMAT;
                } else if(strcmp(arg, "verify") == 0) {
                    part_mode = PART_VERIFY;
                } else if(strcmp(arg, "identify") == 0) {
                    part_mode = PART_IDENTIFY;
                } else {
                    argp_error(state, "unknown part mode '%s'", arg);
                }
            } else if(state->arg_num == 1 && (part_mode == PART_FORMAT || part_mode == PART_VERIFY)) {
                for(size_t i = 0;; i++) {
                    if(PART_TYPE_LOOKUP[i].name == NULL) {
                        argp_error(state, "unknown partition table type '%s'", arg);
                    }

                    if(strcasecmp(PART_TYPE_LOOKUP[i].name, arg) == 0) {
                        part_type = PART_TYPE_LOOKUP[i].type;
                        break;
                    }
                }
            } else {
                return ARGP_ERR_UNKNOWN;
            }
            break;
        case ARGP_KEY_END:
            if(part_mode == PART_NONE) {
                argp_error(state, "not enough arguments");
            } else if(part_mode == PART_FORMAT || part_mode == PART_VERIFY) {
                if(part_type <= 0) {
                    argp_error(state, "not enough arguments");
                }
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

const struct argp part_args_info = {
    .options = args_options,
    .parser  = args_parser,
    .args_doc = "DISK part format TYPE\n" "DISK part verify TYPE\n" "DISK part identify",
};

int mode_part() {
    switch(part_mode) {
        case PART_FORMAT:
            return disk_format(disk, part_type);
        case PART_VERIFY:
            return disk_verify(disk, part_type);
        case PART_IDENTIFY: {
                part_table_type_t type = disk_identify(disk);
                if(type <= 0) {
                    return -1;
                }

                for(size_t i = 0;; i++) {
                    if(PART_TYPE_LOOKUP[i].name == NULL) {
                        return -1;
                    }

                    if(PART_TYPE_LOOKUP[i].type == type) {
                        fprintf(stdout, "%s\n", PART_TYPE_LOOKUP[i].name);
                        return 0;
                    }
                }
            }
        default:
            errno = ENOTSUP;
            return -1;
    }
}
