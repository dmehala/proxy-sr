#include <gtest/gtest.h>
#include <tuple>
#include "proxy/helper.hpp"

TEST(Helper, cutTestSimple)
{
    const std::string input { "https://bitdash-a.akamaihd.net/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8" };
    std::string s1, s2;
    // std::tie(s1, s2) = helper::cut(input, '/', std::next(input.cbegin(), 8), input.cend());
    std::tie(s1, s2) = helper::cut(input, '/');
    EXPECT_EQ("https:", s1);
    EXPECT_EQ("//bitdash-a.akamaihd.net/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8", s2);
}

TEST(Helper, cutTestBegining)
{
    const std::string input { "https://bitdash-a.akamaihd.net/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8" };
    const auto expected = std::make_pair<std::string, std::string>("https://bitdash-a.akamaihd.net", "/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8");
    EXPECT_EQ(expected, helper::cut(input, '/', 8));
}