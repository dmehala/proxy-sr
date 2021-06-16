#include <gtest/gtest.h>
#include "proxy/helper.hpp"

using ExpectedCut = std::pair<std::string, std::string>;

TEST(Helper, cutTestEmptyInput)
{
    EXPECT_EQ(ExpectedCut("", ""), helper::cut("", ':'));
}

TEST(Helper, cutTestMissingDelimiter)
{
    const std::string input { "23:41" };
    EXPECT_EQ(ExpectedCut("", ""), helper::cut(input, '.'));
}

TEST(Helper, cutTestBegAfterEnd)
{
    EXPECT_EQ(ExpectedCut("", ""), helper::cut("23:43", ':', 1, 0));
}

TEST(Helper, cutTestSimple)
{
    const std::string input { "https://bitdash-a.akamaihd.net/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8" };
    EXPECT_EQ(ExpectedCut("https:", "//bitdash-a.akamaihd.net/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8"), helper::cut(input, '/'));
    EXPECT_EQ(ExpectedCut("https://bitdash-a.akamaihd.net", "/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa.m3u8"), helper::cut(input, '/', 8));
}

TEST(Helper, extractUriTestEmptyManifest)
{
    const auto result = helper::extract_uri_from_manifest("");
    ASSERT_TRUE(result.empty());
}

TEST(Helper, extractUriTestRealManifest)
{
    const std::string manifest_content = R"(
        #EXTM3U
        #EXT-X-VERSION:5

        #EXT-X-MEDIA:TYPE=AUDIO,GROUP-ID="audio",NAME="English stereo",LANGUAGE="en",AUTOSELECT=YES,URI="f08e80da-bf1d-4e3d-8899-f0f6155f6efa_audio_1_stereo_128000.m3u8"

        #EXT-X-STREAM-INF:BANDWIDTH=628000,CODECS="avc1.42c00d,mp4a.40.2",RESOLUTION=320x180,AUDIO="audio"
        f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_180_250000.m3u8
        #EXT-X-STREAM-INF:BANDWIDTH=928000,CODECS="avc1.42c00d,mp4a.40.2",RESOLUTION=480x270,AUDIO="audio"
        f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_270_400000.m3u8
        #EXT-X-STREAM-INF:BANDWIDTH=1728000,CODECS="avc1.42c00d,mp4a.40.2",RESOLUTION=640x360,AUDIO="audio"
        f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_360_800000.m3u8
        #EXT-X-STREAM-INF:BANDWIDTH=2528000,CODECS="avc1.42c00d,mp4a.40.2",RESOLUTION=960x540,AUDIO="audio"
        f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_540_1200000.m3u8
        #EXT-X-STREAM-INF:BANDWIDTH=4928000,CODECS="avc1.42c00d,mp4a.40.2",RESOLUTION=1280x720,AUDIO="audio"
        f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_720_2400000.m3u8
        #EXT-X-STREAM-INF:BANDWIDTH=9728000,CODECS="avc1.42c00d,mp4a.40.2",RESOLUTION=1920x1080,AUDIO="audio"
        f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_1080_4800000.m3u8
    )";

    const std::vector<std::string> expected {
        "f08e80da-bf1d-4e3d-8899-f0f6155f6efa_audio_1_stereo_128000.m3u8",
        "f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_180_250000.m3u8",
        "f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_270_400000.m3u8",
        "f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_360_800000.m3u8",
        "f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_540_1200000.m3u8",
        "f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_720_2400000.m3u8",
        "f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_1080_4800000.m3u8"
    };

    const auto result = helper::extract_uri_from_manifest(manifest_content);
    EXPECT_EQ(expected, result);
}

TEST(Helper, getExtensionTestEmpty)
{
    EXPECT_EQ("", helper::get_extension(""));
}

TEST(Helper, getExtensionTestNoExtension)
{
    EXPECT_EQ("", helper::get_extension("chunklist_b1128000"));
}

TEST(Helper, getExtensionTest)
{
    EXPECT_EQ(".m3u8", helper::get_extension("f08e80da-bf1d-4e3d-8899-f0f6155f6efa_audio_1_stereo_128000.m3u8"));
    EXPECT_EQ(".txt", helper::get_extension("hello_world.txt"));
}

TEST(Helper, resolveUri)
{
    EXPECT_EQ("/content/MI201109210084_1/video/180_250000/hls/segment_4.ts", helper::resolve_uri("/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_180_250000.m3u8", "../video/180_250000/hls/segment_4.ts"));
    EXPECT_EQ("/content/MI201109210084_1/m3u8s/seq-0.ts", helper::resolve_uri("/content/MI201109210084_1/m3u8s/f08e80da-bf1d-4e3d-8899-f0f6155f6efa_video_180_250000.m3u8", "seq-0.ts"));
}