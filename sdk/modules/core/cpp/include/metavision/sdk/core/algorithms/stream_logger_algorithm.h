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

#ifndef METAVISION_SDK_CORE_STREAM_LOGGER_ALGORITHM_H
#define METAVISION_SDK_CORE_STREAM_LOGGER_ALGORITHM_H

#include <boost/filesystem.hpp>
#include <string>
#include <limits>
#include <vector>
#include <fstream>
#include <iomanip>

#include "metavision/sdk/base/utils/sdk_log.h"
#include "metavision/sdk/base/utils/timestamp.h"
#include "metavision/sdk/base/utils/generic_header.h"
#include "metavision/sdk/base/events/detail/event_traits.h"
#include "metavision/sdk/base/utils/DAT_helper.h"

namespace Metavision {

/// @brief Logs the stream to a file
class StreamLoggerAlgorithm {
    static constexpr auto InvalidTimestamp = std::numeric_limits<std::int32_t>::max();

public:
    /// @brief Builds a new StreamLogger object with given geometry
    /// @param filename Name of the file to write into. If the file already exists, its previous content will be
    /// lost.
    /// @param width Width of the producer
    /// @param height Height of the producer
    inline StreamLoggerAlgorithm(const std::string &filename, std::size_t width, std::size_t height);

    /// @brief Default destructor
    ~StreamLoggerAlgorithm() = default;

    /// @brief Enables or disables data logging.
    /// @param state Flag to enable/disable the logger
    /// @param reset_ts Flag to reset the timestamp, the timestamp used in the
    ///              last call to update will be considered as timestamp zero
    /// @param split_time_seconds Time in seconds to split the file. By default is disabled: InvalidTimestamp (-1).
    /// @throw It will throw an exception if the user tries to reset the timestamp
    ///        or split the stream while the StreamLogger is enabled and running.
    inline void enable(bool state, bool reset_ts = true, std::int32_t split_time_seconds = InvalidTimestamp);

    /// @brief Returns state of data logging.
    /// @return true if data logging in enabled false otherwise
    inline bool is_enable() const;

    inline std::int32_t get_split_time_seconds() const;

    /// @brief Changes the destination file of the logger.
    /// @param filename Name of the file to write into.
    /// @param reset_ts If we are currently recording,
    /// the timestamp used in the last call to update will be considered as timestamp zero
    inline void change_destination(const std::string &filename, bool reset_ts = true);

    /// @brief Exports the information in the input buffer into the StreamLogger
    /// @tparam InputIterator Read-Only iterator with Event2d base class
    /// @param first Beginning of the input iterator
    /// @param last End of the input iterator
    /// @param ts Input buffer timestamp
    template<class InputIterator>
    inline void process_events(InputIterator first, InputIterator last, timestamp ts);

    /// @note process(...) is deprecated since version 2.2.0 and will be removed in later releases.
    ///       Please use process_events(...) instead
    template<class InputIterator>
    // clang-format off
    [[deprecated("process(...) is deprecated since version 2.2.0 and will be removed in later releases. Please use "
                 "process_events(...) instead")]]
    inline void process(InputIterator first, InputIterator last, timestamp ts)
    // clang-format on
    {
        static bool warning_already_logged = false;
        if (!warning_already_logged) {
            std::ostringstream oss;
            oss << "StreamLoggerAlgorithm::process(...) is deprecated since version 2.2.0 ";
            oss << "and will be removed in later releases. ";
            oss << "Please use StreamLoggerAlgorithm::process_events(...) instead" << std::endl;
            MV_SDK_LOG_WARNING() << oss.str();
            warning_already_logged = true;
        }
        process_events(first, last, ts);
    }

    /// @brief Closes the streaming.
    inline void close();

protected:
    /// @brief Changes the destination file internally
    /// @param filename Name of the file to write into
    inline void set_filename(const std::string &filename);

    /// @brief Returns the StreamLogger file name
    /// @note If the system is working in split mode, it returns the file used in each iteration
    inline std::string get_filename() const;

    /// @brief Splits the current file, if the timestamp reach the timeout
    /// @param ts Current timestamp
    inline void split_file(timestamp ts);

protected:
    std::size_t width_{0};
    std::size_t height_{0};
    std::size_t split_counter_{0};
    std::string filename_{};
    std::string filename_base_{};
    std::string filename_ext_{};
    std::ofstream output_{};
    bool enable_{false};
    bool header_written_{false};
    std::int32_t split_timestamp_secs_{InvalidTimestamp};
    timestamp split_timestamp_us_{InvalidTimestamp};
    timestamp initial_timestamp_{0};
    timestamp last_timestamp_{0};
    std::vector<std::uint8_t> buffer_{};
};

inline StreamLoggerAlgorithm::StreamLoggerAlgorithm(const std::string &filename, std::size_t width,
                                                    std::size_t height) :
    width_(width), height_(height) {
    set_filename(filename);
}

inline void StreamLoggerAlgorithm::enable(bool state, bool reset_ts, std::int32_t split_time_seconds) {
    const auto split_enabled = split_time_seconds != InvalidTimestamp;
    if (split_enabled) {
        const auto previously_enabled = split_timestamp_secs_ != InvalidTimestamp;
        if (!previously_enabled) {
            split_counter_ = 0;
        }

        split_timestamp_secs_ = split_time_seconds;
        split_timestamp_us_   = static_cast<timestamp>(split_time_seconds) * timestamp(1e6);
    }

    if (enable_ == state) {
        return;
    }
    enable_            = state;
    initial_timestamp_ = 0;
    if (enable_) {
        if (output_.is_open()) {
            output_.close();
        }

        output_.open(get_filename(), std::ios::binary);
        if (output_.fail()) {
            throw std::runtime_error(
                "Could not open file '" + get_filename() +
                " to record. Make sure it is a valid filename and that you have permissions to write it.");
        }
        header_written_    = false;
        initial_timestamp_ = reset_ts ? last_timestamp_ : 0;
    } else if (output_.is_open()) {
        output_.close();
    }
}

bool StreamLoggerAlgorithm::is_enable() const {
    return enable_;
}

std::int32_t StreamLoggerAlgorithm::get_split_time_seconds() const {
    return split_timestamp_secs_;
}

inline void StreamLoggerAlgorithm::change_destination(const std::string &filename, bool reset_ts) {
    const auto previous_state = enable_;
    if (enable_) {
        StreamLoggerAlgorithm::enable(false);
    }

    set_filename(filename);
    split_counter_ = 0;
    StreamLoggerAlgorithm::enable(previous_state, reset_ts, split_timestamp_secs_);
}

inline void StreamLoggerAlgorithm::close() {
    output_.close();
}

inline void StreamLoggerAlgorithm::set_filename(const std::string &filename) {
    boost::filesystem::path path(filename);
    filename_      = filename;
    filename_base_ = path.stem().string();
    filename_ext_  = path.extension().string();
}

inline std::string StreamLoggerAlgorithm::get_filename() const {
    if (split_timestamp_us_ != InvalidTimestamp) {
        std::ostringstream split_filename;
        split_filename << filename_base_ << "_" << std::setw(4) << std::setfill('0') << std::to_string(split_counter_)
                       << filename_ext_;
        return split_filename.str();
    } else {
        return filename_;
    }
}

void StreamLoggerAlgorithm::split_file(timestamp ts) {
    if (split_timestamp_us_ != InvalidTimestamp && (ts - initial_timestamp_) >= split_timestamp_us_) {
        ++split_counter_;
        last_timestamp_ = ts;
        //        enable(false, true, split_timestamp_secs_);
        //        enable(true, true, split_timestamp_secs_);
        if (output_.is_open()) {
            output_.close();
        }

        output_.open(get_filename(), std::ios::binary);
        header_written_    = false;
        initial_timestamp_ = last_timestamp_;
    }
}

template<class InputIterator>
inline void StreamLoggerAlgorithm::process_events(InputIterator first, InputIterator last, timestamp ts) {
    using value_type            = typename std::iterator_traits<InputIterator>::value_type;
    constexpr auto RawEventSize = get_event_size<value_type>();
    const auto size             = static_cast<std::size_t>(std::distance(first, last));

    if (size > 0 && enable_ && output_.is_open()) {
        if (!header_written_) {
            GenericHeader::HeaderMap header{{"Width", std::to_string(width_)}, {"Height", std::to_string(height_)}};
            Metavision::write_DAT_header<value_type>(output_, header);
            header_written_ = true;
        }

        buffer_.resize(size * RawEventSize);
        auto *buf         = buffer_.data();
        auto byte_written = 0ul;
        for (; first != last; ++first) {
            if (first->t >= initial_timestamp_) {
                first->write_event(buf, initial_timestamp_);
                buf += RawEventSize;
                byte_written += RawEventSize;
                last_timestamp_ = first->t;
            }
        }
        output_.write((const char *)buffer_.data(), byte_written);
        split_file(ts);
    }
    last_timestamp_ = ts;
}

} // namespace Metavision

#endif // METAVISION_SDK_CORE_STREAM_LOGGER_ALGORITHM_H
