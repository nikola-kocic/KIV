#ifndef POLYFILL_H
#define POLYFILL_H

#pragma once

#include <memory>

namespace cpp14 {
    template<typename T, typename ...Args>
    std::unique_ptr<T> make_unique( Args&& ...args )
    {
        return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
    }
}
#endif  // POLYFILL_H
