/*
 * scorep.hpp
 *
 *  Created on: 09.08.2016
 *      Author: andreas
 */

#ifndef INCLUDE_RRL_UTIL_SCOREP_HPP_
#define INCLUDE_RRL_UTIL_SCOREP_HPP_

#include <cstdint>
#include <string>

extern "C" {
#include <scorep/SCOREP_PublicHandles.h>
#include <scorep/SCOREP_PublicTypes.h>
#include <scorep/SCOREP_SubstrateEvents.h>
#include <scorep/SCOREP_SubstratePlugins.h>
}

/** build c++ interfaces to a few SCOREP_SubstrateAccess function calls.
 *
 */
namespace scorep
{
namespace call
{

    /** Returns the name of the directory, where the data from an Score-P experiment are saved.
     * This should be used to write debug/performance data.
     * Data should be placed under scorep::experiment_dir_name()/rrl/.
     * Per location data should be placed under
     * scorep::experiment_dir_name()/rrl/(prefix)_scorep::location_get_gloabl_id(location)_(suffix).
     *
     * @return name of the Scorpe-P experiment dir.
     */
    std::string experiment_dir_name();

    /** Retruns the name of a SCOREP_RegionHandle.
     *
     * @param handle SCOREP_RegionHandle
     * @return name of the region
     *
     */
    std::string region_handle_get_name(SCOREP_RegionHandle handle);

    /** Retruns the type of a SCOREP_RegionHandle.
     *
     * @param handle SCOREP_RegionHandle
     * @return id of the region
     *
     */
    SCOREP_RegionType region_handle_get_type(SCOREP_RegionHandle handle);

    /** Retruns the global location id of a SCOREP_Location.
     *
     * @param locationData Score-P location handle
     * @return global location from the locationData
     *
     */
    std::uint32_t location_get_gloabl_id(const SCOREP_Location* locationData);

    int ipc_get_size();
    int ipc_get_rank();

} // namespace call

} // namespace scorep
#endif /* INCLUDE_RRL_UTIL_SCOREP_HPP_ */
