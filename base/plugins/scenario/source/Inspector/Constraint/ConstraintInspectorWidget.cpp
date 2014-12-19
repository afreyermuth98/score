#include "ConstraintInspectorWidget.hpp"

#include "Widgets/AddProcessWidget.hpp"
#include "Widgets/AddBoxWidget.hpp"
#include "Widgets/Box/BoxInspectorSection.hpp"

#include "Document/Constraint/ConstraintModel.hpp"
#include "Document/Constraint/Box/BoxModel.hpp"
#include "Document/Constraint/Box/Storey/StoreyModel.hpp"
#include "Commands/Constraint/AddProcessToConstraintCommand.hpp"
#include "Commands/Constraint/AddBoxToConstraint.hpp"
#include "ProcessInterface/ProcessSharedModelInterface.hpp"

#include <InspectorInterface/InspectorSectionWidget.hpp>

#include <tools/ObjectPath.hpp>

#include <QFrame>
#include <QLineEdit>
#include <QLayout>
#include <QFormLayout>
#include <QWidget>
#include <QToolButton>
#include <QPushButton>
#include <QApplication>

using namespace Scenario::Command;


class Separator : public QFrame
{
	public:
		Separator(QWidget* parent):
			QFrame{parent}
		{
			setFrameShape(QFrame::HLine);
			setLineWidth(2);
		}
};

ConstraintInspectorWidget::ConstraintInspectorWidget (ConstraintModel* object, QWidget* parent) :
	InspectorWidgetBase (parent)
{
	setObjectName ("Constraint");

	// Processes
	m_processSection = new InspectorSectionWidget ("Processes", this);
	m_processSection->setObjectName("Processes");

	m_properties.push_back(m_processSection);
	m_properties.push_back(new AddSharedProcessWidget{this});

	// Separator
	m_properties.push_back(new Separator{this});

	// Boxes
	m_boxSection = new InspectorSectionWidget{"Boxes", this};  // TODO Make a custom widget.
	m_boxSection->setObjectName("Boxes");

	m_boxWidget = new BoxWidget{this};

	m_properties.push_back(m_boxSection);
	m_properties.push_back(m_boxWidget);

	// Display data
	updateSectionsView (areaLayout(), m_properties);
	areaLayout()->addStretch(1);

	updateDisplayedValues(object);
}

void ConstraintInspectorWidget::updateDisplayedValues (ConstraintModel* constraint)
{
	// Cleanup the widgets
	for(auto& process : m_processesSectionWidgets) { delete process; }
	m_processesSectionWidgets.clear();

	for(auto& box : m_boxesSectionWidgets) { delete box; }
	m_boxesSectionWidgets.clear();

	// Cleanup the connections
	for(auto& connection : m_connections)
		QObject::disconnect(connection);
	m_connections.clear();


	if (constraint != nullptr)
	{
		m_currentConstraint = constraint;

		// Constraint settings
		setName (constraint->name() );
		setColor (constraint->color() );
		setComments (constraint->comment() );
		setInspectedObject (constraint);
		changeLabelType ("Constraint");

		// Constraint interface
		m_connections.push_back(
					connect(m_currentConstraint, &ConstraintModel::processCreated,
							this,				 &ConstraintInspectorWidget::on_processCreated));
		m_connections.push_back(
					connect(m_currentConstraint, &ConstraintModel::processRemoved,
							this,				 &ConstraintInspectorWidget::on_processRemoved));
		m_connections.push_back(
					connect(m_currentConstraint, &ConstraintModel::boxCreated,
							this,				 &ConstraintInspectorWidget::on_boxCreated));
		m_connections.push_back(
					connect(m_currentConstraint, &ConstraintModel::boxRemoved,
							this,				 &ConstraintInspectorWidget::on_boxRemoved));

		// Processes
		for(ProcessSharedModelInterface* process : constraint->processes())
		{
			displaySharedProcess(process);
		}

		// Box
		m_boxWidget->setModel(m_currentConstraint);
		m_boxWidget->updateComboBox();

		for(BoxModel* box: constraint->boxes())
		{
			setupBox(box);
		}
	}
	else
	{
		m_currentConstraint = nullptr;
		m_boxWidget->setModel(nullptr);
	}
}

void ConstraintInspectorWidget::createProcess(QString processName)
{
	auto cmd = new AddProcessToConstraintCommand(
						ObjectPath::pathFromObject("BaseConstraintModel",
												   m_currentConstraint),
						processName);
	emit submitCommand(cmd);
}

void ConstraintInspectorWidget::createBox()
{
	auto cmd = new AddBoxToConstraint(
						ObjectPath::pathFromObject(
							"BaseConstraintModel",
							m_currentConstraint));
	emit submitCommand(cmd);
}

void ConstraintInspectorWidget::displaySharedProcess(ProcessSharedModelInterface* process)
{
	// TODO specialize by using custom widgets provided by the processes.
	// Also handle the case where a process does not.
	InspectorSectionWidget* newProc = new InspectorSectionWidget (process->processName());
	m_processesSectionWidgets.push_back (newProc);
	m_processSection->addContent (newProc);
}

void ConstraintInspectorWidget::setupBox(BoxModel* box)
{
	// Display the widget
	BoxInspectorSection* newBox = new BoxInspectorSection{QString{"Box.%1"}.arg(box->id()),
														  box,
														  this};

	connect(newBox, &BoxInspectorSection::submitCommand,
			this,	&ConstraintInspectorWidget::submitCommand);

	m_boxesSectionWidgets.push_back(newBox);
	m_boxSection->addContent(newBox);
}


// TODO optimize if necessary
void ConstraintInspectorWidget::on_processCreated(QString processName, int processId)
{
	reloadDisplayedValues();
}

void ConstraintInspectorWidget::on_processRemoved(int processId)
{
	reloadDisplayedValues();
}


void ConstraintInspectorWidget::on_boxCreated(int viewId)
{
	reloadDisplayedValues();
}

void ConstraintInspectorWidget::on_boxRemoved(int viewId)
{
	reloadDisplayedValues();
}

