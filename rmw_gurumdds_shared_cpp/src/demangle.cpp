// Copyright 2019 GurumNetworks, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <algorithm>
#include <cstring>
#include <map>
#include <string>
#include <vector>
#include <regex>

#include "rcutils/logging_macros.h"

#include "rmw_gurumdds_shared_cpp/namespace_prefix.hpp"
#include "rmw_gurumdds_shared_cpp/demangle.hpp"

std::string
_demangle_if_ros_topic(const std::string & topic_name)
{
  std::string prefix = _get_ros_prefix_if_exists(topic_name);
  if (prefix.length() > 0) {
    return topic_name.substr(strlen(ros_topic_prefix));
  }
  return topic_name;
}

std::string
_demangle_if_ros_type(const std::string & dds_type_string)
{
  std::string substring = "dds_::";
  size_t substring_position = dds_type_string.find(substring);
  if (
    dds_type_string[dds_type_string.size() - 1] == '_' &&
    substring_position != std::string::npos)
  {
    std::string type_namespace = dds_type_string.substr(0, substring_position);
    type_namespace = std::regex_replace(type_namespace, std::regex("::"), "/");
    size_t start = substring_position + substring.size();
    std::string type_name = dds_type_string.substr(start, dds_type_string.length() - 1 - start);
    return type_namespace + type_name;
  }
  // not a ROS type
  return dds_type_string;
}

std::string
_demangle_service_from_topic(const std::string & topic_name)
{
  std::string prefix = _get_ros_prefix_if_exists(topic_name);
  if (prefix.length() == 0) {  // not a ROS topic or service
    return "";
  }

  std::vector<std::string> prefixes = {
    ros_service_response_prefix,
    ros_service_requester_prefix,
  };
  if (
    std::none_of(prefixes.cbegin(), prefixes.cend(), [&prefix](auto & x) {return prefix == x;}))
  { // not a ROS service topic
    return "";
  }

  std::map<std::string, std::string> suffixes = {
    {ros_service_response_prefix, "Reply"},
    {ros_service_requester_prefix, "Request"},
  };
  auto & suffix = suffixes[prefix];
  size_t suffix_position = topic_name.rfind(suffix);
  if (suffix_position == std::string::npos) {
    RCUTILS_LOG_WARN_NAMED(
      "rmw_gurumdds_shared_cpp",
      "service topic has prefix but no suffix: '%s'",
      topic_name.c_str());
    return "";
  }

  std::string service_name = topic_name.substr(0, suffix_position + 1);

  size_t start = prefix.length();
  return service_name.substr(start, service_name.length() - 1 - start);
}

std::string
_demangle_service_type_only(const std::string & dds_type_name)
{
  std::string ns_substring = "dds_::";
  size_t ns_substring_position = dds_type_name.find(ns_substring);
  if (ns_substring_position == std::string::npos) {
    return "";
  }

  static const std::string suffixes[] = {
    std::string("_Response_"),
    std::string("_Request_"),
  };
  std::string found_suffix = "";
  size_t suffix_position = std::string::npos;
  for (const auto & suffix : suffixes) {
    suffix_position = dds_type_name.rfind(suffix);
    if (suffix_position != std::string::npos) {
      if (dds_type_name.length() - suffix_position - suffix.length() != 0) {
        RCUTILS_LOG_WARN_NAMED(
          "rmw_gurumdds_shared_cpp",
          "service type contains 'dds_::' and a suffix, but not at the end: '%s'",
          dds_type_name.c_str());
        continue;
      }
      found_suffix = suffix;
      break;
    }
  }

  if (suffix_position == std::string::npos) {
    RCUTILS_LOG_WARN_NAMED(
      "rmw_gurumdds_shared_cpp",
      "service type contains 'dds_::' but does not have a suffix: '%s'",
      dds_type_name.c_str());
    return "";
  }

  // everything checks out, reformat it from '<pkg>::srv::dds_::<type><suffix>'
  // to '<namespace>/<type>'
  std::string type_namespace = dds_type_name.substr(0, ns_substring_position);
  type_namespace = std::regex_replace(type_namespace, std::regex("::"), "/");
  size_t start = ns_substring_position + ns_substring.length();
  std::string type_name = dds_type_name.substr(start, suffix_position - start);
  return type_namespace + type_name;
}
