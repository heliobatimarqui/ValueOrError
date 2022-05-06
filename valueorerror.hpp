/*---------------------------------------------------------------------------------
MIT License

Copyright (c) 2022 Helio Nunes Santos

        Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
        to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
        copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
        copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

---------------------------------------------------------------------------------*/

#ifndef HELIOS_ERRORVAL_HPP
#define HELIOS_ERRORVAL_HPP

#include <type_traits>
#include <utility>

namespace hls {

    template<typename T, typename ErrorT>
    requires (!std::is_reference_v<T> && !std::is_reference_v<ErrorT>)
    class ValueOrError;

    template<typename T, int x>
    class GenericWrapper;

    template<typename T>
    using ErrorType = GenericWrapper<T, 0>;

    template<typename T>
    using ValueType = GenericWrapper<T, 1>;

    template<typename T, int x>
    class GenericWrapper {
        T v;
        GenericWrapper(T&& a) : v(std::forward<T>(a)) {};
        GenericWrapper(const T& a) : v(a) {};
        GenericWrapper(const GenericWrapper&) = default;
        GenericWrapper(GenericWrapper&&) = default;
    public:
        ~GenericWrapper() = default;

        template<typename, typename>
        friend class ValueOrError;

        template<typename U>
        friend auto error(U&& error) -> ErrorType<std::remove_cvref_t<U>>;

        template<typename U>
        friend auto value(U&& value) -> GenericWrapper<std::remove_cvref_t<U>, 1>;

    };

    template<typename T>
    auto error(T&& error) -> ErrorType<std::remove_cvref_t<T>> {
        return {std::forward<T>(error)};
    }

    template<typename T>
    auto value(T&& value) -> ValueType<std::remove_cvref_t<T>> {
        return {std::forward<T>(value)};
    }

    template<typename T, typename ErrorT = int>
    requires (!std::is_reference_v<T> && !std::is_reference_v<ErrorT>)
    class ValueOrError {
        using Type = std::remove_cvref_t<T>;
        using Error = std::remove_cvref_t<ErrorT>;

        union {
            Type value;
            Error error;
        } m_stored;
        bool m_is_error = false;
    public:

        template<typename U>
        requires (std::is_same_v<std::remove_cvref_t<std::remove_pointer_t<U>>, std::remove_cvref_t<std::remove_pointer_t<T>>>)
        ValueOrError(ValueType<U>&& value) {
            new(&m_stored.value)T(std::move(value.v));
        }

        template<typename U>
        requires (std::is_same_v<std::remove_cvref_t<std::remove_pointer_t<U>>, std::remove_cvref_t<std::remove_pointer_t<ErrorT>>>)
        ValueOrError(ErrorType<U>&& error) {
            m_is_error = true;
            new(&m_stored.error)ErrorT(std::move(error.v));
        }

        ~ValueOrError() {
            if(is_error())
                m_stored.error.~ErrorT();
            else
                m_stored.value.~T();
        }

        ValueOrError(const ValueOrError& other) {
            if(other.is_error()) {
                m_is_error = true;
                m_stored.error = other.get_error();
            }
            else {
                m_stored.value = other.get_value();
            }
        }

        ValueOrError(ValueOrError&& other) {
            if(other.is_error()) {
                m_is_error = true;
                m_stored.error = std::move(other.get_error());
            }
            else {
                m_stored.value = std::move(other.get_value());
            }
        }

        bool is_error() const {
            return m_is_error;
        }

        bool is_value() const {
            return !is_error();
        }

        T& get_value() {
            const auto& as_const = *this;
            return const_cast<T&>(as_const.get_value());
        }

        const T& get_value() const {
            return m_stored.value;
        }

        ErrorT& get_error() {
            const auto& as_const = *this;
            return const_cast<ErrorT&>(as_const.get_error());
        }

        const ErrorT& get_error() const {
            return m_stored.error;
        }
    };

}


#endif //HELIOS_ERRORVAL_HPP
