#include "../quickjsbind.h"

#include <iostream>

namespace Test
{
    std::string version = "0.1.0";

    std::string getVersion()
    {
        return version;
    }

    void setVersion(const std::string &version)
    {
        Test::version = version;
    }

    void print(quickjs::args args)
    {
        for (size_t i = 0; i < args.size(); i++)
            std::cout << args[i].cast<std::string>() << " ";

        std::cout << std::endl;
    }

}

std::string g_testScript =
    "test.print(test.getVersion());\n"
    "test.setVersion('1.0.0');\n"
    "test.print(test.getVersion(), true, 233, 3.14, {});\n";

int main()
{
    auto runtime = JS_NewRuntime();
    auto context = JS_NewContext(runtime);

    {
        auto testModule = quickjs::object(context);

        testModule.addFunction<Test::getVersion>("getVersion");
        testModule.addFunction<Test::setVersion>("setVersion");
        testModule.addFunction<Test::print>("print");

        quickjs::object::getGlobal(context).addObject("test", testModule);
    }

    auto executeResult = JS_Eval(
        context,
        g_testScript.data(),
        g_testScript.size(),
        "<input>",
        JS_EVAL_TYPE_GLOBAL);

    if (JS_IsException(executeResult))
    {
        auto exception = JS_GetException(context);
        std::string message = JS_ToCString(context, exception);
        JS_FreeValue(context, exception);
        std::cout << "Error: " << message << std::endl;
    }

    JS_FreeContext(context);
    JS_FreeRuntime(runtime);

    return 0;
}