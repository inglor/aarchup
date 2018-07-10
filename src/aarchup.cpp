#include <libnotify/notify.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <stdbool.h>
#include "CliWrapper.h"

#define AUR_HEADER "AUR updates:\n"
#define STREQ !strcmp
#define VERSION_NUMBER "2.0.0"

/* Prints the help. */
int print_help(char *name) {
    printf("Usage: %s [options]\n\n", name);
    printf("Options:\n");
    printf("          --command|-c [value]        Set the command which gives out the list of updates.\n");
    printf("                                      The default is /usr/bin/checkupdates\n");
    printf("          --icon|-p [value]           Shows the icon, whose path has been given as value, in the notification.\n");
    printf("                                      By default no icon is shown.\n");
    printf("          --maxentries|-m [value]     Set the maximum number of packages which shall be displayed in the notification.\n");
    printf("                                      The default value is 30.\n");
    printf("          --timeout|-t [value]        Set the timeout after which the notification disappers in seconds.\n");
    printf("                                      The default value is 3600 seconds, which is 1 hour.\n");
    printf("          --uid|-i [value]            Set the uid of the process.\n");
    printf("                                      The default is to keep the uid of the user who started aarchup.\n");
    printf("                                      !!You should change this if root is executing aarchup!!\n");
    printf("          --urgency|-u [value]        Set the libnotify urgency-level. Possible values are: low, normal and critical.\n");
    printf("                                      The default value is normal. With changing this value you can change the color of the notification.\n");
    printf("          --loop-time|-l [value]      Using this the program will loop indefinitely. Just use this if you won't use cron.\n");
    printf("                                      The value should be the number of minutes from each update check, if none is informed 60 will be used.\n");
    printf("                                      For more information on it check man.\n");
    printf("          --help                      Prints this help.\n");
    printf("          --version                   Shows the version.\n");
    printf("          --aur                       Check aur for new packages too. Will need auracle installed.\n");
    printf("          --debug|-d                  Print debug info.\n");
    printf("          --ftimeout|-f [value]       Program will manually enforce timeout for closing notification.\n");
    printf("                                      Do NOT use with --timeout, if --timeout works or without --loop-time [value].\n");
    printf("                                      The value for this option should be in minutes.\n");
    printf("          --pkg-no-ignore             If this flag is set will not use the IgnorePkg variable from pacman.conf. Without the flag will ignore those\n");
    printf("                                      packages\n");
    printf("\nMore informations can be found in the manpage.\n");
    exit(0);
}

/* Prints the version. */
int print_version() {
    printf("aarchup %s\n", VERSION_NUMBER);
    printf("Copyright 2011 aericson <de.ericson@gmail.com>\n");
    printf("Copyright 2010 Rorschach <r0rschach@lavabit.com>,\n2011 Andrew Kravchuk <awkravchuk@gmail.com>\n");
    printf("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
    exit(0);
}


int main(int argc, char **argv) {
    unsigned int timeout = 3600 * 1000;
    /* Restricts the number of packages which should be included in the desktop notification.*/
    unsigned int max_number_out = 30;
    unsigned int loop_time = 3600;
    unsigned int manual_timeout = 0;
    const char *command = "/usr/bin/checkupdates";
    gchar *icon = NULL;
    NotifyUrgency urgency;
    bool will_loop = FALSE, debug = FALSE;

    /* Parse commandline options */
    int c;
    while (true) {
        /* Long opts */
        static struct option long_options[] = {
                {"command",       required_argument, 0, 'c'},
                {"icon",          required_argument, 0, 'p'},
                {"maxentries",    required_argument, 0, 'm'},
                {"timeout",       required_argument, 0, 't'},
                {"uid",           required_argument, 0, 'i'},
                {"urgency",       required_argument, 0, 'u'},
                {"loop-time",     required_argument, 0, 'l'},
                {"help",          no_argument,       0, 1},
                {"version",       no_argument,       0, 1},
                {"aur",           no_argument,       0, 1},
                {"ftimeout",      required_argument, 0, 'f'},
                {"debug",         no_argument,       0, 'd'},
                {"pkg-no-ignore", no_argument,       0, 0},
                {0, 0,                               0, 0},
        };
        int option_index = 0;
        c = getopt_long(argc, argv, "c:p:m:t:i:u:l:df:", long_options, &option_index);
        if (c == -1) {
            break;
        }
        /* Short opts */
        switch (c) {
            case 0:
                if (long_options[option_index].flag) {
                    break;
                }
                break;
            case 'c':
                command = optarg;
                break;
            case 'p':
                icon = optarg;
                break;
            case 'm':
                if (!isdigit(optarg[0])) {
                    printf("--maxentries argument needs to be a number\n");
                    exit(1);
                }
                max_number_out = atoi(optarg);
                break;
            case 't':
                if (!isdigit(optarg[0])) {
                    printf("--timeout argument needs to be a number\n");
                    exit(1);
                }
                timeout = atoi(optarg) * 1000;
                break;
            case 'i':
                if (!isdigit(optarg[0])) {
                    printf("--uid argument needs to be a number\n");
                    exit(1);
                }
                if (setuid(atoi(optarg)) != 0) {
                    printf("Couldn't change to the given uid!\n");
                    exit(1);
                }
                break;
            case 'u':
                if (strcmp(optarg, "low") == 0) {
                    urgency = NOTIFY_URGENCY_LOW;
                } else if (strcmp(optarg, "normal") == 0) {
                    urgency = NOTIFY_URGENCY_NORMAL;
                } else if (strcmp(optarg, "critical") == 0) {
                    urgency = NOTIFY_URGENCY_CRITICAL;
                } else {
                    printf("--urgency has to be 'low', 'normal' or 'critical\n");
                    exit(1);
                }
                break;
            case 'l':
                will_loop = TRUE;
                if (!isdigit(optarg[0])) {
                    printf("--loop-time argument needs to be a number\n");
                    exit(1);
                }
                loop_time = atoi(optarg) * 60;
                break;
            case 'd':
                debug = TRUE;
                break;
            case 'f':
                if (!isdigit(optarg[0])) {
                    printf("--ftimeout argument needs to be a number\n");
                    exit(1);
                }
                manual_timeout = atoi(optarg) * 60;
                if (!will_loop) {
                    printf("--ftimeout can't be used without or before --loop-time\n");
                    exit(1);
                }
                if (manual_timeout > loop_time) {
                    printf("Please set a value for --ftimeout that is higher than --loop-time\n");
                    exit(1);
                }
                break;
            case '?':
                print_help(argv[0]);
                break;
            default:
                exit(1);
        }
    }

    auto *cliWrapper = new CliWrapper("/bin/checkupdates");
    notify_init("Sample");
    NotifyNotification *n = notify_notification_new("New updates for Arch Linux available!",
                                                    cliWrapper->execute().c_str(), nullptr);
    notify_notification_set_timeout(n, 5000);
    if (!notify_notification_show(n, nullptr)) {
        std::cerr << "show has failed" << std::endl;
        return -1;
    }
    return 0;
}
