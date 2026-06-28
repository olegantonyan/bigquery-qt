#include "spinnerbutton.h"

#include <QPainter>
#include <QTimer>

SpinnerButton::SpinnerButton(QWidget *parent)
  : QPushButton(parent)
  , m_spinTimer(new QTimer(this))
{
  m_spinTimer->setInterval(80);
  connect(m_spinTimer, &QTimer::timeout, this, &SpinnerButton::advance);
}

void SpinnerButton::setStaticIcon(const QIcon &icon)
{
  m_staticIcon = icon;
  if (!isSpinning())
    setIcon(m_staticIcon);
}

void SpinnerButton::startSpinning()
{
  if (m_inFlight++ == 0) {
    setEnabled(false);
    m_angle = 0;
    m_basePixmap = m_staticIcon.pixmap(iconSize());
    m_spinTimer->start();
  }
}

void SpinnerButton::stopSpinning()
{
  if (--m_inFlight <= 0) {
    m_inFlight = 0;
    m_spinTimer->stop();
    setIcon(m_staticIcon);
    setEnabled(true);
  }
}

void SpinnerButton::advance()
{
  if (m_basePixmap.isNull())
    return;
  m_angle = (m_angle + 30) % 360;
  const qreal dpr = m_basePixmap.devicePixelRatio();
  const qreal w = m_basePixmap.width() / dpr;
  const qreal h = m_basePixmap.height() / dpr;

  QPixmap rotated(m_basePixmap.size());
  rotated.setDevicePixelRatio(dpr);
  rotated.fill(Qt::transparent);

  QPainter painter(&rotated);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.translate(w / 2.0, h / 2.0);
  painter.rotate(m_angle);
  painter.translate(-w / 2.0, -h / 2.0);
  painter.drawPixmap(0, 0, m_basePixmap);
  painter.end();

  setIcon(QIcon(rotated));
}
