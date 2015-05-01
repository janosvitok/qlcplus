include(../../variables.pri)

TEMPLATE = subdirs
TARGET = models

models.files += par.osgt
models.files += PAR64.osgt
models.files += stage.osgt
models.files += moving-head.osgt
models.files += moving_fixture-head.osgt
models.files += moving_fixture-yoke.osgt
models.files += moving_fixture-base.osgt
models.files += ledBar.osgt

models.files += mic.3ds
models.files += truss-horiz.3ds
models.files += truss-vertical.3ds
models.files += truss-corner.3ds

models.path = $$INSTALLROOT/$$MODELSDIR
INSTALLS += models
