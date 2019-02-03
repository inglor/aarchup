#include <ctype.h>
#include <getopt.h>
#include <libnotify/notify.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Log.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include "CliWrapper.hh"

#define AUR_HEADER "AUR updates:\n"
#define VERSION_NUMBER "2.1.0"

/* Prints the help. */
int print_help() {
  std::cout
      << "Usage: aarchup [options]\n\n"
         "Options:\n"
         "          --command|-c [value]        Set the command which gives "
         "out the list of updates.\n"
         "                                      The default is "
         "/usr/bin/checkupdates\n"
         "          --icon|-p [value]           Shows the icon, whose path has "
         "been given as value, in the notification.\n"
         "                                      By default no icon is shown.\n"
         "          --maxentries|-m [value]     Set the maximum number of "
         "packages which shall be displayed in the notification.\n"
         "                                      The default value is 30.\n"
         "          --timeout|-t [value]        Set the timeout after which "
         "the notification disappears in seconds.\n"
         "                                      The default value is 3600 "
         "seconds, which is 1 hour.\n"
         "          --uid|-i [value]            Set the uid of the process.\n"
         "                                      The default is to keep the uid "
         "of the user who started aarchup.\n"
         "                                      !!You should change this if "
         "root is executing aarchup!!\n"
         "          --urgency|-u [value]        Set the libnotify "
         "urgency-level. Possible values are: low, normal and critical.\n"
         "                                      The default value is normal. "
         "With changing this value you can change the color of the "
         "notification.\n"
         "          --loop-time|-l [value]      Using this the program will "
         "loop indefinitely. Just use this if you won't use cron.\n"
         "                                      The value should be the number "
         "of minutes from each update check, if none is informed 60 will be "
         "used.\n"
         "                                      For more information on it "
         "check man.\n"
         "          --help|-h                   Prints this help.\n"
         "          --version|-v                Shows the version.\n"
         "          --aur                       Check aur for new packages "
         "too. Will need auracle installed.\n"
         "          --debug|-d                  Print debug info.\n"
         "          --ftimeout|-f [value]       Program will manually enforce "
         "timeout for closing notification.\n"
         "                                      Do NOT use with --timeout, if "
         "--timeout works or without --loop-time [value].\n"
         "                                      The value for this option "
         "should be in minutes.\n"
         "\nMore information can be found in the manpage.\n";
  exit(0);
}

/* Prints the version. */
int print_version() {
  std::cout
      << "aarchup " VERSION_NUMBER
         "\nCopyright 2018 Leonidas Spyropoulos <artafinde@gmail.com>\n"
         "License GPLv3+: GNU GPL version 3 or later "
         "<http://gnu.org/licenses/gpl.html>.\n"
         "This is free software, you are free to change and redistribute it.\n"
         "There is NO WARRANTY, to the extent permitted by law.\n";
  exit(0);
}

std::vector<std::string> split(const std::string &s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

int main(int argc, char **argv) {
  NotifyUrgency urgency = NOTIFY_URGENCY_NORMAL;
  const char *command = "/usr/bin/checkupdates";
  const char *aurCommand = "/usr/bin/auracle sync";

  long timeout = 3600 * 1000;
  long max_number_out = 30;
  long loop_time = 3600;
  long manual_timeout = 0;
  gchar *icon = nullptr;
  bool will_loop = FALSE;
  static int help_flag = 0;
  static int version_flag = 0;
  static int aur = 0;

  plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
  plog::init(plog::warning, &consoleAppender);

  if (argc > 1) {
    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)
      print_version();
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
      print_help();
  }

  /* Parse commandline options */
  const char *const short_opts = "c:p:m:t:i:u:l:df:";
  const option long_opts[] = {
      {"command", required_argument, nullptr, 'c'},
      {"icon", required_argument, nullptr, 'p'},
      {"maxentries", required_argument, nullptr, 'm'},
      {"timeout", required_argument, nullptr, 't'},
      {"uid", required_argument, nullptr, 'i'},
      {"urgency", required_argument, nullptr, 'u'},
      {"loop-time", required_argument, nullptr, 'l'},
      {"help", no_argument, &help_flag, 1},
      {"version", no_argument, &version_flag, 1},
      {"aur", no_argument, &aur, 1},
      {"ftimeout", required_argument, nullptr, 'f'},
      {"debug", no_argument, nullptr, 'd'},
      {nullptr, 0, nullptr, 0},
  };

  while (true) {
    int option_index = 0;
    const auto opt =
        getopt_long(argc, argv, short_opts, long_opts, &option_index);
    if (-1 == opt) {
      break;
    }
    /* Short opts */
    switch (opt) {
      case 'd':
        plog::get()->setMaxSeverity(plog::verbose);
        break;
      case 0:
        if (long_opts[option_index].flag) {
          break;
        }
        LOGE << "Undefined option " << long_opts[option_index].name;
        if (optarg) {
          LOGE << " with args " << optarg;
        }
        break;
      case 'v':
        LOGV << "Printing version";
        print_version();
        break;
      case 'c':
        command = optarg;
        LOGV << "Command set: '" << command << "'";
        break;
      case 'p':
        icon = optarg;
        LOGV << "Icon set: '" << icon << "'";
        break;
      case 'm':
        if (!isdigit(optarg[0])) {
          LOGF << "Argument '--maxentries' should be number";
          exit(1);
        }
        max_number_out = std::stol(optarg);
        LOGV << "Max_number set: '" << max_number_out << "' lines";
        break;
      case 't':
        if (!isdigit(optarg[0])) {
          LOGF << "Argument '--timeout' should be number";
          exit(1);
        }
        timeout = std::stol(optarg) * 1000;
        LOGV << "Timeout set: " << timeout / 1000 << " sec(s)";
        ;
        break;
      case 'i':
        if (!isdigit(optarg[0])) {
          LOGF << "Argument '--uid' should be number";
          exit(1);
        }
        if (setuid(static_cast<__uid_t>(std::stol(optarg))) != 0) {
          LOGF << "Couldn't change to the given uid, aborting";
          exit(1);
        }
        LOGV << "Setting uid to: " << optarg;
        break;
      case 'u':
        if (strcmp(optarg, "low") == 0) {
          urgency = NOTIFY_URGENCY_LOW;
        } else if (strcmp(optarg, "normal") == 0) {
          urgency = NOTIFY_URGENCY_NORMAL;
        } else if (strcmp(optarg, "critical") == 0) {
          urgency = NOTIFY_URGENCY_CRITICAL;
        } else {
          LOGF << "Argument '--urgency' has to be 'low', 'normal' or 'critical";
          exit(1);
        }
        LOGV << "Urgency set: " << urgency;
        break;
      case 'l':
        will_loop = TRUE;
        if (!isdigit(optarg[0])) {
          LOGF << "Argument '--loop-time' should be number";
          exit(1);
        }
        loop_time = std::stol(optarg) * 60;
        LOGV << "Loop_time set: " << loop_time / 60 << " min(s)";
        break;
      case 'f':
        if (!isdigit(optarg[0])) {
          LOGF << "Argument '--ftimeout' should be number";
          exit(1);
        }
        manual_timeout = std::stol(optarg) * 60;
        if (!will_loop) {
          LOGF << "Argument '--ftimeout' can't be used without or before "
                  "'--loop-time'";
          exit(1);
        }
        if (manual_timeout > loop_time) {
          LOGF << "Please set a value for '--ftimeout' that is lower than "
                  "'--loop-time'";
          exit(1);
        }
        LOGV << "Manual_timeout: " << manual_timeout / 60 << " min(s)";
        break;
      case 'h':
      case '?':
        print_help();
        break;
      default:
        exit(1);
    }
  }

  long offset = 0;
  NotifyNotification *my_notify = nullptr;
  const char *name = "New Updates";
  const char *category = "update";
  GError *error = nullptr;
  do {
    LOGD << "Executing command '" << command << "' for updates";
    auto checkUpdatesCmd = std::make_unique<CliWrapper>(command);
    const std::string &checkUpdateOut = checkUpdatesCmd->execute();
    std::string aurHelperOut;
    if (aur) {
      LOGD << "Executing command '" << aurCommand << "' for AUR updates";
      auto aurHelperCmd = std::make_unique<CliWrapper>(aurCommand);
      aurHelperOut = aurHelperCmd->execute();
    }
    if (!checkUpdateOut.empty() || (!aurHelperOut.empty())) {
      std::string finalOut = "There are updates for:\n";
      if (aurHelperOut.empty()) {
        finalOut = finalOut + checkUpdateOut;
      } else if (checkUpdateOut.empty()) {
        finalOut = finalOut + AUR_HEADER + aurHelperOut;
      } else {
        finalOut = finalOut + checkUpdateOut + AUR_HEADER + aurHelperOut;
      }
      auto outputLines = split(finalOut, '\n');
      int lines = 0;
      std::stringstream ss;
      for (auto &outputLine : outputLines) {
        ss << outputLine << '\n';
        lines++;
        if (lines >= max_number_out) {
          break;
        }
      }
      if (!notify_is_initted()) {
        notify_init(name);
      }
      bool persist = TRUE;
      gboolean success;
      do {
        if (!my_notify) {
          my_notify = notify_notification_new(
              "New updates for Arch Linux available!", ss.str().c_str(), icon);
        } else {
          notify_notification_update(my_notify,
                                     "New updates for Arch Linux available!",
                                     ss.str().c_str(), icon);
        }
        notify_notification_set_timeout(my_notify, timeout);
        notify_notification_set_category(my_notify, category);
        notify_notification_set_urgency(my_notify, urgency);
        success = notify_notification_show(my_notify, &error);
        if (success)
          LOGD << "Notification shown successfully";
        else {
          if (persist) {
            LOGW << "Notification failed, reason:\n\t[" << error->code << "] "
                 << error->message << "\n";
          } else {
            LOGE << "Notification failed, reason:\n\t[" << error->code << "] "
                 << error->message << "\n";
          }
          g_error_free(error);
          error = nullptr;
          if (persist) {
            LOGW << "It could have been caused by an environment restart, "
                    "trying to work around it by re-init libnotify";
            my_notify = nullptr;
            notify_uninit();
            notify_init(name);
            persist = FALSE;
          }
        }
      } while (!my_notify);
      if (error) {
        g_error_free(error);
        error = nullptr;
      }
      if (manual_timeout && success && will_loop) {
        LOGD << "Will close notification in " << manual_timeout / 60
             << " minutes (this time will be reduced from the loop-time)";
        sleep(static_cast<unsigned int>(manual_timeout));
        offset = manual_timeout;
        if (notify_notification_close(my_notify, &error))
          LOGD << "Notification closed";
        else {
          LOGW << "Failed to close, reason:\n\t[" << error->code << "] "
               << error->message;
        }
        if (error) {
          g_error_free(error);
          error = nullptr;
        }
      }
    } else {
      LOGI << "No updates found";
      if (my_notify) {
        LOGD << "Previous notification found. Closing it in case it was "
                "still opened";
        if (notify_notification_close(my_notify, &error))
          LOGD << "Notification closed";
        else {
          LOGW << "Failed to close, reason:\n\t[" << error->code << "] "
               << error->message;
        }
        if (error) {
          g_error_free(error);
          error = nullptr;
        }
      }
    }

    if (will_loop) {
      LOGD << "Next run will be in " << (loop_time - offset) / 60 << " minutes";
      sleep(static_cast<unsigned int>(loop_time - offset));
      offset = 0;
    }
  } while (will_loop);
  return 0;
}
