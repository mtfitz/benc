#include "benc.hpp"

#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <getopt.h>

using namespace std;

struct args_t {
    string source;
    string infile;
    string outfile; 
    bool print_remainder = false;
};

optional<args_t> get_args(int argc, char** argv)
{
    args_t args;
    int c;
    bool got_text = false;
    while (optind < argc) {
        if ((c = getopt(argc, argv, "i:o:r")) != -1) {
            switch (c) {
            case 'i':
                args.infile = optarg;
                break;
            case 'o':
                args.outfile = optarg;
                break;
            case 'r':
                args.print_remainder = true;
                break;
            default:
                return {};
            }
        } else {
            if (got_text) {
                return {};
            }
            args.source = argv[optind];
            got_text = true;
            optind++;
        }
    }

    // if we're given an input file, we shouldn't have STDIN source text
    if (args.source.size() && args.infile.size()) {
        return {};
    }

    // we need at least one kind of input
    if (args.source.empty() && args.infile.empty()) {
        return {};
    }

    return args;
}

void print_usage()
{
    cout << "Usage: benc [OPTIONS] input_text" << endl;
    cout << "Options:" << endl;
    cout << "\t-i FILE\t\tuse input file, instead of STDIN" << endl;
    cout << "\t-o FILE\t\tuse output file, instead of STDOUT" << endl;
    cout << "\t-r FILE\t\tgive input text via terminal/STDIN" << endl;
}

int main(int argc, char** argv)
{
    // get args
    auto args_wrapped = get_args(argc, argv);
    if (!args_wrapped.has_value()) {
        print_usage();
        exit(0);
    }
    args_t args = args_wrapped.value();

    // grab input and setup files, if necessary
    vector<benc::byte_t> src;
    ostream* out = &cout;

    if (args.infile.size()) {
        ifstream in(args.infile, std::ifstream::binary | std::ifstream::ate);
        if (!in) {
            cout << "Invalid input file" << endl;
            exit(0);
        }

        // get file size (should be OK for binary files)
        size_t file_size = (size_t) in.tellg();
        src.resize(file_size);
        in.seekg(0);

        in.read((char *) src.data(), file_size);
    } else {
        src = vector<benc::byte_t>(args.source.begin(), args.source.end());
    }

    // open output file
    if (args.outfile.size()) {
        out = new ofstream(args.outfile);
        if (!out) {
            cout << "Invalid output file" << endl;
            exit(0);
        }
    }

    // parse the data!
    auto result_wrapped = benc::decode(src);
    if (!result_wrapped.has_value()) {
        *out << "Error!" << endl;
        exit(0);
    }
    benc::BencTree result(result_wrapped.value());

    *out << result.to_string() << endl;

    if (args.print_remainder) {
        *out << "Remainder: " << string(src.begin(), src.end()) << endl;
    }

    return 0;
}