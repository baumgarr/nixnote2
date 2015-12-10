/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

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

#include "searchpreferences.h"
#include "global.h"
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>

extern Global global;

SearchPreferences::SearchPreferences(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    int row=0;

    syncAttachments = new QCheckBox(tr("Index Attachments"));
    mainLayout->addWidget(syncAttachments,row++,0);
    syncAttachments->setChecked(global.synchronizeAttachments());

    // Disabled for performance reasons
    syncAttachments->setChecked(false);
    syncAttachments->setVisible(false);

    clearNotebookOnSearch = new QCheckBox(tr("Clear Notebook Selection on Search Text Changes"));
    mainLayout->addWidget(clearNotebookOnSearch,row++,0);
    clearNotebookOnSearch->setChecked(global.getClearNotebookOnSearch());

    clearTagsOnSearch = new QCheckBox(tr("Clear Tag Selection on Search Text Changes"));
    mainLayout->addWidget(clearTagsOnSearch,row++,0);
    clearTagsOnSearch->setChecked(global.getClearTagsOnSearch());

    clearSearchOnNotebook = new QCheckBox(tr("Clear Search Text on Notebook Changes"));
    mainLayout->addWidget(clearSearchOnNotebook,row++,0);
    clearSearchOnNotebook->setChecked(global.getClearSearchOnNotebook());

    tagSelectionOr = new QCheckBox(tr("Show Any Matching Tags When Selecting Multiple Tags"));
    mainLayout->addWidget(tagSelectionOr,row++,0);
    tagSelectionOr->setChecked(global.indexPDFLocally);

    indexPDF = new QCheckBox(tr("Index PDFs locally"));
    mainLayout->addWidget(indexPDF,row++,0);
    indexPDF->setChecked(global.indexPDFLocally);

    weight = new QSpinBox(this);
    mainLayout->addWidget(new QLabel(tr("Minimum Image Recognition Weight")), row,0);
    mainLayout->addWidget(weight,row++,1);
    weight->setMinimum(1);
    weight->setMaximum(100);
    weight->setValue(global.getMinimumRecognitionWeight());
    this->setFont(global.getGuiFont(font()));

    mainLayout->setAlignment(Qt::AlignTop);

}




void SearchPreferences::saveValues() {
    global.setMinimumRecognitionWeight(weight->value());
    //global.setSynchronizeAttachments(syncAttachments->isChecked());
    global.setClearNotebookOnSearch(clearNotebookOnSearch->isChecked());
    global.setClearTagsOnSearch(clearNotebookOnSearch->isChecked());
    global.setClearSearchOnNotebook(clearSearchOnNotebook->isChecked());
    global.setTagSelectionOr(tagSelectionOr->isChecked());
    global.setIndexPDFLocally(indexPDF->isChecked());
}
