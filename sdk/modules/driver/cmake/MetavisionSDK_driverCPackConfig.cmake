# Copyright (c) Prophesee S.A.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
# on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and limitations under the License.

##########################################################
#
#    Metavision SDK Driver - debian packages information
#

# File and package name of the components are automatically set, just need to set the package description
# and potential dependencies

# Runtime (library)
set(CPACK_COMPONENT_METAVISION-SDK-DRIVER_DESCRIPTION "Metavision SDK Driver library.\n${OPEN_PACKAGE_LICENSE}")
set(CPACK_COMPONENT_METAVISION-SDK-DRIVER_DEPENDS metavision-hal metavision-sdk-base metavision-sdk-core)

# Runtime (apps)
set(CPACK_COMPONENT_METAVISION-SDK-DRIVER-BIN_DESCRIPTION "Binaries for the Metavision SDK Driver applications.\n${OPEN_PACKAGE_LICENSE}")
set(CPACK_COMPONENT_METAVISION-SDK-DRIVER-BIN_DEPENDS metavision-sdk-driver metavision-sdk-core)

# Development package
set(CPACK_COMPONENT_METAVISION-SDK-DRIVER-DEV_DESCRIPTION "Development (C++) files for Metavision SDK Driver library.\n${OPEN_PACKAGE_LICENSE}")
set(CPACK_COMPONENT_METAVISION-SDK-DRIVER-DEV_DEPENDS metavision-sdk-driver metavision-sdk-base-dev metavision-sdk-core-dev metavision-hal-dev)

# Samples
set(CPACK_COMPONENT_METAVISION-SDK-DRIVER-SAMPLES_DESCRIPTION "Samples for Metavision SDK Driver library.\n${OPEN_PACKAGE_LICENSE}")
set(CPACK_COMPONENT_METAVISION-SDK-DRIVER-SAMPLES_DEPENDS
    metavision-sdk-base-dev
    metavision-sdk-core-dev
    metavision-sdk-driver-dev
    metavision-sdk-ui-dev)