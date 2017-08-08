#pragma once
#include <iscore/plugins/customfactory/FactoryInterface.hpp>
#include <Inspector/InspectorWidgetFactoryInterface.hpp>
#include <iscore_lib_process_export.h>
namespace Process
{
class ProcessModel;
class StateProcess;
class InspectorWidgetDelegate;
class StateProcessInspectorWidgetDelegate;
}
namespace iscore
{
struct DocumentContext;
}

namespace Process
{
class ISCORE_LIB_PROCESS_EXPORT InspectorWidgetDelegateFactory
    : public Inspector::InspectorWidgetFactory
{
public:
  virtual ~InspectorWidgetDelegateFactory();
  virtual QWidget* make_process(
      const Process::ProcessModel&,
      const iscore::DocumentContext& doc,
      QWidget* parent) const = 0;
  virtual bool matches_process(const Process::ProcessModel&) const = 0;

  bool matches_process(
      const Process::ProcessModel& proc,
      const iscore::DocumentContext& doc,
      QWidget* parent) const
  {
    return matches_process(proc);
  }

  QWidget* make(
       const QList<const QObject*>& objects,
       const iscore::DocumentContext& doc,
       QWidget* parent) const final override;
   bool matches(const QList<const QObject*>& objects) const final override;
};

template <typename Process_T, typename Widget_T>
class InspectorWidgetDelegateFactory_T
    : public Process::InspectorWidgetDelegateFactory
{
private:
  QWidget* make_process(
      const Process::ProcessModel& process,
      const iscore::DocumentContext& doc,
      QWidget* parent) const override
  {
    return new Widget_T{safe_cast<const Process_T&>(process), doc, parent};
  }

  bool matches_process(const Process::ProcessModel& process) const override
  {
    return dynamic_cast<const Process_T*>(&process);
  }
};

class ISCORE_LIB_PROCESS_EXPORT StateProcessInspectorWidgetDelegateFactory
    : public Inspector::InspectorWidgetFactory
{
public:
  virtual ~StateProcessInspectorWidgetDelegateFactory();
  virtual QWidget* make_process(
      const Process::StateProcess&,
      const iscore::DocumentContext& doc,
      QWidget* parent) const = 0;
  virtual bool matches_process(const Process::StateProcess&) const = 0;

  bool matches_process(
      const Process::StateProcess& proc,
      const iscore::DocumentContext& doc,
      QWidget* parent) const
  {
    return matches_process(proc);
  }

  QWidget* make(
       const QList<const QObject*>& objects,
       const iscore::DocumentContext& doc,
       QWidget* parent) const final override;
   bool matches(const QList<const QObject*>& objects) const final override;
};

template <typename Process_T, typename Widget_T>
class StateProcessInspectorWidgetDelegateFactory_T
    : public Process::StateProcessInspectorWidgetDelegateFactory
{
private:
  using Inspector::InspectorWidgetFactory::make;
  using Inspector::InspectorWidgetFactory::matches;
  QWidget* make_process(
      const Process::StateProcess& process,
      const iscore::DocumentContext& doc,
      QWidget* parent) const override
  {
    return new Widget_T{safe_cast<const Process_T&>(process), doc, parent};
  }

  bool matches_process(const Process::StateProcess& process) const override
  {
    return dynamic_cast<const Process_T*>(&process);
  }
};
}
