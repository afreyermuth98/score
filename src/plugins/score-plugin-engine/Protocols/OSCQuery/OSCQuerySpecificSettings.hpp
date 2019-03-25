#pragma once
#include <ossia/detail/optional.hpp>

#include <QMetaType>
#include <QString>

#include <wobjectdefs.h>

namespace Protocols
{
struct OSCQuerySpecificSettings
{
  QString host;
  ossia::optional<int> rate{};
};
}
Q_DECLARE_METATYPE(Protocols::OSCQuerySpecificSettings)
W_REGISTER_ARGTYPE(Protocols::OSCQuerySpecificSettings)