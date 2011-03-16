// Copyright 2010 Google
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

#ifndef UTIL_XML_HELPER_H_
#define UTIL_XML_HELPER_H_

#include <stack>
#include <string>
#include <utility>
#include "base/macros.h"
#include "base/strutil.h"

using std::string;
using std::pair;
using std::stack;


namespace operations_research {

// Lightweight XML writer that is optimized for CPViz output. As it only
// supports the features required by CPViz, it should not be used to generate
// general-purpose XML data.
class XmlHelper {
 public:
  XmlHelper();

  // Starts a new XML document
  void StartDocument();

  // Starts a new element
  void StartElement(const string& name);

  // Adds a key-value pair to the current element.
  void AddAttribute(const string& key, int value);

  // Adds a key-value pair to the current element.
  void AddAttribute(const string& key, const string& value);

  // Ends the current element and goes back to the previous element.
  void EndElement();

  // Ends the document.
  void EndDocument();

  // Returns the XML content written so far.
  const string& GetContent() const;

 private:
  typedef pair<char, string> EscapePair;
  string content_;
  stack<string> tags_;
  bool direction_down_;

  DISALLOW_COPY_AND_ASSIGN(XmlHelper);
};
}
#endif  // UTIL_XML_HELPER_H_
