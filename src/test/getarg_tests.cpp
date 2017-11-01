#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-etg");
    BOOST_CHECK(GetBoolArg("-etg"));
    BOOST_CHECK(GetBoolArg("-etg", false));
    BOOST_CHECK(GetBoolArg("-etg", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-fooo"));
    BOOST_CHECK(!GetBoolArg("-fooo", false));
    BOOST_CHECK(GetBoolArg("-fooo", true));

    ResetArgs("-etg=0");
    BOOST_CHECK(!GetBoolArg("-etg"));
    BOOST_CHECK(!GetBoolArg("-etg", false));
    BOOST_CHECK(!GetBoolArg("-etg", true));

    ResetArgs("-etg=1");
    BOOST_CHECK(GetBoolArg("-etg"));
    BOOST_CHECK(GetBoolArg("-etg", false));
    BOOST_CHECK(GetBoolArg("-etg", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nofoo");
    BOOST_CHECK(!GetBoolArg("-etg"));
    BOOST_CHECK(!GetBoolArg("-etg", false));
    BOOST_CHECK(!GetBoolArg("-etg", true));

    ResetArgs("-nofoo=1");
    BOOST_CHECK(!GetBoolArg("-etg"));
    BOOST_CHECK(!GetBoolArg("-etg", false));
    BOOST_CHECK(!GetBoolArg("-etg", true));

    ResetArgs("-etg -nofoo");  // -etg should win
    BOOST_CHECK(GetBoolArg("-etg"));
    BOOST_CHECK(GetBoolArg("-etg", false));
    BOOST_CHECK(GetBoolArg("-etg", true));

    ResetArgs("-etg=1 -nofoo=1");  // -etg should win
    BOOST_CHECK(GetBoolArg("-etg"));
    BOOST_CHECK(GetBoolArg("-etg", false));
    BOOST_CHECK(GetBoolArg("-etg", true));

    ResetArgs("-etg=0 -nofoo=0");  // -etg should win
    BOOST_CHECK(!GetBoolArg("-etg"));
    BOOST_CHECK(!GetBoolArg("-etg", false));
    BOOST_CHECK(!GetBoolArg("-etg", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--etg=1");
    BOOST_CHECK(GetBoolArg("-etg"));
    BOOST_CHECK(GetBoolArg("-etg", false));
    BOOST_CHECK(GetBoolArg("-etg", true));

    ResetArgs("--nofoo=1");
    BOOST_CHECK(!GetBoolArg("-etg"));
    BOOST_CHECK(!GetBoolArg("-etg", false));
    BOOST_CHECK(!GetBoolArg("-etg", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-etg", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-etg", "eleven"), "eleven");

    ResetArgs("-etg -bar");
    BOOST_CHECK_EQUAL(GetArg("-etg", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-etg", "eleven"), "");

    ResetArgs("-etg=");
    BOOST_CHECK_EQUAL(GetArg("-etg", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-etg", "eleven"), "");

    ResetArgs("-etg=11");
    BOOST_CHECK_EQUAL(GetArg("-etg", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-etg", "eleven"), "11");

    ResetArgs("-etg=eleven");
    BOOST_CHECK_EQUAL(GetArg("-etg", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-etg", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-etg", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-etg", 0), 0);

    ResetArgs("-etg -bar");
    BOOST_CHECK_EQUAL(GetArg("-etg", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-etg=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-etg", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-etg=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-etg", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--etg");
    BOOST_CHECK_EQUAL(GetBoolArg("-etg"), true);

    ResetArgs("--etg=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-etg", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nofoo");
    BOOST_CHECK(!GetBoolArg("-etg"));
    BOOST_CHECK(!GetBoolArg("-etg", true));
    BOOST_CHECK(!GetBoolArg("-etg", false));

    ResetArgs("-nofoo=1");
    BOOST_CHECK(!GetBoolArg("-etg"));
    BOOST_CHECK(!GetBoolArg("-etg", true));
    BOOST_CHECK(!GetBoolArg("-etg", false));

    ResetArgs("-nofoo=0");
    BOOST_CHECK(GetBoolArg("-etg"));
    BOOST_CHECK(GetBoolArg("-etg", true));
    BOOST_CHECK(GetBoolArg("-etg", false));

    ResetArgs("-etg --nofoo");
    BOOST_CHECK(GetBoolArg("-etg"));

    ResetArgs("-nofoo -etg"); // etg always wins:
    BOOST_CHECK(GetBoolArg("-etg"));
}

BOOST_AUTO_TEST_SUITE_END()
