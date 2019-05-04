#pragma once

#include "rev/Types.h"
#include "rev/Unit.h"

namespace rev::physics {
struct DistanceComponent {
};
struct TimeComponent {
};
struct MassComponent {
};

using PhysicsComponents = UnitComponentEnumeration<DistanceComponent, TimeComponent, MassComponent>;

using DistanceComposition = PhysicsComponents::MakeComposition<DistanceComponent>;
using TimeComposition = PhysicsComponents::MakeComposition<TimeComponent>;
using MassComposition = PhysicsComponents::MakeComposition<MassComponent>;

using VelocityComposition = MultiplyComposition<DistanceComposition,
    InvertComposition<TimeComposition>>;
using AccelerationComposition = MultiplyComposition<VelocityComposition,
    InvertComposition<TimeComposition>>;
using MomentumComposition = MultiplyComposition<VelocityComposition, MassComposition>;
using ForceComposition = MultiplyComposition<MassComposition, AccelerationComposition>;

template <typename ValueType>
using Distance = Unit<ValueType, DistanceComposition>;

template <typename ValueType>
using Time = Unit<ValueType, TimeComposition>;

template <typename ValueType>
using Mass = Unit<ValueType, MassComposition>;

template <typename ValueType>
using Velocity = Unit<ValueType, VelocityComposition>;

template <typename ValueType>
using Acceleration = Unit<ValueType, AccelerationComposition>;

template <typename ValueType>
using Momentum = Unit<ValueType, MomentumComposition>;

template <typename ValueType>
using Force = Unit<ValueType, ForceComposition>;

inline Time<float> durationToPhysicsTime(Duration duration)
{
    using FloatSecondsDuration = std::chrono::duration<float>;
    FloatSecondsDuration floatDuration = duration;
    return floatDuration.count();
}

} // namespace rev::physics