#include <ScenarioPlugin.hpp>
#include <QStringList>
#include <Control/ScenarioControl.hpp>
#include <Process/ScenarioFactory.hpp>
#include <Panel/ProcessPanelFactory.hpp>

#include <Inspector/Constraint/ConstraintInspectorFactory.hpp>
#include <Inspector/Event/EventInspectorFactory.hpp>
#include <Inspector/Scenario/ScenarioInspectorFactory.hpp>
#include <Inspector/TimeNode/TimeNodeInspectorFactory.hpp>

#include <State/Message.hpp>

ScenarioPlugin::ScenarioPlugin() :
    QObject {},
        iscore::PluginControlInterface_QtInterface {},
        iscore::DocumentDelegateFactoryInterface_QtInterface {},
        iscore::FactoryFamily_QtInterface {},
        iscore::FactoryInterface_QtInterface {},
m_control {new ScenarioControl{nullptr}}
{
    setObjectName("ScenarioPlugin");

    // TODO there should be another way... :'(
    QMetaType::registerComparators<Message>();
    qRegisterMetaTypeStreamOperators<MessageList>();
}

// Interfaces implementations :
QStringList ScenarioPlugin::document_list() const
{
    return {"Scenario document"};
}

#include "Document/BaseElement/ScenarioDocument.hpp"
iscore::DocumentDelegateFactoryInterface* ScenarioPlugin::document_make(QString name)
{
    if(name == QString("Scenario document"))
    {
        return new ScenarioDocument;
    }

    return nullptr;
}

iscore::PluginControlInterface* ScenarioPlugin::control_make()
{
    return m_control;
}

QStringList ScenarioPlugin::panel_list() const
{
    // Disabled for now.
    // TODO Later
    // return {"ProcessPanel"};

    return {};
}

iscore::PanelFactoryInterface*ScenarioPlugin::panel_make(QString name)
{
    if(name == "ProcessPanel")
        return new ProcessPanelFactory;

    return nullptr;
}

QVector<iscore::FactoryFamily> ScenarioPlugin::factoryFamilies_make()
{
    return {{"Process",
            std::bind(&ProcessList::registerProcess,
                      m_control->processList(),
            std::placeholders::_1)}
    };
}

QVector<iscore::FactoryInterface*> ScenarioPlugin::factories_make(QString factoryName)
{
    if(factoryName == "Process")
    {
        return {new ScenarioFactory};
    }

    if(factoryName == "Inspector")
    {
        return {new ConstraintInspectorFactory,
                new EventInspectorFactory,
                new ScenarioInspectorFactory,
                new TimeNodeInspectorFactory};
    }

    return {};
}
