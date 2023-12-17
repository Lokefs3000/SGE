#include "pch.h"
#include "CompressionUtility.h"

#include <sstream>

#include <zlib/zlib.h>

#include "DebugMessenger.h"

bool CompressionUtility::Compress(std::vector<char>& data, std::vector<char>& result)
{
    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    int ret = deflateInit(&zs, Z_BEST_COMPRESSION);
    if (ret != Z_OK) {
        DebugError("Failed to deflateInit! Code: ", ret);
        return false;
    }

    zs.next_in = (Bytef*)data.data();
    zs.avail_in = data.size();           // set the z_stream's input

    char outbuffer[32768];

    // retrieve the compressed bytes blockwise
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (result.size() < zs.total_out) {
            result.resize(zs.total_out);
            memcpy(result.data(), outbuffer + result.size(), zs.total_out - result.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        DebugError("Zlib stream error occured! Code: ", ret, ", Message: ", zs.msg);
        return false;
    }

    return true;
}

bool CompressionUtility::Decompress(std::vector<char>& data, std::vector<char>& result)
{
    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    int ret = inflateInit(&zs);
    if (ret != Z_OK) {
        DebugError("Failed to inflateInit! Code: ", ret);
        return false;
    }

    zs.next_in = (Bytef*)data.data();
    zs.avail_in = data.size();

    char outbuffer[32768];

    // get the decompressed bytes blockwise using repeated calls to inflate
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (result.size() < zs.total_out) {
            result.resize(zs.total_out);
            memcpy(result.data(), outbuffer + result.size(), zs.total_out - result.size());
        }
    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        DebugError("Zlib stream error occured! Code: ", ret, ", Message: ", zs.msg);
        return false;
    }

    return true;
}
