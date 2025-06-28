#pragma once

#include "fetch_hyperliquid_bbo.h"  // For BBOLevel struct
#include <string_view>
#include <vector>
#include <cstdlib>
#include <iostream>

namespace NPriv {
using StrPos = int32_t;
using RollingHash = uint64_t;

template <size_t keyLen, size_t windowSize>
struct Key {
    static_assert(keyLen == windowSize, "Key length and window size must match!");
    static constexpr StrPos KeyLen = keyLen;
    static constexpr StrPos WINDOW_SIZE = windowSize;

    const RollingHash keyHash;
    StrPos relPos = 0;

    Key(const std::string_view key, StrPos initialRelPos) : keyHash(calcHashFrom(key, 0)), relPos(initialRelPos) {}

    StrPos findIn(const std::string_view json, StrPos prevKeyPos) const {
        StrPos absKeyPos = -1;
        for (StrPos left = std::min<StrPos>(prevKeyPos + relPos, (StrPos)json.size() - KeyLen), right = left + 1;
             left >= 0 || right + KeyLen <= json.size();
             --left, ++right) {
            if (left >= 0 && keyHash == calcHashFrom(json, left)) { absKeyPos = left; break; }
            if (right + KeyLen <= json.size() && keyHash == calcHashFrom(json, right)) { absKeyPos = right; break; }
        }
        if (absKeyPos == -1) {
            std::cerr << "Failed to find key in JSON!\n";
            std::abort();
        }
        return absKeyPos;
    }

private:
    static constexpr int getShift(int pos) { return 5 * (1 + pos); }
    static RollingHash calcHashFrom(const std::string_view str, const StrPos start) {
        RollingHash hash = 0;
        for (StrPos windowEnd = start; windowEnd - start != WINDOW_SIZE; ++windowEnd)
            hash += RollingHash(str[windowEnd]) << getShift(windowEnd - start);
        return hash;
    }
};
} // namespace NPriv

class HyperliquidParser {
public:
    BBOLevel parseLevel(std::string_view json) {
        return {
            .price = parsePriceKey(json, 0, priceKey),
            .size  = parseSizeKey(json, priceKey.relPos, sizeKey),
            .num_orders = parseIntKeyAndUpdateRelPos(json, priceKey.relPos + sizeKey.relPos, numOrdersKey)
        };
    }

private:
    using StrPos = NPriv::StrPos;

    static constexpr std::string_view PRICE_KEY = "px\":\"";   // len 5
    static constexpr std::string_view SIZE_KEY  = "sz\":\"";   // len 5
    static constexpr std::string_view N_KEY     = "\"n\":";    // len 4

    NPriv::Key<PRICE_KEY.size(), PRICE_KEY.size()> priceKey   = {PRICE_KEY, 0};
    NPriv::Key<SIZE_KEY.size(), SIZE_KEY.size()>   sizeKey    = {SIZE_KEY, PRICE_KEY.size() + 1};
    NPriv::Key<N_KEY.size(), N_KEY.size()>         numOrdersKey = {N_KEY, SIZE_KEY.size() + 1};

    static StrPos skipToValue(std::string_view json, StrPos start) {
        for (; start < json.size() && (json[start] == ':' || json[start] == ' ' || json[start] == '"' || json[start] == '\\'); ++start)
            ;
        return start;
    }

    static double parseFixedDecimal2(std::string_view str, StrPos start) {
        int64_t intPart = 0, fracPart = 0;
        while (start < str.size() && str[start] >= '0' && str[start] <= '9') {
            intPart = intPart * 10 + (str[start] - '0');
            ++start;
        }
        if (start < str.size() && str[start] == '.') {
            ++start;
            for (int i = 0; i < 2; ++i) {
                fracPart *= 10;
                if (start < str.size() && str[start] >= '0' && str[start] <= '9') {
                    fracPart += (str[start] - '0');
                    ++start;
                }
            }
        }
        return intPart + (static_cast<double>(fracPart) / 100.0);
    }

    static double parseFixedDecimal4(std::string_view str, StrPos start) {
        int64_t intPart = 0, fracPart = 0;
        while (start < str.size() && str[start] >= '0' && str[start] <= '9') {
            intPart = intPart * 10 + (str[start] - '0');
            ++start;
        }
        if (start < str.size() && str[start] == '.') {
            ++start;
            for (int i = 0; i < 4; ++i) {
                fracPart *= 10;
                if (start < str.size() && str[start] >= '0' && str[start] <= '9') {
                    fracPart += (str[start] - '0');
                    ++start;
                }
            }
        }
        return intPart + (static_cast<double>(fracPart) / 10000.0);
    }

    template <class TKey>
    static double parsePriceKey(std::string_view json, const StrPos prevKeyPos, TKey& key) {
        const StrPos absKeyPos = key.findIn(json, prevKeyPos);
        key.relPos = absKeyPos - prevKeyPos;
        const StrPos start = skipToValue(json, absKeyPos + TKey::KeyLen);
        return parseFixedDecimal2(json, start);
    }

    template <class TKey>
    static double parseSizeKey(std::string_view json, const StrPos prevKeyPos, TKey& key) {
        const StrPos absKeyPos = key.findIn(json, prevKeyPos);
        key.relPos = absKeyPos - prevKeyPos;
        const StrPos start = skipToValue(json, absKeyPos + TKey::KeyLen);
        return parseFixedDecimal4(json, start);
    }

    template <class TKey>
    static int parseIntKeyAndUpdateRelPos(std::string_view json, const StrPos prevKeyPos, TKey &key) {
        const StrPos absKeyPos = key.findIn(json, prevKeyPos);
        key.relPos = absKeyPos - prevKeyPos;
        const StrPos start = skipToValue(json, absKeyPos + TKey::KeyLen);
        return parseInt(json, start);
    }

    static int parseInt(std::string_view str, StrPos start) {
        int result = 0;
        for (; start < str.size() && str[start] >= '0' && str[start] <= '9'; ++start)
            result = result * 10 + (str[start] - '0');
        return result;
    }
};
