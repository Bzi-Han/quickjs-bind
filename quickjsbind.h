#ifndef QUICKJS_BIND_H // !QUIKJS_BIND_H
#define QUICKJS_BIND_H

#include <quickjs/quickjs.h>

#include <string>

namespace quickjs
{
    namespace Detail
    {
        template <typename... any_t>
        struct Value
        {
        };

        template <>
        struct Value<JSValue>
        {
            JSContext *context;
            JSValue value;

            template <typename any_t>
            any_t cast() const
            {
                return js_type_traits<any_t>::cast(context, 0, nullptr, value);
            }

            bool isNull() const
            {
                return JS_IsNull(value);
            }

            bool isBoolean() const
            {
                return JS_IsBool(value);
            }

            bool isNumber() const
            {
                return JS_IsNumber(value);
            }

            bool isString() const
            {
                return JS_IsString(value);
            }

            bool isObject() const
            {
                return JS_IsObject(value);
            }

            bool isArray() const
            {
                return JS_IsArray(context, value);
            }

            bool isFunction() const
            {
                return JS_IsFunction(context, value);
            }

            bool isUndefined() const
            {
                return JS_IsUndefined(value);
            }
        };

        template <typename any_t>
        struct Value<any_t>
        {
            JSContext *context;
            any_t value;
        };

        template <typename... any_t>
        struct js_type_traits
        {
        };

        template <>
        struct js_type_traits<JSValue>
        {
            static JSValue cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept { return value; }
        };

        template <>
        struct js_type_traits<JSContext *>
        {
            static JSValue cast(JSContext *context, int argc, JSValue *args, JSContext *) noexcept { return JS_UNDEFINED; }
            static JSContext *cast(JSContext *context, int argc, JSValue *args, JSValue) noexcept { return context; }
        };

        template <>
        struct js_type_traits<nullptr_t>
        {
            static JSValue cast(JSContext *context, int argc, JSValue *args, void *) noexcept { return JS_NULL; }
            static void *cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept { return nullptr; }
        };

        template <>
        struct js_type_traits<bool>
        {
            static JSValue cast(JSContext *context, int argc, JSValue *args, bool value) noexcept { return JS_NewBool(context, value); }
            static bool cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept { return JS_ToBool(context, value); }
        };

        template <>
        struct js_type_traits<int>
        {
            static JSValue cast(JSContext *context, int argc, JSValue *args, int value) noexcept { return JS_NewInt32(context, value); }
            static int cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                int result = 0;
                JS_ToInt32(context, &result, value);
                return result;
            }
        };

        template <>
        struct js_type_traits<unsigned int>
        {
            static JSValue cast(JSContext *context, int argc, JSValue *args, unsigned int value) noexcept { return JS_NewUint32(context, value); }
            static unsigned int cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                unsigned int result = 0;
                JS_ToUint32(context, &result, value);
                return result;
            }
        };

        template <>
        struct js_type_traits<__int64>
        {
            static JSValue cast(JSContext *context, int argc, JSValue *args, __int64 value) noexcept { return JS_NewInt64(context, value); }
            static __int64 cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                __int64 result = 0;
                JS_ToInt64(context, &result, value);
                return result;
            }
        };

        template <>
        struct js_type_traits<unsigned __int64>
        {
            static JSValue cast(JSContext *context, int argc, JSValue *args, unsigned __int64 value) noexcept { return JS_NewInt64(context, value); }
            static unsigned __int64 cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                __int64 result = 0;
                JS_ToInt64(context, &result, value);
                return static_cast<unsigned __int64>(result);
            }
        };

        template <>
        struct js_type_traits<double>
        {
            static JSValue cast(JSContext *context, int argc, JSValue *args, double value) noexcept { return JS_NewFloat64(context, value); }
            static double cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                double result = 0.0;
                JS_ToFloat64(context, &result, value);
                return result;
            }
        };

        template <>
        struct js_type_traits<std::string>
        {
            static JSValue cast(JSContext *context, int argc, JSValue *args, std::string value) noexcept { return JS_NewStringLen(context, value.data(), value.length()); }
            static std::string cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                size_t length = 0;
                const char *data = JS_ToCStringLen(context, &length, value);
                return {data, length};
            }
        };

        template <typename any_t>
        struct js_type_traits<Value<any_t>>
        {
            static JSValue cast(JSContext *context, int argc, JSValue *args, Value<any_t> value) noexcept { return js_type_traits<any_t>::cast(context, argc, args, value.value); }
            static Value<any_t> cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept { return {context, js_type_traits<any_t>::cast(context, argc, args, value)}; }
        };

        template <typename tuple_t, size_t... i>
        constexpr auto js_args_tuple_impl(JSContext *context, int argc, JSValue *args, std::index_sequence<i...>) noexcept
        {
            return tuple_t{js_type_traits<std::tuple_element_t<i, tuple_t>>::cast(context, argc, args, args[i])...};
        }

        template <typename... params_t>
        constexpr auto js_args_tuple(JSContext *context, int argc, JSValue *args)
        {
            return js_args_tuple_impl<std::tuple<std::decay_t<params_t>...>>(context, argc, args, std::make_index_sequence<sizeof...(params_t)>{});
        }

        template <auto function_pointer>
        struct function_info_packer
        {
        };

        template <typename... any_t>
        struct js_function_castper
        {
        };

        template <typename return_t, typename... params_t, return_t (*runable)(params_t...)>
        struct js_function_castper<function_info_packer<runable>>
        {
            static constexpr JSValue cast(JSContext *context) noexcept
            {
                return JS_NewCFunction(
                    context,
                    +[](JSContext *context, JSValueConst this_val, int argc, JSValueConst *argv)
                    {
                        if constexpr (std::is_same_v<void, return_t>)
                        {
                            std::apply(runable, js_args_tuple<params_t...>(context, argc, argv));
                            return JS_UNDEFINED;
                        }
                        else
                            return js_type_traits<return_t>::cast(context, argc, argv, std::apply(runable, js_args_tuple<params_t...>(context, argc, argv)));
                    },
                    nullptr,
                    0);
            }
        };

        class JSObject
        {
        public:
            static JSObject getGlobal(JSContext *context)
            {
                JSObject result(context, true);

                result.m_object = JS_GetGlobalObject(context);

                return result;
            }

        public:
            JSObject(JSContext *context)
                : m_context(context)
            {
                m_object = JS_NewObject(context);
            }
            JSObject(JSObject &) = delete;
            JSObject(JSObject &&other)
                : m_context(other.m_context),
                  m_object(other.m_object),
                  m_needDestroy(other.m_needDestroy)
            {
                other.m_needDestroy = false;
            }

            ~JSObject()
            {
                if (m_needDestroy)
                    JS_FreeValue(m_context, m_object);
            }

            JSObject &append(JSValue value)
            {
                JS_SetPropertyInt64(m_context, m_object, m_arrayIndex++, value);

                return *this;
            }

            JSObject &addObject(const char *name, JSValue object)
            {
                JS_SetPropertyStr(m_context, m_object, name, object);

                return *this;
            }

            template <auto runable>
            constexpr JSObject &addFunction(const char *name)
            {
                JS_SetPropertyStr(m_context, m_object, name, Detail::js_function_castper<Detail::function_info_packer<runable>>::cast(m_context));

                return *this;
            }

            template <typename any_t>
            JSObject &setProperty(const char *name, any_t value)
            {
                JS_SetPropertyStr(m_context, m_object, name, Detail::js_type_traits<any_t>::cast(m_context, 0, nullptr, value));

                return *this;
            }

            template <typename any_t>
            JSObject &setProperty(size_t index, any_t value)
            {
                JS_SetPropertyInt64(m_context, m_object, index, Detail::js_type_traits<any_t>::cast(m_context, 0, nullptr, value));

                return *this;
            }

            operator JSValue() const
            {
                return m_object;
            }

        private:
            JSObject(JSContext *context, bool m_needDestroy)
                : m_context(context),
                  m_needDestroy(m_needDestroy)
            {
            }

        private:
            JSContext *m_context;
            JSValue m_object;
            bool m_needDestroy = false;
            size_t m_arrayIndex = 0;
        };

        class JSArguments
        {
        public:
            JSArguments(JSContext *context, int argc, JSValue *argv)
                : m_context(context),
                  m_argc(argc),
                  m_argv(argv)
            {
            }

            Value<JSValue> operator[](size_t index) const
            {
                return {m_context, m_argv[index]};
            }

            size_t size() const
            {
                return m_argc;
            }

            operator JSContext *() const
            {
                return m_context;
            }

        private:
            JSContext *m_context;
            int m_argc;
            JSValue *m_argv;
        };

        template <>
        struct js_type_traits<JSArguments>
        {
            static JSValue cast(JSContext *context, int argc, JSValue *args, JSArguments value) noexcept
            {
                return JS_UNDEFINED;
            }
            static JSArguments cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                return {context, argc, args};
            }
        };

    }

    template <typename value_t>
    using value = Detail::Value<value_t>;

    using object = Detail::JSObject;

    using args = Detail::JSArguments;
}

#endif // !QUIKJS_BIND_H