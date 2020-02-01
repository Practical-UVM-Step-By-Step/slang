//------------------------------------------------------------------------------
// Time.cpp
// Contains various time-related utilities and functions
//
// File is under the MIT license; see LICENSE for details
//------------------------------------------------------------------------------
#include "slang/numeric/Time.h"

#include <fmt/format.h>

#include "slang/util/StringTable.h"

namespace slang {

const static StringTable<TimeUnit> strToUnit = {
    { "s", TimeUnit::Seconds },       { "ms", TimeUnit::Milliseconds },
    { "us", TimeUnit::Microseconds }, { "ns", TimeUnit::Nanoseconds },
    { "ps", TimeUnit::Picoseconds },  { "fs", TimeUnit::Femtoseconds }
};

bool suffixToTimeUnit(string_view timeSuffix, TimeUnit& unit) {
    return strToUnit.lookup(timeSuffix, unit);
}

string_view timeUnitToSuffix(TimeUnit unit) {
    switch (unit) {
        case TimeUnit::Seconds:
            return "s";
        case TimeUnit::Milliseconds:
            return "ms";
        case TimeUnit::Microseconds:
            return "us";
        case TimeUnit::Nanoseconds:
            return "ns";
        case TimeUnit::Picoseconds:
            return "ps";
        case TimeUnit::Femtoseconds:
            return "fs";
    }
    THROW_UNREACHABLE;
}

TimeScaleValue::TimeScaleValue(string_view str) {
    size_t idx;
    int i = std::stoi(std::string(str), &idx);

    while (idx < str.size() && str[idx] == ' ')
        idx++;

    TimeUnit u;
    if (idx >= str.size() || !suffixToTimeUnit(str.substr(idx), u))
        throw std::invalid_argument("Time value suffix is missing or invalid");

    auto tv = fromLiteral(double(i), u);
    if (!tv)
        throw std::invalid_argument("Invalid time scale value");

    *this = *tv;
}

optional<TimeScaleValue> TimeScaleValue::fromLiteral(double value, TimeUnit unit) {
    if (value == 1)
        return TimeScaleValue(unit, TimeScaleMagnitude::One);
    if (value == 10)
        return TimeScaleValue(unit, TimeScaleMagnitude::Ten);
    if (value == 100)
        return TimeScaleValue(unit, TimeScaleMagnitude::Hundred);

    return std::nullopt;
}

std::string TimeScaleValue::toString() const {
    std::string result = std::to_string(int(magnitude));
    result.append(timeUnitToSuffix(unit));
    return result;
}

bool TimeScaleValue::operator>(const TimeScaleValue& rhs) const {
    // Unit enum is specified in reverse order, so check in the opposite direction.
    if (unit < rhs.unit)
        return true;
    if (unit > rhs.unit)
        return false;
    return magnitude > rhs.magnitude;
}

bool TimeScaleValue::operator==(const TimeScaleValue& rhs) const {
    return unit == rhs.unit && magnitude == rhs.magnitude;
}

std::ostream& operator<<(std::ostream& os, const TimeScaleValue& tv) {
    return os << tv.toString();
}

std::string TimeScale::toString() const {
    return fmt::format("{} / {}", base.toString(), precision.toString());
}

bool TimeScale::operator==(const TimeScale& rhs) const {
    return base == rhs.base && precision == rhs.precision;
}

std::ostream& operator<<(std::ostream& os, const TimeScale& ts) {
    return os << ts.toString();
}

} // namespace slang
