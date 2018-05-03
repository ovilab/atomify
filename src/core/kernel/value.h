#ifndef VALUE_H
#define VALUE_H

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

namespace atomify {

struct InPlaceT
{
    explicit InPlaceT() = default;
};
constexpr InPlaceT InPlace{};

template <typename T>
struct Value
{
    template <typename U>
    struct is_derived
        : public std::integral_constant<bool, std::is_base_of<T, std::decay_t<U>>::value>
    {
    };

    Value()
        : container(std::make_shared<T>())
        , copy_container(&Value::make_shared_helper<T>)
    {
    }

    // Copy constructors
    template <typename U, typename UU = std::decay_t<U>,
              typename = std::enable_if_t<is_derived<U>::value>>
    Value(const Value<U> &other)
        : container(other.container)
        , copy_container(&Value::make_shared_helper<UU>)
    {
    }

    template <typename U, typename UU = std::decay_t<U>,
              typename = std::enable_if_t<is_derived<U>::value>>
    Value(const U &arg)
        : container(std::make_shared<UU>(arg))
        , copy_container(&Value::make_shared_helper<UU>)
    {
    }

    // Move constructors
    template <typename U, typename UU = std::decay_t<U>,
              typename = std::enable_if_t<is_derived<U>::value>>
    Value(Value<U> &&other)
        : container(std::move(other.container))
        , copy_container(&Value::make_shared_helper<UU>)
    {
    }

    template <typename U, typename UU = std::decay_t<U>,
              typename = std::enable_if_t<is_derived<U>::value>>
    Value(U &&value)
        : container(std::make_shared<UU>(std::forward<U>(value)))
        , copy_container(&Value::make_shared_helper<UU>)
    {
    }

    // Copy assignment
    template <typename U, typename UU = std::decay_t<U>,
              typename = std::enable_if_t<is_derived<U>::value>>
    Value &operator=(const Value<U> &other)
    {
        container = std::make_shared<UU>(*other.container);
        copy_container = &Value::make_shared_helper<UU>;
        return *this;
    }

    template <typename U, typename UU = std::decay_t<U>,
              typename = std::enable_if_t<is_derived<U>::value>>
    Value &operator=(const U &value)
    {
        container = std::make_shared<UU>(value);
        copy_container = &Value::make_shared_helper<UU>;
        return *this;
    }

    // Move assignment
    template <typename U, typename UU = std::decay_t<U>,
              typename = std::enable_if_t<is_derived<U>::value>>
    Value &operator=(Value<U> &&other)
    {
        container = std::make_shared<UU>(std::move<std::shared_ptr<UU>>(*other.container));
        copy_container = &Value::make_shared_helper<UU>;
        return *this;
    }

    template <typename U, typename UU = std::decay_t<U>,
              typename = std::enable_if_t<is_derived<U>::value>>
    Value &operator=(U &&value)
    {
        container = std::make_shared<UU>(std::forward<U>(value));
        copy_container = &Value::make_shared_helper<UU>;
        return *this;
    }

    template <typename... Args>
    Value(InPlaceT, Args &&... args)
        : container(std::make_shared<T>(std::forward<Args>(args)...))
        , copy_container(&Value::make_shared_helper<T>)
    {
    }

    template <typename U>
    bool can_convert() const
    {
        auto typed_container = std::dynamic_pointer_cast<const U>(container);
        if (typed_container == nullptr) {
            return false;
        }
        return true;
    }

    const T &operator*() const { return *container; }

    const T &get() const { return *container; }

    template <typename U>
    U *as()
    {
        if (container == nullptr) {
            return nullptr;
        }
        if (container.use_count() != 1) {
            container = copy_container(container);
        }
        auto typed_container = std::dynamic_pointer_cast<U>(container);
        if (typed_container == nullptr) {
            std::stringstream error_message;
            error_message << "Cannot cast from ";
            error_message << typeid(T).name();
            error_message << " to ";
            error_message << typeid(U).name();
            throw std::runtime_error(error_message.str());
        }
        return typed_container.get();
    }

    Value<T> modified(std::function<T(T)> f)
    {
        if (container.use_count() != 1) {
            container = copy_container(container);
        }
        T& data = *container;
        T result = f(std::move(data));
        auto val = Value<T>(std::move(result));
        return val;
    }

    const T *operator->() const { return container.get(); }

    T *operator->()
    {
        if (container == nullptr) {
            return nullptr;
        }
        if (container.use_count() != 1) {
            container = copy_container(container);
        }
        return container.get();
    }

    template <typename U>
    static std::shared_ptr<T> make_shared_helper(std::shared_ptr<T> c)
    {
        auto typed_container = std::static_pointer_cast<U>(c);
        return std::make_shared<U>(*typed_container);
    }

private:
    std::shared_ptr<T> container;

    std::function<std::shared_ptr<T>(std::shared_ptr<T>)> copy_container;

    template <typename U>
    friend struct Value;
};
}

#endif // VALUE_H
