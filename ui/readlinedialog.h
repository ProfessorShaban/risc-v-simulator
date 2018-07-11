#ifndef READLINEDIALOG_H
#define READLINEDIALOG_H

#include <QDialog>

namespace Ui {
class ReadLineDialog;
}

class ReadLineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReadLineDialog(QWidget *parent = 0);
    ~ReadLineDialog();
    QString GetUserInputLine ();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::ReadLineDialog *ui;
};

#endif // READLINEDIALOG_H
