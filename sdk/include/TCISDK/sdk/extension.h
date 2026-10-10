#pragma once

#include <TCISDK/sdk/message.h>
#include <functional>

extern "C" __declspec(dllexport) int TCIExtension(const char *funcName, int argc, const char **argv);

using namespace TCISDK_Internal;

namespace TCISDK {
    
template<typename T>
inline T ConvertArgument(const char *arg) {
    if constexpr (std::is_same_v<T, std::string>) {
        return arg ? std::string(arg) : std::string();
    } else if constexpr (std::is_same_v<T, const char *>) {
        return arg;
    } else {
        std::stringstream ss(arg ? arg : "0");
        T val{};
        ss >> val;
        return val;
    }
}

class Extension {
public:
    using ExtensionCallback = std::function<int(int argc, const char **argv)>;

    static void Register(const std::string& funcName, ExtensionCallback callback) {
        GetRegistry()[funcName] = callback;
    }

    template<typename ReturnType, typename... Args>
    static void Register(const std::string& funcName, ReturnType(*funcPtr)(Args...)) {
        GetRegistry()[funcName] = [funcPtr](int argc, const char **argv) -> int {
            constexpr size_t expectedArgsCount = sizeof...(Args);
            if (static_cast<size_t>(argc) < expectedArgsCount) {
                Message msg("TCISDK Error: Too little args. Expected: ");
                msg << expectedArgsCount << ", but got: " << argc;
                msg.Log();
                return -1;
            }

            return CallUserFunction<ReturnType, Args...>(funcPtr, argv, std::make_index_sequence<expectedArgsCount>{});
        };
    }

private:
    template<typename ReturnType, typename... Args, size_t... Is>
    static int CallUserFunction(ReturnType(*funcPtr)(Args...), const char** argv, std::index_sequence<Is...>) {
        if constexpr (std::is_void_v<ReturnType>) {
            funcPtr(ConvertArgument<std::tuple_element_t<Is, std::tuple<Args...>>>(argv[Is])...);
            return 0;
        } else {
            return static_cast<int>(funcPtr(ConvertArgument<std::decay_t<std::tuple_element_t<Is, std::tuple<Args...>>>>(argv[Is])...));
        }
    }

    static int Execute(const char *funcName, int argc, const char **argv) {
        auto& registry = GetRegistry();
        auto it = registry.find(funcName);
        if (it != registry.end()) {
            return it->second(argc, argv);
        }
        return -1;
    }

    static std::unordered_map<std::string, ExtensionCallback>& GetRegistry() {
        static std::unordered_map<std::string, ExtensionCallback> registry;
        return registry;
    }

    friend int ::TCIExtension(const char *funcName, int argc, const char **argv);
};

} // namespace TCISDK