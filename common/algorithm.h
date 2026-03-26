#include <types.h>

class BinPackingAlgorithm {
public:
    virtual string name() const = 0;
    virtual Packing pack(const vector<Item> &items) = 0;
    virtual ~BinPackingAlgorithm() {}
};