/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <map>
#include <string>
#include <vector>

#include <base/logging.h>

#include "command.h"

static std::map<std::string, android::base::LogSeverity> log_severity_map = {
    {"debug", android::base::DEBUG},
    {"warning", android::base::WARNING},
    {"error", android::base::ERROR},
    {"fatal", android::base::FATAL},
};

int main(int argc, char** argv) {
  InitLogging(argv, android::base::StderrLogger);
  std::vector<std::string> args;
  android::base::LogSeverity log_severity = android::base::ERROR;

  if (argc == 1) {
    args.push_back("help");
  } else {
    for (int i = 1; i < argc; ++i) {
      if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
        args.insert(args.begin(), "help");
      } else if (strcmp(argv[i], "--log") == 0) {
        if (i + 1 < argc) {
          ++i;
          auto it = log_severity_map.find(argv[i]);
          if (it != log_severity_map.end()) {
            log_severity = it->second;
          } else {
            LOG(ERROR) << "Unknown log severity: " << argv[i];
            return 1;
          }
        } else {
          LOG(ERROR) << "Missing argument for --log option.\n";
          return 1;
        }
      } else {
        args.push_back(argv[i]);
      }
    }
  }
  android::base::ScopedLogSeverity severity(log_severity);

  std::unique_ptr<Command> command = CreateCommandInstance(args[0]);
  if (command == nullptr) {
    LOG(ERROR) << "malformed command line: unknown command " << args[0];
    return 1;
  }
  std::string command_name = args[0];
  args.erase(args.begin());

  LOG(DEBUG) << "command '" << command_name << "' starts running";
  bool result = command->Run(args);
  LOG(DEBUG) << "command '" << command_name << "' "
             << (result ? "finished successfully" : "failed");
  return result ? 0 : 1;
}
