#ifndef CARDWIDGET_HPP
#define CARDWIDGET_HPP

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class CardWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit CardWidget(const QString &title, QWidget *parent = nullptr);

    signals:
        void cardClicked();

    private:
        QPushButton *button;
};

#endif // CARDWIDGET_HPP
