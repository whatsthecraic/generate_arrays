#include <cinttypes>
#include <climits> // PATH_MAX
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <string>

#include "lib/args/args.hxx" // Parser for the command line arguments

#include "error.hpp"
#include "filesystem.hpp"
#include "quantity.hpp"

using namespace args;
using namespace common;
using namespace std;

static void generate(string folder, uint64_t num_arrays, uint64_t sz_array, int digits, uint64_t seed){
    cout << "[Generate] Path: " << folder << ", arrays: " << num_arrays << ", cardinality: " << sz_array << ", digits: " << digits << ", seed: " << seed << endl;

    filesystem::mkdir(folder);

    mt19937_64 generator{seed};
    uniform_int_distribution<uint64_t> distribution{ 0, numeric_limits<uint64_t>::max() };

    for(uint64_t k =0; k < num_arrays; k++){
        char path[PATH_MAX];
        sprintf(path, "%s/%0*" PRIu64 ".bin", folder.c_str(), digits, k);
        cout << "[" << (k+1) << "/" << num_arrays << "] Generating " << path << " ..." << endl;
        fstream output(path, ios_base::out | ios_base::binary | ios_base::trunc );
        if(!output.good()) ERROR("Cannot open the file `" << path << "' for writing");

        for(uint64_t i = 0; i < sz_array; i++){
            uint64_t value = distribution(generator);
            output.write((char*) &value, sizeof(value));
            if(!output.good()) ERROR("Cannot write the value in the file `" << path << "'");
        }

        output.close();
    }

}

int main(int argc, char* argv[]) {
    ArgumentParser parser {
        "Generate N random arrays with the uniform distribution"
    };
    HelpFlag argument_help{parser, "help", "Display this help menu", {'h', "help"}};

    // Number of arrays to generate
    ValueFlag<Quantity> argument_num_arrays{parser, "value >0", "Number of arrays to generate", {'N'}};
    ValueFlag<Quantity> argument_size_arrays{parser, "value >0", "The target cardinality of each array", {'M'}};
    ValueFlag<int> argument_digits{parser, "value > 0", "Number of each digits for the name of each path", {'d'}, 3};
    ValueFlag<uint64_t> argument_seed{parser, "value", "The seed for the random generator", {'s', "seed"}, 1ULL};

    // Destination path
    Positional<string> argument_path{parser, "path", "The path (folder) where to store the generated files"};

    try {
        parser.ParseCLI(argc, argv);

        string path = argument_path.Get();
        if(!argument_path) ERROR("Missing mandatory argument `path'. Usage: " << argv[0] << " {options} <path>");

        uint64_t num_arrays = static_cast<uint64_t>(argument_num_arrays.Get());
        if(num_arrays <= 0) ERROR("Invalid amount of arrays to generate (-N): " << num_arrays);

        uint64_t size_arrays = static_cast<uint64_t>(argument_size_arrays.Get());
        if(size_arrays <= 0) ERROR("Invalid target cardinality (-M): " << size_arrays);

        int digits = argument_digits.Get();
        if(digits <= 0) ERROR("Invalid number of digits: " << digits);

        uint64_t seed = argument_seed.Get();

        generate(path, num_arrays, size_arrays, digits, seed);
    } catch(Help){
        cout << parser;

    } catch(common::Error& e){
        cout << e << endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}