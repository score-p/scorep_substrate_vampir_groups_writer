/*
 * exception.hpp
 *
 *  Created on: 09.08.2016
 *      Author: andreas
 */

#ifndef INCLUDE_RRL_EXCEPTION_HPP_
#define INCLUDE_RRL_EXCEPTION_HPP_

#include <log.hpp>

namespace vampir
{
namespace exception
{
    static void print_uncaught_exception(std::exception& e, std::string where)
    {
        vampir::logging::fatal() << "Uncaught exception with message:";
        vampir::logging::fatal() << e.what();
        vampir::logging::fatal() << "at call:\"" << where << "\"";
    }
}
}

#endif /* INCLUDE_RRL_EXCEPTION_HPP_ */
