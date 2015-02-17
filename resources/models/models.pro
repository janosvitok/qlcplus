include(../../variables.pri)

TEMPLATE = subdirs
TARGET = models

models.files += par.osgt PAR64.osgt stage.osgt

models.path = $$INSTALLROOT/$$MODELSDIR
INSTALLS += models
