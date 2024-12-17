// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <QTimer>
#include <QListWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct SourceData {
    QLineSeries *voltageSeries;
    QLineSeries *currentSeries;
    QLineSeries *temperatureSeries;
    QLineSeries *humiditySeries;
    double lastVoltage;
    double lastCurrent;
    double lastTemperature;
    double lastHumidity;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    const double VOLTAGE_MAX = 220.0;
    const double CURRENT_MAX = 80.0;
    const double TEMP_MAX = 40.0;
    const double HUMIDITY_MAX = 90.0;

private slots:
    void updateCharts();
    void onSourceSelected(QListWidgetItem* item);

private:
    const QString sourcesFile = "/home/me/code/AetherView/sources.ae";
    Ui::MainWindow *ui;
    double timeCounter = 0;

    // Sidebar
    QListWidget *sourcesList;
    QString currentSource;

    // Charts
    QChart *voltageChart;
    QChart *currentChart;
    QChart *temperatureChart;
    QChart *humidityChart;

    // Chart Views
    QChartView *voltageChartView;
    QChartView *currentChartView;
    QChartView *temperatureChartView;
    QChartView *humidityChartView;

    // Series (one per measurement type)
    QLineSeries *voltageSeries;
    QLineSeries *currentSeries;
    QLineSeries *temperatureSeries;
    QLineSeries *humiditySeries;

    QMap<QString, SourceData*> sourcesData;

    QTimer *updateTimer;

    void setupUI();
    void setupCharts();
    QChart* createChart(const QString &title, double yMin, double yMax);
    void checkAlarms(const QString &source, SourceData *data);

    QStringList loadSourcesFromFile(const QString &fileName);
    void saveSourceToFile(const QString &SourceName);
};
#endif // MAINWINDOW_H
