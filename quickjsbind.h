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

        template <typename... any_t>
        struct JSTypeTraits
        {
        };

        template <>
        struct JSTypeTraits<JSValue>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept { return value; }
        };

        template <>
        struct JSTypeTraits<JSContext *>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, JSContext *) noexcept { return JS_UNDEFINED; }
            static JSContext *Cast(JSContext *context, int argc, JSValue *args, JSValue) noexcept { return context; }
        };

        template <>
        struct JSTypeTraits<std::nullptr_t>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, void *) noexcept { return JS_NULL; }
            static void *Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept { return nullptr; }
        };

        template <>
        struct JSTypeTraits<bool>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, bool value) noexcept { return JS_NewBool(context, value); }
            static bool Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept { return JS_ToBool(context, value); }
        };

        template <>
        struct JSTypeTraits<int>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, int value) noexcept { return JS_NewInt32(context, value); }
            static int Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                int result = 0;
                JS_ToInt32(context, &result, value);
                return result;
            }
        };

        template <>
        struct JSTypeTraits<unsigned int>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, unsigned int value) noexcept { return JS_NewUint32(context, value); }
            static unsigned int Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                unsigned int result = 0;
                JS_ToUint32(context, &result, value);
                return result;
            }
        };

        template <>
        struct JSTypeTraits<int64_t>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, int64_t value) noexcept { return JS_NewInt64(context, value); }
            static int64_t Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                int64_t result = 0;
                JS_ToInt64(context, &result, value);
                return result;
            }
        };

        template <>
        struct JSTypeTraits<uint64_t>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, uint64_t value) noexcept { return JS_NewInt64(context, value); }
            static uint64_t Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                int64_t result = 0;
                JS_ToInt64(context, &result, value);
                return static_cast<uint64_t>(result);
            }
        };

        template <>
        struct JSTypeTraits<double>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, double value) noexcept { return JS_NewFloat64(context, value); }
            static double Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                double result = 0.0;
                JS_ToFloat64(context, &result, value);
                return result;
            }
        };

        template <>
        struct JSTypeTraits<std::string>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, std::string value) noexcept { return JS_NewStringLen(context, value.data(), value.length()); }
            static std::string Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                size_t length = 0;
                const char *data = JS_ToCStringLen(context, &length, value);
                return {data, length};
            }
        };

        template <typename any_t>
        struct JSTypeTraits<Value<any_t>>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, Value<any_t> value) noexcept { return JSTypeTraits<any_t>::Cast(context, argc, args, value.value); }
            static Value<any_t> Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept { return {context, JSTypeTraits<any_t>::Cast(context, argc, args, value)}; }
        };

        template <typename any_t>
        struct Value<any_t>
        {
            JSContext *context;
            any_t value;
        };

        template <>
        struct Value<JSValue>
        {
            JSContext *context;
            JSValue value;

            template <typename any_t>
            any_t Cast() const
            {
                return JSTypeTraits<any_t>::Cast(context, 0, nullptr, value);
            }

            bool IsNull() const
            {
                return JS_IsNull(value);
            }

            bool IsBoolean() const
            {
                return JS_IsBool(value);
            }

            bool IsNumber() const
            {
                return JS_IsNumber(value);
            }

            bool IsString() const
            {
                return JS_IsString(value);
            }

            bool IsObject() const
            {
                return JS_IsObject(value);
            }

            bool IsArray() const
            {
                return JS_IsArray(context, value);
            }

            bool IsFunction() const
            {
                return JS_IsFunction(context, value);
            }

            bool IsUndefined() const
            {
                return JS_IsUndefined(value);
            }
        };

        template <auto functionPointer>
        struct FunctionInfoPacker
        {
        };

        template <typename... any_t>
        struct JSFunctionCaster
        {
        };

        template <typename tuple_t, size_t... i>
        constexpr auto JSArgsTupleImpl(JSContext *context, int argc, JSValue *args, std::index_sequence<i...>) noexcept
        {
            return tuple_t{JSTypeTraits<std::tuple_element_t<i, tuple_t>>::Cast(context, argc, args, args[i])...};
        }

        template <typename... params_t>
        constexpr auto JSArgsTuple(JSContext *context, int argc, JSValue *args)
        {
            return JSArgsTupleImpl<std::tuple<std::decay_t<params_t>...>>(context, argc, args, std::make_index_sequence<sizeof...(params_t)>{});
        }

        template <typename return_t, typename... params_t, return_t (*runable)(params_t...)>
        struct JSFunctionCaster<FunctionInfoPacker<runable>>
        {
            static constexpr JSValue Cast(JSContext *context) noexcept
            {
                return JS_NewCFunction(
                    context,
                    +[](JSContext *context, JSValueConst this_val, int argc, JSValueConst *argv)
                    {
                        if constexpr (std::is_same_v<void, return_t>)
                        {
                            std::apply(runable, JSArgsTuple<params_t...>(context, argc, argv));
                            return JS_UNDEFINED;
                        }
                        else
                            return JSTypeTraits<return_t>::Cast(context, argc, argv, std::apply(runable, JSArgsTuple<params_t...>(context, argc, argv)));
                    },
                    nullptr,
                    0);
            }
        };

        class JSObject
        {
        public:
            static JSObject GetGlobal(JSContext *context)
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

            JSObject &Append(JSValue value)
            {
                JS_SetPropertyInt64(m_context, m_object, m_arrayIndex++, value);

                return *this;
            }

            JSObject &AddObject(const char *name, JSValue object)
            {
                JS_SetPropertyStr(m_context, m_object, name, object);

                return *this;
            }

            template <auto runable>
            constexpr JSObject &AddFunction(const char *name)
            {
                JS_SetPropertyStr(m_context, m_object, name, Detail::JSFunctionCaster<Detail::FunctionInfoPacker<runable>>::Cast(m_context));

                return *this;
            }

            template <typename any_t>
            JSObject &SetProperty(const char *name, any_t value)
            {
                JS_SetPropertyStr(m_context, m_object, name, Detail::JSTypeTraits<any_t>::Cast(m_context, 0, nullptr, value));

                return *this;
            }

            template <typename any_t>
            JSObject &SetProperty(size_t index, any_t value)
            {
                JS_SetPropertyInt64(m_context, m_object, index, Detail::JSTypeTraits<any_t>::Cast(m_context, 0, nullptr, value));

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

            size_t Size() const
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
        struct JSTypeTraits<JSArguments>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, JSArguments value) noexcept
            {
                return JS_UNDEFINED;
            }
            static JSArguments Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                return {context, argc, args};
            }
        };

    }

    template <typename value_t>
    using Value = Detail::Value<value_t>;

    using Object = Detail::JSObject;

    using Args = Detail::JSArguments;
}

#endif // !QUIKJS_BIND_H