/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2017 Randy Baumgarte

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
***********************************************************************************/


#include "tablepropertiesdialog.h"
#include "global.h"

#include <QLabel>
#include <QScrollArea>


extern Global global;



// Default constructor
TablePropertiesDialog::TablePropertiesDialog(QString tableCss, QString cellCss, QWidget *parent) : QDialog(parent)
{
    tableLeftMargin = 0;
    tableRightMargin = 0;
    tableBottomMargin = 0;
    tableTopMargin = 0;
    tableTopMarginUnit = "px";
    tableBottomMarginUnit = "px";
    tableLeftMarginUnit = "px";
    tableRightMarginUnit = "px";
    cellMargin = 10;
    cellWidth = 33.33;
    tableBorderCollapse = "collapse";
    tableExtras="-evernote-table:true;width:100%;table-layout:fixed;";
    cellBorderStyleBottom = "solid";
    cellBorderStyleTop = "solid";
    cellBorderStyleLeft = "solid";
    cellBorderStyleRight = "solid";
    cellExtras = "margin:0px;width:33.33%;";
    cellBorderWidth=1;
    cellBorderColorB = 211;
    cellBorderColorG = 211;
    cellBorderColorR = 211;
    cellBackgroundColorR = -1;
    cellBackgroundColorG = -1;
    cellBackgroundColorB = -1;
    cellFontColorR = -1;
    cellFontColorG = -1;
    cellFontColorB = -1;
    tableWidth = 80;
    tableWidthUnit = "%";
    cellPaddingSize = 10;
    horizontalAlignment = "";
    verticalAlignment="";
    evernoteTable = false;
    tableWidth=80;
    tableLayout = "fixed";

    okButtonPressed = false;
    setWindowTitle(tr("Table Properties"));
    mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    tabs = new QTabWidget(this);
    scrollArea->setWidget(tabs);
    mainLayout->addWidget(scrollArea);
    this->setFont(global.getGuiFont(font()));

    scrollArea->setMinimumHeight(200);
    scrollArea->setMinimumHeight(this->height()/2);

    cancelButton = new QPushButton(tr("Cancel"), this);
    okButton = new QPushButton(tr("OK"), this);

    connect(okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));

    QSpacerItem *spacer1 = new QSpacerItem(0,0,QSizePolicy::MinimumExpanding);
    QSpacerItem *spacer2 = new QSpacerItem(0,0,QSizePolicy::MinimumExpanding);

    buttonLayout = new QHBoxLayout();
    buttonLayout->addSpacerItem(spacer1);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addSpacerItem(spacer2);
    buttonLayout->setStretch(0,100);
    buttonLayout->setStretch(3,100);

    cellPanel = new QWidget();
    tablePanel = new QWidget();
    tabs->addTab(tablePanel, tr("Table Properties"));
    tabs->addTab(cellPanel,tr("Cell Properties"));

    preview = new QWebView();
    mainLayout->addWidget(preview);

    mainLayout->addLayout(buttonLayout);
    this->resize(this->size().width()*1.2, this->size().height()*1.2);

    this->setupTablePanel();
    this->setupCellPanel();

    setCellStyle(cellCss);
    setTableStyle(tableCss);
    loadValues();

    this->generatePreview();
}



// Load the initial values passed into the constructor into the widgets
void TablePropertiesDialog::loadValues() {
    tableWidthSpinner->setValue(this->tableWidth);
    if (tableWidthUnit.toLower() == "px")
        tableWidthUnitCombo->setCurrentIndex(1);
    else
        tableWidthUnitCombo->setCurrentIndex(0);


    tableTopMarginSpinner->setValue(tableTopMargin);
    tableBottomMarginSpinner->setValue(tableBottomMargin);
    tableLeftMarginSpinner->setValue(tableLeftMargin);
    tableRightMarginSpinner->setValue(tableRightMargin);

    if (tableTopMarginUnit == "%")
        tableTopMarginUnitCombo->setCurrentIndex(0);
    else
        tableTopMarginUnitCombo->setCurrentIndex(1);
    if (tableBottomMarginUnit == "%")
        tableBottomMarginUnitCombo->setCurrentIndex(0);
    else
        tableBottomMarginUnitCombo->setCurrentIndex(1);
    if (tableLeftMarginUnit == "%")
        tableLeftMarginUnitCombo->setCurrentIndex(0);
    else
        tableLeftMarginUnitCombo->setCurrentIndex(1);
    if (tableRightMarginUnit == "%")
        tableRightMarginUnitCombo->setCurrentIndex(0);
    else
        tableRightMarginUnitCombo->setCurrentIndex(1);

    borderSizeSpinner->setValue(this->cellBorderWidth);
    paddingSizeSpinner->setValue(cellPaddingSize);

    setInitialBorderSelection(borderComboTop, cellBorderStyleTop);
    setInitialBorderSelection(borderComboBottom, cellBorderStyleBottom);
    setInitialBorderSelection(borderComboLeft, cellBorderStyleLeft);
    setInitialBorderSelection(borderComboRight, cellBorderStyleRight);

    horizontalAlignmentCombo->setCurrentIndex(0);
    if (horizontalAlignment.toLower() == "left")
        horizontalAlignmentCombo->setCurrentIndex(1);
    if (horizontalAlignment.toLower() == "right")
        horizontalAlignmentCombo->setCurrentIndex(2);
    if (horizontalAlignment.toLower() == "center")
        horizontalAlignmentCombo->setCurrentIndex(3);
}


// Set initial values for some of the border combo boxes
void TablePropertiesDialog::setInitialBorderSelection(QComboBox *box, QString value) {
    box->setCurrentIndex(0);

    box->blockSignals(true);
    if (value.toLower() == "none") box->setCurrentIndex(1);
    if (value.toLower() == "dotted") box->setCurrentIndex(2);
    if (value.toLower() == "dashed") box->setCurrentIndex(3);
    if (value.toLower() == "double") box->setCurrentIndex(4);
    if (value.toLower() == "right") box->setCurrentIndex(5);
    if (value.toLower() == "inset") box->setCurrentIndex(6);
    if (value.toLower() == "outset") box->setCurrentIndex(7);
    box->blockSignals(false);
}



// User clicked the cancel button
void TablePropertiesDialog::cancelButtonClicked() {
    this->close();
}



// User clicked the "OK" button
void TablePropertiesDialog::okButtonClicked() {
    okButtonPressed = true;
    this->close();
}



// Generate a preview of the table from user selected values
void TablePropertiesDialog::generatePreview() {
    QString cellCss = "style='"+getCellCss()+ "'";
    QString html = "<html><body><table style='"+
                     getTableCss()+"'"+
                     "><tr><td "+
                     cellCss+
                     ">"+tr("Row 1 Cell 1")+"</td><td "+
                     cellCss+
                     ">"+tr("Row 1 Cell 2")+"</td><td "+
                     cellCss+
                     ">"+tr("Row 1 Cell 3")+"</td></tr><tr><td "+
                     cellCss+
                     ">"+tr("Row 2 Cell 1")+"</td><td "+
                     cellCss+
                     ">"+tr("Row 2 Cell 2")+"</td><td "+
                     cellCss+
                     ">"+tr("Row 2 Cell 3")+"</td></tr><tr><td "+
                     cellCss+
                     ">"+tr("Row 3 Cell 1")+"</td><td "+
                     cellCss+
                     ">"+tr("Row 3 Cell 2")+"</td><td "+
                     cellCss+
                     ">"+tr("Row 3 Cell 3")+"</td></tr>"+
                     "</table></body></html>";
    preview->setHtml(html);
}




// Parse out the table CSS values & save them for later use
void TablePropertiesDialog::setTableStyle(QString value) {
    tableExtras = "";
    QStringList attributes = value.split(";");
    for (int i=0; i<attributes.size(); i++) {
        QString attr = attributes[i];
        if (attr.startsWith("border-collapse:")) {
            attr = attr.remove(0,16);
            tableBorderCollapse = attr;
        } else if (attr.startsWith("margin-left:")) {
            attr = attr.mid(12);
            if (attr.endsWith("px")) {
                attr.chop(2);
                tableLeftMarginUnit = "px";
            } else {
                tableLeftMarginUnit = "%";
                attr.chop(1);
            }
            this->tableLeftMargin = attr.toInt();
        } else if (attr.startsWith("margin-right:")) {
            attr = attr.mid(13);
            if (attr.endsWith("px")) {
                attr.chop(2);
                tableRightMarginUnit = "px";
            } else {
                attr.chop(1);
                tableRightMarginUnit = "%";
            }
            this->tableRightMargin = attr.toInt();
        } else if (attr.startsWith("margin-top:")) {
            attr = attr.mid(11);
            if (attr.endsWith("px")) {
                attr.chop(2);
                tableTopMarginUnit = "px";
            } else {
                attr.chop(1);
                tableTopMarginUnit = "%";
            }
            this->tableTopMargin = attr.toInt();
        } else if (attr.startsWith("margin-bottom:")) {
            attr = attr.mid(15);
            if (attr.endsWith("px")) {
                attr.chop(2);
                tableBottomMarginUnit = "px";
            } else {
                attr.chop(1);
                tableBottomMarginUnit = "%";
            }
            this->tableBottomMargin = attr.toInt();
        } else if (attr.startsWith("table-layout:")) {
            attr = attr.remove(0,13);
            tableLayout = attr;
        } else if (attr.startsWith("width:")) {
            attr = attr.remove(0,6);
            if (attr.endsWith("%")) {
                attr.chop(1);
                tableWidthUnit = "%";
            } else {
                attr.chop(2);
                tableWidthUnit="px";
            }
            tableWidth = attr.toInt();
        }  else if (attr.startsWith("-evernote-table:true")) {
            evernoteTable = true;
        } else if (attr.startsWith("text-align:")) {
            attr = attr.remove(0,11);
            this->horizontalAlignment = attr;
        } else if (attr.startsWith("vertical-align:")) {
            attr = attr.remove(0,15);
            this->verticalAlignment = attr;
        } else if (attr.trimmed() != "") {
            this->tableExtras = tableExtras+";"+attr;
        }
    }
}



// Return a string of the table CSS from user selected values
QString TablePropertiesDialog::getTableCss() {
    QString retVal = tableExtras+";";
    if (evernoteTable) {
        retVal = retVal + "-evernote-table:true;";
    }
    retVal = retVal + "border-collapse:"+tableBorderCollapse+";";
    retVal = retVal + "width:"+QString::number(tableWidth)+tableWidthUnit+";";
    retVal = retVal + "table-layout:" + tableLayout + ";";
    retVal = retVal + "margin-left:"+QString::number(tableLeftMargin)+tableLeftMarginUnit+";";
    retVal = retVal + "margin-right:"+QString::number(tableRightMargin)+tableRightMarginUnit+";";
    retVal = retVal + "margin-top:"+QString::number(tableTopMargin)+tableTopMarginUnit+";";
    retVal = retVal + "margin-bottom:"+QString::number(tableBottomMargin)+tableBottomMarginUnit+";";
//    if (verticalAlignment != "")
//       retVal =retVal + "vertical-align:"+verticalAlignment+";";
    if (horizontalAlignment != "")
        retVal =retVal + "text-align:"+horizontalAlignment+";";
    retVal = retVal.replace(";;", ";");  // Just n case we have a double ;
    if (retVal.startsWith(";"))
        retVal.remove(0,1);
    return retVal;
}




// Parse out the cell CSS and save values for later use
void TablePropertiesDialog::setCellStyle(QString value) {
    cellExtras.clear();
    horizontalAlignment = "";
    verticalAlignment = "";
    QStringList attributes = value.split(";");
    for (int i=0; i<attributes.size(); i++) {
        QString attr = attributes[i];
        if (attr.startsWith("border-style:")) {
            attr = attr.mid(13);
            this->cellBorderStyleTop = attr;
            this->cellBorderStyleLeft = attr;
            this->cellBorderStyleRight = attr;
            this->cellBorderStyleBottom = attr;
        } else if (attr.startsWith("border-top-style:")) {
            attr = attr.remove(0,17);
            this->cellBorderStyleTop = attr;
        } else if (attr.startsWith("border-bottom-style:")) {
            attr = attr.remove(0,20);
            this->cellBorderStyleBottom = attr;
        } else if (attr.startsWith("border-left-style:")) {
            attr = attr.remove(0,18);
            this->cellBorderStyleLeft = attr;
        } else if (attr.startsWith("border-right-style:")) {
            attr = attr.remove(0,19);
            this->cellBorderStyleRight = attr;
        } else if (attr.startsWith("text-align:")) {
            attr = attr.remove(0,11);
            this->horizontalAlignment = attr;
        } else if (attr.startsWith("vertical-align:")) {
            attr = attr.remove(0,15);
            this->verticalAlignment = attr;
        } else if (attr.startsWith("border-width:")) {
            attr = attr.mid(13);
            if (attr.endsWith("px"))
               attr.chop(2);
            if (attr.endsWith("%"))
                attr.chop(1);
            this->cellBorderWidth = attr.toInt();
        } else if (attr.startsWith("border-color:rgb(")) {
            attr = attr.remove(0,17);
            attr.chop(1);
            QStringList c = attr.split(",");
            cellBorderColorR = c.at(0).toInt();
            cellBorderColorG = c.at(1).toInt();
            cellBorderColorB = c.at(2).toInt();
        } else if (attr.startsWith("padding:")) {
            attr = attr.remove(0,8);
            if (attr.endsWith("px"))
                attr.chop(2);
            cellPaddingSize = attr.toInt();
        } else if (attr.startsWith("margin:")) {
            attr = attr.remove(0,7);
            if (attr.endsWith("px"))
                attr.chop(2);
            cellMargin = attr.toInt();
        } else if (attr.startsWith("width:")) {
            attr = attr.remove(0,6);
            if (attr.endsWith("%"))
                attr.chop(1);
            cellWidth = attr.toDouble();
        }  else if (attr.trimmed() != "") {
            cellExtras = cellExtras +";" + ";" +attr;
        }
    }
}



// Return the current user CSS values for the cells.
QString TablePropertiesDialog::getCellCss() {
    QString retVal;
    retVal = cellExtras+";";
    retVal = retVal + "border-top-style:" + cellBorderStyleTop + ";";
    retVal = retVal + "border-bottom-style:" + cellBorderStyleBottom + ";";
    retVal = retVal + "border-left-style:" + cellBorderStyleLeft + ";";
    retVal = retVal + "border-right-style:" + cellBorderStyleRight + ";";
    retVal = retVal + "border-width:"+QString::number(cellBorderWidth)+"px;border-color:rgb("+
            QString::number(cellBorderColorR)+","+
            QString::number(cellBorderColorG)+","+
            QString::number(cellBorderColorB)+");";

    if (cellBackgroundColorB != -1 && cellBackgroundColorG != -1 && cellBackgroundColorR != -1) {
        retVal = retVal + "background-color:rgb(" + QString::number(cellBackgroundColorR) + "," +
                QString::number(cellBackgroundColorG) + "," +
                QString::number(cellBackgroundColorB) + ");";
    }
    if (cellFontColorB != -1 && cellFontColorG != -1 && cellFontColorR != -1) {
        retVal = retVal + "color:rgb(" + QString::number(cellFontColorR) + "," +
                QString::number(cellFontColorG) + "," +
                QString::number(cellFontColorB) + ");";
    }
    retVal = retVal + "padding:"+QString::number(cellPaddingSize)+"px;";

    if (retVal.startsWith(";;"))
        retVal.remove(0,1);
    retVal = retVal.replace(";;",";");
    return retVal;
}





// Build the table tab widget
void TablePropertiesDialog::setupTablePanel() {
    int row=0;
    int col=0;

    tablePanelLayout = new QGridLayout();
    tablePanelLayout->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    tablePanel->setLayout(tablePanelLayout);

    QHBoxLayout *widthLayout = new QHBoxLayout();
    tableWidthLabel= new QLabel(tr("Width"));
    tableWidthLabel->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    widthLayout->addWidget(tableWidthLabel);
    tableWidthSpinner = new QSpinBox();
    tableWidthSpinner->setMinimum(1);
    tableWidthSpinner->setMaximum(100);
    widthLayout->addWidget(tableWidthSpinner);
    tableWidthUnitCombo = new QComboBox();
    tableWidthUnitCombo->addItem(tr("Percent"), "%");
    tableWidthUnitCombo->addItem(tr("Pixels"), "px");
    widthLayout->addWidget(tableWidthUnitCombo);
    tablePanelLayout->addLayout(widthLayout, row, col++);

    row=1;
    col=0;

    QHBoxLayout *topMarginLayout = new QHBoxLayout();
    tableTopMarginSpinner = new QSpinBox();
    tableTopMarginSpinner->setMinimum(0);
    tableTopMarginSpinner->setMaximum(100);
    tableTopMarginUnitLabel = new QLabel(tr("Top Margin"));
    tableTopMarginUnitLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    tableTopMarginUnitCombo = new QComboBox();
    tableTopMarginUnitCombo->addItem(tr("Pixels"),"px");
    tableTopMarginUnitCombo->addItem(tr("Percent"),"%");
    topMarginLayout->addWidget(tableTopMarginUnitLabel);
    topMarginLayout->addWidget(tableTopMarginSpinner);
    topMarginLayout->addWidget(tableTopMarginUnitCombo);
    tablePanelLayout->addLayout(topMarginLayout, row, col++);

    QHBoxLayout *bottomMarginLayout = new QHBoxLayout();
    tableBottomMarginSpinner = new QSpinBox();
    tableBottomMarginSpinner->setMinimum(0);
    tableBottomMarginSpinner->setMaximum(100);
    tableBottomMarginUnitLabel = new QLabel(tr("Bottom Margin"));
    tableBottomMarginUnitLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    tableBottomMarginUnitCombo = new QComboBox();
    tableBottomMarginUnitCombo->addItem(tr("Pixels"),"px");
    tableBottomMarginUnitCombo->addItem(tr("Percent"),"%");
    bottomMarginLayout->addWidget(tableBottomMarginUnitLabel);
    bottomMarginLayout->addWidget(tableBottomMarginSpinner);
    bottomMarginLayout->addWidget(tableBottomMarginUnitCombo);
    tablePanelLayout->addLayout(bottomMarginLayout, row, col++);

    row++;
    col=0;

    QHBoxLayout *leftMarginLayout = new QHBoxLayout();
    tableLeftMarginSpinner = new QSpinBox();
    tableLeftMarginSpinner->setMinimum(0);
    tableLeftMarginSpinner->setMaximum(100);
    tableLeftMarginUnitLabel = new QLabel(tr("Left Margin"));
    tableLeftMarginUnitLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    tableLeftMarginUnitCombo = new QComboBox();
    tableLeftMarginUnitCombo->addItem(tr("Pixels"),"px");
    tableLeftMarginUnitCombo->addItem(tr("Percent"),"%");
    leftMarginLayout->addWidget(tableLeftMarginUnitLabel);
    leftMarginLayout->addWidget(tableLeftMarginSpinner);
    leftMarginLayout->addWidget(tableLeftMarginUnitCombo);
    tablePanelLayout->addLayout(leftMarginLayout, row, col++);


    QHBoxLayout *rightMarginLayout = new QHBoxLayout();
    tableRightMarginSpinner = new QSpinBox();
    tableRightMarginSpinner->setMinimum(0);
    tableRightMarginSpinner->setMaximum(100);
    tableRightMarginUnitLabel = new QLabel(tr("Right Margin"));
    tableRightMarginUnitLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    tableRightMarginUnitCombo = new QComboBox();
    tableRightMarginUnitCombo->addItem(tr("Pixels"),"px");
    tableRightMarginUnitCombo->addItem(tr("Percent"),"%");
    rightMarginLayout->addWidget(tableRightMarginUnitLabel);
    rightMarginLayout->addWidget(tableRightMarginSpinner);
    rightMarginLayout->addWidget(tableRightMarginUnitCombo);
    tablePanelLayout->addLayout(rightMarginLayout, row, col++);

    connect(tableWidthSpinner, SIGNAL(valueChanged(int)), this, SLOT(tableWidthChanged(int)));
    connect(tableWidthUnitCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(tableWidthUnitChanged(int)));

    connect(tableTopMarginSpinner, SIGNAL(valueChanged(int)), this, SLOT(tableTopMarginChanged(int)));
    connect(tableBottomMarginSpinner, SIGNAL(valueChanged(int)), this, SLOT(tableBottomMarginChanged(int)));
    connect(tableLeftMarginSpinner, SIGNAL(valueChanged(int)), this, SLOT(tableLeftMarginChanged(int)));
    connect(tableRightMarginSpinner, SIGNAL(valueChanged(int)), this, SLOT(tableRightMarginChanged(int)));

    connect(tableRightMarginUnitCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(tableMarginUnitChanged(int)));
    connect(tableLeftMarginUnitCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(tableMarginUnitChanged(int)));
    connect(tableTopMarginUnitCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(tableMarginUnitChanged(int)));
    connect(tableBottomMarginUnitCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(tableMarginUnitChanged(int)));

}




// Build the cell tab widget
void TablePropertiesDialog::setupCellPanel() {
    int row=0;
    int col=0;

    cellLayout = new QGridLayout();
    borderColorButton = new QPushButton();
    backgroundColorButton = new QPushButton();
    fontColorButton = new QPushButton();

    borderColorButton->setText(tr("Border Color"));
    fontColorButton->setText(tr("Font Color"));
    backgroundColorButton->setText(tr("Background Color"));
    cellPanel->setLayout(cellLayout);

    cellLayout->addWidget(borderColorButton, row, col++);
    cellLayout->addWidget(backgroundColorButton, row, col++);
    cellLayout->addWidget(fontColorButton, row, col++);

    row++;
    col=0;

    borderSizeLayout = new QHBoxLayout();
    borderSizeSpinner = new QSpinBox();
    borderSizeSpinner->setMinimum(0);
    borderSizeSpinner->setMaximum(50);
    borderSizeLabel = new QLabel(tr("Border Size"));
    borderSizeLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    borderSizeLayout->addWidget(borderSizeLabel);
    borderSizeLayout->addWidget(borderSizeSpinner);
    cellLayout->addLayout(borderSizeLayout, row, col++);

    paddingLayout = new QHBoxLayout();
    paddingSizeSpinner = new QSpinBox(this);

    paddingSizeSpinner->setMinimum(0);
    paddingSizeSpinner->setMaximum(50);
    paddingLabel = new QLabel(tr("Padding"));
    paddingLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    paddingLayout->addWidget(paddingLabel);
    paddingLayout->addWidget(paddingSizeSpinner);
    cellLayout->addLayout(paddingLayout, row, col++);

    row++;
    col=0;

    QHBoxLayout *borderLayoutTop = new QHBoxLayout();
    QLabel *borderLabelTop = new QLabel(tr("Top Border"));
    borderLabelTop->setAlignment(Qt::AlignCenter | Qt::AlignRight);
    borderComboTop = new QComboBox();
    borderLayoutTop->addWidget(borderLabelTop);
    borderLayoutTop->addWidget(borderComboTop);
    loadBorderCombo(borderComboTop);
    cellLayout->addLayout(borderLayoutTop, row, col++);

    QHBoxLayout *borderLayoutBottom = new QHBoxLayout();
    QLabel *borderLabelBottom = new QLabel(tr("Bottom Border"));
    borderLabelBottom->setAlignment(Qt::AlignCenter | Qt::AlignRight);
    borderComboBottom = new QComboBox();
    borderLayoutBottom->addWidget(borderLabelBottom);
    borderLayoutBottom->addWidget(borderComboBottom);
    loadBorderCombo(borderComboBottom);
    cellLayout->addLayout(borderLayoutBottom, row, col++);

    QHBoxLayout *borderLayoutLeft = new QHBoxLayout();
    QLabel *borderLabelLeft = new QLabel(tr("Left Border"));
    borderLabelLeft->setAlignment(Qt::AlignCenter | Qt::AlignRight);
    borderComboLeft = new QComboBox();
    borderLayoutLeft->addWidget(borderLabelLeft);
    borderLayoutLeft->addWidget(borderComboLeft);
    loadBorderCombo(borderComboLeft);
    cellLayout->addLayout(borderLayoutLeft, row, col++);

    QHBoxLayout *borderLayoutRight = new QHBoxLayout();
    QLabel *borderLabelRight = new QLabel(tr("Right Border"));
    borderLabelRight->setAlignment(Qt::AlignCenter | Qt::AlignRight);
    borderComboRight = new QComboBox();
    borderLayoutRight->addWidget(borderLabelRight);
    borderLayoutRight->addWidget(borderComboRight);
    loadBorderCombo(borderComboRight);
    cellLayout->addLayout(borderLayoutRight, row, col++);

    row++;
    col=0;

    QHBoxLayout *alignmentLayoutHorizontal = new QHBoxLayout();
    QLabel *alignmentLabelHorizontal = new QLabel(tr("Horizontal Alignment"));
    alignmentLayoutHorizontal->setAlignment(Qt::AlignCenter | Qt::AlignRight);
    horizontalAlignmentCombo = new QComboBox();
    alignmentLayoutHorizontal->addWidget(alignmentLabelHorizontal);
    alignmentLayoutHorizontal->addWidget(horizontalAlignmentCombo);
    horizontalAlignmentCombo->addItem(tr("Default"), "");
    horizontalAlignmentCombo->addItem(tr("Left"),"left");
    horizontalAlignmentCombo->addItem(tr("Right"),"right");
    horizontalAlignmentCombo->addItem(tr("Center"),"center");
    cellLayout->addLayout(alignmentLayoutHorizontal, row, col++);
    connect(horizontalAlignmentCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(horizontalAlignmentChanged(int)));

    // Vertical alignment doesn't work for some reason.
//    QHBoxLayout *alignmentLayoutVertical = new QHBoxLayout();
//    QLabel *alignmentLabelVertical = new QLabel(tr("Vertical Alignment"));
//    alignmentLabelVertical->setAlignment(Qt::AlignCenter | Qt::AlignRight);
//    verticalAlignmentCombo = new QComboBox();
//    alignmentLayoutVertical->addWidget(alignmentLabelVertical);
//    alignmentLayoutVertical->addWidget(verticalAlignmentCombo);
//    verticalAlignmentCombo->addItem(tr("Default"), "");
//    verticalAlignmentCombo->addItem(tr("Bottom"),"bottom");
//    verticalAlignmentCombo->addItem(tr("Top"),"top");
//    verticalAlignmentCombo->addItem(tr("Center"),"center");
//    cellLayout->addLayout(alignmentLayoutVertical, row, col++);
//    connect(verticalAlignmentCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(verticalAlignmentChanged(int)));

    colorDialog = new QColorDialog();

    connect(borderColorButton, SIGNAL(pressed()), this, SLOT(cellBorderColorButtonPressed()));
    connect(backgroundColorButton, SIGNAL(pressed()), this, SLOT(cellBackgroundColorButtonPressed()));
    connect(fontColorButton, SIGNAL(pressed()), this, SLOT(cellFontColorButtonPressed()));
    connect(borderSizeSpinner, SIGNAL(valueChanged(int)), this, SLOT(cellBorderSizeChanged(int)));
    connect(paddingSizeSpinner, SIGNAL(valueChanged(int)), this, SLOT(cellPaddingSizeChanged(int)));
}




// Load the border combo box styles
void TablePropertiesDialog::loadBorderCombo(QComboBox *box) {
    box->addItem(tr("Solid"), "solid");
    box->addItem(tr("None"), "none");
    box->addItem(tr("Dotted"), "dotted");
    box->addItem(tr("Dashed"), "dashed");
    box->addItem(tr("Double"), "double");
    box->addItem(tr("Ridge"), "ridge");
    box->addItem(tr("Inset"), "inset");
    box->addItem(tr("Outset"), "outset");
    connect(box, SIGNAL(currentIndexChanged(int)), SLOT(cellBorderChanged(int)));
}



// Signaled when a border color button is pressed
void TablePropertiesDialog::cellBorderColorButtonPressed() {
    QColor c = colorDialog->getColor();
    if (!c.isValid())
        return;
    cellBorderColorB = c.blue();
    cellBorderColorG = c.green();
    cellBorderColorR = c.red();
    generatePreview();
}




// Signaled when the background color button si presesd
void TablePropertiesDialog::cellBackgroundColorButtonPressed() {
    QColor c = colorDialog->getColor();
    if (!c.isValid())
        return;
    cellBackgroundColorB = c.blue();
    cellBackgroundColorG = c.green();
    cellBackgroundColorR = c.red();
    generatePreview();

}



// Signaled when the font color button is pressed
void TablePropertiesDialog::cellFontColorButtonPressed() {
    QColor c = colorDialog->getColor();
    if (!c.isValid())
        return;
    cellFontColorB = c.blue();
    cellFontColorG = c.green();
    cellFontColorR = c.red();
    generatePreview();
}



// Signaled when the cell padding spinner chnanges values
void TablePropertiesDialog::cellPaddingSizeChanged(int size) {
    cellPaddingSize = size;
    generatePreview();
}




// Signaled when a cell border style changes
void TablePropertiesDialog::cellBorderChanged(int index) {
    Q_UNUSED(index);
#if QT_VERSION >= 0x050000
    cellBorderStyleBottom = borderComboBottom->currentData().toString();
    cellBorderStyleTop = borderComboTop->currentData().toString();
    cellBorderStyleLeft = borderComboLeft->currentData().toString();
    cellBorderStyleRight = borderComboRight->currentData().toString();
#endif
#if QT_VERSION < 0x050000
    cellBorderStyleBottom = borderComboBottom->itemData(borderComboBottom->currentIndex()).toString();
    cellBorderStyleTop = borderComboTop->itemData(borderComboTop->currentIndex()).toString();
    cellBorderStyleLeft = borderComboLeft->itemData(borderComboLeft->currentIndex()).toString();
    cellBorderStyleRight = borderComboRight->itemData(borderComboRight->currentIndex()).toString();
#endif
    generatePreview();
}





// Signaled when the horizontal alignment changes
void TablePropertiesDialog::horizontalAlignmentChanged(int index) {
    Q_UNUSED(index);
#if QT_VERSION >= 0x050000
    horizontalAlignment = horizontalAlignmentCombo->currentData().toString();
#endif
#if QT_VERSION < 0x050000
    horizontalAlignment = horizontalAlignmentCombo->itemData(horizontalAlignmentCombo->currentIndex()).toString();
#endif
    generatePreview();
}



// Signaled when the vertical alignment changes
void TablePropertiesDialog::verticalAlignmentChanged(int index) {
    Q_UNUSED(index);
#if QT_VERSION >= 0x050000
    verticalAlignment = verticalAlignmentCombo->currentData().toString();
#endif
#if QT_VERSION < 0x050000
    verticalAlignment = verticalAlignmentCombo->itemData(verticalAlignmentCombo->currentIndex()).toString();
#endif
    generatePreview();
}




// Signald when the table left margin changes
void TablePropertiesDialog::tableLeftMarginChanged(int size) {
    tableLeftMargin = size;
    generatePreview();
}



// Signaled when the table right margin changes
void TablePropertiesDialog::tableRightMarginChanged(int size) {
    tableRightMargin = size;
    generatePreview();
}



// Signaled when the table top margn changes
void TablePropertiesDialog::tableTopMarginChanged(int size) {
    tableTopMargin = size;
    generatePreview();
}




// Signaled when a table bottom margen changes
void TablePropertiesDialog::tableBottomMarginChanged(int size) {
    tableBottomMargin = size;
    generatePreview();
}


// Signaled when a cell border size changes
void TablePropertiesDialog::cellBorderSizeChanged(int size) {
    cellBorderWidth = size;
    generatePreview();
}


// Signald when a table margin unit changes
void TablePropertiesDialog::tableMarginUnitChanged(int index) {
    Q_UNUSED(index);

#if QT_VERSION >= 0x050000
    tableRightMarginUnit = tableRightMarginUnitCombo->currentData().toString();
    tableTopMarginUnit = tableTopMarginUnitCombo->currentData().toString();
    tableLeftMarginUnit = tableLeftMarginUnitCombo->currentData().toString();
    tableBottomMarginUnit = tableBottomMarginUnitCombo->currentData().toString();
#endif
#if QT_VERSION < 0x050000
    tableRightMarginUnit = tableRightMarginUnitCombo->itemData(tableRightMarginUnitCombo->currentIndex()).toString();
    tableTopMarginUnit = tableTopMarginUnitCombo->itemData(tableTopMarginUnitCombo->currentIndex()).toString();
    tableLeftMarginUnit = tableLeftMarginUnitCombo->itemData(tableLeftMarginUnitCombo->currentIndex()).toString();
    tableBottomMarginUnit = tableBottomMarginUnitCombo->itemData(tableBottomMarginUnitCombo->currentIndex()).toString();
#endif

    generatePreview();
}


// Signaled when a table width spinner changes
void TablePropertiesDialog::tableWidthChanged(int size) {
    tableWidth = size;
    generatePreview();
}



// Signaled when a table width unit changes
void TablePropertiesDialog::tableWidthUnitChanged(int index) {
    tableWidthUnit = tableWidthUnitCombo->itemData(index).toString();
    if (tableWidthUnit == "%")
        tableWidthSpinner->setMaximum(100);
    else
        tableWidthSpinner->setMaximum(1000);
    generatePreview();
}


