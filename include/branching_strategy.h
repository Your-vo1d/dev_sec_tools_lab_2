#ifndef BRANCHING_STRATEGY_H
#define BRANCHING_STRATEGY_H

class BoolEquation;

class BranchingStrategy
{
public:
    virtual ~BranchingStrategy() = default;
    virtual int ChooseColumn(BoolEquation &equation) const = 0;
};

#endif // BRANCHING_STRATEGY_H
