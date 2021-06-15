#pragma once

#include <qwidget.h>
#include <qvariant.h>

class AbstractRouterWidget : public QWidget {
public:
    explicit AbstractRouterWidget(const QVariant& data, QWidget* parent);

    virtual void onNavigateResult(const QVariant& data);

    virtual bool attempClose();

private:
    QVariant navigateData;
    bool firstShow;

protected:
    QVariant getNavigateData(bool clear = true);

    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void hideEvent(QHideEvent* event) override;

    virtual void onViewLoad();
    virtual void onReshow();
    virtual void onHidden();
    virtual void onResizeWindow();
};