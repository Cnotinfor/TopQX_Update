#ifndef UPDATEEXAMPLE_H
#define UPDATEEXAMPLE_H

#include <QWidget>

namespace Ui {
    class UpdateExample;
}

class UpdateExample : public QWidget
{
    Q_OBJECT

public:
    explicit UpdateExample(QWidget *parent = 0);
    ~UpdateExample();

private:
    Ui::UpdateExample *ui;
};

#endif // UPDATEEXAMPLE_H
