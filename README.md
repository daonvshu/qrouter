# qrouter
这是一个使用QStackedWidget进行子页面切换的路由框架，基本原理基于qt的反射实现解耦合，使用QList保存页面栈，页面之间通信通过QRouter转发

## 如何使用
### 1. 编写一个page类继承于AbstractRouterWidget，并使这个类可反射
```cpp
//YourPage.h
class YourPage : public AbstractRouterWidget {
    Q_OBJECT

public:
    Q_INVOKABLE explicit YourPage(const QVariant& data, QWidget* parent = nullptr);

    //optional, navigate page closed with data
    void onNavigateResult(const QVariant& data) override;

    //optional, be called when attemp close this page
    bool attempClose() override;

protected:
    //optional, receive event from other page
    void runRouterEvent(const QString& event, const QVariant& data) override;

    //optional, receive event from other page with return data
    QVariant executeRouterEvent(const QString& event, const QVariant& data) override;

    //optional, showEvent first be called
    void onViewLoad();

    //optional, showEvent called at other time
    void onReshow();

    //optional, same as hideEvent
    void onHidden();

    //optional, same as resizeEvent
    void onResizeWindow();

private:
    Ui::YourPageUi ui;

    //reflect type id
    static int typeId;
}
//register to meta-object system
Q_DECLARE_METATYPE(YourPage*);


//YourPage.cpp

//register meta type
int YourPage::typeId = qRegisterMetaType<YourPage*>();

YourPage::YourPage(const QVariant& data, QWidget* parent)
    : YourPage(data, parent)
{
    ui.setupUi(this);
}

```
可选重载函数说明  
- `virtual void onNavigateResult(const QVariant& data)`  
当跳到的目标界面关闭时并携带数据，这个函数将被调用用于接收返回数据`data`，`data`可以传递任何类型数据

- `virtual bool attempClose()`  
qrouter尝试关闭当前页面时，可以重载此函数阻止页面切换，返回false阻止关闭

- `virtual void runRouterEvent(const QString& event, const QVariant& data)`  
接收其他页面发送的消息，`event`为事件名称，`data`为传递的数据

- `QVariant executeRouterEvent(const QString& event, const QVariant& data)`  
等同于`runRouterEvent`，可额外返回数据到发送者

- `virtual void onViewLoad()`  
当页面第一次界面加载完时调用，等同于`showEvent`第一次被调用

- `virtual void onReshow()`  
除了第一次调用，其他时候`showEvent`被调用

- `void onHidden()`  
等同于`hideEvent`

- `void onResizeWindow()`  
等同于`resizeEvent`

### 2. 注册页面容器
```cpp
QRouter::install(ui.stackedWidget, 0);
```
- `static QRouter& install(QStackedWidget* stackContainer, int contextId = 0)`  
`stackContainer`为用于切换的页面容器，`contextId`为当前页面栈id

### 3. 如何使用qrouter切换页面
```cpp
CustomData data;
data.message = "send data from page1 to page2";

//初始化并创建页面栈
QRouter::of().initStack({"Page1", "Page2"});

//从页面1跳转到page2，"Page2"为页面2的类名，of函数传递页面栈id，默认id为0的页面栈
QRouter::of().push("Page2", QVariant::fromValue(data));

//从页面栈移除当前页面1并跳转到页面2
QRouter::of().pushReplace("Page2", QVariant::fromValue(data));

//创建新页面page2并入栈，如果page2已经存在，则移动到栈顶
QRouter::of().pushOrMove2Top("Page2", QVariant::fromValue(data));

//尝试从页面栈移除当前页面1（如果可以关闭页面1）并跳转到页面2
QRouter::of().popAndPush("Page2", QVariant::fromValue(data));

//关闭指定页面2，页面2不一定在栈顶
QRouter::of().close("Page2");

//清除当前页面栈并跳转到页面2
QRouter::of().pushAndClear("Page2", QVariant::fromValue(data));

//将页面2移动到栈顶
QRouter::of().move2Top("Page2");

//关闭当前页面，并向上个页面传递数据
QRouter::of().pop(QVariant::fromValue(data));

//关闭页面栈Page2上面的所有页面
QRouter::of().popUntil("Page2", QVariant::fromValue(data));

//页面栈只保留4个页面，并关闭栈之上的所有页面
QRouter::of().popUntil(4);

//其他功能函数

//读取当前页面栈所有页面名称
QRouter::of().readStack();

//获取当前栈顶页面名
QRouter::of().currentName();

//获取当前栈栈顶实例
QRouter::of().current();
QRouter::currentInstance<MyPage>();

//从当前页面栈中获取指定页面实例
QRouter::of().getInstanceFromStack("MyPage");
QRouter::getInstance<MyPage>("MyPage");

//获取页面栈容器的id
QRouter::getIdByContainer(ui.page_container);
```

### 4. 自定义切换动画

默认动画是页面透明度与垂直位移组合：新页面从下方小距离滑入并淡入，旧页面截图向下滑出并淡出。可以通过实现 `RouterAnimation` 替换进入和退出动画：

```cpp
class MyRouterAnimation : public RouterAnimation {
public:
    QAbstractAnimation* animateEnter(QWidget* widget) override {
        // create and return your enter animation...
        return nullptr;
    }

    QAbstractAnimation* animateExit(QWidget* widget) override {
        // create and return your exit animation on the prepared widget...
        return nullptr;
    }
};

QRouter::of().setAnimation(QSharedPointer<MyRouterAnimation>::create());
```

`setAnimation(nullptr)` 或 `resetAnimation()` 会恢复默认的透明加位移动画。默认实现使用 10px 位移、300ms 时长和 `OutCubic` 曲线，并由默认动画自行创建透明度 effect；自定义实现自行决定是否创建 effect、位移、时长、曲线及其他动画参数。退出动画传入的 `widget` 已由容器准备好并显示截图，自定义实现只需要在它上面创建并返回未启动的动画对象，动画的启动、完成处理和资源释放由框架统一负责。

### 5. 发送事件
```cpp
//同步发送事件到当前页面（页面栈顶）
QRouter::of().sendEventCur("eventName");

//同步发送事件到Page1
QRouter::of().sendEventTo("Page1", "eventName");

//同步发送事件到当前页面栈所有页面
QRouter::of().sendEventAll("eventName");

//发送事件到当前页面，使用事件队列
QRouter::of().postEventCur("eventName");

//发送事件到Page1，使用事件队列
QRouter::of().postEventTo("Page1", "eventName");

//发送事件到当前页面栈所有页面，使用事件队列
QRouter::of().postEventAll("eventName");

//发送事件到容器父页面
QRouter::of().postEventToRoot("eventName");

//容器所在父页面接收子页面发送的事件
bool event(QEvent* event) override {
    if (event->type() == QRouterPageEvent::type) {
        //do something...
        return true;
    }
    return QWidget::event(event);
}

//父页面必须是stackWidget的parent，否则使用eventFilter接收事件
ui.page_container->parent()->installEventFilter(this);
bool eventFilter(QObject *watched, QEvent *event) override {
    if (event->type() == QRouterPageEvent::type) {
        if (watched == ui.page_container->parent()) {
            //do something...
        }
        return true;
    }
    return QWidget::eventFilter(watched, event);
}
```
