/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

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

#ifndef SHORTCUTKEYS_H
#define SHORTCUTKEYS_H

#include <QObject>
#include <QHash>
#include <string>

using namespace std;


class ShortcutKeys : public QObject
{
    Q_OBJECT
private:
    void loadCustomKeys(QString fileName);

public:
    explicit ShortcutKeys(QObject *parent = 0);

    QString *File_Note_Add;  	  			// Add a new note
    QString *File_Note_Reindex;			// Reindex the current note
    QString *File_Note_Modify_Tags;		// Change current note tags
    QString *File_Note_Delete;				// Delete a tag
    QString *File_Note_Restore;			// Undelete a note
    QString *File_Note_Duplicate;			// duplicate a note
    QString *File_Notebook_Add;			// Add a notebook
    QString *File_Notebook_Edit;			// Edit an existing notebook
    QString *File_Notebook_Delete;			// Delete the existing notebook
    QString *File_Notebook_Open; 			// Open a closed (i.e. archived) notebook
    QString *File_Notebook_Close;			// Close (i.e. archive) a notebook
    QString *File_Tag_Add;					// Add a notebook
    QString *File_Tag_Edit;				// Edit an existing notebook
    QString *File_Tag_Delete;				// Delete the existing notebook
    QString *File_SavedSearch_Add;			// Add a notebook
    QString *File_SavedSearch_Edit;		// Edit an existing notebook
    QString *File_SavedSearch_Delete;		// Delete the existing notebook
    QString *File_Email;					// Email note
    QString *File_Print;					// Print
    QString *File_Backup;					// Backup the database
    QString *File_Restore;					// Restore the database
    QString *File_Empty_Trash;				// Purge all delete notes
    QString *File_Exit;					// I'm outahere

    QString *Edit_Find_In_Note;			// Search only within the current note
    QString *Edit_Undo;					// Undo last change
    QString *Edit_Redo;					// Redo the last undone change
    QString *Edit_Cut;						// Cut current selection to the clipboard
    QString *Edit_Copy;					// Copy the current selection to the clipboard
    QString *Edit_Paste;					// Paste
    QString *Edit_Remove_Format;             // Remove formatting
    QString *Edit_Paste_Without_Formatting; // Paste as plain text
    QString *Edit_Preferences;				// Settings dialog box
    QString *Edit_Insert_Hyperlink;		// Encrypt selected text
    QString *Edit_Insert_Table;			// Insert table into note
    QString *Edit_Insert_Table_Row;		// Insert row into table
    QString *Edit_Delete_Table_Row;		// Delete a table row
    QString *Edit_Insert_Todo;				// Insert todo
    QString *Edit_Encrypt_Text;			// Encrypt selected text
    QString *Edit_Rotate_Image_Right;		// Rotate an image right
    QString *Edit_Rotate_Image_Left;		// Rotate an image left
    QString *Edit_Select_All;
    QString *Edit_Search_Notes;
    QString *Edit_Reset_Search;
    QString *Edit_Search_Find;
    QString *Edit_Search_Find_Next;
    QString *Edit_Search_Find_Prev;
    QString *Edit_Search_Find_Replace;

    QString *View_Extended_Information;	// View details on the current note
    QString *View_Show_Left_Side;           // Show/hide the left hand panel
    QString *View_Show_Note_List;
    QString *View_Show_Note_Panel;
    QString *View_Source;

    QString *Format_Bold;					// Bold (duh)
    QString *Format_Underline;				// Underline
    QString *Format_Italic;				// Italic
    QString *Format_Strikethrough;			// Strikethrough
    QString *Format_Horizontal_Line;		// Href line
    QString *Format_Superscript;			// Set superscript
    QString *Format_Subscript;				// Subscript
    QString *Format_Alignment_Left;		// Left align text
    QString *Format_Alignment_Center;		// Center text
    QString *Format_Alignment_Right;		// Right align text
    QString *Format_List_Bullet;			// Bullet list
    QString *Format_List_Numbered;			// Numbered list
    QString *Format_Indent_Increase;		// Increase the indentation
    QString *Format_Indent_Decrease;		// Decrease the indent

    QString *Online_Note_History;			// Synchronize with Evernote

    QString *Online_Synchronize;			// Synchronize with Evernote
    QString *Online_Connect;				// Connect to Evernote
    QString *Tools_Account_Information;	// Show account information
    QString *Tools_Database_Status;		// Current database information
    QString *Tools_Database_Reindex;    // Reindex all notes & resources
    QString *Tools_Import_Folders;      // setup import folders

    QString *About_Release_Notes;			// Current version's release notes
    QString *About_Log;					// Message log
    QString *About_About;					// About dialog box

    QString *Focus_Title;					// Switch focus to the title bar
    QString *Focus_Tag;					// Switch focus to the tag edit
    QString *Focus_Note;					// Switch focus to the note
    QString *Focus_Author;					// Switch focus to the author
    QString *Focus_Url;					// Switch focus to the URL

    QString *Prev_Tab;                  // Previous Tab Shortcut
    QString *Next_Tab;                  // Next Tab Shortcut
    QString *Close_Tab;                 // Close the current tab
    QString *Down_Note;                 // Move down one note in the list
    QString *Up_Note;                   // Move up one note in the list

    QString *Insert_DateTime;				// Insert the current date/time

    QHash<QString, QString> *actionMap;
    QHash<QString, QString> *shortcutMap;

    void loadkey(QString action, QString *shortcut);
    void removeByShortcut(QString shortcut);
    void removeByAction(QString action);
    bool containsShortcut(QString *shortcut);
    bool containsAction(QString *action);
    QString getShortcut(QString *action);
    QString getAction(QString *shortcut);

    
signals:
    
public slots:
    
};

#endif // SHORTCUTKEYS_H





