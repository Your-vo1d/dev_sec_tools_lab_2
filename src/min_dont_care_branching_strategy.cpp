#include "min_dont_care_branching_strategy.h"
#include "boolequation.h"
#include <vector>
#include <algorithm>

IMPLEMENT_ALLOCATOR(MinDontCareBranchingStrategy, 0, 0)

int MinDontCareBranchingStrategy::ChooseColumn(BoolEquation &equation) const
{
    std::vector<int> indexes;
    std::vector<int> values;
    bool rezInit = false;

    for (int i = 0; i < equation.mask.getSize(); i++) {
        if (equation.mask[i] == 0)
            indexes.push_back(i);
    }

    if (indexes.empty())
        return -1;

    for (int i = 0; i < equation.cnfSize; i++) {
        BoolInterval *interval = equation.cnf[i];
        if (interval != nullptr) {
            if (!rezInit) {
                for (int k = 0; k < static_cast<int>(indexes.size()); k++) {
                    values.push_back(interval->getValue(indexes[k]) == '-' ? 1 : 0);
                }
                rezInit = true;
            } else {
                for (int k = 0; k < static_cast<int>(indexes.size()); k++) {
                    if (interval->getValue(indexes[k]) == '-')
                        values[k]++;
                }
            }
        }
    }

    if (values.empty())
        return indexes.front();

    int minIdx = static_cast<int>(
        std::min_element(values.begin(), values.end()) - values.begin());
    return indexes[minIdx];
}
