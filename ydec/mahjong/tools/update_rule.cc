// Copyright 2016 Yuki Hamada
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

#include <iostream>
#include <fstream>

#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/text_format.h"

#include "ydec/mahjong/proto/mahjong-rule.pb.h"

using std::cerr;
using std::endl;
using std::ifstream;
using std::istream;
using std::ofstream;
using std::ostream;

using google::protobuf::TextFormat;
using google::protobuf::io::IstreamInputStream;

using ydec::mahjong::Rule;

int main(int argc, char** argv) {
  if (argc != 3) {
    cerr << "Missing arguments" << endl;
    return -1;
  }

  const char* input_path = argv[1];
  const char* output_path = argv[2];

  Rule rule;

  ifstream is;
  is.open(input_path, istream::in);
  IstreamInputStream iis(&is);
  bool succeeded = TextFormat::Parse(&iis, &rule);
  is.close();

  if (!succeeded) {
    cerr << "Failed to parse the given rule text format proto." << endl;
    return -1;
  }

  ofstream os;
  os.open(output_path, ostream::out | ostream::binary);
  succeeded = rule.SerializeToOstream(&os);
  os.close();

  if (!succeeded) {
    cerr << "Failed to serialize rule proto to the given output path." << endl;
    return -1;
  }

  return 0;
}
