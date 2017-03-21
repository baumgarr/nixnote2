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



#ifndef TABLEPROPERTIESDIALOG_H
#define TABLEPROPERTIESDIALOG_H

#include <QWidget>
#include <QPushButton>
#include <QColorDialog>
#include <QListView>
#include <QTabWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWebView>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>

class TablePropertiesDialog : public QDialog
{
    Q_OBJECT
private:

    int tableLeftMargin;                           // current user value for the table left margin
    int tableRightMargin;                          // current user value for the table right margin
    int tableBottomMargin;                         // current user value for the table bottom margin
    int tableTopMargin;                            // current user value for the table top margin

    QString tableLeftMarginUnit;                   // current user value for the table left margin unit
    QString tableRightMarginUnit;                  // current user value for the table right margin unit
    QString tableBottomMarginUnit;                 // current user value for the table bottom margin unit
    QString tableTopMarginUnit;                    // current user value for the table top margin unit

    QString tableLayout;                           // Current user value for the table layout. Typically "fixed" & I don't provide a way to change it.
    bool evernoteTable;                            // Evernote style table?
    QString tableExtras;                           // Anything extra that isn't specified goes in this CSS value string.
    QString tableBorderCollapse;                   // table-collapse value.

    int tableWidth;                                // Table overall width
    QString tableWidthUnit;                        // Table width units (% or px)
    QLabel *tableWidthLabel;                       // Label for user width selection
    QSpinBox *tableWidthSpinner;                   // Spinner to allow user to specify value for the total table width
    QComboBox *tableWidthUnitCombo;                // Combo box to change table width units (pixels or percent).

    QSpinBox *tableLeftMarginSpinner;              // Table left margin spin box
    QSpinBox *tableRightMarginSpinner;             // Table right margin spin box
    QSpinBox *tableTopMarginSpinner;               // table top margin spin box
    QSpinBox *tableBottomMarginSpinner;            // Table bottom margin spin box

    QComboBox *tableTopMarginUnitCombo;            // Table top margin unit combo box
    QComboBox *tableBottomMarginUnitCombo;         // Table bottom margin unit combo box
    QComboBox *tableLeftMarginUnitCombo;           // Table left margin unit combo box
    QComboBox *tableRightMarginUnitCombo;          // Table right margin unit combo box

    QLabel *tableTopMarginUnitLabel;               // Label for user top margin selection
    QLabel *tableBottomMarginUnitLabel;            // Label for user bottom margin selection
    QLabel *tableLeftMarginUnitLabel;              // Label for unit left margin selection
    QLabel *tableRightMarginUnitLabel;             // Label for unit right margin selection

    QString horizontalAlignment;                   // current user value for horizontal alignment value
    QString verticalAlignment;                     // current user value for vertical alignment value

    double cellWidth;                              // Cell overall width
    QString cellExtras;                            // any current table css value that we don't allow specification for.
    QString cellBorderStyleBottom;                 // Cell bottom border style
    QString cellBorderStyleTop;                    // cell top border style
    QString cellBorderStyleLeft;                   // cell left border style
    QString cellBorderStyleRight;                  // cell right border style

    int cellBorderWidth;                           // Width of the cell border
    int cellMargin;                                // Cell margins
    int cellBorderColorR;                          // cell red border value
    int cellBorderColorG;                          // cell green border value
    int cellBorderColorB;                          // cell blue border value

    int cellBackgroundColorB;                      // cell background blue value
    int cellBackgroundColorR;                      // cell background red value
    int cellBackgroundColorG;                      // cell backgronud green value

    int cellFontColorR;                            // cell font red value
    int cellFontColorG;                            // cell font green value
    int cellFontColorB;                            // cell font blue vaule

    int cellPaddingSize;                          // Current cell padding sizes

    void setupCellPanel();                        // Setup the cell tab
    void setupTablePanel();                       // Setup the table tab

    QHBoxLayout *buttonLayout;                    // Layout used for the "OK" & "Cancel" buttons.
    QGridLayout *tablePanelLayout;                // Man layout used in the "table" tab
    QVBoxLayout *mainLayout;                      // Main dialog layout
    QPushButton *okButton;                        // Dialog box OK button
    QPushButton *cancelButton;                    // Dialog box cancel button
    QTabWidget  *tabs;                            // Tab widget holding table & cell options
    QWebView *preview;                            // HTML page showing the user's selected values

    QLabel *paddingLabel;                         // Label used to show cell padding selection
    QHBoxLayout *borderSizeLayout;                // layout used for the border size selection
    QHBoxLayout *paddingLayout;                   // layout used for the cell padding selection
    QLabel *borderSizeLabel;                      // Label for border size spinner

    QColorDialog *colorDialog;                    // Color dialog used for user selections
    QPushButton *borderColorButton;               // Show the border color dialog
    QGridLayout *cellLayout;                      // Main layout used in the cell tab
    QPushButton *fontColorButton;                 // show the dialog for the font color
    QPushButton *backgroundColorButton;           // show the dialog for the background color
    QSpinBox *borderSizeSpinner;                  // border size spinner
    QSpinBox *paddingSizeSpinner;                 // cell padding spinner

    QComboBox *borderComboBottom;                 // Show the bottom border units
    QComboBox *borderComboLeft;                   // Show the left border units
    QComboBox *borderComboRight;                  // Show the right border units
    QComboBox *borderComboTop;                    // Show the top border units.

    QComboBox *verticalAlignmentCombo;             // Shows the vertical alignment values
    QComboBox *horizontalAlignmentCombo;           // Shows the horizontal alignment values

    void loadBorderCombo(QComboBox *combo);        // Load a list of available border types into a combo box

    void setTableStyle(QString value);             // Setup the table values from the current css.
    void setCellStyle(QString value);              // Setup the cell values from the current css.
    QWidget *cellPanel;                            // Cell settings tab
    QWidget *tablePanel;                           // Table settings tab

    void loadValues();                             // load initial values into widgets
    void setInitialBorderSelection(QComboBox *box, QString value);  // Set initial value for the border combos

public:
    explicit TablePropertiesDialog(QString tableCss, QString cellCss, QWidget *parent = 0);
    bool okButtonPressed;                           // Was OK pressed? true=yes, false=no
    QString getCellCss();                           // User selected cell CSS values
    QString getTableCss();                          // User selected table CSS values

signals:

public slots:
    void okButtonClicked();                         // Called when user clicks "OK"
    void cancelButtonClicked();                     // Called when user clicks "Cancel"
    void generatePreview();                         // Generate a preview of a table using user-selected values
    void cellBorderColorButtonPressed();            // Display a color dialog so user can set border color
    void cellBackgroundColorButtonPressed();        // Display a color dialog so user can set background color
    void cellFontColorButtonPressed();              // Display a color dialog so user can set font color
    void cellPaddingSizeChanged(int size);          // Change cell padding in the table
    void verticalAlignmentChanged(int index);       // Change the vertical alignment. Doesn't currently work.
    void horizontalAlignmentChanged(int index);     // Set horizontal alignment
    void cellBorderChanged(int index);              // Change the type of border in a table
    void tableLeftMarginChanged(int size);          // Set the left margin around a table
    void tableRightMarginChanged(int size);         // Set the right margin around a table
    void tableTopMarginChanged(int size);           // Set the top margin around a table
    void tableBottomMarginChanged(int size);        // Set the bottom margin around a table
    void cellBorderSizeChanged(int size);           // Set the border size
    void tableMarginUnitChanged(int index);         // Called when a table margin unit (i.e. pixel or %) changes.
    void tableWidthUnitChanged(int index);          // Called when a table width unit (i.e. pixel or %) changes.
    void tableWidthChanged(int size);               // Called when a table width changes

};

#endif // TABLEPROPERTIESDIALOG_H
