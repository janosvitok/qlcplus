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

models.path = $$INSTALLROOT/$$MODELSDIR
INSTALLS += models
