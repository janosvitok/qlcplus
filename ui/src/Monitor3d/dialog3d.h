#ifndef DIALOG3D_H
#define DIALOG3D_H

#include <QDialog>
#include <doc.h>

namespace Ui {
class Dialog3d;
}

class Dialog3d : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog3d(Doc* doc, QWidget *parent = 0);
    ~Dialog3d();

protected:
    Doc* _doc;

private:
    Ui::Dialog3d *ui;
};

#endif // DIALOG3D_H
