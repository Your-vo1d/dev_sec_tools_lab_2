#include "benchmarks_example.h"
#include "min_dont_care_branching_strategy.h"
#include "first_free_column_branching_strategy.h"
#include "boolequation.h"
#include "boolinterval.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <new>

using namespace std;

static void out_of_memory() { throw std::bad_alloc(); }

static vector<string> readPLA(const string &path)
{
    vector<string> clauses;
    ifstream file(path);
    if (!file.is_open())
        return clauses;
    string line;
    while (getline(file, line)) {
        while (!line.empty() &&
               (line.back() == '\r' || line.back() == ' ' || line.back() == '\t'))
            line.pop_back();
        if (!line.empty() && line[0] != '.')
            clauses.push_back(line);
    }
    return clauses;
}

// sol is filled with the satisfying assignment when a solution is found
static bool solveDPLL(BoolEquation *eq, const BranchingStrategy &strategy, string *sol)
{
    while (true) {
        if (eq->count == 0) {
            if (sol) {
                int n = eq->root->length();
                sol->resize(n);
                for (int i = 0; i < n; ++i)
                    (*sol)[i] = eq->root->getValue(i);
            }
            return true;
        }
        int result = eq->CheckRules();
        if (result == 0) return false;
        if (result == 2) break;
    }

    int col = eq->ChooseColForBranching(strategy);
    if (col < 0) return false;

    {
        BoolEquation eq0(*eq);
        eq0.Simplify(col, '0');
        if (solveDPLL(&eq0, strategy, sol)) return true;
    }
    {
        BoolEquation eq1(*eq);
        eq1.Simplify(col, '1');
        return solveDPLL(&eq1, strategy, sol);
    }
}

static void printUsage(const char *prog)
{
    cerr << "Usage:\n"
         << "  " << prog << " FILE [--strategy=STRATEGY | -s STRATEGY]\n"
         << "  " << prog << " --bench\n\n"
         << "Branching strategies:\n"
         << "  min-dont-care  (default)  select column with minimum '-' count\n"
         << "  first-free                select first free column\n";
}

int main(int argc, char *argv[])
{
    set_new_handler(out_of_memory);

    string filepath;
    string strategyName = "min-dont-care";
    bool bench = false;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--bench") {
            bench = true;
        } else if (arg.rfind("--strategy=", 0) == 0) {
            strategyName = arg.substr(11);
        } else if ((arg == "-s" || arg == "--strategy") && i + 1 < argc) {
            strategyName = argv[++i];
        } else if (arg[0] != '-') {
            filepath = arg;
        } else {
            cerr << "Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    if (bench) {
        RunAllocatorBenchmarks();
        return 0;
    }

    if (filepath.empty()) {
        printUsage(argv[0]);
        return 1;
    }

    MinDontCareBranchingStrategy stratMin;
    FirstFreeColumnBranchingStrategy stratFirst;
    const BranchingStrategy *strategy = nullptr;

    if (strategyName == "min-dont-care") {
        strategy = &stratMin;
    } else if (strategyName == "first-free") {
        strategy = &stratFirst;
    } else {
        cerr << "Unknown strategy: " << strategyName << "\n";
        printUsage(argv[0]);
        return 1;
    }

    auto clauses = readPLA(filepath);
    if (clauses.empty()) {
        cerr << "Error: file not found or empty: " << filepath << "\n";
        return 1;
    }

    int nvars = static_cast<int>(clauses[0].size());
    int nrows = static_cast<int>(clauses.size());

    vector<BoolInterval *> intervals;
    intervals.reserve(nrows);
    for (const auto &c : clauses)
        intervals.push_back(new BoolInterval(c.c_str()));

    BoolInterval *root = new BoolInterval(static_cast<size_t>(nvars));
    BBV mask(nvars);
    BoolEquation eq(intervals.data(), root, nrows, nrows, mask);

    string solution;
    bool sat = false;
    try {
        sat = solveDPLL(&eq, *strategy, &solution);
    } catch (int e) {
        cerr << "BBV exception (" << e << ")\n";
        delete root;
        for (auto *iv : intervals) delete iv;
        return 1;
    }

    if (sat) {
        cout << "SAT\n" << solution << "\n";
    } else {
        cout << "UNSAT\n";
    }

    delete root;
    for (auto *iv : intervals) delete iv;
    return 0;
}
