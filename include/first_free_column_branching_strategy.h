#ifndef FIRST_FREE_COLUMN_BRANCHING_STRATEGY_H
#define FIRST_FREE_COLUMN_BRANCHING_STRATEGY_H

#include "branching_strategy.h"
#include "Allocator.h"

class FirstFreeColumnBranchingStrategy : public BranchingStrategy
{
public:
    int ChooseColumn(BoolEquation &equation) const override;
    DECLARE_ALLOCATOR
};

#endif // FIRST_FREE_COLUMN_BRANCHING_STRATEGY_H
