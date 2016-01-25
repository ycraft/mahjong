#include <iostream>
#include <fstream>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "src-gen/cpp/mahjong-rule.pb.h"

using namespace std;
using namespace google::protobuf;
using namespace ydec::mahjong;
using namespace google::protobuf::io;

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
