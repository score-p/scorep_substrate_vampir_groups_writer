/*
 * Copyright (c) 2015-2016, Technische Universität Dresden, Germany
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to
 *    endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file interface.cpp
 *
 * @brief Interface to Score-P SCOREP_SubstratePlugins.h
 */

#ifndef INCLUDE_vampir_INTERFACE_CPP_
#define INCLUDE_vampir_INTERFACE_CPP_

#include <environment.hpp>
#include <exception.hpp>
#include <groups_writer.hpp>
#include <log.hpp>
#include <scorep.hpp>

#include <string.h>

namespace scorep
{

static const SCOREP_SubstratePluginCallbacks* calls;

namespace call
{
    std::string experiment_dir_name()
    {
        return std::string(scorep::calls->SCOREP_GetExperimentDirName());
    }

    std::string region_handle_get_name(SCOREP_RegionHandle handle)
    {
        return std::string(scorep::calls->SCOREP_RegionHandle_GetName(handle));
    }

    SCOREP_RegionType region_handle_get_type(SCOREP_RegionHandle handle)
    {
        return scorep::calls->SCOREP_RegionHandle_GetType(handle);
    }

    std::string string_handle_get(SCOREP_StringHandle handle)
    {
        return std::string(scorep::calls->SCOREP_StringHandle_Get(handle));
    }

    int ipc_get_size()
    {
        return scorep::calls->SCOREP_Ipc_GetSize();
    }

    int ipc_get_rank()
    {
        return scorep::calls->SCOREP_Ipc_GetRank();
    }

    void ipc_send(const void* buf, int count, SCOREP_Ipc_Datatype datatype, int dest)
    {
        scorep::calls->SCOREP_Ipc_Send(buf, count, datatype, dest);
    }

    void ipc_recv(void* buf, int count, SCOREP_Ipc_Datatype datatype, int source)
    {
        scorep::calls->SCOREP_Ipc_Recv(buf, count, datatype, source);
    }

    std::uint32_t location_get_gloabl_id(const SCOREP_Location* locationData)
    {
        return scorep::calls->SCOREP_Location_GetGlobalId(locationData);
    }
} // namespace call

namespace callback
{

    /*Management Callbacks
     *
     * This callback manages the plugin
     */
    namespace management
    {

        static void new_definition_handle(SCOREP_AnyHandle handle, SCOREP_HandleType type)
        {
            try
            {
                vampir::groups_writer::instance().new_definition_handle(handle, type);
            }
            catch (std::exception& e)
            {
                vampir::exception::print_uncaught_exception(e, "new_definition_handle");
                return;
            }
        }

        /** This function is called form scorep before MPI_Init() is done.
         *
         */
        static int init()
        {
            try
            {
                auto log_verbose = vampir::environment::get("VERBOSE", "WARN");
                auto level = severity_from_string(log_verbose, nitro::log::severity_level::info);
                vampir::log::set_min_severity_level(level);

                vampir::logging::info() << "[Score-P] Initalising vampir";

                vampir::groups_writer::_instance_.reset(new vampir::groups_writer());
                vampir::logging::debug() << "[Score-P] Init Done";

                return 0;
            }
            catch (std::exception& e)
            {
                vampir::exception::print_uncaught_exception(e, "init");
                return -1;
            }
        }

        /** This function is called form scorep after MPI_Init() comunication is called.
         *
         */
        static void init_mpp()
        {
            vampir::logging::debug() << "init_mpp called";

            try
            {
                vampir::log::set_ipc_rank(call::ipc_get_rank());
                vampir::groups_writer::instance().init_mpp();
            }
            catch (std::exception& e)
            {
                vampir::exception::print_uncaught_exception(e, "init_mpp");
            }
        }

        static void finalize()
        {
            /*
             * reset unique_ptr, thus calling destructor of the plugin instance
             */
            try
            {
                vampir::logging::info() << "[Score-P] Finalising vampir";
                vampir::groups_writer::_instance_.reset(nullptr);
            }
            catch (std::exception& e)
            {
                /*
                 * honestly if this happens something is realy wrong with that code!
                 * A destruktor should normaly not throw an exeption
                 */
                vampir::exception::print_uncaught_exception(e, "finalize");
            }
        }

        static void write_data(void)
        {
            vampir::logging::debug() << "write_data called";

            try
            {
                vampir::groups_writer::instance().write_data();
            }
            catch (std::exception& e)
            {
                vampir::exception::print_uncaught_exception(e, "write_data");
            }
        }

        static void pre_unify(void)
        {
            vampir::logging::debug() << "pre_unify called";

            try
            {
                vampir::groups_writer::instance().pre_unify();
            }
            catch (std::exception& e)
            {
                vampir::exception::print_uncaught_exception(e, "pre_unify");
            }
        }

        static uint32_t get_event_functions(SCOREP_Substrates_Mode mode,
                                            SCOREP_Substrates_Callback** functions)
        {
            SCOREP_Substrates_Callback* registered_functions = (SCOREP_Substrates_Callback*)calloc(
                SCOREP_SUBSTRATES_NUM_EVENTS, sizeof(SCOREP_Substrates_Callback));

            *functions = registered_functions;
            return SCOREP_SUBSTRATES_NUM_EVENTS;
        }

        static void set_callbacks(const SCOREP_SubstratePluginCallbacks* callbacks, size_t size)
        {
            if (sizeof(SCOREP_SubstratePluginCallbacks) > size)
            {
                vampir::logging::error()
                    << "[Score-P Interface] "
                       "-----------------------------------------------------------";

                vampir::logging::error()
                    << "[Score-P Interface] "
                       "SCOREP_SubstratePluginCallbacks size missmatch. The plugin ";

                vampir::logging::error()
                    << "[Score-P Interface] "
                       "knows more calls than this version of Score-P provides. ";

                vampir::logging::error()
                    << "[Score-P Interface] "
                       "This might lead to problems, including program crashes and ";

                vampir::logging::error() << "[Score-P Interface] "
                                            "invlaid data";

                vampir::logging::error()
                    << "[Score-P Interface] "
                       "-----------------------------------------------------------";
            }
            scorep::calls = callbacks;
        }
    } // namespace management

} // namespace callback

} // namespace scorep

extern "C" {

static SCOREP_SubstratePluginInfo get_info()
{
    // FIXME Workaround for this bug in glibc:
    // https://sourceware.org/ml/libc-alpha/2016-06/msg00203.html
    // Solution here, declare a thread_local variable and access it once.
    // This seems to solve the problem. ¯\_(ツ)_/¯
    static thread_local volatile int tls_dummy;
    tls_dummy++;

    SCOREP_SubstratePluginInfo info;
    memset(&info, 0, sizeof(SCOREP_SubstratePluginInfo));

    info.plugin_version = SCOREP_SUBSTRATE_PLUGIN_VERSION;

    info.init = scorep::callback::management::init;

    info.init_mpp = scorep::callback::management::init_mpp;

    info.finalize = scorep::callback::management::finalize;

    info.new_definition_handle = scorep::callback::management::new_definition_handle;

    info.get_event_functions = scorep::callback::management::get_event_functions;

    info.set_callbacks = scorep::callback::management::set_callbacks;

    info.pre_unify = scorep::callback::management::pre_unify;

    info.write_data = scorep::callback::management::write_data;

    return info;
}

/** Initial call for Score-P
 */
SCOREP_SUBSTRATE_PLUGIN_ENTRY(vampir_groups_writer)
{
    return get_info();
}
}

#endif /*INCLUDE_vampir_INTERFACE_CPP_*/
