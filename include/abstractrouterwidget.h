#pragma once

#include <qwidget.h>
#include <qvariant.h>

class AbstractRouterWidget : public QWidget {
public:
    explicit AbstractRouterWidget(const QVariant& data, QWidget* parent);

    virtual void onNavigateResult(const QVariant& data);

    virtual bool attemptClose();

    QVariant onRouterEvent(const QString& event, const QVariant& data);

private:
    QVariant navigateData;
    bool firstShow;

protected:
    QVariant getNavigateData(bool clear = true);

    virtual void runRouterEvent(const QString& event, const QVariant& data);

    virtual QVariant executeRouterEvent(const QString& event, const QVariant& data);

    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void hideEvent(QHideEvent* event) override;

    bool event(QEvent* event) override;

    virtual void onViewLoad();
    virtual void onReshow();
    virtual void onHidden();
    virtual void onResizeWindow();
};