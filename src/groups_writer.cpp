/*
 * groups_writer.cpp
 *
 *  Created on: 09.02.2018
 *      Author: gocht
 */
#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>
#include <vector>

#include <groups_writer.hpp>
#include <log.hpp>

namespace vampir
{
std::unique_ptr<groups_writer> groups_writer::_instance_;

void groups_writer::init_mpp()
{
}

void groups_writer::new_definition_handle(SCOREP_AnyHandle handle, SCOREP_HandleType type)
{
    if (type == SCOREP_HANDLE_TYPE_REGION)
    {
        if (SCOREP_REGION_FUNCTION == scorep::call::region_handle_get_type(handle))
        {
            auto region_name = scorep::call::region_handle_get_name(handle);
            auto module_pos = region_name.find(":"); // check for python modules
            if (module_pos != std::string::npos)
            {
                auto main_module_pos = region_name.find(".");
                if (main_module_pos == std::string::npos)
                {
                    main_module_pos = module_pos;
                }
                auto group_name = region_name.substr(0, main_module_pos);
                groups.insert(group_name);
            }
        }
    }
}

void groups_writer::pre_unify()
{
    /** only collect data if necesarry
     *
     */
    auto size = scorep::call::ipc_get_size();
    if (size > 1)
    {

        vampir::logging::debug() << "got " << size << " processes";
        auto rank = scorep::call::ipc_get_rank();
        if (rank == 0)
        {
            std::vector<std::uint64_t> group_sizes;

            for (int i = 1; i < size; i++)
            {
                std::uint64_t buf;
                scorep::call::ipc_recv(&buf, 1, SCOREP_IPC_UINT64_T, i);
                group_sizes.push_back(buf);
                vampir::logging::debug() << "got size: " << buf << " from rank " << i;
            }

            for (int i = 1; i < size; i++)
            {
                char* ext_group = (char*)(malloc(group_sizes[i - 1] * sizeof(char)));
                scorep::call::ipc_recv(ext_group, group_sizes[i - 1], SCOREP_IPC_CHAR, i);
                logging::debug() << "got group from rank: " << i;
                auto ext_group_str = std::string(ext_group, group_sizes[i - 1]);
                free(ext_group);

                std::stringstream ss(ext_group_str);
                std::string item;
                while (std::getline(ss, item, '\n'))
                {
                    if (item.length() > 0)
                    {
                        logging::debug() << "Add Item: \"" << item << "\"";
                        groups.insert(item);
                    }
                }
            }
        }
        else
        {
            std::stringstream ss;
            for (auto& elem : groups)
            {
                ss << elem << '\n';
            }
            auto group = ss.str();
            std::uint64_t group_size = std::strlen(group.c_str());

            vampir::logging::debug() << "send size: " << group_size << " from rank " << rank;
            scorep::call::ipc_send(&group_size, 1, SCOREP_IPC_UINT64_T, 0);
            vampir::logging::debug() << "send group from rank " << rank;
            scorep::call::ipc_send(group.c_str(), group_size, SCOREP_IPC_CHAR, 0);
        }
    }
}
void vampir::groups_writer::write_data()
{
    auto rank = scorep::call::ipc_get_rank();

    if (rank == 0)
    {
        auto filename = scorep::call::experiment_dir_name() + "/scorep.fgp";
        std::ofstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(filename);
        file << "BEGIN OPTIONS\n"
                "\tMATCHING_STRATEGY=FIRST\n"
                "\tCASE_SENSITIVITY_FUNCTION_NAME=NO\n"
                "\tCASE_SENSITIVITY_MANGLED_NAME=NO\n"
                "\tCASE_SENSITIVITY_SOURCE_FILE_NAME=NO\n"
                "END OPTIONS\n";
        for (auto& elem : groups)
        {
            file << "BEGIN FUNCTION_GROUP " << elem << "\n";
            file << "\t NAME=" << elem << "*\n";
            file << "END FUNCTION_GROUP\n";
        }
        file.close();
    }
}
}
