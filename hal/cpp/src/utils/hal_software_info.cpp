/**********************************************************************************************************************
 * Copyright (c) Prophesee S.A.                                                                                       *
 *                                                                                                                    *
 * Licensed under the Apache License, Version 2.0 (the "License");                                                    *
 * you may not use this file except in compliance with the License.                                                   *
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0                                 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed   *
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.                      *
 * See the License for the specific language governing permissions and limitations under the License.                 *
 **********************************************************************************************************************/

#include "metavision/hal/utils/hal_software_info.h"
#include "metavision/hal/version.h"

namespace Metavision {

Metavision::SoftwareInfo &get_hal_software_info() {
    static Metavision::SoftwareInfo hal_info(METAVISION_HAL_VERSION_MAJOR, METAVISION_HAL_VERSION_MINOR,
                                             METAVISION_HAL_VERSION_PATCH, METAVISION_HAL_GIT_COMMIT_DATE,
                                             METAVISION_HAL_GIT_BRANCH_RAW, METAVISION_HAL_GIT_HASH_RAW,
                                             std::to_string(METAVISION_HAL_GIT_COMMIT_DATE));

    return hal_info;
}

} // namespace Metavision
