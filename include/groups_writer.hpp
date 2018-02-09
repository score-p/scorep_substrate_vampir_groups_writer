/*
 * groups_writer.hpp
 *
 *  Created on: 09.02.2018
 *      Author: gocht
 */

#ifndef INCLUDE_GROUPS_WRITER_HPP_
#define INCLUDE_GROUPS_WRITER_HPP_

#include <cassert>
#include <memory>
#include <string>
#include <unordered_set>

#include <scorep.hpp>

namespace vampir
{
class groups_writer
{
public:
    groups_writer() = default;
    ~groups_writer() = default;

    void new_definition_handle(SCOREP_AnyHandle handle, SCOREP_HandleType type);
    void init_mpp();
    void write_data();
    void pre_unify();

    static std::unique_ptr<groups_writer> _instance_;

    static groups_writer& instance()
    {
        assert(_instance_);
        return *_instance_;
    }

private:
    std::unordered_set<std::string> groups;
};
}

#endif /* INCLUDE_GROUPS_WRITER_HPP_ */
