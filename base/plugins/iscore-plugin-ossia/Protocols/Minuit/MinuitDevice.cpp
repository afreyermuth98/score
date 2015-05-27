#include "MinuitDevice.hpp"
#include <API/Headers/Network/Node.h>
#include <API/Headers/Network/AddressValue.h>
MinuitDevice::MinuitDevice(const DeviceSettings &settings):
    OSSIADevice{settings}
{
    using namespace OSSIA;

    auto stgs = settings.deviceSpecificSettings.value<MinuitSpecificSettings>();
    Minuit minuitDeviceParameters{stgs.host.toStdString(),
                                  stgs.inPort,
                                  stgs.outPort};

    m_dev = Device::create(minuitDeviceParameters, settings.name.toStdString());
}

bool MinuitDevice::canRefresh() const
{
    return true;
}

// Utility functions to convert from one node to another.
namespace
{
QString valueTypeToString(OSSIA::AddressValue::Type t)
{
    switch(t)
    {
        case OSSIA::AddressValue::Type::NONE:
            return "None";
        case OSSIA::AddressValue::Type::BOOL:
            return "Bool";
        case OSSIA::AddressValue::Type::INT:
            return "Int";
        case OSSIA::AddressValue::Type::FLOAT:
            return "Float";
        case OSSIA::AddressValue::Type::CHAR:
            return "Char";
        case OSSIA::AddressValue::Type::STRING:
            return "String";
        case OSSIA::AddressValue::Type::TUPLE:
            return "Tuple";
        case OSSIA::AddressValue::Type::GENERIC:
            return "Generic";
        default:
            Q_ASSERT(false);
            return "";
    }
}

IOType accessModeToIOType(OSSIA::Address::AccessMode t)
{
    switch(t)
    {
        case OSSIA::Address::AccessMode::GET:
            return IOType::In;
        case OSSIA::Address::AccessMode::SET:
            return IOType::Out;
        case OSSIA::Address::AccessMode::BI:
            return IOType::InOut;
        default:
            Q_ASSERT(false);
            return IOType::Invalid;
    }
}


void nodeFullName_rec(const OSSIA::Node& node, QStringList& names)
{
    names.push_back(QString::fromStdString(node.getName()));
    try {
        const OSSIA::Node& n = node.getParent();
        nodeFullName_rec(n, names);
    }
    catch(...)
    {
        return;
    }
}

QString nodeFullName(const OSSIA::Node& node)
{
    QStringList names;
    nodeFullName_rec(node, names);

    std::reverse(names.begin(), names.end());

    auto name = names.join("/");
    name.prepend("/");

    return name;
}

AddressSettings extractAddressSettings(const OSSIA::Node& node)
{
    AddressSettings s;
    const auto& addr = node.getAddress();

    s.name = nodeFullName(node);
    s.valueType = valueTypeToString(addr->getValueType());
    s.ioType = accessModeToIOType(addr->getAccessMode());
    return s;
}

Node* OssiaToDeviceExplorer(const OSSIA::Node& node)
{
    Node* n = new Node;

    // 1. Set the parameters
    n->setAddressSettings(extractAddressSettings(node));

    // 2. Recurse on the children
    for(const auto& ossia_child : node.children())
    {
        n->addChild(OssiaToDeviceExplorer(*ossia_child.get()));
    }

    return n;
}
}

Node MinuitDevice::refresh()
{
    Node device;
    if(m_dev->updateNamespace())
    {
        // Make a device explorer node from the current state of the device.
        // First make the node corresponding to the root node.

        device.setDeviceSettings(settings());
        device.setAddressSettings(extractAddressSettings(*m_dev.get()));

        // Recurse on the children
        for(const auto& node : m_dev->children())
        {
            device.addChild(OssiaToDeviceExplorer(*node.get()));
        }
    }

    device.setName(QString::fromStdString(m_dev->getName()));

    return device;
}
