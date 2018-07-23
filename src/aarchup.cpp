#include <libnotify/notify.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <stdbool.h>
#include <memory>
#include <iostream>
#include "CliWrapper.hpp"

#define AUR_HEADER "AUR updates:\n"
#define STREQ !strcmp
#define VERSION_NUMBER "2.0.0"

/* Prints the help. */
int PrintHelp(char *name) {
    std::cout << "Usage: aarchup [options]\n\n"
                 "Options:\n"
                 "          --command|-c [value]        Set the command which gives out the list of updates.\n"
                 "                                      The default is /usr/bin/checkupdates\n"
                 "          --icon|-p [value]           Shows the icon, whose path has been given as value, in the notification.\n"
                 "                                      By default no icon is shown.\n"
                 "          --maxentries|-m [value]     Set the maximum number of packages which shall be displayed in the notification.\n"
                 "                                      The default value is 30.\n"
                 "          --timeout|-t [value]        Set the timeout after which the notification disappers in seconds.\n"
                 "                                      The default value is 3600 seconds, which is 1 hour.\n"
                 "          --uid|-i [value]            Set the uid of the process.\n"
                 "                                      The default is to keep the uid of the user who started aarchup.\n"
                 "                                      !!You should change this if root is executing aarchup!!\n"
                 "          --urgency|-u [value]        Set the libnotify urgency-level. Possible values are: low, normal and critical.\n"
                 "                                      The default value is normal. With changing this value you can change the color of the notification.\n"
                 "          --loop-time|-l [value]      Using this the program will loop indefinitely. Just use this if you won't use cron.\n"
                 "                                      The value should be the number of minutes from each update check, if none is informed 60 will be used.\n"
                 "                                      For more information on it check man.\n"
                 "          --help                      Prints this help.\n"
                 "          --version                   Shows the version.\n"
                 "          --aur                       Check aur for new packages too. Will need auracle installed.\n"
                 "          --debug|-d                  Print debug info.\n"
                 "          --ftimeout|-f [value]       Program will manually enforce timeout for closing notification.\n"
                 "                                      Do NOT use with --timeout, if --timeout works or without --loop-time [value].\n"
                 "                                      The value for this option should be in minutes.\n"
                 "          --pkg-no-ignore             If this flag is set will not use the IgnorePkg variable from pacman.conf. Without the flag will ignore those\n"
                 "                                      packages\n"
                 "\nMore information can be found in the manpage.\n";
    exit(0);
}

/* Prints the version. */
int print_version() {
    std::cout << "aarchup " VERSION_NUMBER
                 "\nCopyright 2018 Leonidas Spyropoulos <artafinde@gmail.com>\n"
                 "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n"
                 "This is free software, you are free to change and redistribute it.\n"
                 "There is NO WARRANTY, to the extent permitted by law.\n";
    exit(0);
}


int main(int argc, char **argv) {
    unsigned int timeout = 3600 * 1000;
    /* Restricts the number of packages which should be included in the desktop notification.*/
    int max_number_out = 30;
    unsigned int loop_time = 3600;
    unsigned int manual_timeout = 0;
    const char *command = "/usr/bin/checkupdates";
    gchar *icon = NULL;
    NotifyUrgency urgency = NOTIFY_URGENCY_NORMAL;
    bool will_loop = FALSE;
    bool debug = FALSE;
    static int help_flag = 0;
    static int version_flag = 0;
    static int aur = 0;
    static int ignore_pkg_flag = 1;

    /* Parse commandline options */
    const char *const short_opts = "c:p:m:t:i:u:l:df:";
    const option long_opts[] = {
            {"command",       required_argument, nullptr,          'c'},
            {"icon",          required_argument, nullptr,          'p'},
            {"maxentries",    required_argument, nullptr,          'm'},
            {"timeout",       required_argument, nullptr,          't'},
            {"uid",           required_argument, nullptr,          'i'},
            {"urgency",       required_argument, nullptr,          'u'},
            {"loop-time",     required_argument, nullptr,          'l'},
            {"help",          no_argument,       &help_flag,       1},
            {"version",       no_argument,       &version_flag,    1},
            {"aur",           no_argument,       &aur,             1},
            {"ftimeout",      required_argument, nullptr,          'f'},
            {"debug",         no_argument,       nullptr,          'd'},
            {"pkg-no-ignore", no_argument,       &ignore_pkg_flag, 0},
            {nullptr, 0,                         nullptr,          0},
    };

    while (true) {
        int option_index = 0;
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, &option_index);
        if (-1 == opt) {
            break;
        }
        /* Short opts */
        switch (opt) {
            case 0:
                if (long_opts[option_index].flag) {
                    break;
                }
                if (debug) {
                    printf("DEBUG(error): erroropt: option %s", long_opts[option_index].name);
                    if (optarg) {
                        printf(" with args %s", optarg);
                    }
                    printf("\n");
                }
                break;
            case 'v':
                print_version();
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
                max_number_out = std::stoi(optarg);
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
            case 'h': // -h or --help
            case '?': // Unrecognized option
            default:
                PrintHelp(argv[0]);
                break;
        }
    }

    std::unique_ptr<CliWrapper> checkUpdatesCmd = std::make_unique<CliWrapper>("/bin/checkupdates");
    const std::string &checkUpdateOut = checkUpdatesCmd->execute();
    std::string aurHelperOut;
    if (aur) {
        std::cout << "Checking AUR" << std::endl;
        std::unique_ptr<CliWrapper> aurHelperCmd = std::make_unique<CliWrapper>("/bin/auracle sync");
        aurHelperOut = aurHelperCmd->execute();
    }
    if (!checkUpdateOut.empty() || (!aurHelperOut.empty())) {
        notify_init("Aarchup");
        std::string finalOut;
        if (aurHelperOut.empty()) {
            finalOut = checkUpdateOut;
        } else if (checkUpdateOut.empty()) {
            finalOut = "AUR updates:\n"+aurHelperOut;
        } else {
            finalOut = checkUpdateOut+"\nAUR updates:\n"+aurHelperOut;
        }
        NotifyNotification *n = notify_notification_new("New updates for Arch Linux available!",
                                                        finalOut.c_str(), nullptr);
        notify_notification_set_timeout(n, 5000);
        notify_notification_set_urgency(n, urgency);
        if (!notify_notification_show(n, nullptr)) {
            std::cerr << "Failed to show notification" << std::endl;
            return -1;
        }
    } else {
        std::cout << "No updates found" << std::endl;
    }

    return 0;
}
