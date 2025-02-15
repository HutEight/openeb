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

#include "metavision/hal/device/device.h"
#include "metavision/hal/facilities/i_hw_identification.h"
#include "metavision/sdk/driver/camera_generation.h"
#include "metavision/sdk/driver/camera_error_code.h"
#include "metavision/sdk/driver/camera_exception.h"
#include "metavision/sdk/driver/internal/camera_generation_internal.h"

namespace Metavision {

CameraGeneration::Private::Private(short version_major, short version_minor) :
    major_(version_major), minor_(version_minor) {}

CameraGeneration::Private::~Private() {}

CameraGeneration *CameraGeneration::Private::build(short version_major, short version_minor) {
    return new CameraGeneration(new Private(version_major, version_minor));
}

CameraGeneration *CameraGeneration::Private::build(Device &device) {
    auto *hw_id             = device.get_facility<I_HW_Identification>();
    const auto &sensor_info = hw_id->get_sensor_info();
    return CameraGeneration::Private::build(sensor_info.major_version_, sensor_info.minor_version_);
}

CameraGeneration::~CameraGeneration() {}

CameraGeneration::CameraGeneration(Private *pimpl) : pimpl_(pimpl) {}

short CameraGeneration::version_major() const {
    return pimpl_->major_;
}

short CameraGeneration::version_minor() const {
    return pimpl_->minor_;
}

CameraGeneration::Type CameraGeneration::type() const {
    throw CameraException(CameraErrorCode::DeprecatedFeature, "type not supported anymore.");
}

CameraGeneration::Private &CameraGeneration::get_pimpl() {
    return *pimpl_;
}

bool CameraGeneration::operator==(const CameraGeneration &c) const {
    return *pimpl_ == *c.pimpl_;
}

bool CameraGeneration::operator!=(const CameraGeneration &c) const {
    return *pimpl_ != *c.pimpl_;
}

bool CameraGeneration::operator<(const CameraGeneration &c) const {
    return *pimpl_ < *c.pimpl_;
}

bool CameraGeneration::operator<=(const CameraGeneration &c) const {
    return *pimpl_ <= *c.pimpl_;
}

bool CameraGeneration::operator>(const CameraGeneration &c) const {
    return *pimpl_ > *c.pimpl_;
}

bool CameraGeneration::operator>=(const CameraGeneration &c) const {
    return *pimpl_ >= *c.pimpl_;
}

bool CameraGeneration::Private::operator==(const CameraGeneration::Private &c) const {
    return major_ == c.major_ && minor_ == c.minor_;
}

bool CameraGeneration::Private::operator!=(const CameraGeneration::Private &c) const {
    return !(*this == c);
}

bool CameraGeneration::Private::operator<(const CameraGeneration::Private &c) const {
    return major_ < c.major_ || (major_ == c.major_ && minor_ < c.minor_);
}

bool CameraGeneration::Private::operator<=(const CameraGeneration::Private &c) const {
    return (*this == c) || (*this < c);
}

bool CameraGeneration::Private::operator>(const CameraGeneration::Private &c) const {
    return !(*this <= c);
}

bool CameraGeneration::Private::operator>=(const CameraGeneration::Private &c) const {
    return (*this == c) || (*this > c);
}

} // namespace Metavision
