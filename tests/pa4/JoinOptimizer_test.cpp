#include <gtest/gtest.h>
#include <db/JoinOptimizer.h>

class JoinOptimizerImpl : public db::JoinOptimizer {
    db::CostCard *computeCostAndCardOfSubplan(std::unordered_map<std::string, db::TableStats> stats,
                                              std::unordered_map<std::string, double> filterSelectivities,
                                              db::LogicalJoinNode joinToRemove,
                                              std::unordered_set<db::LogicalJoinNode> joinSet, double bestCostSoFar,
                                              db::PlanCache pc) override {
        return nullptr;
    }

public:
    JoinOptimizerImpl(std::vector<db::LogicalJoinNode> &joins) : db::JoinOptimizer(joins) {}
};

TEST(JoinOptimizerTest, estimateJoinCost) {
    auto x = JoinOptimizerImpl::estimateJoinCost(1229, 1381, 1523, 1663);
    EXPECT_TRUE(x == double(1523) + 1229 * double(1663) + 1229 * 1381 or x == double(1523) + 1229 * double(1663));
}

void testJoinCardinality(JoinOptimizerImpl &opt, db::Predicate::Op op, int card1, int card2, bool t1pkey, bool t2pkey, double expected) {
    EXPECT_NEAR(opt.estimateTableJoinCardinality(op, "", "", "", "", card1, card2, t1pkey, t2pkey, {}, {}), expected, 0.001);
}

TEST(JoinOptimizerTest, estimateTableJoinCardinality) {
    std::vector<db::LogicalJoinNode> joins;
    JoinOptimizerImpl opt(joins);
    testJoinCardinality(opt, db::Predicate::Op::EQUALS, 1229, 1381, true, true, 1229);
    testJoinCardinality(opt, db::Predicate::Op::EQUALS, 1381, 1229, true, true, 1229);
    testJoinCardinality(opt, db::Predicate::Op::EQUALS, 1381, 1229, true, false, 1229);
    testJoinCardinality(opt, db::Predicate::Op::EQUALS, 1381, 1229, false, true, 1381);
    testJoinCardinality(opt, db::Predicate::Op::EQUALS, 1381, 1229, false, false, 1381);
    testJoinCardinality(opt, db::Predicate::Op::GREATER_THAN, 1381, 1229, false, false, int(1381 * 1229 * 0.3));
}
