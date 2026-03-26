#include <vector>
#include <string>
#include <numeric>
#include <set>
#include <algorithm>
using namespace std;

using Item = double;
using Bin = std::vector<Item>;
using Packing = std::vector<Bin>;
const Item MaxBinCapacity = 1.0;    // Maximum capacity of a bin