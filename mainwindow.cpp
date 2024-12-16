
// mainwindow.cpp
#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set window properties
    setWindowTitle("AETHER Monitoring System");
    resize(1200, 800);

    setupUI();
    setupCharts();

    // Initialize timer for real-time updates
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateCharts);
    updateTimer->start(500); // Update every 1/2 second
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // Create main widget and layout
    QWidget *centralWidget = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Create sidebar
    sourcesList = new QListWidget;
    sourcesList->setMinimumWidth(200);
    sourcesList->setMaximumWidth(200);

    QStringList sources = loadSourcesFromFile(sourcesFile);  // Load Sources from file
    if (!sources.isEmpty()) {
        sourcesList->addItems(sources);  // Add Sources to the list
    } else {
        // Display a message if no Sources are loaded
        QMessageBox::warning(this, "Error", "No sources found in the file!");
    }

    // Connect Source selection signal
    connect(sourcesList, &QListWidget::itemClicked, this, &MainWindow::onSourceSelected);

    QLineEdit *sourceInput = new QLineEdit;
    sourceInput->setPlaceholderText("Enter new source name");
    sourceInput->setMaximumWidth(200);

    QPushButton *addSourceButton = new QPushButton("+ Add Source");
    addSourceButton->setMaximumWidth(200);
    connect(addSourceButton, &QPushButton::clicked, [this, sourceInput]() {
        QString newSource = sourceInput->text().trimmed();
        if (!newSource.isEmpty() && !sourcesList->findItems(newSource, Qt::MatchExactly).isEmpty()) {
            return;  // Don't allow duplicate Sources
        }
        if (!newSource.isEmpty()) {
            sourcesList->addItem(newSource);
            saveSourceToFile(newSource);
            sourceInput->clear();
        }
    });

    // Create layout for sidebar
    QVBoxLayout *sidebarLayout = new QVBoxLayout;
    sidebarLayout->addWidget(sourcesList);
    sidebarLayout->addWidget(sourceInput);
    sidebarLayout->addWidget(addSourceButton);
    mainLayout->addLayout(sidebarLayout);


    // Create charts container
    QWidget *chartsWidget = new QWidget;
    QGridLayout *chartsLayout = new QGridLayout(chartsWidget);

    // Add sidebar and charts to main layout
    //mainLayout->addWidget(sourcesList);
    mainLayout->addWidget(chartsWidget);

    setCentralWidget(centralWidget);

    // Initialize series
    voltageSeries = new QLineSeries();
    currentSeries = new QLineSeries();
    temperatureSeries = new QLineSeries();
    humiditySeries = new QLineSeries();

    voltageChart = createChart("Voltage (V)", 0, 250);
    currentChart = createChart("Current (A)", 0, 100);
    temperatureChart = createChart("Temperature (°C)", 0, 50);
    humidityChart = createChart("Humidity (%)", 0, 100);

    // Create and setup chart views
    voltageChartView = new QChartView(voltageChart);
    currentChartView = new QChartView(currentChart);
    temperatureChartView = new QChartView(temperatureChart);
    humidityChartView = new QChartView(humidityChart);

    // Enable anti-aliasing
    voltageChartView->setRenderHint(QPainter::Antialiasing);
    currentChartView->setRenderHint(QPainter::Antialiasing);
    temperatureChartView->setRenderHint(QPainter::Antialiasing);
    humidityChartView->setRenderHint(QPainter::Antialiasing);

    // Add charts to layout
    chartsLayout->addWidget(voltageChartView, 0, 0);
    chartsLayout->addWidget(currentChartView, 0, 1);
    chartsLayout->addWidget(temperatureChartView, 1, 0);
    chartsLayout->addWidget(humidityChartView, 1, 1);

    // Select first Source by default
    if (!sources.isEmpty()) {
        currentSource = sources.first();
        sourcesList->setCurrentRow(0);
    }
}

void MainWindow::setupCharts()
{
    // Add series to charts
    voltageChart->addSeries(voltageSeries);
    currentChart->addSeries(currentSeries);
    temperatureChart->addSeries(temperatureSeries);
    humidityChart->addSeries(humiditySeries);

    // Attach axes
    voltageSeries->attachAxis(voltageChart->axes(Qt::Horizontal).first());
    voltageSeries->attachAxis(voltageChart->axes(Qt::Vertical).first());

    currentSeries->attachAxis(currentChart->axes(Qt::Horizontal).first());
    currentSeries->attachAxis(currentChart->axes(Qt::Vertical).first());

    temperatureSeries->attachAxis(temperatureChart->axes(Qt::Horizontal).first());
    temperatureSeries->attachAxis(temperatureChart->axes(Qt::Vertical).first());

    humiditySeries->attachAxis(humidityChart->axes(Qt::Horizontal).first());
    humiditySeries->attachAxis(humidityChart->axes(Qt::Vertical).first());
}

QChart* MainWindow::createChart(const QString &title, double yMin, double yMax)
{
    QChart *chart = new QChart();
    chart->setTitle(title);

    // Create axes
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 10);  // Show last 10 seconds of data
    axisX->setTitleText("Time (s)");

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(yMin, yMax);
    axisY->setTitleText(title);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    return chart;
}
void MainWindow::onSourceSelected(QListWidgetItem* item)
{
    currentSource = item->text();

    // Clear existing data
    voltageSeries->clear();
    currentSeries->clear();
    temperatureSeries->clear();
    humiditySeries->clear();

    // Update chart titles
    voltageChart->setTitle(QString("Voltage (V) - %1").arg(currentSource));
    currentChart->setTitle(QString("Current (A) - %1").arg(currentSource));
    temperatureChart->setTitle(QString("Temperature (°C) - %1").arg(currentSource));
    humidityChart->setTitle(QString("Humidity (%) - %1").arg(currentSource));

    // Reset time counter when switching Sources
    timeCounter = 0;
}

void MainWindow::updateCharts()
{
    if (currentSource.isEmpty()) return;

    timeCounter += 1;

    // Generate random values (replace with your actual data)
    // Okay so somehow we have to obtain the current Source name and
    // fetch values from remote by name of the Source.
    double voltage = rand() % 250;
    double current = rand() % 100;
    double temperature = 20 + (rand() % 30);
    double humidity = rand() % 100;

    voltageSeries->append(timeCounter, voltage);
    currentSeries->append(timeCounter, current);
    temperatureSeries->append(timeCounter, temperature);
    humiditySeries->append(timeCounter, humidity);

    if(timeCounter > 10) {
        voltageSeries->remove(0);
        currentSeries->remove(0);
        temperatureSeries->remove(0);
        humiditySeries->remove(0);
    }

    // Update axes ranges
    for(QChart *chart : {voltageChart, currentChart, temperatureChart, humidityChart}) {
        chart->axes(Qt::Horizontal).first()->setRange(qMax(0.0, timeCounter - 10), timeCounter);
    }
}

QStringList MainWindow::loadSourcesFromFile(const QString &fileName)
{
    QStringList Sources;

    QFile file(fileName);
    if (!file.exists()) {
        QMessageBox::warning(this, "File Not Found", "The Sources file does not exist.");
        return Sources;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "File Open Error", "Unable to open Sources file.");
        return Sources;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            Sources.append(line);
        }
    }

    file.close();
    return Sources;
}

void MainWindow::saveSourceToFile(const QString &SourceName)
{
    QFile file(sourcesFile);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        QMessageBox::warning(this, "File Open Error", "Unable to open Sources file for writing.");
        return;
    }

    QTextStream out(&file);
    out << SourceName << "\n";  // Append the new Source name to the file
    file.close();
}
