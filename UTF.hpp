//see license (Apache)
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <cassert>

using u16string_view = std::basic_string_view<char16_t>;
using u32string_view = std::basic_string_view<char32_t>;

struct UTF {
    const uint8_t static MAXCHARLEN = 6;
    const uint16_t static REPLACEMENT = 0xfffd;
    int errors = 0;
/**
 int errambig: check ambiguity against hacker attacks
 some chars , for example ASCII / can be expressed as:
    00101111
    11000000 10101111
    11100000 10000000 10101111i
    etc..
 * */
    int errambig = 0;
    const static char32_t MaxCP = 0x10ffff;

    static char16_t swap16(char16_t c) {
        return ((c & 0xFF) << 8) | ((c & 0xFF00) >> 8);
    }

    static char32_t swap32(char32_t c) {
        return ((c & 0xFFFF) << 16) | ((c & 0xFFFF0000) >> 16);
    }

    static char32_t reverse32(char32_t c) {
        return ((c & 0xFF) << 24) | ((c & 0xFF00) << 8) | ((c & 0xFF0000) >> 8) | ((c & 0xFF000000) >> 24);
    }

    static void swapIt(std::u16string &u16) {
        for (auto &c : u16)
            c = swap16(c);
    }

    static void reverseIt(std::u32string &u32) {
        for (auto &c : u32)
            c = reverse32(c);
    }

    static const char *strend(const char *s) {
        while (*s)
            s++;
        return s;
    }

    static const char16_t *strend(const char16_t *s) {
        while (*s)
            s++;
        return s;
    }

    static std::u32string substr32(const u32string_view dstr, int64_t start, int64_t len) {
        if (start < 0) {
            len += start;
            start = 0;
        }
        if (len <= 0) return {};
        int64_t end = start + len;
        end = std::min(end, (int64_t) dstr.size());
        auto first = dstr.begin() + start;
        auto last = dstr.begin() + end;
        std::u32string result(first, last);
        return result;
    }

    static uint8_t one8len(char c) {
        uint8_t b0 = c;
        if ((b0 & 0x80) == 0)
            return 1;
        else if ((b0 & 0x20) == 0)
            return 2;
        else if ((b0 & 0x10) == 0)
            return 3;
        return 4;
    }

    static uint8_t one8len(char32_t d) {
        if (d <= 0x7f)
            return 1;
        else if (d <= 0x7ff)
            return 2;
        else if (d <= 0xffff)
            return 3;
        else
            return 4;
    }

    static uint8_t one16len(char32_t d) {
        if (d < 0x10000)
            return 1;
        else
            return 2;
    }

    static bool isSurrogate1(char32_t w) {
        return w >= 0xD800 && w <= 0xDBFF;
    }
    static bool isSurrogate2(char32_t w) {
        return w >= 0xDC00 && w <= 0xDFFF;
    }

    static bool isSurrogate(char32_t w) {
        return isSurrogate1(w) || isSurrogate2(w);
    }

    static uint8_t one16len(char16_t wc) {
        auto w = (uint16_t) wc;
        if (w >= 0xD800 && w <= 0xDBFF)
            return 2;
        else
            return 1;
    }

    static bool insideU8code(unsigned char b) {
        return (b & 0b11000000) == 0b10000000;
    }
    /*
     * By first byte:
     * 0: inside UTF8 multibyte
     * 1: ASCII
     * 2..6 len
     * 7 bad 11111110
     * 8 bad 11111111
     * */
    static uint8_t determineU8Len(uint8_t b) {
        if ((b & 0b10000000) == 0) return 1;
        if (insideU8code(b)) return 0;
        uint8_t mask = 0b00100000;
        for (uint8_t len = 2; len < 8; len++) {
            if ((b & mask) == 0) return len;
            mask >>= 1;
        }
        return 8;
    }

    /* not check ambiguity in this stage */
    static bool isCorrectU8code(const char *s, const char *eos, uint8_t &len) {
        len = 1;
        if (insideU8code(*s))
            return false;
        uint8_t expectLen = determineU8Len(*s);
        if (expectLen > MAXCHARLEN)
            return false;
        for (uint8_t i = 2; i <= expectLen; i++) {
            s++;
            if (s >= eos)
                return false;
            if (!insideU8code(*s))
                return false;
            len++;
        }
        return true;
    }

    char32_t one8to32(const char *s, const char *eos, const char **end) {
        uint8_t len;
        bool isOK = isCorrectU8code(s, eos, len);
        *end = s + len;
        if (!isOK) {
            errors++;
            return REPLACEMENT;
        }
        if (len == 1)
            return *s;
        else {
            assert(len > 1 && len <= MAXCHARLEN);
            uint8_t mask0 = 127 >> len;
            char32_t d = *s & mask0;
            int minimal = 0;
            for (uint8_t i = 1; i < len; i++) {
                d = (d << 6) | s[i] & 63;
                if (minimal == 0)
                    minimal = 0x80;
                else if (minimal == 0x80)
                    minimal = 0x800;
                else
                    minimal *= 2;
            }
            if (d < minimal) {
                errambig++;
                errors++;
                return REPLACEMENT;
            }
            return d;
        }
    }

    uint8_t one32to16(char32_t d, char16_t *buf) {
        if (isSurrogate(d) || d > MaxCP) {
            d = REPLACEMENT;
            errors++;
        }
        if (d < 0x10000) {
            buf[0] = (char16_t) d;
            return 1;
        } else {
            buf[0] = (char16_t) ((d - 0x10000) / 0x400 + 0xD800);
            buf[1] = (char16_t) ((d - 0x10000) % 0x400 + 0xDC00);
            return 2;
        }
    }

    int64_t getU32Len(const char *s, const char *eos) {
        int64_t result = 0;
        while (s < eos) {
            char32_t d = one8to32(s, eos, &s);
            result++;
        }
        return result;
    }

    int64_t getU32Len(const std::string_view str) {
        int64_t result = 0;
        const char *s;
        const char *sc = s = str.data();
        const char *eos = sc + str.length();
        while (s - sc < str.size()) {
            char32_t d = one8to32(s, eos, &s);
            result++;
        }
        return result;
    }

    static int64_t getU32Len(const u16string_view wstr) {
        int64_t result = 0;
        int64_t n = 0;
        while (n < (int64_t) wstr.size()) {
            result++;
            n += one16len(wstr[n]);
        }
        return result;
    }

    int64_t getU16Len(const std::string_view strView) {
        int64_t result = 0;
        const char *s = strView.data();
        const char *eos = strView.data() + strView.length();
        while (s < eos) {
            char32_t d = one8to32(s, eos, &s);
            result += one16len(d);
        }
        return result;
    }

    int64_t getU16Len(const char *str) {
        int64_t result = 0;
        const char *s = str;
        const char *eos = strend(str);
        while (s < eos) {
            char32_t d = one8to32(s, eos, &s);
            result += one16len(d);
        }
        return result;
    }

    int64_t getU16LenSubstr(const std::string_view strView, int64_t start, int64_t subLen) {
        if (start < 0) {
            subLen += start;
            start = 0;
        }
        if (subLen <= 0) return 0;
        int64_t result = 0;
        const char *s = strView.data();
        const char *eos = strView.data() + strView.length();
        int64_t dcounter = 0;
        while (s < eos) {
            char32_t d = one8to32(s, eos, &s);
            if (dcounter >= start)
                result += one16len(d);
            dcounter++;
            if (dcounter == start + subLen)
                return result;
        }
        return result;
    }

    int64_t getU32LenSubstr(const std::string_view strView, int64_t start, int64_t subLen) {
        if (start < 0) {
            subLen += start;
            start = 0;
        }
        if (subLen <= 0) return 0;
        int64_t result = 0;
        const char *s = strView.data();
        const char *eos = strView.data() + strView.length();
        int64_t dcounter = 0;
        while (s < eos) {
            char32_t d = one8to32(s, eos, &s);
            if (dcounter >= start)
                result++;
            dcounter++;
            if (dcounter == start + subLen)
                return result;
        }
        return result;
    }

    int64_t getU8LenSubstr(const std::string_view strView, int64_t start, int64_t subLen) {
        if (start < 0) {
            subLen += start;
            start = 0;
        }
        if (subLen <= 0) return 0;
        int64_t result = 0;
        const char *s = strView.data();
        const char *eos = strView.data() + strView.length();
        int64_t dcounter = 0;
        while (s < eos) {
            char32_t d = one8to32(s, eos, &s);
            if (dcounter >= start)
                result += one8len(d);
            dcounter++;
            if (dcounter == start + subLen)
                return result;
        }
        return result;
    }

    std::string_view u8subview(const std::string_view view, int64_t start, int64_t subLen) {
        if (start < 0) {
            subLen += start;
            start = 0;
        }
        if (subLen <= 0) return {};
        const char *s = view.data();
        const char *eos = view.data() + view.length();
        int64_t dcounter = 0;
        const char *startView = nullptr;
        const char *endView = nullptr;
        while (s < eos) {
            if (dcounter >= start)
                if (!startView)
                    startView = s;
            char32_t d = one8to32(s, eos, &s);
            dcounter++;
            if (dcounter == start + subLen) {
                endView = s;
                break;
            }
        }
        if (!endView)
            endView = eos;
        std::string_view result(startView, endView - startView);
        return result;
    }

    static int64_t getU8Len(const u16string_view wstr) {
        const char16_t *wsc;
        const char16_t *ws = wsc = wstr.data();
        int64_t len = 0;
        while (ws - wsc < wstr.size()) {
            char32_t d = one16to32(ws, &ws);
            len += one8len(d);
        }
        return len;
    }

    static int64_t getU8Len(const char16_t *wstr) {
        const char16_t *ws = wstr;
        const char16_t *eos = strend(wstr);
        int64_t len = 0;
        while (ws  < eos) {
            char32_t d = one16to32(ws, &ws);
            len += one8len(d);
        }
        return len;
    }

    static int64_t getU8LenSubstr(const u16string_view wstr, int64_t start, int64_t subLen) {
        if (start < 0) {
            subLen += start;
            start = 0;
        }
        if (subLen <= 0)
            return 0;
        const char16_t *wsc;
        const char16_t *ws = wsc = wstr.data();
        int64_t len = 0;
        int64_t dcounter = 0;
        while (ws - wsc < wstr.size()) {
            char32_t d = one16to32(ws, &ws);
            if (dcounter >= start)
                len += one8len(d);
            dcounter++;
            if (dcounter == start + subLen) return len;
        }
        return len;
    }

    static int64_t getU16LenSubstr(const u16string_view wstrView, int64_t start, int64_t subLen) {
        if (start < 0) {
            subLen += start;
            start = 0;
        }
        if (subLen <= 0)
            return 0;
        const char16_t *ws = wstrView.data();
        const char16_t *eos = wstrView.data() + wstrView.length();
        int64_t len = 0;
        int64_t dcounter = 0;
        while (ws < eos) {
            char32_t d = one16to32(ws, &ws);
            if (dcounter >= start)
                len += one16len(d);
            dcounter++;
            if (dcounter == start + subLen) return len;
        }
        return len;
    }

    static u16string_view u16subview(const u16string_view wstrView, int64_t start, int64_t subLen) {
        if (start < 0) {
            subLen += start;
            start = 0;
        }
        if (subLen <= 0)
            return {};
        const char16_t *ws = wstrView.data();
        const char16_t *eos = wstrView.data() + wstrView.length();
        const char16_t *startView = nullptr;
        const char16_t *endView = nullptr;
        int64_t dcounter = 0;
        while (ws < eos) {
            if (dcounter >= start)
                if (!startView)
                    startView = ws;
            char32_t d = one16to32(ws, &ws);
            dcounter++;
            if (dcounter == start + subLen) {
                endView = ws;
                break;
            }
        }
        if (!endView)
            endView = eos;
        return u16string_view(startView, endView - startView);
    }

    static int64_t getU8Len(const std::u32string_view &dstr) {
        int64_t len8 = 0;
        for (int64_t i = 0; i < (int64_t) dstr.size(); i++)
            len8 += one8len(dstr[i]);
        return len8;
    }

    int64_t getU16Len(const std::u32string_view &dstr) const {
        int64_t len16 = 0;
        for (int64_t i = 0; i < (int64_t) dstr.size(); i++) {
            char32_t d = dstr[i];
            if (isSurrogate(d) || d > MaxCP) {
                d = REPLACEMENT;
            }
            if (d < 0x10000)
                len16++;
            else
                len16 += 2;
        }
        return len16;
    }

    std::u16string u8to16(const std::string_view strView) {
        std::u16string result;
        result.resize(getU16Len(strView));
        const char *s = strView.data();
        const char *eos = strView.data() + strView.length();
        int64_t len = 0;
        while (s < eos) {
            char32_t d = one8to32(s, eos, &s);
            char16_t pair[2];
            uint8_t k = one32to16(d, pair);
            result[len] = pair[0];
            if (k > 1)
                result[len + 1] = pair[1];
            len += k;
        }
        return result;
    }

    std::u16string u8to16(const char *str) {
        std::u16string result;
        result.resize(getU16Len(str));
        const char *s = str;
        const char *eos = strend(str);
        int64_t len = 0;
        while (s < eos) {
            char32_t d = one8to32(s, eos, &s);
            char16_t pair[2];
            uint8_t k = one32to16(d, pair);
            result[len] = pair[0];
            if (k > 1)
                result[len + 1] = pair[1];
            len += k;
        }
        return result;
    }

    std::u16string u8to16substr(const std::string_view strView, int64_t start, int64_t subLen) {
        if (start < 0) {
            subLen += start;
            start = 0;
        }
        if (subLen <= 0)
            return {};
        std::u16string result;
        result.resize(getU16LenSubstr(strView, start, subLen));
        const char *s = strView.data();
        const char *eos = strView.data() + strView.length();
        int64_t len = 0;
        int64_t dcounter = 0;
        while (s < eos) {
            char32_t d = one8to32(s, eos, &s);
            char16_t pair[2];
            if (dcounter >= start) {
                uint8_t k = one32to16(d, pair);
                result[len] = pair[0];
                if (k > 1)
                    result[len + 1] = pair[1];
                len += k;
            }
            dcounter++;
            if (dcounter == start + subLen)
                return result;
        }
        return result;
    }

    std::u32string u8to32substr(const std::string_view strView, int64_t start, int64_t subLen) {
        if (start < 0) {
            subLen += start;
            start = 0;
        }
        if (subLen <= 0)
            return {};
        std::u32string result;
        result.resize(getU32LenSubstr(strView, start, subLen));
        const char *s = strView.data();
        const char *eos = strView.data() + strView.length();
        int64_t len = 0;
        int64_t dcounter = 0;
        while (s < eos) {
            char32_t d = one8to32(s, eos, &s);
            if (dcounter >= start) {
                result[len] = d;
                len++;
            }
            dcounter++;
            if (dcounter == start + subLen)
                break;
        }
        return result;
    }

    std::string u8to8substr(const std::string_view strView, int64_t start, int64_t subLen) {
        if (start < 0) {
            subLen += start;
            start = 0;
        }
        if (subLen <= 0)
            return {};
        std::string result;
        result.resize(getU8LenSubstr(strView, start, subLen));
        const char *s = strView.data();
        const char *eos = strView.data() + strView.length();
        int64_t len = 0;
        int64_t dcounter = 0;
        while (s < eos) {
            char32_t d = one8to32(s, eos, &s);
            if (dcounter >= start) {
                char buf[4];
                uint8_t k = one32to8(d, buf);
                for (uint8_t i = 0; i < k; i++) {
                    result[len] = buf[i];
                    len++;
                }
            }
            dcounter++;
            if (dcounter == start + subLen)
                return result;
        }
        return result;
    }

    std::u32string u8to32(const std::string_view str) {
        const char *sc = str.data();
        const char *eos = sc + str.length();
        std::u32string result;
        result.resize(getU32Len(str));
        errors = errambig = 0;
        for (int64_t i = 0; i < (int64_t) result.size(); i++) {
            result[i] = one8to32(sc, eos, &sc);
        }
        assert(sc == str.data() + str.size());
        return result;
    }

    std::string u16to8(const u16string_view wstr) {
        std::string result;
        result.resize(getU8Len(wstr));
        const char16_t *wsc;
        const char16_t *ws = wsc = wstr.data();
        int64_t len = 0;
        while (ws - wsc < wstr.size()) {
            char32_t d = one16to32(ws, &ws);
            char buf[4];
            uint8_t k = one32to8(d, buf);
            for (uint8_t i = 0; i < k; i++) {
                result[len] = buf[i];
                len++;
            }
        }
        return result;
    }

    std::string u16to8(const char16_t *wstr) {
        std::string result;
        result.resize(getU8Len(wstr));
        const char16_t *ws = wstr;
        const char16_t *eos = strend(wstr);
        int64_t len = 0;
        while (ws < eos) {
            char32_t d = one16to32(ws, &ws);
            char buf[4];
            uint8_t k = one32to8(d, buf);
            for (uint8_t i = 0; i < k; i++) {
                result[len] = buf[i];
                len++;
            }
        }
        return result;
    }

    std::string u16to8substr(const u16string_view wstr, int64_t start, int64_t subLen) {
        if (start < 0) {
            subLen += start;
            start = 0;
        }
        if (subLen <= 0)
            return {};
        std::string result;
        result.resize(getU8LenSubstr(wstr, start, subLen));
        const char16_t *wsc;
        const char16_t *ws = wsc = wstr.data();
        int64_t len = 0;
        int64_t dcounter = 0;
        while (ws - wsc < wstr.size()) {
            char32_t d = one16to32(ws, &ws);
            if (dcounter >= start) {
                char buf[4];
                uint8_t k = one32to8(d, buf);
                for (uint8_t i = 0; i < k; i++) {
                    result[len] = buf[i];
                    len++;
                }
            }
            dcounter++;
            if (dcounter == start + subLen)
                return result;
        }
        return result;
    }

    std::u16string u16to16substr(const u16string_view wstr, int64_t start, int64_t subLen) {
        if (start < 0) {
            subLen += start;
            start = 0;
        }
        if (subLen <= 0)
            return {};
        std::u16string result;
        result.resize(getU16LenSubstr(wstr, start, subLen));
        const char16_t *wsc;
        const char16_t *ws = wsc = wstr.data();
        int64_t len = 0;
        int64_t dcounter = 0;
        while (ws - wsc < wstr.size()) {
            char32_t d = one16to32(ws, &ws);
            char16_t pair[2];
            if (dcounter >= start) {
                uint8_t k = one32to16(d, pair);
                result[len] = pair[0];
                if (k > 1)
                    result[len + 1] = pair[1];
                len += k;
            }
            dcounter++;
            if (dcounter == start + subLen)
                return result;
        }
        return result;
    }

    static int64_t one16to32(const char16_t *text, const char16_t **end) {
        *end = text;
        auto w1 = (char16_t) **end;
        (*end)++;
        if (w1 >= 0xD800 && w1 <= 0xDBFF) {
            auto w2 = (char16_t) **end;
            (*end)++;
            return 0x400 * ((char32_t) w1 - 0xD800) + ((char32_t) w2 - 0xDC00) + 0x10000;
        } else
            return w1;
    }

    static std::u32string u16to32(const u16string_view wstr) {
        const char16_t *cws = wstr.data();
        std::u32string result;
        result.resize(getU32Len(wstr));
        for (int64_t i = 0; i < result.size(); i++) {
            result[i] = one16to32(cws, &cws);
        }
        return result;
    }

    uint8_t one32to8(char32_t d, char *buf) {
        if (isSurrogate(d) || d > MaxCP) {
            d = REPLACEMENT;
            errors++;
        }
        if (d <= 0x7f) {
            buf[0] = (char) d;
            return 1;
        } else if (d <= 0x7ff) {
            buf[0] = 0xc0 | (d >> 6);
            buf[1] = 0x80 | (d & 0x3f);
            return 2;
        } else if (d <= 0xffff) {
            buf[0] = 0xe0 | (d >> 12);
            buf[1] = 0x80 | ((d >> 6) & 0x3f);
            buf[2] = 0x80 | (d & 0x3f);
            return 3;
        } else {
            buf[0] = 0xf0 | (d >> 18);
            buf[1] = 0x80 | ((d >> 12) & 0x3f);
            buf[2] = 0x80 | ((d >> 6) & 0x3f);
            buf[3] = 0x80 | (d & 0x3f);
            return 4;
        }
    }

    std::string u32to8(const std::u32string_view &dstr) {
        std::string result;
        result.resize(getU8Len(dstr));
        int64_t len = 0;
        for (int64_t i = 0; i < (int64_t) dstr.size(); i++) {
            char buf[10];
            uint8_t k = one32to8(dstr[i], buf);
            for (uint8_t j = 0; j < k; j++) {
                result[len] = buf[j];
                len++;
            }
        }
        return result;
    }

    std::u16string u32to16(const std::u32string_view &dstr) {
        std::u16string result;
        result.resize(getU16Len(dstr));
        int64_t len = 0;
        for (int64_t i = 0; i < (int64_t) dstr.size(); i++) {
            char16_t pair[2];
            uint8_t k = one32to16(dstr[i], pair);
            result[len] = pair[0];
            assert(result[len]);
            if (k > 1)
                result[len + 1] = pair[1];
            len += k;
        }
        return result;
    }

    /*
     * back to ss (start stream) or first !insideU8code or
     * maximal 5 insideU8code
     * */
    static const char *findUtf8(const char *s, const char *ss) {
        uint8_t len = MAXCHARLEN;
        const char *const start = s;
        while (s > ss && insideU8code(*s)) {
            s--;
            len--;
            if (!len)
                return start;
        }
        if (determineU8Len(*s) >= start - s + 1)
            return s;
        else
            return start;
    }

    static const char *findPrevUtf8AtHeader(const char *s, const char *ss) {
        return findUtf8(s - 1, ss);
    }

    static const char *forwardNcodes(const char *s, int64_t N, const char *send, int64_t &actual) {
        assert(s <= send);
        actual = 0;
        if (s == send) return send;
        for (int64_t i = 0; i < N; i++) {
            s = findNextUtf8AtHeader(s, send);
            actual++;
            assert(s <= send);
            if (s == send) break;
        }
        return s;
    }

    static const char *backwardNcodes(const char *s, int64_t N, const char *sstart, int64_t &actual) {
        assert(s >= sstart);
        actual = 0;
        if (s == sstart) return sstart;
        for (int64_t i = 0; i < N; i++) {
            s = findPrevUtf8AtHeader(s, sstart);
            actual++;
            assert(s >= sstart);
            if (s == sstart) break;
        }
        return s;
    }

    static int64_t numCodesBetween(const char *s, const char *s1) {
        assert(s <= s1);
        int64_t N = 0;
        if (s == s1) return 0;
        while (s < s1) {
            s = findNextUtf8AtHeader(s, s1);
            N++;
        }
        return N;
    }

    static const char *findNextUtf8AtHeader(const char *s, const char *eos) {
        assert(s <= eos);
        if (s == eos)
            return eos;
        if (insideU8code(*s))
            //assume we have at start previus or on bad char
            //no real inside UTF8 code
            return s + 1;
        uint8_t expectLen = determineU8Len(*s);
        if (expectLen > MAXCHARLEN || expectLen < 2)
            return s + 1;
        for (uint8_t i = 2; i <= expectLen; i++) {
            s++;
            if (s >= eos)
                return eos;
            if (!insideU8code(*s))
                return s;
        }
        return s + 1;
    }

    static const char *findNextUtf8(const char *s, const char *ss, const char *eos) {
        const char *headPos = findUtf8(s, ss);
        return findNextUtf8AtHeader(headPos, eos);
    }

    static const char *findNextUtf8OrTheSame(const char *s, const char *ss, const char *eos) {
        const char *headPos = findUtf8(s, ss);
        if (headPos == s)
            return s;
        else
            return findNextUtf8AtHeader(headPos, eos);
    }

    static int64_t determineU8LenExact(const char *s, const char *eos) {
        assert(s <= eos);
        return findNextUtf8AtHeader(s, eos) - s;
    }
};
