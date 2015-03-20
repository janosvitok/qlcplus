include(../../variables.pri)

TEMPLATE = subdirs
TARGET = models

models.files += par.osgt
models.files += PAR64.osgt
models.files += stage.osgt
models.files += moving-head.osgt

models.path = $$INSTALLROOT/$$MODELSDIR
INSTALLS += models
