#pragma once

#include <qwidget.h>
#include <qvariant.h>

struct ImplPrivate;

class ImplClass : public QWidget {
protected:
    ImplClass(ImplPrivate& pd, QWidget* parent);

protected:
    QScopedPointer<ImplPrivate> d_ptr;
};

struct ImplPrivate : public QObject {
    ImplClass* q_ptr;

    virtual void init(const QVariant& data) {};
    virtual void bindView(QWidget* parent) {};
};

inline ImplClass::ImplClass(ImplPrivate& pd, QWidget* parent)
    : QWidget(parent)
    , d_ptr(&pd) {
    d_ptr->q_ptr = this;
}

template<typename Ui>
struct BaseView : public Ui {
    virtual void initUi(QWidget* parent) = 0;
};

template<typename T>
class ImplClassWrapper : public T, public ImplClass {
protected:
    using ImplClass::ImplClass;
};