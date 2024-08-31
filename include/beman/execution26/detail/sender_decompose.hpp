// include/beman/execution26/detail/sender_decompose.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_DECOMPOSE
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_SENDER_DECOMPOSE

#include <tuple>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename Tag, typename Data, typename Children>
    struct sender_meta
    {
        using tag_type = ::std::remove_cvref_t<Tag>;
        using data_type = ::std::remove_cvref_t<Data>;
        using children_type = ::std::remove_cvref_t<Children>;
    };

    template <typename Tag, typename Data, typename Children>
    struct sender_data
    {
        ::std::remove_cvref_t<Tag> tag;
        Data&                      data;
        Children                   children;
    };

    template <typename Sender>
    auto get_sender_meta(Sender&& sender)
    {
        #if 0
        //-dk:TODO should use a dynamic/language approach:
        auto&& [tag, data, ... children] = sender;
        return sender_meta<decltype(tag), decltype(data), ::std::tuple<decltype(children)...>>;
        #endif
        using sender_type = ::std::remove_cvref_t<Sender>;

        if constexpr (requires(){
            []{ auto&&[tag, data, c0, c1, c2, c3] = *static_cast<sender_type*>(nullptr); };
            })
        {
            auto&& [tag, data, c0, c1, c2, c3] = sender;
            return ::beman::execution26::detail::sender_meta<decltype(tag), decltype(data), ::std::tuple<decltype(c0), decltype(c1), decltype(c2), decltype(c3)>>{};
        }
        else if constexpr (requires(){
            []{ auto&&[tag, data, c0, c1, c2] = *static_cast<sender_type*>(nullptr); };
            })
        {
            auto&& [tag, data, c0, c1, c2] = sender;
            return ::beman::execution26::detail::sender_meta<decltype(tag), decltype(data), ::std::tuple<decltype(c0), decltype(c1), decltype(c2)>>{};
        }
        else if constexpr (requires(){
            []{ auto&&[tag, data, c0, c1] = *static_cast<sender_type*>(nullptr); };
            })
        {
            auto&& [tag, data, c0, c1] = sender;
            return ::beman::execution26::detail::sender_meta<decltype(tag), decltype(data), ::std::tuple<decltype(c0), decltype(c1)>>{};
        }
        else if constexpr (requires(){
            []{ auto&&[tag, data, c0] = *static_cast<sender_type*>(nullptr); };
            })
        {
            auto&& [tag, data, c0] = sender;
            return ::beman::execution26::detail::sender_meta<decltype(tag), decltype(data), ::std::tuple<decltype(c0)>>{};
        }
        else if constexpr (requires(){
            []{ auto&&[tag, data] = *static_cast<sender_type*>(nullptr); };
            })
        {
            auto&& [tag, data] = sender;
            return ::beman::execution26::detail::sender_meta<decltype(tag), decltype(data), ::std::tuple<>>{};
        }
        else
        {
            return ::beman::execution26::detail::sender_meta<void, void, void>{};
        }
    }

    template <typename Sender>
    auto get_sender_data(Sender&& sender)
    {
        #if 0
        //-dk:TODO should use a dynamic/language approach:
        auto&& [tag, data, ... children] = sender;
        return sender_meta<decltype(tag), decltype(data), ::std::tuple<decltype(children)...>>;
        #endif
        using sender_type = ::std::remove_cvref_t<Sender>;
        
        if constexpr (requires(){
            []{ auto&&[tag, data, c0, c1, c2, c3] = *static_cast<sender_type*>(nullptr); };
            })
        {
            auto&& [tag, data, c0, c1, c2, c3] = sender;
            return ::beman::execution26::detail::sender_data<decltype(tag), decltype(data), decltype(::std::tie(c0, c1, c2, c3))>{
                tag, data, ::std::tie(c0, c1, c2, c3)
            };
        }
        else if constexpr (requires(){
            []{ auto&&[tag, data, c0, c1, c2] = *static_cast<sender_type*>(nullptr); };
            })
        {
            auto&& [tag, data, c0, c1, c2] = sender;
            return ::beman::execution26::detail::sender_data<decltype(tag), decltype(data), decltype(::std::tie(c0, c1, c2))>{
                tag, data, ::std::tie(c0, c1, c2)
            };
        }
        else if constexpr (requires(){
            []{ auto&&[tag, data, c0, c1] = *static_cast<sender_type*>(nullptr); };
            })
        {
            auto&& [tag, data, c0, c1] = sender;
            return ::beman::execution26::detail::sender_data<decltype(tag), decltype(data), decltype(::std::tie(c0, c1))>{
                tag, data, ::std::tie(c0, c1)
            };
        }
        else if constexpr (requires(){
            []{ auto&&[tag, data, c0] = *static_cast<sender_type*>(nullptr); };
            })
        {
            auto&& [tag, data, c0] = sender;
            return ::beman::execution26::detail::sender_data<decltype(tag), decltype(data), decltype(::std::tie(c0))>{
                tag, data, ::std::tie(c0)
            };
        }
        else if constexpr (requires(){
            []{ auto&&[tag, data] = *static_cast<sender_type*>(nullptr); };
            })
        {
            auto&& [tag, data] = sender;
            return ::beman::execution26::detail::sender_data<decltype(tag), decltype(data), ::std::tuple<>>{
                tag, data, ::std::tuple<>()
            };
        }
        else
        {
            return ::beman::execution26::detail::sender_meta<void, void, void>{};
        }
    }
}

namespace beman::execution26
{
    template <typename Sender>
    using tag_of_t = typename decltype(::beman::execution26::detail::get_sender_meta(::std::declval<Sender&&>()))::tag_type;
}

// ----------------------------------------------------------------------------

#endif
