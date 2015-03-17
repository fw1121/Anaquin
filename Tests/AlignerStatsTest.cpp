#include "gtest/gtest.h"
#include "AlignerAnalyst.hpp"

using namespace std;

TEST(Generated_1000, AlignerStatsTest)
{
    const auto stats = AlignerAnalyst::base("/Users/user1/Sources/ABCD/aligned_output/accepted_hits.sam", Sequins(), 1000);

    ASSERT_EQ(1, stats.m.sp());
    ASSERT_TRUE(isnan(stats.m.sn()));
    ASSERT_EQ(1000, stats.m.tp);
    ASSERT_EQ(0, stats.m.fp);
    ASSERT_EQ(0, stats.m.fn);
    ASSERT_EQ(0, stats.m.tn);
    ASSERT_EQ(1000, stats.nr);
    ASSERT_EQ(1, stats.dilution);
    ASSERT_EQ(0, stats.nq);
}

TEST(Cufflink, AlignerStatsTest)
{
	// The sample file was taken from Cufflink's source distribution. It's obviously independent to the standards.
	const auto stats = AlignerAnalyst::base("/Users/user1/Sources/QA/Tests/Data/CufflinksTest.sam");
	//const auto stats = AlignerAnalyst::analyze("C://Sources//QA//Tests//Data//CufflinksTest.sam");

	/*
	 * There shouldn't be any match. Both sensivity and specificity are NAN because the experiement gives no
     * power to detect anything.
	 */

	ASSERT_TRUE(isnan(stats.m.sp()));
	ASSERT_EQ(1, stats.m.sn());
	ASSERT_EQ(0, stats.m.tp);
	ASSERT_EQ(0, stats.m.fp);
	ASSERT_EQ(0, stats.m.fn);
	ASSERT_EQ(3271, stats.m.tn);
	ASSERT_EQ(0, stats.nr);
    ASSERT_EQ(0, stats.dilution);
	ASSERT_EQ(3307, stats.nq);
}