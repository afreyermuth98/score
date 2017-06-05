#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>

#include <Automation/AutomationModel.hpp>
#include <Automation/Inspector/AutomationInspectorWidget.hpp>

namespace Automation
{
class InspectorFactory final
    : public Process::
          InspectorWidgetDelegateFactory_T<ProcessModel, InspectorWidget>
{
  ISCORE_CONCRETE("2c5410ba-d3df-45b8-8444-6d8578b4e1a8")
};
}

namespace Gradient
{
class InspectorFactory final
    : public Process::
          InspectorWidgetDelegateFactory_T<ProcessModel, InspectorWidget>
{
  ISCORE_CONCRETE("7d61cbcb-7980-4d50-86c9-3d54a0299fc5")
};
}

namespace Spline
{
class InspectorFactory final
    : public Process::
    InspectorWidgetDelegateFactory_T<ProcessModel, InspectorWidget>
{
    ISCORE_CONCRETE("94c5afc5-c8c3-44e2-b8b4-6e53d8518677")
};
}
