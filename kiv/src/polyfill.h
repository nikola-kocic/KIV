template <typename... T>
constexpr auto make_array(T&&... values) ->
        std::array<
            typename std::decay<
                typename std::common_type<T...>::type>::type,
            sizeof...(T)> {
    return std::array<
        typename std::decay<
            typename std::common_type<T...>::type>::type,
        sizeof...(T)>{{std::forward<T>(values)...}};
}

template<class C, class T>
auto contains(const C& v, const T& x)
-> decltype(std::end(v), true)
{
    return std::end(v) != std::find(std::begin(v), std::end(v), x);
}
