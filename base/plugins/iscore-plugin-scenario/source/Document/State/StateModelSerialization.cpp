#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include "source/Document/State/StateModel.hpp"

template<> void Visitor<Reader<DataStream>>::readFrom(const StateModel& s)
{
    readFrom(static_cast<const IdentifiedObject<StateModel>&>(s));

    readFrom(s.metadata);

    m_stream << s.m_eventId
             << s.m_previousConstraint
             << s.m_nextConstraint
             << s.m_heightPercentage

             << s.m_messageItemModel->rootNode();
    insertDelimiter();
}

template<> void Visitor<Writer<DataStream>>::writeTo(StateModel& s)
{
    writeTo(s.metadata);


    m_stream >> s.m_eventId
            >> s.m_previousConstraint
            >> s.m_nextConstraint
            >> s.m_heightPercentage;

    iscore::Node n2;
    m_stream >> n2;
    s.m_messageItemModel = new iscore::MessageItemModel{&s};
    s.messages() = n2;

    checkDelimiter();
}

template<> void Visitor<Reader<JSONObject>>::readFrom(const StateModel& s)
{
    readFrom(static_cast<const IdentifiedObject<StateModel>&>(s));
    m_obj["Metadata"] = toJsonObject(s.metadata);

    m_obj["Event"] = toJsonValue(s.m_eventId);
    m_obj["PreviousConstraint"] = toJsonValue(s.m_previousConstraint);
    m_obj["NextConstraint"] = toJsonValue(s.m_nextConstraint);
    m_obj["HeightPercentage"] = s.m_heightPercentage;

    m_obj["Messages"] = toJsonObject(s.m_messageItemModel->rootNode());
}

template<> void Visitor<Writer<JSONObject>>::writeTo(StateModel& s)
{
    s.metadata = fromJsonObject<ModelMetadata>(m_obj["Metadata"].toObject());

    s.m_eventId = fromJsonValue<Id<EventModel>>(m_obj["Event"]);
    s.m_previousConstraint = fromJsonValue<Id<ConstraintModel>>(m_obj["PreviousConstraint"]);
    s.m_nextConstraint = fromJsonValue<Id<ConstraintModel>>(m_obj["NextConstraint"]);
    s.m_heightPercentage = m_obj["HeightPercentage"].toDouble();

    s.m_messageItemModel = new iscore::MessageItemModel{&s};
    s.messages() = fromJsonObject<iscore::Node>(m_obj["Messages"].toObject());
}
