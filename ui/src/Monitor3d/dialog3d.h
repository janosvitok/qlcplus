#ifndef DIALOG3D_H
#define DIALOG3D_H

#include <QDialog>
#include <doc.h>
#include "myscene.h"

namespace Ui {
class Dialog3d;
}

class Dialog3d : public QDialog
{
    Q_OBJECT

public:
    /** Get the monitor singleton instance. Can be NULL. */
    static Dialog3d* instance();

    /** Create or show Dialog3d */
    static void createAndShow(QWidget* parent, Doc* doc);

protected slots:
    void slotUniversesWritten(int index, const QByteArray& ua);

private:
    explicit Dialog3d(QWidget *parent, Doc* doc);
    ~Dialog3d();

protected:
    /** The singleton Dialog3d instance */
    static Dialog3d* s_instance;

    Doc* _doc;
    MyScene* _scene;

private:
    Ui::Dialog3d *ui;
};

#endif // DIALOG3D_H
