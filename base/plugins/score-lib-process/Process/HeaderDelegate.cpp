#include <Process/Dataflow/CableItem.hpp>
#include <Process/Dataflow/Port.hpp>
#include <Process/Dataflow/PortFactory.hpp>
#include <Process/Dataflow/PortItem.hpp>
#include <Process/HeaderDelegate.hpp>
#include <Process/Process.hpp>
#include <Process/Style/ScenarioStyle.hpp>
#include <QApplication>
#include <QPainter>
#include <QTextLayout>
#include <QTextLine>
namespace Process
{
QImage makeGlyphs(const QString& glyph, const QPen& pen)
{
  QImage path;

  QTextLayout lay;
  lay.setFont(ScenarioStyle::instance().Medium8Pt);
  lay.setText(glyph);
  lay.beginLayout();
  QTextLine line = lay.createLine();
  lay.endLayout();
  line.setPosition(QPointF{0., 0.});

  auto r = line.glyphRuns();
  if (r.size() >= 1)
  {
    auto rect = line.naturalTextRect();
    double ratio = qApp->devicePixelRatio();
    path = QImage(
        rect.width() * ratio, rect.height() * ratio,
        QImage::Format_ARGB32_Premultiplied);
    path.setDevicePixelRatio(ratio);
    path.fill(Qt::transparent);

    QPainter p{&path};
    p.setPen(pen);
    p.drawGlyphRun(QPointF{0, 0}, r[0]);
  }

  return path;
}

static double portY()
{
  //  static double& foo = [] () -> double& {
  //    static double d;
  //    auto s = new QSpinBox; s->setRange(0, 50);
  //    QObject::connect(s, SignalUtils::QSpinBox_valueChanged_int(),
  //            [&] (int x) { d = x; });
  //    s->show();
  //    return d;
  //  }();
  return 7.;
}

static double textY()
{
  return 1.;
}
static double minPortWidth()
{
  return 20.;
}

static const QImage& fromGlyphGray()
{
  static const QImage gl{
      makeGlyphs("I:", ScenarioStyle::instance().GrayTextPen)};
  return gl;
}
static const QImage& toGlyphGray()
{
  static const QImage gl{
      makeGlyphs("O:", ScenarioStyle::instance().GrayTextPen)};
  return gl;
}
static const QImage& fromGlyphWhite()
{
  static const QImage gl{
      makeGlyphs("I:", ScenarioStyle::instance().IntervalHeaderTextPen)};
  return gl;
}
static const QImage& toGlyphWhite()
{
  static const QImage gl{
      makeGlyphs("O:", ScenarioStyle::instance().IntervalHeaderTextPen)};
  return gl;
}

DefaultHeaderDelegate::DefaultHeaderDelegate(const Process::LayerPresenter& p)
    : HeaderDelegate{p}
{
  con(p.model(), &Process::ProcessModel::prettyNameChanged, this, [=] {
    updateName();
    updatePorts();
    update();
  });

  con(p.model(), &Process::ProcessModel::inletsChanged, this,
      [=] { updatePorts(); });
  con(p.model(), &Process::ProcessModel::outletsChanged, this,
      [=] { updatePorts(); });
  con(p.model(), &Process::ProcessModel::benchmark, this,
      [=](double d) { updateBench(d); });
  con(p.model().selection, &Selectable::changed, this, [=] (bool b) {
    m_sel = b;
    updateName();
    updatePorts();
    update();
  });

  updateName();
  updatePorts();
}

DefaultHeaderDelegate::~DefaultHeaderDelegate()
{
}

void DefaultHeaderDelegate::updateBench(double d)
{
  const auto& style = ScenarioStyle::instance();
  m_bench = makeGlyphs(
      QString::number(d, 'g', 3),
      m_sel ? style.IntervalHeaderTextPen : style.GrayTextPen);
  update();
}

void DefaultHeaderDelegate::updateName()
{
  if (presenter)
  {
    const auto& style = ScenarioStyle::instance();
    m_line = makeGlyphs(
        presenter->model().prettyName(),
        m_sel ? style.IntervalHeaderTextPen : style.GrayTextPen);
    update();
  }
}

void DefaultHeaderDelegate::setSize(QSizeF sz)
{
  GraphicsShapeItem::setSize(sz);

  for (auto p : m_inPorts)
  {
    if (p->x() > sz.width())
    {
      if (p->isVisible())
        p->setPortVisible(false);
    }
    else
    {
      if (!p->isVisible())
        p->setPortVisible(true);
    }
  }

  for (auto p : m_outPorts)
  {
    if (p->x() > sz.width())
    {
      if (p->isVisible())
        p->setPortVisible(false);
    }
    else
    {
      if (!p->isVisible())
        p->setPortVisible(true);
    }
  }
}

void DefaultHeaderDelegate::updatePorts()
{
  if (!presenter)
    return;
  qDeleteAll(m_inPorts);
  m_inPorts.clear();
  qDeleteAll(m_outPorts);
  m_outPorts.clear();
  const auto& ctx = presenter->context().context;

  qreal x = 0.;
  const auto& inlets = presenter->model().inlets();

  auto& portFactory
      = score::AppContext().interfaces<Process::PortFactoryList>();
  if (!inlets.empty())
  {
    x += 16.;
    for (Process::Inlet* port : inlets)
    {
      if (port->hidden)
        continue;
      auto fact = portFactory.get(port->concreteKey());
      auto item = fact->makeItem(*port, ctx, this, this);
      item->setPos(x, portY());
      m_inPorts.push_back(item);
      x += 10.;
    }
  }

  x += 24. + m_line.width();
  for (Process::Outlet* port : presenter->model().outlets())
  {
    if (port->hidden)
      continue;
    auto fact = portFactory.get(port->concreteKey());
    auto item = fact->makeItem(*port, ctx, this, this);
    item->setPos(x, portY());
    m_outPorts.push_back(item);
    x += 10.;
  }
}

void DefaultHeaderDelegate::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  const auto w = boundingRect().width();
  if (w > minPortWidth())
  {
    if (m_inPorts.empty())
    {
      painter->drawImage(QPointF{0., 1.}, m_line);
      painter->drawImage(QPointF{w - 32., 1.}, m_bench);
      if (m_sel)
      {
        painter->drawImage(
            QPointF{8. + m_line.width(), textY()}, toGlyphWhite());
      }
      else
      {
        painter->drawImage(
            QPointF{8. + m_line.width(), textY()}, toGlyphGray());
      }
    }
    else
    {
      double startText = 16. + m_inPorts.size() * 10.;
      painter->drawImage(QPointF{startText, 1.}, m_line);
      painter->drawImage(QPointF{w - 32., 1.}, m_bench);
      if (m_sel)
      {
        painter->drawImage(QPointF{4., textY()}, fromGlyphWhite());
        painter->drawImage(
            QPointF{startText + 8. + m_line.width(), textY()}, toGlyphWhite());
      }
      else
      {
        painter->drawImage(QPointF{4., textY()}, fromGlyphGray());
        painter->drawImage(
            QPointF{startText + 8. + m_line.width(), textY()}, toGlyphGray());
      }
    }
  }
}
}