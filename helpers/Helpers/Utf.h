#pragma once

#include <string>
#include <vector>

namespace Helpers
{
    // https://dbj.org/c17-codecvt-deprecated-panic/#:~:text=The%20entire%20header%20(,they%20are%20even%20specified%20correctly
    class Utf
    {
    public:
        static void Test();

        template<class ResultStr, class StrView>
        static ResultStr Cvt(const StrView& view)
        {
            return Utf::ToUtf<ResultStr>::Cvt(Utf::ToMultibyte(view));
        }

        template<class ResultStr, class StrView>
        static void Cvt(ResultStr& result, const StrView& view)
        {
            result = Utf::Cvt(view);
        }

    private:
        static std::vector<char> ToMultibyte(const std::string_view& view);
        static std::vector<char> ToMultibyte(const std::u16string_view& view);
        static std::vector<char> ToMultibyte(const std::wstring_view& view);
        static std::vector<char> ToMultibyte(const std::u32string_view& view);

        template<class StrResult>
        struct ToUtf {};

        template<>
        struct ToUtf<std::string>
        {
            static std::string Cvt(const std::vector<char>& multiByte);
        };

        template<>
        struct ToUtf<std::u16string>
        {
            static std::u16string Cvt(const std::vector<char>& multiByte);
        };

        template<>
        struct ToUtf<std::wstring>
        {
            static std::wstring Cvt(const std::vector<char>& multiByte);
        };

        template<>
        struct ToUtf<std::u32string>
        {
            static std::u32string Cvt(const std::vector<char>& multiByte);
        };

        /*static std::vector<char> Utf8ToMultibyte(const std::string_view& u8);
        static std::vector<char> Utf16ToMultibyte(const std::u16string_view& u16);
        static std::vector<char> Utf16ToMultibyte(const std::wstring_view& u16);
        static std::vector<char> Utf32ToMultibyte(const std::u32string_view& u32);*/

        /*static std::string MultibyteToUtf8(const std::vector<char>& multiByte);
        static std::u16string MultibyteToUtf16(const std::vector<char>& multiByte);
        static std::wstring MultibyteToWUtf16(const std::vector<char>& multiByte);
        static std::u32string MultibyteToUtf32(const std::vector<char>& multiByte);*/

        template<class StrView, class CrtombFn>
        static std::vector<char> Crtomb(const StrView& view, CrtombFn crtombFn);

        template<class ResultStr, class MbrtocFn>
        static ResultStr Mbrtoc(const std::vector<char>& multiByte, MbrtocFn mbrtocFn);

        // win api wrapper
        static size_t Utf8ToUtf16(const std::string_view& u8, std::u16string* result = nullptr);
        // win api wrapper
        static size_t Utf16ToUtf8(const std::u16string_view& u16, std::string* result = nullptr);
    };
}
