#pragma once
#include <QPointer>
#include <score/model/IdentifiedObjectAbstract.hpp>

namespace score
{
struct SCORE_LIB_BASE_EXPORT FocusManager : public QObject
{
  Q_OBJECT
public:
  const IdentifiedObjectAbstract* get()
  {
    return m_obj;
  }

  template <typename T>
  void set(QPointer<const T> obj)
  {
    m_obj = obj.data();
    emit changed();
  }

  void set(std::nullptr_t)
  {
    m_obj.clear();
    emit changed();
  }

Q_SIGNALS:
  void changed();

private:
  QPointer<const IdentifiedObjectAbstract> m_obj{};
};

struct SCORE_LIB_BASE_EXPORT FocusFacade
{
private:
  FocusManager& m_mgr;

public:
  FocusFacade(FocusManager& mgr) : m_mgr{mgr}
  {
  }

  const IdentifiedObjectAbstract* get() const
  {
    return m_mgr.get();
  }
};
}
