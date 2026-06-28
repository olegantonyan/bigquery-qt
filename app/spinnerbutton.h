#ifndef SPINNERBUTTON_H
#define SPINNERBUTTON_H

#include <QIcon>
#include <QPixmap>
#include <QPushButton>

class QTimer;

class SpinnerButton : public QPushButton
{
  Q_OBJECT

public:
  explicit SpinnerButton(QWidget *parent = nullptr);

  void setStaticIcon(const QIcon &icon);
  bool isSpinning() const { return m_inFlight > 0; }

public slots:
  void startSpinning();
  void stopSpinning();

private:
  void advance();

  QIcon m_staticIcon;
  QPixmap m_basePixmap;
  QTimer *m_spinTimer;
  int m_angle = 0;
  int m_inFlight = 0;
};

#endif // SPINNERBUTTON_H
