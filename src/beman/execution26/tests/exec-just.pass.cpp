// src/beman/execution26/tests/exec-just.pass.cpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/just.hpp>
#include <beman/execution26/detail/empty_env.hpp>
#include <beman/execution26/detail/sender.hpp>
#include <beman/execution26/detail/sender_in.hpp>
#include <test/execution.hpp>

// ----------------------------------------------------------------------------

namespace
{
    struct not_movable
    {
        not_movable() = default;
        not_movable(not_movable&&) = delete;
    };
    template <bool Expect, typename Completion, typename CPO, typename... T>
    auto test_just_constraints(CPO const& cpo, T&&... args) -> void
    {
        static_assert(Expect == requires{ cpo(::std::forward<T>(args)...); });
        if constexpr (requires{ cpo(::std::forward<T>(args)...); })
        {
            auto sender{cpo(::std::forward<T>(args)...)};
            using sender_t = decltype(sender);

            static_assert(test_std::sender<decltype(cpo(::std::forward<T>(args)...))>);
            static_assert(test_std::sender_in<decltype(cpo(::std::forward<T>(args)...))>);
            static_assert(std::same_as<
                test_std::completion_signatures<Completion(std::remove_cvref_t<T>...)>,
                beman::execution26::detail::completion_signatures_for<
                    sender_t, test_std::empty_env>
            >);
            static_assert(std::same_as<
                test_std::completion_signatures<Completion(std::remove_cvref_t<T>...)>,
                decltype(beman::execution26::get_completion_signatures(sender, test_std::empty_env{}))
            >);
        }
    }

    auto test_just_constraints() -> void
    {
        using Error = test_std::set_error_t;
        using Stopped = test_std::set_stopped_t;
        using Value = test_std::set_value_t;

        test_just_constraints<true, Value>(test_std::just);
        test_just_constraints<true, Value>(test_std::just, 0);
        test_just_constraints<false, Error>(test_std::just_error);
        test_just_constraints<true, Error>(test_std::just_error, 0);
        test_just_constraints<true, Stopped>(test_std::just_stopped);
        test_just_constraints<false, Stopped>(test_std::just_stopped, 0);

        static_assert(not std::movable<not_movable>);
        test_just_constraints<false, Value>(test_std::just, not_movable{});
        test_just_constraints<false, Value>(test_std::just, 0, not_movable{});
        test_just_constraints<false, Value>(test_std::just, 0, 0, not_movable{});
        test_just_constraints<false, Value>(test_std::just, 0, 0, not_movable{}, 0);
        test_just_constraints<false, Error>(test_std::just_error, not_movable{});
    }

    template <typename... T>
    struct value_receiver
    {
        using receiver_concept = test_std::receiver_t;
        bool* called;
        test_detail::product_type<std::decay_t<T>...> expect{};

        template <typename... A>
        auto set_value(A&&... a) && noexcept -> void
        {
            *this->called = true;
	    [this, &a...]<std::size_t... I>(std::index_sequence<I...>){
                assert(((this->expect.template get<I>() == a) && ...));
	    }(std::index_sequence_for<T...>{});
        }
    };
    template <typename... T>
    value_receiver(bool*, T&&...) -> value_receiver<T...>;

    template <typename T>
    struct error_receiver
    {
        using receiver_concept = test_std::receiver_t;

        bool*           called;
        std::decay_t<T> error;

        template <typename E>
        auto set_error(E&& e) && noexcept -> void
        {
            *this->called = true;
            assert(error == e);
        }
    };
    template <typename E>
    error_receiver(bool*, E&&) -> error_receiver<E>;

    struct stopped_receiver
    {
        using receiver_concept = test_std::receiver_t;

        bool* called;

        auto set_stopped() && noexcept -> void
        {
            *this->called = true;
        }
    };

    template <typename... T>
    auto test_just_value(T&&... value) -> void
    {
        bool called{false};
        static_assert(test_std::receiver<decltype(value_receiver{&called, value...})>);
        auto op{test_std::connect(
            test_std::just(value...),
            value_receiver{&called, value...}
            )};
        static_assert(test_std::operation_state<decltype(op)>);

        assert(not called);
        test_std::start(op);
        assert(called);
    }

    template <typename E>
    auto test_just_error(E&& error) -> void
    {
        bool called{false};
        static_assert(test_std::receiver<decltype(error_receiver{&called, error})>);
        auto op{test_std::connect(
            test_std::just_error(error),
            error_receiver{&called, error}
            )};
        static_assert(test_std::operation_state<decltype(op)>);

        assert(not called);
        test_std::start(op);
        assert(called);
    }

    auto test_just_stopped() -> void
    {
        bool called{false};
        static_assert(test_std::receiver<decltype(stopped_receiver{&called})>);
        auto op{test_std::connect(
            test_std::just_stopped(),
            stopped_receiver{&called}
            )};
        static_assert(test_std::operation_state<decltype(op)>);

        assert(not called);
        test_std::start(op);
        assert(called);
    }


    struct error
    {
        int value{};
        auto operator== (error const&) const -> bool = default;
    };

    auto test_just() -> void
    {
        test_just_value();
        test_just_value(1);
        test_just_value(1, 2);
        test_just_value(1, 2, 3);

        test_just_error(1);
        test_just_error(error{42});

        test_just_stopped();
    }
}

auto main() -> int
{
    test_just_constraints();
    test_just();
    using type = test_detail::call_result_t<test_std::get_completion_signatures_t,
        decltype(test_std::just()), test_std::empty_env>;

    static_assert(std::same_as<test_std::completion_signatures<test_std::set_value_t()>, type>);
}