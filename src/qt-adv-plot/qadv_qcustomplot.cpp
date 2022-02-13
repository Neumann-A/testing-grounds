
#include "qadv_qcustomplot.hpp"

#include <qcustomplot.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qsizepolicy.h>
#include <qvalidator.h>
#include <qwidget.h>

#include <QAction>
#include <QContextMenuEvent>
#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QWidget>
#include <QWidgetAction>
#include <QPalette>
#include <cmath>
#include <limits>

namespace
{
    void setupDefaultAxisStyle(QCPAxis &axis)
    {
        axis.setVisible(true);
        auto pen = axis.basePen();
        pen.setWidth(2);
        axis.setBasePen(pen);
        axis.setTickPen(pen);
        axis.setSubTickPen(pen);
        axis.setTickLengthIn(static_cast<int>(std::floor(axis.tickLengthIn() * 1.5)));
        axis.setSubTickLengthIn(axis.subTickLengthIn() * 2);
        auto font = axis.tickLabelFont();
        font.setBold(true);
        axis.setTickLabelFont(font);
    }

    auto generateRandomData()
    {
        int n          = 100; // number of points in graph
        double xScale  = (rand() / (double) RAND_MAX + 0.5) * 2;
        double yScale  = (rand() / (double) RAND_MAX + 0.5) * 2;
        double xOffset = (rand() / (double) RAND_MAX - 0.5) * 4;
        double yOffset = (rand() / (double) RAND_MAX - 0.5) * 10;
        double r1      = (rand() / (double) RAND_MAX - 0.5) * 2;
        double r2      = (rand() / (double) RAND_MAX - 0.5) * 2;
        double r3      = (rand() / (double) RAND_MAX - 0.5) * 2;
        double r4      = (rand() / (double) RAND_MAX - 0.5) * 2;
        QVector<double> x(n), y(n);
        for (int i = 0; i < n; i++) {
            x[i] = (i / (double) n - 0.5) * 10.0 * xScale + xOffset;
            y[i] = (qSin(x[i] * r1 * 5) * qSin(qCos(x[i] * r2) * r4 * 3) + r3 * qCos(qSin(x[i]) * r4 * 2)) * yScale +
                yOffset;
        }

        return std::tuple{x, y};
    }

} // namespace
QAdvCustomPlot::QAdvCustomPlot(QWidget *parent)
    : QCustomPlot(parent)
{
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);

    axisRect()->setupFullAxesBox();

    xAxis2->setTickLabels(false);
    yAxis2->setTickLabels(false);
    xAxis->setLabel("x Axis Label");
    yAxis->setLabel("y Axis Label");
    setupDefaultAxisStyle(*xAxis);
    setupDefaultAxisStyle(*xAxis2);
    setupDefaultAxisStyle(*yAxis);
    setupDefaultAxisStyle(*yAxis2);

    legend->setVisible(true);

    QFont legendFont = font();
    legendFont.setPointSize(10);
    legend->setFont(legendFont);
    legend->setSelectedFont(legendFont);
    legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

    setContextMenuPolicy(Qt::ContextMenuPolicy::DefaultContextMenu);
};
QAdvCustomPlot::~QAdvCustomPlot(){

};

bool QAdvCustomPlot::event(QEvent *event) {
    //Q_EMIT propertyChanged();
    return QWidget::event(event);
};

void QAdvCustomPlot::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    auto pos = event->pos();

    if (legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {
        //QActionGroup
        auto move_menu = menu->addMenu("Move legend to");
        move_menu->addAction("top left", this, SLOT(moveLegend()))->setData((int) (Qt::AlignTop | Qt::AlignLeft));
        move_menu->addAction("top center", this, SLOT(moveLegend()))->setData((int) (Qt::AlignTop | Qt::AlignHCenter));
        move_menu->addAction("top right", this, SLOT(moveLegend()))->setData((int) (Qt::AlignTop | Qt::AlignRight));
        move_menu->addAction("bottom right", this, SLOT(moveLegend()))
            ->setData((int) (Qt::AlignBottom | Qt::AlignRight));
        move_menu->addAction("bottom left", this, SLOT(moveLegend()))->setData((int) (Qt::AlignBottom | Qt::AlignLeft));
    }
    else if (xAxis->selectTest(pos, false) >= 0) { // x-axis
        auto xaxis_menu = menu->addMenu("Ranges");


        auto range_label = new QLabel("Ranges",xaxis_menu);
        range_label->setFrameStyle(QFrame::StyledPanel);
        range_label->setMaximumWidth(xaxis_menu->maximumWidth()-4);
        range_label->setAlignment( Qt::AlignVCenter | Qt::AlignLeft);
        range_label->setStyleSheet("color: white; font: bold; font-size: 9pt; background-color: gray;");

        auto separator_action = new QWidgetAction(xaxis_menu);
        separator_action->setDefaultWidget(range_label);

        separator_action->setDisabled(true);
        xaxis_menu->addAction(separator_action);

        auto lineeditaction = new QWidgetAction(xaxis_menu);

        auto rangewidget = new QWidget(xaxis_menu);


        auto lineedit_lower   = new QLineEdit(xaxis_menu);
        lineedit_lower->setMinimumWidth(30);
        lineedit_lower->setFixedWidth(80);
        lineedit_lower->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred));
        lineedit_lower->setValidator(new QDoubleValidator(std::numeric_limits<double>::lowest(),
                                                    std::numeric_limits<double>::max(),
                                                    std::numeric_limits<double>::digits, xaxis_menu));
        lineedit_lower->setText(QString::number(xAxis->range().lower));
        auto size_policy_lower = lineedit_lower->sizePolicy();
        size_policy_lower.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
        lineedit_lower->setSizePolicy(size_policy_lower);
        lineedit_lower->setFrame(false);
        QObject::connect(lineedit_lower, &QLineEdit::textChanged,
                         [&](const QString &text) { this->xAxis->setRangeLower(text.toDouble()); });
        QObject::connect(lineedit_lower, &QLineEdit::editingFinished, this, [=, this]() { this->replot(); });

        auto lineedit_upper   = new QLineEdit(xaxis_menu);
        lineedit_upper->setMinimumWidth(30);
        lineedit_upper->setFixedWidth(80);
        lineedit_upper->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred));
        lineedit_upper->setValidator(new QDoubleValidator(std::numeric_limits<double>::lowest(),
                                                    std::numeric_limits<double>::max(),
                                                    std::numeric_limits<double>::digits, xaxis_menu));
        lineedit_upper->setText(QString::number(xAxis->range().upper));
        auto size_policy_upper = lineedit_upper->sizePolicy();
        size_policy_upper.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
        lineedit_upper->setSizePolicy(size_policy_upper);

        lineedit_upper->setFrame(false);
        QObject::connect(lineedit_upper, &QLineEdit::textChanged,
                         [&](const QString &text) { 
                                this->xAxis->setRangeUpper(text.toDouble()); 
                             });
        QObject::connect(lineedit_upper, &QLineEdit::editingFinished, this, [=,this]() { this->replot(); });
        
        QObject::connect(xAxis, qOverload<const QCPRange &>(&QCPAxis::rangeChanged), [&](const QCPRange &newRange) {qDebug() << "range changed";});

        rangewidget->setLayout(new QHBoxLayout());
        auto hboxlayout = static_cast<QHBoxLayout *>(rangewidget->layout());
        hboxlayout->setContentsMargins(1,1,1,1);
        hboxlayout->setSpacing(1);

        auto xlabel = new QLabel("x", xaxis_menu);
        xlabel->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter );
        //xlabel->setFrameStyle(QFrame::StyledPanel);
        //xlabel->setContentsMargins(3,1,1,1);
        //xlabel->setMargin(3);
        hboxlayout->addWidget(xlabel, 0, Qt::AlignmentFlag::AlignJustify | Qt::AlignmentFlag::AlignCenter );
        hboxlayout->addWidget(lineedit_lower, 0, Qt::AlignmentFlag::AlignJustify | Qt::AlignmentFlag::AlignCenter );
        hboxlayout->addWidget(lineedit_upper, 0, Qt::AlignmentFlag::AlignJustify | Qt::AlignmentFlag::AlignCenter );
        lineeditaction->setDefaultWidget(rangewidget);
        xaxis_menu->addAction(lineeditaction);
    }
    else { // general context menu on graphs requested
        
        //menu->addAction("Add random graph", this, SLOT(addRandomGraph()));

        //if (selectedGraphs().size() > 0)
        //  menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
        //if (graphCount() > 0)
        //menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
    }

    menu->popup(mapToGlobal(pos));
}

#include <moc_qadv_qcustomplot.cpp>
