#ifndef MIN_DONT_CARE_BRANCHING_STRATEGY_H
#define MIN_DONT_CARE_BRANCHING_STRATEGY_H

#include "branching_strategy.h"
#include "Allocator.h"

class MinDontCareBranchingStrategy : public BranchingStrategy
{
public:
    int ChooseColumn(BoolEquation &equation) const override;
    DECLARE_ALLOCATOR
};

#endif // MIN_DONT_CARE_BRANCHING_STRATEGY_H
