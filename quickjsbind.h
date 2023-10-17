#ifndef QUICKJS_BIND_H // !QUIKJS_BIND_H
#define QUICKJS_BIND_H

#include <quickjs/quickjs.h>

#include <string>
#include <vector>

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
        struct JSTypeTraits<int8_t>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, int8_t value) noexcept { return JS_NewInt32(context, value); }
            static int8_t Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                int result = 0;
                JS_ToInt32(context, &result, value);
                return static_cast<int8_t>(result);
            }
        };

        template <>
        struct JSTypeTraits<uint8_t>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, uint8_t value) noexcept { return JS_NewInt32(context, value); }
            static uint8_t Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                int result = 0;
                JS_ToInt32(context, &result, value);
                return static_cast<uint8_t>(result);
            }
        };

        template <>
        struct JSTypeTraits<int16_t>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, int16_t value) noexcept { return JS_NewInt32(context, value); }
            static int16_t Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                int result = 0;
                JS_ToInt32(context, &result, value);
                return static_cast<int16_t>(result);
            }
        };

        template <>
        struct JSTypeTraits<uint16_t>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, uint16_t value) noexcept { return JS_NewInt32(context, value); }
            static uint16_t Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                int result = 0;
                JS_ToInt32(context, &result, value);
                return static_cast<uint16_t>(result);
            }
        };

        template <>
        struct JSTypeTraits<int32_t>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, int32_t value) noexcept { return JS_NewInt32(context, value); }
            static int32_t Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                int32_t result = 0;
                JS_ToInt32(context, &result, value);
                return result;
            }
        };

        template <>
        struct JSTypeTraits<uint32_t>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, uint32_t value) noexcept { return JS_NewUint32(context, value); }
            static uint32_t Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                uint32_t result = 0;
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
        struct JSTypeTraits<float>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, float value) noexcept { return JS_NewUint32(context, static_cast<uint32_t>(value)); }
            static float Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                uint32_t result = 0;
                JS_ToUint32(context, &result, value);
                return static_cast<float>(result);
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

        template <>
        struct JSTypeTraits<void *>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, void *value) noexcept
            {
                auto object = JS_NewObject(context);
                JS_SetOpaque(object, value);
                return object;
            }
            static void *Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                return JS_GetOpaque(value, 1); // JS_CLASS_OBJECT
            }
        };

        template <>
        struct JSTypeTraits<std::vector<uint8_t>>
        {
            static JSValue Cast(JSContext *context, int argc, JSValue *args, const std::vector<uint8_t> &value) noexcept
            {
                return JS_NewArrayBufferCopy(context, value.data(), value.size());
            }
            static std::vector<uint8_t> Cast(JSContext *context, int argc, JSValue *args, JSValue value) noexcept
            {
                size_t length = 0;
                auto data = JS_GetArrayBuffer(context, &length, value);
                return {data, data + length};
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

        template <typename method_t, method_t methodPointer>
        struct MethodInfoPacker
        {
        };

        template <typename field_t, field_t fieldPointer>
        struct FieldInfoPacker
        {
        };

        template <typename... any_t>
        struct JSFunctionCaster
        {
        };

        template <typename... any_t>
        struct JSMethodCaster
        {
        };

        template <typename... any_t>
        struct JSFieldCaster
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

        template <typename return_t, typename... params_t, return_t (*runnable)(params_t...)>
        struct JSFunctionCaster<FunctionInfoPacker<runnable>>
        {
            static constexpr JSValue Cast(JSContext *context) noexcept
            {
                return JS_NewCFunction(
                    context,
                    [](JSContext *context, JSValueConst this_val, int argc, JSValueConst *argv)
                    {
                        if constexpr (std::is_same_v<void, return_t>)
                        {
                            std::apply(runnable, JSArgsTuple<params_t...>(context, argc, argv));
                            return JS_UNDEFINED;
                        }
                        else
                            return JSTypeTraits<return_t>::Cast(context, argc, argv, std::apply(runnable, JSArgsTuple<params_t...>(context, argc, argv)));
                    },
                    nullptr,
                    0);
            }
        };

        template <typename class_t, typename return_t, typename... params_t, return_t (class_t::*runnable)(params_t...)>
        struct JSMethodCaster<MethodInfoPacker<return_t (class_t::*)(params_t...), runnable>>
        {
            static constexpr JSValue Cast(JSContext *context) noexcept
            {
                return JS_NewCFunction(
                    context,
                    [](JSContext *context, JSValueConst this_val, int argc, JSValueConst *argv)
                    {
                        auto object = JS_GetOpaque(this_val, 1); // JS_CLASS_OBJECT
                        if (nullptr == object)
                            return JS_ThrowInternalError(context, "Call invalid object");

                        if constexpr (std::is_same_v<void, return_t>)
                        {
                            std::apply(runnable, std::tuple_cat(std::make_tuple(reinterpret_cast<class_t *>(object)), JSArgsTuple<params_t...>(context, argc, argv)));
                            return JS_UNDEFINED;
                        }
                        else
                            return JSTypeTraits<return_t>::Cast(context, argc, argv, std::apply(runnable, std::tuple_cat(std::make_tuple(reinterpret_cast<class_t *>(object)), JSArgsTuple<params_t...>(context, argc, argv))));
                    },
                    nullptr,
                    0);
            }
        };

        template <typename class_t, typename return_t, typename... params_t, return_t (class_t::*runnable)(params_t...) const>
        struct JSMethodCaster<MethodInfoPacker<return_t (class_t::*)(params_t...) const, runnable>>
        {
            static constexpr JSValue Cast(JSContext *context) noexcept
            {
                return JS_NewCFunction(
                    context,
                    [](JSContext *context, JSValueConst this_val, int argc, JSValueConst *argv)
                    {
                        auto object = JS_GetOpaque(this_val, 1); // JS_CLASS_OBJECT
                        if (nullptr == object)
                            return JS_ThrowInternalError(context, "Call invalid object");

                        if constexpr (std::is_same_v<void, return_t>)
                        {
                            std::apply(runnable, std::tuple_cat(std::make_tuple(reinterpret_cast<class_t *>(object)), JSArgsTuple<params_t...>(context, argc, argv)));
                            return JS_UNDEFINED;
                        }
                        else
                            return JSTypeTraits<return_t>::Cast(context, argc, argv, std::apply(runnable, std::tuple_cat(std::make_tuple(reinterpret_cast<class_t *>(object)), JSArgsTuple<params_t...>(context, argc, argv))));
                    },
                    nullptr,
                    0);
            }
        };

        template <typename value_t, typename class_t, value_t class_t::*field>
        struct JSFieldCaster<FieldInfoPacker<value_t class_t::*, field>>
        {
            static constexpr std::pair<JSValue, JSValue> Cast(JSContext *context) noexcept
            {
                union PointerCaster
                {
                    void *normal;
                    value_t class_t::*member;
                };

                auto dataPointerValue = JS_NewObject(context);
                JS_SetOpaque(dataPointerValue, PointerCaster{.member = field}.normal);

                auto getter = JS_NewCFunctionData(
                    context,
                    [](JSContext *context, JSValueConst this_val, int argc, JSValueConst *argv, int magic, JSValue *func_data) -> JSValue
                    {
                        auto object = reinterpret_cast<class_t *>(JS_GetOpaque(this_val, 1)); // JS_CLASS_OBJECT
                        if (nullptr == object)
                            return JS_ThrowInternalError(context, "Invalid object");

                        auto dataPointer = PointerCaster{.normal = JS_GetOpaque(func_data[0], 1)}.member; // JS_CLASS_OBJECT

                        return JSTypeTraits<value_t>::Cast(context, argc, argv, object->*dataPointer);
                    },
                    0, 0,
                    1, reinterpret_cast<JSValue *>(&dataPointerValue));
                auto setter = JS_NewCFunctionData(
                    context,
                    [](JSContext *context, JSValueConst this_val, int argc, JSValueConst *argv, int magic, JSValue *func_data) -> JSValue
                    {
                        auto object = reinterpret_cast<class_t *>(JS_GetOpaque(this_val, 1)); // JS_CLASS_OBJECT
                        if (nullptr == object)
                            return JS_ThrowInternalError(context, "Invalid object");

                        auto dataPointer = PointerCaster{.normal = JS_GetOpaque(func_data[0], 1)}.member; // JS_CLASS_OBJECT

                        object->*dataPointer = JSTypeTraits<value_t>::Cast(context, argc, argv, argv[0]);
                        return JS_UNDEFINED;
                    },
                    0, 0,
                    1, reinterpret_cast<JSValue *>(&dataPointerValue));

                JS_FreeValue(context, dataPointerValue);
                return {getter, setter};
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
            JSObject(JSContext *context, void *bindToObject)
                : m_context(context)
            {
                m_object = JS_NewObject(context);
                if (nullptr != bindToObject)
                    JS_SetOpaque(m_object, bindToObject);
            }
            JSObject(JSContext *context) : JSObject(context, nullptr) {}
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

            template <auto runnable>
            constexpr JSObject &AddFunction(const char *name)
            {
                JS_SetPropertyStr(m_context, m_object, name, Detail::JSFunctionCaster<Detail::FunctionInfoPacker<runnable>>::Cast(m_context));

                return *this;
            }

            template <typename pointer_t>
            constexpr JSObject &AddProperty(const char *name, pointer_t *dataPointer)
            {
                auto nameAtom = JS_NewAtom(m_context, name);
                auto dataPointerValue = JS_NewObject(m_context);
                JS_SetOpaque(dataPointerValue, dataPointer);

                auto getter = JS_NewCFunctionData(
                    m_context,
                    [](JSContext *context, JSValueConst this_val, int argc, JSValueConst *argv, int magic, JSValue *func_data) -> JSValue
                    {
                        auto dataPointer = reinterpret_cast<pointer_t *>(JS_GetOpaque(func_data[0], 1)); // JS_CLASS_OBJECT

                        return JSTypeTraits<pointer_t>::Cast(context, argc, argv, *dataPointer);
                    },
                    0, 0,
                    1, reinterpret_cast<JSValue *>(&dataPointerValue));
                auto setter = JS_NewCFunctionData(
                    m_context,
                    [](JSContext *context, JSValueConst this_val, int argc, JSValueConst *argv, int magic, JSValue *func_data) -> JSValue
                    {
                        auto dataPointer = reinterpret_cast<pointer_t *>(JS_GetOpaque(func_data[0], 1)); // JS_CLASS_OBJECT

                        *dataPointer = JSTypeTraits<pointer_t>::Cast(context, argc, argv, argv[0]);
                        return JS_UNDEFINED;
                    },
                    0, 0,
                    1, reinterpret_cast<JSValue *>(&dataPointerValue));

                JS_DefinePropertyGetSet(m_context, m_object, nameAtom, getter, setter, 0);

                JS_FreeValue(m_context, dataPointerValue);
                JS_FreeAtom(m_context, nameAtom);

                return *this;
            }

            template <auto runnable>
            constexpr JSObject &AddMethod(const char *name)
            {
                JS_SetPropertyStr(m_context, m_object, name, Detail::JSMethodCaster<Detail::MethodInfoPacker<decltype(runnable), runnable>>::Cast(m_context));

                return *this;
            }

            template <auto field>
            constexpr JSObject &AddField(const char *name)
            {
                auto nameAtom = JS_NewAtom(m_context, name);

                auto [getter, setter] = JSFieldCaster<Detail::FieldInfoPacker<decltype(field), field>>::Cast(m_context);
                JS_DefinePropertyGetSet(m_context, m_object, nameAtom, getter, setter, 0);

                JS_FreeAtom(m_context, nameAtom);

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