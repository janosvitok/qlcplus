include(../../variables.pri)

TEMPLATE = subdirs
TARGET = models

models.files += par.osgt

models.path = $$INSTALLROOT/$$MODELSDIR
INSTALLS += models
