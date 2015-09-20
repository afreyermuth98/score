#include "ClearEvent.hpp"

#include "Document/State/StateModel.hpp"

#include "Process/ScenarioModel.hpp"

#include <iscore/serialization/VisitorCommon.hpp>


using namespace iscore;
using namespace Scenario::Command;

ClearState::ClearState(Path<StateModel>&& path) :
    SerializableCommand {factoryName(),
                         commandName(),
                         description()},
    m_path {std::move(path) }
{
    const auto& state = m_path.find();

    m_serializedStates = marshall<DataStream>(state.messages().rootNode());
}

void ClearState::undo()
{
    auto& state = m_path.find();
    iscore::Node states;
    QDataStream s(m_serializedStates);
    s >> states;

    state.messages() = states;
}

void ClearState::redo()
{
    auto& state = m_path.find();

    state.messages() = iscore::Node{};
}

void ClearState::serializeImpl(QDataStream& s) const
{
    s << m_path << m_serializedStates;
}

void ClearState::deserializeImpl(QDataStream& s)
{
    s >> m_path >> m_serializedStates;
}
