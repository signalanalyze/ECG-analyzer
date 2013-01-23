#include "ecg2ch.h"
#include "math.h"

void gsl_vector_minmax_to_index(const gsl_vector *v, double* min_val, double* max_val, int index) {
    *min_val = gsl_vector_max(v);
    *max_val = gsl_vector_min(v);

    int size = min(int(v->size), index);

    for (int i = 0; i < size; i++)
    {
        *min_val = min(*min_val, v->data[i*v->stride]);
        *max_val = max(*max_val, v->data[i*v->stride]);
    }
}

Ecg2Ch::Ecg2Ch(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QToolBar *toolBar = new QToolBar(this);
    syncButton = new QToolButton(toolBar);
    syncButton->setIcon(QIcon(":/ECGanalyzer/icons/locked.png"));
    syncButton->setCheckable(true);
    syncButton->setChecked(true);
    toolBar->addWidget(syncButton);
    zoomInFirst = new QToolButton(toolBar);
    zoomInFirst->setIcon(QIcon(":/ECGanalyzer/icons/magnifier_plus"));
    toolBar->addWidget(zoomInFirst);
    zoomOutFirst = new QToolButton(toolBar);
    zoomOutFirst->setIcon(QIcon(":/ECGanalyzer/icons/magnifier_minus"));
    toolBar->addWidget(zoomOutFirst);

    mainLayout->addWidget(toolBar);

    QWidget *plotsWidget = new QWidget(this);
    QVBoxLayout *plotsLayout = new QVBoxLayout(plotsWidget);
    EcgCh *plot1 = new EcgCh(plotsWidget);
    EcgCh *plot2 = new EcgCh(plotsWidget);
    plotsLayout->addWidget(plot1);
    plotsLayout->addWidget(plot2);
    plotsWidget->setLayout(plotsLayout);
    mainLayout->addWidget(plotsWidget);

    setLayout(mainLayout);

    ch1 = plot1;
    ch2 = plot2;
    control = new PlotControl(plot1, plot2);
    control->enableSync(true);
    connect(syncButton, SIGNAL(toggled(bool)), this, SLOT(syncToggled(bool)));
    connect(zoomInFirst, SIGNAL(clicked()), control, SLOT(zoomInFirst()));
    connect(zoomOutFirst, SIGNAL(clicked()), control, SLOT(zoomOutFirst()));
}

Ecg2Ch::~Ecg2Ch()
{
    delete ch1;
    delete ch2;
}

void Ecg2Ch::setSignal(ECGSignal *signal, ECGInfo *info)
{
    double max_value_one, min_value_one, max_value_two, min_value_two;
    gsl_vector_minmax_to_index(signal->channel_one->signal, &min_value_one, &max_value_one, 1000);
    gsl_vector_minmax_to_index(signal->channel_two->signal, &min_value_two, &max_value_two, 1000);
    float max_value = max(max_value_one, max_value_two);
    float min_value = min(min_value_one, min_value_two);

    ch1->setSignal(signal->channel_one, info->channel_one);
    ch2->setSignal(signal->channel_two, info->channel_two);
    QRectF rect(QPointF(0, max_value), QPointF(1000, min_value));
    control->setZoomBase(rect);
}

void Ecg2Ch::setSignal(ECGSignal *signal, ECGInfo *info, ECGRs *peaks)
{
    double max_value_one, min_value_one, max_value_two, min_value_two;
    gsl_vector_minmax_to_index(signal->channel_one->signal, &min_value_one, &max_value_one, 1000);
    gsl_vector_minmax_to_index(signal->channel_two->signal, &min_value_two, &max_value_two, 1000);
    float max_value = max(max_value_one, max_value_two);
    float min_value = min(min_value_one, min_value_two);

    ch1->setSignal(signal->channel_one, info->channel_one, peaks->GetRs());
    ch2->setSignal(signal->channel_two, info->channel_two, peaks->GetRs());
    QRectF rect(QPointF(0, max_value), QPointF(1000, min_value));
    control->setZoomBase(rect);
}

void Ecg2Ch::syncToggled(bool toggle)
{
    if (toggle)
    {
        syncButton->setIcon(QIcon(":/ECGanalyzer/icons/locked.png"));
    }
    else
    {
        syncButton->setIcon(QIcon(":/ECGanalyzer/icons/unlocked.png"));
    }
    control->enableSync(toggle);
}
