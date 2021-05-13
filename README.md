## Пример использования при подключении в виде библиотеки
```c++
#include "icustomplotbuilder.h"
// ...
QLibrary lib(nameLib);
if(!lib.load()) {
    qDebug() << "Loading failed!";
}

typedef CustomPlotBuilderPtr (*GetInstance)();
GetInstance Instance = (GetInstance) lib.resolve("Instance");

if (Instance) {
    CustomPlotBuilderPtr instance = Instance();

if (instance != nullptr) {
    instance->showViewer();

    instance->setTitle(QString("Custom title"));        // Установить заголовок для графиков
    
    std::vector<std::pair<double, double>> data;        // QVector<QPair<double, double>> data
    // ...
    instance->addValues(data);                          // Добавление данных
    instance->draw();                                   // Отрисовка графиков

    instance->addGraph(data, QString("Graph name"));    // Добавить еще график 

    instance->showHideLegend();                         // Отобразить легенду графиков
}
```