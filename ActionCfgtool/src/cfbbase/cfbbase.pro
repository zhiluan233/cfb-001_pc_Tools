include (../windows.pri)
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += qutils \
           smtp \
           utils \
           cfbactiondata \
           cfbhttpclient
cfbactiondata.depends = utils
cfbhttpclient.depends = utils
