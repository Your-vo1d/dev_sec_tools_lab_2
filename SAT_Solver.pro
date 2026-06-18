QT -= core gui

TARGET   = SAT_Solver
TEMPLATE = app
CONFIG  += c++14 console
CONFIG  -= app_bundle

QMAKE_CXXFLAGS += -fpermissive

INCLUDEPATH += \
    $$PWD/include \
    $$PWD \
    $$PWD/Allocator-1.0.0

SOURCES += \
    src/BBV.cpp \
    src/boolequation.cpp \
    src/boolinterval.cpp \
    src/first_free_column_branching_strategy.cpp \
    src/main.cpp \
    src/min_dont_care_branching_strategy.cpp \
    src/NodeBoolTree.cpp \
    Allocator-1.0.0/Allocator.cpp \
    src/benchmarks_example.cpp

HEADERS += \
    include/BBV.h \
    include/boolequation.h \
    include/boolinterval.h \
    include/branching_strategy.h \
    include/DataTypes.h \
    include/first_free_column_branching_strategy.h \
    include/min_dont_care_branching_strategy.h \
    include/NodeBoolTree.h \
    include/benchmarks_example.h \
    Allocator-1.0.0/Allocator.h \
    Allocator-1.0.0/DataTypes.h

