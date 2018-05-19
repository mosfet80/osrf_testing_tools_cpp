// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#ifndef OSRF_TESTING_TOOLS_CPP__STACK_TRACE_HPP_
#define OSRF_TESTING_TOOLS_CPP__STACK_TRACE_HPP_

#include <memory>
#include <regex>
#include <string>
#include <thread>

#include "./visibility_control.hpp"

namespace osrf_testing_tools_cpp
{
namespace memory_tools
{

struct SourceLocationImpl;

struct OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC SourceLocation
{
  explicit SourceLocation(std::shared_ptr<SourceLocationImpl> impl);
  virtual ~SourceLocation();

  const std::string &
  function() const;

  const std::string &
  filename() const;

  size_t
  line() const;

  size_t
  column() const;

  private:
    std::shared_ptr<SourceLocationImpl> impl_;
};

struct TraceImpl;

struct OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC Trace
{
  explicit Trace(std::unique_ptr<TraceImpl> impl);
  explicit Trace(const Trace & other);
  virtual ~Trace();

  void *
  address() const;

  size_t
  index_in_stack() const;

  const std::string &
  object_filename() const;

  const std::string &
  object_function() const;

  const SourceLocation &
  source_location() const;

  const std::vector<SourceLocation> &
  inlined_source_locations() const;

private:
  std::unique_ptr<TraceImpl> impl_;
};

struct StackTraceImpl;

/// Return true if the given regex is found to match the object_filename of any trace.
template<typename CharT>
bool
matches_any_object_filename_of_traces(
  const std::basic_regex<CharT> & regex,
  const std::vector<Trace> & traces)
{
  for (const auto & trace : traces) {
    if (std::regex_search(trace.object_filename(), regex)) {
      return true;
    }
  }
  return false;
}

/// Return true if the given regex is found to match the object_function of any trace.
template<typename CharT>
bool
matches_any_object_function_of_traces(
  const std::basic_regex<CharT> & regex,
  const std::vector<Trace> & traces)
{
  for (const auto & trace : traces) {
    if (std::regex_search(trace.object_function(), regex)) {
      return true;
    }
  }
  return false;
}

struct OSRF_TESTING_TOOLS_CPP_MEMORY_TOOLS_PUBLIC StackTrace
{
  explicit StackTrace(std::unique_ptr<StackTraceImpl> impl);
  virtual ~StackTrace();

  /// Return the thread id of the thread from which the call stack originates.
  std::thread::id
  thread_id() const;

  /// Return a list of traces in the call stack.
  /** Limited to a max call stack depth of 256 */
  const std::vector<Trace> &
  get_traces() const;

  /// Return a list of traces which end with the last call to the function name given.
  /**
   * This can be used, in combination with the name of the replacement memory
   * function, to avoid a lot of boilerplate calls in the stack that are part
   * of memory tools itself.
   *
   * An empty vector returned indicates the function name was not found in the
   * callstack.
   */
  std::vector<Trace>
  get_traces_from_function_name(const char * function_name) const;

  /// Return true if the given regex is found to match the object_filename of any trace.
  template<typename CharT>
  bool
  matches_any_object_filename(const std::basic_regex<CharT> & regex) const
  {
    return matches_any_object_filename_of_traces(regex, this->get_traces());
  }

  /// Return true if the given regex is found to match the object_function of any trace.
  template<typename CharT>
  bool
  matches_any_object_function(const std::basic_regex<CharT> & regex) const
  {
    return matches_any_object_function_of_traces(regex, this->get_traces());
  }

private:
  std::unique_ptr<StackTraceImpl> impl_;
};

}  // namespace memory_tools
}  // namespace osrf_testing_tools_cpp

#endif  // OSRF_TESTING_TOOLS_CPP__STACK_TRACE_HPP_
