#include <cstdio>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "BBV.h"
#include "boolequation.h"
#include "boolinterval.h"
#include "first_free_column_branching_strategy.h"
#include "min_dont_care_branching_strategy.h"

static int g_passed = 0;
static int g_failed = 0;

#define CHECK(cond) \
    do { \
        if (cond) { \
            std::cout << "  PASS: " #cond "\n"; \
            g_passed++; \
        } else { \
            std::cout << "  FAIL: " #cond "  [line " << __LINE__ << "]\n"; \
            g_failed++; \
        } \
    } while (0)


static bool solveDPLL(BoolEquation *eq, const BranchingStrategy &strat,
                      std::string *sol = nullptr)
{
    while (true) {
        if (eq->count == 0) {
            if (sol) {
                int n = eq->root->length();
                sol->resize(n);
                for (int i = 0; i < n; ++i) (*sol)[i] = eq->root->getValue(i);
            }
            return true;
        }
        int r = eq->CheckRules();
        if (r == 0) return false;
        if (r == 2) break;
    }
    int col = eq->ChooseColForBranching(strat);
    if (col < 0) return false;
    { BoolEquation e(*eq); e.Simplify(col, '0'); if (solveDPLL(&e, strat, sol)) return true; }
    { BoolEquation e(*eq); e.Simplify(col, '1'); return solveDPLL(&e, strat, sol); }
}

static std::vector<std::string> readPLA(const std::string &path)
{
    std::vector<std::string> clauses;
    std::ifstream f(path);
    if (!f.is_open()) return clauses;
    std::string line;
    while (std::getline(f, line)) {
        while (!line.empty() &&
               (line.back() == '\r' || line.back() == ' ' || line.back() == '\t'))
            line.pop_back();
        if (!line.empty() && line[0] != '.') clauses.push_back(line);
    }
    return clauses;
}

static std::vector<std::string> allPlaFiles()
{
    std::vector<std::string> files;
    for (int i = 1; i <= 4; i++) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "../SatExamples/sat_ex_%d.pla", i);
        files.push_back(buf);
    }
    for (int n = 9; n <= 30; n++) {
        for (int v = 1; v <= 3; v++) {
            char buf[64];
            std::snprintf(buf, sizeof(buf), "../SatExamples/Sat_ex%d_%d.pla", n, v);
            files.push_back(buf);
        }
    }
    return files;
}


void test_firstfree_returns_column_zero_when_none_masked()
{
    std::cout << "\n[FirstFreeColumnBranchingStrategy::ChooseColumn]\n";

    std::vector<BoolInterval *> ivs;
    ivs.push_back(new BoolInterval("010"));
    ivs.push_back(new BoolInterval("101"));
    BoolInterval *root = new BoolInterval((size_t)3);
    BBV mask(3);
    BoolEquation eq(ivs.data(), root, 2, 2, mask);

    FirstFreeColumnBranchingStrategy strat;
    CHECK(strat.ChooseColumn(eq) == 0);

    delete root;
    for (auto *iv : ivs) delete iv;
}

void test_firstfree_skips_masked_prefix()
{
    std::vector<BoolInterval *> ivs;
    ivs.push_back(new BoolInterval("0101"));
    BoolInterval *root = new BoolInterval((size_t)4);
    BBV mask(4);
    BoolEquation eq(ivs.data(), root, 1, 1, mask);
    eq.mask.Set1(0);
    eq.mask.Set1(1);

    FirstFreeColumnBranchingStrategy strat;
    CHECK(strat.ChooseColumn(eq) == 2);

    delete root;
    for (auto *iv : ivs) delete iv;
}

void test_firstfree_returns_minus_one_when_all_masked()
{
    std::vector<BoolInterval *> ivs;
    ivs.push_back(new BoolInterval("01"));
    BoolInterval *root = new BoolInterval((size_t)2);
    BBV mask(2);
    BoolEquation eq(ivs.data(), root, 1, 1, mask);
    eq.mask.Set1(0);
    eq.mask.Set1(1);

    FirstFreeColumnBranchingStrategy strat;
    CHECK(strat.ChooseColumn(eq) == -1);

    delete root;
    for (auto *iv : ivs) delete iv;
}


void test_mindontcare_picks_column_with_fewest_dashes()
{
    std::cout << "\n[MinDontCareBranchingStrategy::ChooseColumn]\n";

    std::vector<BoolInterval *> ivs;
    ivs.push_back(new BoolInterval("-0-"));
    ivs.push_back(new BoolInterval("-01"));
    ivs.push_back(new BoolInterval("001"));
    BoolInterval *root = new BoolInterval((size_t)3);
    BBV mask(3);
    BoolEquation eq(ivs.data(), root, 3, 3, mask);

    MinDontCareBranchingStrategy strat;
    CHECK(strat.ChooseColumn(eq) == 1);

    delete root;
    for (auto *iv : ivs) delete iv;
}

void test_mindontcare_avoids_column_full_of_dashes()
{
    std::vector<BoolInterval *> ivs;
    ivs.push_back(new BoolInterval("0-"));
    ivs.push_back(new BoolInterval("1-"));
    BoolInterval *root = new BoolInterval((size_t)2);
    BBV mask(2);
    BoolEquation eq(ivs.data(), root, 2, 2, mask);

    MinDontCareBranchingStrategy strat;
    CHECK(strat.ChooseColumn(eq) == 0);

    delete root;
    for (auto *iv : ivs) delete iv;
}

void test_mindontcare_tie_goes_to_first_index()
{
    std::vector<BoolInterval *> ivs;
    ivs.push_back(new BoolInterval("--"));
    ivs.push_back(new BoolInterval("--"));
    BoolInterval *root = new BoolInterval((size_t)2);
    BBV mask(2);
    BoolEquation eq(ivs.data(), root, 2, 2, mask);

    MinDontCareBranchingStrategy strat;
    CHECK(strat.ChooseColumn(eq) == 0);

    delete root;
    for (auto *iv : ivs) delete iv;
}

void test_mindontcare_returns_minus_one_when_all_masked()
{
    std::vector<BoolInterval *> ivs;
    ivs.push_back(new BoolInterval("01"));
    BoolInterval *root = new BoolInterval((size_t)2);
    BBV mask(2);
    BoolEquation eq(ivs.data(), root, 1, 1, mask);
    eq.mask.Set1(0);
    eq.mask.Set1(1);

    MinDontCareBranchingStrategy strat;
    CHECK(strat.ChooseColumn(eq) == -1);

    delete root;
    for (auto *iv : ivs) delete iv;
}


void test_strategy_performance()
{
    std::cout << "\n[Strategy performance on all PLA files]\n";

    auto files = allPlaFiles();

    MinDontCareBranchingStrategy mdc;
    FirstFreeColumnBranchingStrategy ffc;

    long long total_mdc = 0, total_ffc = 0;
    int solved = 0, skipped = 0;

    printf("  %-28s  %10s  %10s  %s\n", "file", "mdc(us)", "ffc(us)", "result");
    std::cout << "  " << std::string(60, '-') << "\n";

    for (auto &path : files) {
        auto clauses = readPLA(path);
        if (clauses.empty()) {
            ++skipped;
            continue;
        }

        int n   = (int)clauses[0].size();
        int cnt = (int)clauses.size();

        bool sat_mdc;
        long long us_mdc;
        {
            std::vector<BoolInterval *> ivs;
            for (auto &c : clauses) ivs.push_back(new BoolInterval(c.c_str()));
            BoolInterval *root = new BoolInterval((size_t)n);
            BBV mask(n);
            BoolEquation eq(ivs.data(), root, cnt, cnt, mask);
            auto t0 = std::chrono::steady_clock::now();
            sat_mdc = solveDPLL(&eq, mdc);
            auto t1 = std::chrono::steady_clock::now();
            us_mdc = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
            delete root;
            for (auto *iv : ivs) delete iv;
        }

        bool sat_ffc;
        long long us_ffc;
        {
            std::vector<BoolInterval *> ivs;
            for (auto &c : clauses) ivs.push_back(new BoolInterval(c.c_str()));
            BoolInterval *root = new BoolInterval((size_t)n);
            BBV mask(n);
            BoolEquation eq(ivs.data(), root, cnt, cnt, mask);
            auto t0 = std::chrono::steady_clock::now();
            sat_ffc = solveDPLL(&eq, ffc);
            auto t1 = std::chrono::steady_clock::now();
            us_ffc = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
            delete root;
            for (auto *iv : ivs) delete iv;
        }

        CHECK(sat_mdc == sat_ffc);

        total_mdc += us_mdc;
        total_ffc += us_ffc;
        ++solved;

        std::string fname = path;
        size_t slash = fname.rfind('/');
        if (slash != std::string::npos) fname = fname.substr(slash + 1);

        printf("  %-28s  %10lld  %10lld  %s\n",
               fname.c_str(), us_mdc, us_ffc, sat_mdc ? "SAT" : "UNSAT");
    }

    std::cout << "  " << std::string(60, '-') << "\n";
    printf("  %-28s  %10lld  %10lld\n", "TOTAL", total_mdc, total_ffc);
    printf("  files solved: %d  skipped: %d\n", solved, skipped);

    CHECK(solved > 0);
    CHECK(total_mdc < 60000000LL);
    CHECK(total_ffc < 60000000LL);
}

int main()
{
    test_firstfree_returns_column_zero_when_none_masked();
    test_firstfree_skips_masked_prefix();
    test_firstfree_returns_minus_one_when_all_masked();

    test_mindontcare_picks_column_with_fewest_dashes();
    test_mindontcare_avoids_column_full_of_dashes();
    test_mindontcare_tie_goes_to_first_index();
    test_mindontcare_returns_minus_one_when_all_masked();

    test_strategy_performance();

    std::cout << "Results: " << g_passed << " passed, " << g_failed << " failed\n";
    return g_failed > 0 ? 1 : 0;
}
