#pragma once

#include <string>  //  std::string
#include <sqlite3.h>
#include <tuple>  //  std::tuple
#include <functional>  //  std::function

namespace sqlite_orm {

    namespace internal {

        struct scalar_function_t {
            std::string name;
            int argumentsCount = 0;
            std::function<void *()> create;
            std::function<void(sqlite3_context *context, void *functionPointer, int argsCount, sqlite3_value **values)>
                run;
            std::function<void(void *)> destroy;
        };

        template<class F>
        struct run_member_pointer {
            using type = decltype(&F::operator());
        };

        template<class T>
        struct member_function_arguments;

        template<class O, class R, class... Args>
        struct member_function_arguments<R (O::*)(Args...) const> {
            using member_function_type = R (O::*)(Args...) const;
            using tuple_type = std::tuple<typename std::decay<Args>::type...>;
            using return_type = R;
        };

        template<class F>
        struct callable_arguments {
            using function_member_pointer = typename run_member_pointer<F>::type;
            using args_tuple = typename member_function_arguments<function_member_pointer>::tuple_type;
            using return_type = typename member_function_arguments<function_member_pointer>::return_type;
        };

        template<class F, class... Args>
        struct function_call {
            using function_type = F;
            using args_tuple = std::tuple<Args...>;

            args_tuple args;
        };
    }

    template<class F, class... Args>
    internal::function_call<F, Args...> func(Args... args) {
        return {std::make_tuple(std::forward<Args>(args)...)};
    }

}
