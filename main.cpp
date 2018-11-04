/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>
 */

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

static void save_random(fstream& output, uint64_t cardinality, uint64_t seed){
    mt19937_64 generator{seed};
    uniform_int_distribution<uint64_t> distribution{ 0, numeric_limits<uint64_t>::max() };

    for(uint64_t i = 0; i < cardinality; i++){
        uint64_t value = distribution(generator);
        output.write((char*) &value, sizeof(value));
        if(!output.good()) ERROR("Cannot write the value in the file");
    }
}

static void save_sequential(fstream& output, uint64_t cardinality){
    for(uint64_t i = 0; i < cardinality; i++){
        uint64_t value = i;
        output.write((char*) &value, sizeof(value));
        if(!output.good()) ERROR("Cannot write the value in the file");
    }
}

static void generate(string folder, uint64_t num_arrays, uint64_t sz_array, int digits, uint64_t seed, bool random){
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

        if(random){
            save_random(output, sz_array, seed);
        } else {
            save_sequential(output, sz_array);
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
    ValueFlag<Quantity> argument_cardinality{parser, "value >0", "The target cardinality of each array", {'c', "cardinality"}};
    ValueFlag<int> argument_digits{parser, "value > 0", "Number of each digits for the name of each path", {'d'}, 3};
    ValueFlag<Quantity> argument_num_arrays{parser, "value >0", "Number of arrays to generate", {'N'}};
    Flag argument_random{parser, "random", "Whether to store random integers in the file", {'r', "random"}};
    ValueFlag<Quantity> argument_size{parser, "value > 0", "The size (in bytes) of teach array", {'S', "size"}, Quantity{0, true}};
    ValueFlag<uint64_t> argument_seed{parser, "value", "The seed for the random generator", {'s', "seed"}, 1ULL};

    // Destination path
    Positional<string> argument_path{parser, "path", "The path (folder) where to store the generated files"};

    try {
        parser.ParseCLI(argc, argv);

        string path = argument_path.Get();
        if(!argument_path) ERROR("Missing mandatory argument `path'. Usage: " << argv[0] << " {options} -S <size> <path>");

        uint64_t num_arrays = static_cast<uint64_t>(argument_num_arrays.Get());
        if(num_arrays <= 0) ERROR("Invalid amount of arrays to generate (-N): " << num_arrays);

        uint64_t size_arrays = 0;
        if(!argument_cardinality && argument_size.Get() == 0){
            ERROR("Missing both the cardinality and the size of the target array. Usage: " << argv[0] << " {options} -S <size> <path>");
        } else if (argument_cardinality){
            size_arrays = static_cast<uint64_t>(argument_cardinality.Get());
        } else {
            size_arrays = static_cast<uint64_t>(argument_size.Get()) / sizeof(uint64_t);
        }
        if(size_arrays <= 0) ERROR("Invalid target cardinality: " << size_arrays);

        int digits = argument_digits.Get();
        if(digits <= 0) ERROR("Invalid number of digits: " << digits);

        uint64_t seed = argument_seed.Get();
        bool is_random = argument_random.Get();

        generate(path, num_arrays, size_arrays, digits, seed, is_random);
    } catch(Help){
        cout << parser;

    } catch(common::Error& e){
        cout << e << endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}