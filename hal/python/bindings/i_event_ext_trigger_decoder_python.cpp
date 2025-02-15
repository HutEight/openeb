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

#include <pybind11/numpy.h>

#include "metavision/utils/pybind/deprecation_warning_exception.h"
#include "hal_python_binder.h"
#include "metavision/sdk/base/events/event_ext_trigger.h"
#include "metavision/hal/facilities/i_event_decoder.h"
#include "pb_doc_hal.h"

namespace Metavision {

namespace {
void set_add_decoded_event_callback_deprecated_helper(I_EventDecoder<EventExtTrigger> &, py::object) {
    throw DeprecationWarningException("set_add_decoded_event_callback", "add_event_buffer_callback");
}

void set_add_decoded_vevent_callback_deprecated_helper(I_EventDecoder<EventExtTrigger> &, py::object) {
    throw DeprecationWarningException("set_add_decoded_vevent_callback", "add_event_buffer_callback");
}

void set_end_decode_callback_deprecated_helper(I_EventDecoder<EventExtTrigger> &, py::object) {
    throw DeprecationWarningException("set_end_decode_callback");
}
} // anonymous namespace

static DeviceFacilityGetter<I_EventDecoder<EventExtTrigger>> getter("get_i_event_ext_trigger_decoder");

static HALFacilityPythonBinder<I_EventDecoder<EventExtTrigger>> bind_decoder(
    [](auto &module, auto &class_binding) {
        using EventExtTriggerIterator_t = I_EventDecoder<EventExtTrigger>::EventIterator_t;

        class_binding
            .def(
                "add_event_buffer_callback",
                +[](I_EventDecoder<EventExtTrigger> &self, py::object object) {
                    std::function<void(EventExtTriggerIterator_t begin, EventExtTriggerIterator_t end)> gil_cb =
                        [=](EventExtTriggerIterator_t begin, EventExtTriggerIterator_t end) {
                            // Wrap memory space as a python readable buffer.
                            auto capsule  = py::capsule(begin, [](void *v) {});
                            auto py_array = py::array_t<EventExtTrigger>(end - begin, begin, capsule);
                            py::gil_scoped_acquire acquire;
                            object(py_array);
                        };
                    return self.add_event_buffer_callback(gil_cb);
                },
                pybind_doc_hal["Metavision::I_EventDecoder::add_event_buffer_callback"])
            .def("remove_callback", &I_EventDecoder<EventExtTrigger>::remove_callback,
                 pybind_doc_hal["Metavision::I_EventDecoder::remove_callback"])
            .def("set_add_decoded_event_callback", set_add_decoded_event_callback_deprecated_helper,
                 pybind_doc_hal["Metavision::I_EventDecoder::set_add_decoded_event_callback"])
            .def("set_add_decoded_vevent_callback", set_add_decoded_vevent_callback_deprecated_helper,
                 pybind_doc_hal["Metavision::I_EventDecoder::set_add_decoded_vevent_callback"])
            .def("set_end_decode_callback", set_end_decode_callback_deprecated_helper,
                 pybind_doc_hal["Metavision::I_EventDecoder::set_end_decode_callback"]);
    },
    "I_EventDecoder_EventExtTrigger");

} // namespace Metavision
