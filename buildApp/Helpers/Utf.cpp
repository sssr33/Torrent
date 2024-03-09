#include "Utf.h"
#include "WinApiException.h"

#include <cuchar>
#include <array>
#include <stdexcept>
#include <Windows.h>

namespace Helpers
{
    void Utf::Test()
    {
        std::u32string_view strv = U"zß水🍌"; // or z\u00df\u6c34\U0001F34C

        std::vector<char> mbFrom32 = ToMultibyte(strv);

        std::string u8 = Utf::ToUtf<std::string>::Cvt(mbFrom32);
        std::u16string u16 = Utf::ToUtf<std::u16string>::Cvt(mbFrom32);
        std::u32string u32 = Utf::ToUtf<std::u32string>::Cvt(mbFrom32);

        std::vector<char> mbFrom8 = ToMultibyte(u8);
        std::vector<char> mbFrom16 = ToMultibyte(u16);
        std::vector<char> mbFrom32_2 = ToMultibyte(u32);

        bool mb8Same = mbFrom8 == mbFrom32;
        bool mb16Same = mbFrom16 == mbFrom32;
        bool mb32Same = mbFrom32_2 == mbFrom32;

        std::string u8Fromu32 = Utf::Cvt<std::string>(strv);
        std::wstring w16Fromu32 = Utf::Cvt<std::wstring>(strv);
        std::u16string u16Fromu32 = Utf::Cvt<std::u16string>(strv);

        std::wstring w16Fromu8 = Utf::Cvt<std::wstring>(u8Fromu32);
        std::u16string u16Fromu8 = Utf::Cvt<std::u16string>(u8Fromu32);
        std::u32string u32Fromu8 = Utf::Cvt<std::u32string>(u8Fromu32);

        std::wstring w16Fromu16 = Utf::Cvt<std::wstring>(u16Fromu32);
        std::string u8Fromu16 = Utf::Cvt<std::string>(u16Fromu32);
        std::u32string u32Fromu16 = Utf::Cvt<std::u32string>(u16Fromu32);

        std::u16string u16Fromw16 = Utf::Cvt<std::u16string>(w16Fromu32);
        std::string u8Fromw16 = Utf::Cvt<std::string>(w16Fromu32);
        std::u32string u32Fromw16 = Utf::Cvt<std::u32string>(w16Fromu32);

        int stop = 234; stop++;
    }

    std::vector<char> Utf::ToMultibyte(const std::string_view& view)
    {
        // u8 --> u16 --> mb
        std::u16string u16;

        u16.resize(Utf::Utf8ToUtf16(view));

        Utf::Utf8ToUtf16(view, &u16);

        std::vector<char> result = Utf::ToMultibyte(u16);
        return result;
    }

    std::vector<char> Utf::ToMultibyte(const std::u16string_view& view)
    {
        return Utf::Crtomb(view,
            [](char* s, char16_t c, mbstate_t* ms)
            {
                return std::c16rtomb(s, c, ms);
            });
    }

    std::vector<char> Utf::ToMultibyte(const std::wstring_view& view)
    {
        return Utf::ToMultibyte(
            std::u16string_view(reinterpret_cast<const char16_t*>(view.data()), view.size())
        );
    }

    std::vector<char> Utf::ToMultibyte(const std::u32string_view& view)
    {
        return Utf::Crtomb(view,
            [](char* s, char32_t c, mbstate_t* ms)
            {
                return std::c32rtomb(s, c, ms);
            });
    }

    std::string Utf::ToUtf<std::string>::Cvt(const std::vector<char>& multiByte)
    {
        // mb --> utf16 --> utf8
        std::u16string u16 = Utf::ToUtf<std::u16string>::Cvt(multiByte);

        std::string result;

        result.resize(Utf::Utf16ToUtf8(u16));

        Utf::Utf16ToUtf8(u16, &result);

        return result;
    }

    std::u16string Utf::ToUtf<std::u16string>::Cvt(const std::vector<char>& multiByte)
    {
        return Utf::Mbrtoc<std::u16string>(multiByte,
            [](char16_t* c, const char* s, size_t n, mbstate_t* ms)
            {
                return std::mbrtoc16(c, s, n, ms);
            });
    }

    std::wstring Utf::ToUtf<std::wstring>::Cvt(const std::vector<char>& multiByte)
    {
        return Utf::Mbrtoc<std::wstring>(multiByte,
            [](wchar_t* c, const char* s, size_t n, mbstate_t* ms)
            {
                return std::mbrtoc16(reinterpret_cast<char16_t*>(c), s, n, ms);
            });
    }

    std::u32string Utf::ToUtf<std::u32string>::Cvt(const std::vector<char>& multiByte)
    {
        return Utf::Mbrtoc<std::u32string>(multiByte,
            [](char32_t* c, const char* s, size_t n, mbstate_t* ms)
            {
                return std::mbrtoc32(c, s, n, ms);
            });
    }

    //std::vector<char> Utf::Utf8ToMultibyte(const std::string_view& u8)
    //{
    //    // u8 --> u16 --> mb
    //    std::u16string u16;

    //    u16.resize(Utf8ToUtf16(u8));

    //    Utf8ToUtf16(u8, &u16);

    //    std::vector<char> result = Utf16ToMultibyte(u16);
    //    return result;
    //}

    //std::vector<char> Utf::Utf16ToMultibyte(const std::u16string_view& u16)
    //{
    //    return Crtomb(u16,
    //        [](char* s, char16_t c, mbstate_t* ms)
    //        {
    //            return std::c16rtomb(s, c, ms);
    //        });
    //}

    //std::vector<char> Utf::Utf16ToMultibyte(const std::wstring_view& u16)
    //{
    //    return Utf16ToMultibyte(std::u16string_view(reinterpret_cast<const char16_t*>(u16.data()), u16.size()));
    //}

    //std::vector<char> Utf::Utf32ToMultibyte(const std::u32string_view& u32)
    //{
    //    return Crtomb(u32,
    //        [](char* s, char32_t c, mbstate_t* ms)
    //        {
    //            return std::c32rtomb(s, c, ms);
    //        });
    //}

    //std::string Utf::MultibyteToUtf8(const std::vector<char>& multiByte)
    //{
    //    // mb --> utf16 --> utf8
    //    std::u16string u16 = MultibyteToUtf16(multiByte);

    //    std::string result;

    //    result.resize(Utf16ToUtf8(u16));

    //    Utf16ToUtf8(u16, &result);

    //    return result;
    //}

    //std::u16string Utf::MultibyteToUtf16(const std::vector<char>& multiByte)
    //{
    //    return Mbrtoc<std::u16string>(multiByte,
    //        [](char16_t* c, const char* s, size_t n, mbstate_t* ms)
    //        {
    //            return std::mbrtoc16(c, s, n, ms);
    //        });
    //}

    //std::wstring Utf::MultibyteToWUtf16(const std::vector<char>& multiByte)
    //{
    //    return Mbrtoc<std::wstring>(multiByte,
    //        [](wchar_t* c, const char* s, size_t n, mbstate_t* ms)
    //        {
    //            return std::mbrtoc16(reinterpret_cast<char16_t*>(c), s, n, ms);
    //        });
    //}

    //std::u32string Utf::MultibyteToUtf32(const std::vector<char>& multiByte)
    //{
    //    return Mbrtoc<std::u32string>(multiByte,
    //        [](char32_t* c, const char* s, size_t n, mbstate_t* ms)
    //        {
    //            return std::mbrtoc32(c, s, n, ms);
    //        });
    //}

    template<class StrView, class CrtombFn>
    static std::vector<char> Utf::Crtomb(const StrView& view, CrtombFn crtombFn)
    {
        std::vector<char> result;
        std::mbstate_t state{};
        std::array<char, MB_LEN_MAX> out{};

        result.reserve(view.size());

        for (typename StrView::value_type c : view)
        {
            std::size_t used = crtombFn(out.data(), c, &state);

            if (used == static_cast<size_t>(-1))
            {
                throw std::runtime_error("invalid code point");
            }

            result.insert(result.end(), out.begin(), out.begin() + used);
        }

        return result;
    }

    template<class ResultStr, class MbrtocFn>
    static ResultStr Utf::Mbrtoc(const std::vector<char>& multiByte, MbrtocFn mbrtocFn)
    {
        ResultStr result;
        std::mbstate_t state{};
        typename ResultStr::value_type c = 0;
        size_t usedMultibytes = 0;

        while (size_t used = mbrtocFn(&c, multiByte.data() + usedMultibytes, multiByte.size() - usedMultibytes, &state))
        {
            if (used == static_cast<size_t>(-3))
            {
                // earlier surrogate pair
            }
            else if (used == static_cast<size_t>(-2))
            {
                if (usedMultibytes == multiByte.size())
                {
                    break;
                }

                throw std::runtime_error("incomplete multibyte char");
            }
            else if (used == static_cast<size_t>(-1))
            {
                throw std::runtime_error("invalid code point");
            }
            else
            {
                usedMultibytes += used;
            }

            result.push_back(c);
        }

        return result;
    }

    size_t Utf::Utf8ToUtf16(const std::string_view& u8, std::u16string* result)
    {
        wchar_t* dst = result ? reinterpret_cast<wchar_t*>(result->data()) : nullptr;
        int dstLength = result ? static_cast<int>(result->size()) : 0;

        int length = MultiByteToWideChar(
            CP_UTF8,
            /*MB_ERR_INVALID_CHARS*/ 0, // TODO check why error when installing boost
            u8.data(),
            static_cast<int>(u8.size()),
            dst,
            dstLength);

        if (length <= 0)
        {
            throw WinApiException();
        }

        return static_cast<size_t>(length);
    }

    size_t Utf::Utf16ToUtf8(const std::u16string_view& u16, std::string* result)
    {
        char* dst = result ? reinterpret_cast<char*>(result->data()) : nullptr;
        int dstLength = result ? static_cast<int>(result->size()) : 0;

        int length = WideCharToMultiByte(
            CP_UTF8,
            /*WC_ERR_INVALID_CHARS*/ 0, // TODO check why error when installing boost
            reinterpret_cast<const wchar_t*>(u16.data()),
            static_cast<int>(u16.size()),
            dst,
            dstLength,
            nullptr,
            nullptr);

        if (length <= 0)
        {
            throw WinApiException();
        }

        return static_cast<size_t>(length);
    }
}
