/*
 * environment.hpp
 *
 *  Created on: 09.08.2016
 *      Author: andreas
 */

#ifndef INCLUDE_vampir_ENVIRONMENT_HPP_
#define INCLUDE_vampir_ENVIRONMENT_HPP_

#include <log.hpp>
#include <string>

namespace vampir
{

namespace environment
{
    inline std::string get(std::string name, std::string default_value)
    {
        std::string full_name = std::string("SCOREP_VAMPIR_GROUPS_WRITER_") + name;
        vampir::logging::trace() << "[UTIL] Access to environment variable '" << full_name << "'";

        char* tmp = std::getenv(full_name.c_str());

        if (tmp == NULL)
        {
            return default_value;
        }
        return std::string(tmp);
    }
}
}

#endif /* INCLUDE_vampir_ENVIRONMENT_HPP_ */
