#include "shortcutkeys.h"
#include "global.h"

#include <QFile>

extern Global global;

ShortcutKeys::ShortcutKeys(QObject *parent) :
    QObject(parent)
{
    File_Note_Add = new QString("Ctrl+N");		// Add a new note
    File_Note_Reindex = new QString();			// Reindex the current note
    File_Note_Modify_Tags = new QString();		// Change current note tags
    File_Note_Delete = new QString("Ctrl+Shift+D");	// Delete a note
    File_Note_Restore = new QString();			// Undelete a note
    File_Note_Duplicate = new QString();			// Duplicate a note
    File_Notebook_Add = new QString();			// Add a notebook
    File_Notebook_Edit = new QString();			// Edit an existing notebook
    File_Notebook_Delete = new QString();		// Delete the existing notebook
    File_Notebook_Open = new QString(); 			// Open a closed (i.e. archived) notebook
    File_Notebook_Close = new QString();			// Close (i.e. archive) a notebook
    File_Tag_Add = new QString("Ctrl+Shift+T");	// Add a notebook
    File_Tag_Edit = new QString();				// Edit an existing notebook
    File_Tag_Delete = new QString();				// Delete the existing notebook
    File_SavedSearch_Add = new QString();		// Add a notebook
    File_SavedSearch_Edit = new QString();		// Edit an existing notebook
    File_SavedSearch_Delete = new QString();		// Delete the existing notebook
    File_Email = new QString("Ctrl+Shift+E");	// Email note
    File_Print = new QString("Ctrl+P");			// Print
    File_Backup = new QString();				// Backup
    File_Restore = new QString();				// Restore
    File_Empty_Trash = new QString();			// Purge all delete notes
    File_Exit = new QString("Ctrl+Q");			// I'm outahere

    Edit_Find_In_Note = new QString("Ctrl+F");	// Search only within the current note
    Edit_Undo = new QString("Ctrl+Z");			// Undo last change
    Edit_Redo = new QString("Ctrl+Y");			// Redo the last undone change
    Edit_Cut = new QString("Ctrl+X");			// Cut current selection to the clipboard
    Edit_Copy = new QString("Ctrl+C");			// Copy the current selection to the clipboard
    Edit_Paste = new QString("Ctrl+V");			// Paste
    Edit_Paste_Without_Formatting = new QString("Ctrl+Shift+P"); // Paste as plain text
    Edit_Preferences = new QString();			// Settings dialog box
    Edit_Select_All = new QString("Ctrl+A");

    Edit_Insert_Hyperlink = new QString("Ctrl+K");  	// Insert a hyperlink
    Edit_Insert_Table = new QString();  				// Insert a table
    Edit_Insert_Table_Row = new QString();  			// Insert a table row
    Edit_Delete_Table_Row = new QString();  			// Delete a table row
    Edit_Insert_Todo = new QString();
    Edit_Encrypt_Text = new QString();
    Edit_Rotate_Image_Right = new QString();
    Edit_Rotate_Image_Left = new QString();
    Edit_Search_Notes = new QString("F6");
    Edit_Search_Find = new QString("Ctrl+F");
    Edit_Search_Find_Next = new QString("Ctrl+G");
    Edit_Search_Find_Prev = new QString("Ctrl+Shift+G");
    Edit_Search_Find_Replace = new QString("Ctrl+H");
    Edit_Reset_Search = new QString("Ctrl+Shift+A");

    View_Extended_Information = new QString("F8");	// View details on the current note
    View_Show_Left_Side = new QString("F10");       // Show/hide the left side panel
    View_Show_Note_List = new QString("F11");      // Show/hide the list of notes
    View_Show_Note_Panel = new QString("Ctrl+F11");  // Show/hide the note browser
    View_Source = new QString();

    Format_Bold = new QString("Ctrl+B");			// Bold (duh)
    Format_Underline = new QString("Ctrl+U");		// Underline
    Format_Italic = new QString("Ctrl+I");			// Italic
    Format_Strikethrough = new QString("Ctrl+-");	// Strikethrough
    Format_Horizontal_Line = new QString();		// Href line
    Format_Superscript = new QString("Ctrl+=");	// Set superscript
    Format_Subscript = new QString("Ctrl+Shift+=");	// Subscript
    Format_Alignment_Left = new QString("Ctrl+L");	// Left align text
    Format_Alignment_Center = new QString("Ctrl+E");	// Center text
    Format_Alignment_Right = new QString("Ctrl+R");	// Right align text
    Format_List_Bullet = new QString("Ctrl+Shift+B");	// Bullet list
    Format_List_Numbered = new QString("Ctrl+Shift+N");	// Numbered list
    Format_Indent_Increase = new QString("Ctrl+M");		// Increase the indentation
    Format_Indent_Decrease = new QString("Ctrl+Shift+M");	// Decrease the indent

    Online_Note_History = new QString();

    Online_Synchronize = new QString("F9");		// Synchronize with Evernote
    Online_Connect = new QString();				// Connect to Evernote
    Tools_Account_Information = new QString();	// Show account information
    Tools_Database_Status = new QString();      // Show database status

    About_Release_Notes = new QString();			// Current version's release notes
    About_Log = new QString();					// Message log
    About_About = new QString();					// About dialog box

    Insert_DateTime = new QString("Ctrl+;");

    Focus_Title = new QString("Ctrl+Space");
    Focus_Tag = new QString();
    Focus_Note = new QString("Ctrl+E");
    Focus_Author = new QString();
    Focus_Url = new QString();

    // Setup value Array
    shortcutMap = new QHash<QString, QString>();
    actionMap = new QHash<QString, QString>();

    // Load the defaults
    loadkey(QString("File_Note_Add"), File_Note_Add);
    loadkey(QString("File_Tag_Add"), File_Tag_Add);
    loadkey(QString("File_Email"), File_Email);
    loadkey(QString("File_Print"), File_Print);
    loadkey(QString("File_Backup"), File_Backup);
    loadkey(QString("File_Restore"), File_Restore);
    loadkey(QString("File_Exit"), File_Exit);

    loadkey(QString("File_Note_Delete"), File_Note_Delete);

    loadkey(QString("Edit_Find_In_Note"), Edit_Find_In_Note);
    loadkey(QString("Edit_Undo"), Edit_Undo);
    loadkey(QString("Edit_Redo"), Edit_Redo);
    loadkey(QString("Edit_Cut"), Edit_Cut);
    loadkey(QString("Edit_Copy"), Edit_Copy);
    loadkey(QString("Edit_Paste"), Edit_Paste);
    loadkey(QString("Edit_Paste_Without_Formatting"), Edit_Paste_Without_Formatting);
    loadkey(QString("Edit_Insert_Hyperlink"), Edit_Insert_Hyperlink);
    loadkey(QString("Edit_Insert_Table_Row"), Edit_Insert_Table_Row);
    loadkey(QString("Edit_Delete_Table_Row"), Edit_Delete_Table_Row);
    loadkey(QString("Edit_Insert_Todo"), Edit_Insert_Todo);
    loadkey(QString("Edit_Rotate_Image_Right"), Edit_Rotate_Image_Right);
    loadkey(QString("Edit_Rotate_Image_Left"), Edit_Rotate_Image_Left);
    loadkey(QString("Edit_Select_All"), Edit_Select_All);
    loadkey(QString("Edit_Search_Notes"), Edit_Search_Notes);
    loadkey(QString("Edit_Search_Find"), Edit_Search_Find);
    loadkey(QString("Edit_Search_Find_Next"), Edit_Search_Find_Next);
    loadkey(QString("Edit_Search_Find_Prev"), Edit_Search_Find_Prev);
    loadkey(QString("Edit_Search_Find_Replace"), Edit_Search_Find_Replace);
    loadkey(QString("Edit_Reset_Search"), Edit_Reset_Search);

    loadkey(QString("View_Extended_Information"), View_Extended_Information);
    loadkey(QString("View_Source"), View_Source);
    loadkey(QString("View_Show_Left_Side"), View_Show_Left_Side);
    loadkey(QString("View_Show_Note_List"), View_Show_Note_List);
    loadkey(QString("View_Show_Note_Panel"), View_Show_Note_Panel);

    loadkey(QString("Format_Bold"), Format_Bold);
    loadkey(QString("Format_Underline"), Format_Underline);
    loadkey(QString("Format_Italic"), Format_Italic);
    loadkey(QString("Format_Strikethrough"), Format_Strikethrough);
    loadkey(QString("Format_Superscript"), Format_Superscript);
    loadkey(QString("Format_Subscript"), Format_Subscript);
    loadkey(QString("Format_Alignment_Left"), Format_Alignment_Left);
    loadkey(QString("Format_Alignment_Center"), Format_Alignment_Center);
    loadkey(QString("Format_Alignment_Right"), Format_Alignment_Right);
    loadkey(QString("Format_List_Bullet"), Format_List_Bullet);
    loadkey(QString("Format_List_Numbered"), Format_List_Numbered);
    loadkey(QString("Format_Indent_Increase"), Format_Indent_Increase);
    loadkey(QString("Format_Indent_Decrease"), Format_Indent_Decrease);
    loadkey(QString("Tools_Synchronize"), Online_Synchronize);
    loadkey(QString("Tools_Database_Status"), Tools_Database_Status);

    loadkey(QString("Focus_Title"), Focus_Title);
    loadkey(QString("Focus_Tag"), Focus_Tag);
    loadkey(QString("Focus_Note"), Focus_Note);
    loadkey(QString("Focus_Author"), Focus_Author);
    loadkey(QString("Focus_Url"), Focus_Url);

    loadkey(QString("Insert_DateTime"), Insert_DateTime);

    loadCustomKeys();

}

// Read in the custom keys (if they exist)
void ShortcutKeys::loadCustomKeys() {
    QString fileName = global.fileManager.getHomeDirPath("") + QString("shortcuts.txt");
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    if (file.isOpen()) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.replace("\t", " ");
            line = line.replace("\n", " ");
            line = line.replace("\r", " " );
            line = line.trimmed();
            QStringList list = line.split(" ");
            QStringList keyvalue;
            for(int i=0;i<list.size(); i++) {
                if (list[i].trimmed() != "" && !list[i].trimmed().startsWith("//"))
                    keyvalue.append(list[i].trimmed().toLower());
                if (list[i].trimmed().startsWith("//"))
                    i = list.size();
            }
            if (keyvalue.size() == 1)
                removeByAction(keyvalue[0]);
            if (keyvalue.size() >= 2)
                loadkey(keyvalue[0], &keyvalue[1]);
        }
        file.close();
    }
}


// Load a key value into the map for later use
void ShortcutKeys::loadkey(QString action, QString *shortcut) {
    action = action.toLower().trimmed();
    QString sc = shortcut->toLower().trimmed();

    // If we have an existing one, remove it.
    if (actionMap->contains(action))
        removeByAction(action);
    if (shortcutMap->contains(sc.toLower().trimmed()))
        removeByShortcut(sc.toLower().trimmed());

    if (sc == "") {
        removeByShortcut(sc);
        return;
    }

    //Add the new value
    actionMap->insert(action.toLower(), sc);
    shortcutMap->insert(sc.toLower(), action);
}

// Remove a shortcut by the Shortcut key
void ShortcutKeys::removeByShortcut(QString shortcut) {
    QString action = shortcutMap->key(shortcut.toLower(), "");
    shortcutMap->remove(shortcut.toLower());
    if (action != "")
        actionMap->remove(action.toLower());
}

// Remove a shortcut by the action itself
void ShortcutKeys::removeByAction(QString action) {
    QString shortcut = actionMap->key(action.toLower(),"");
    actionMap->remove(action.toLower());
    if (shortcut != "")
        shortcutMap->remove(shortcut.toLower());
}

// Check if a shortcut key exists
bool ShortcutKeys::containsShortcut(QString *shortcut) {
    QString sk(shortcut->toLower());
    QString key = shortcutMap->value(sk, "");
    if (key.trimmed() == "")
        return false;
    else
        return true;
}

// Check if an action exists
bool ShortcutKeys::containsAction(QString *action) {
    QString key = actionMap->value(action->toLower(), "");
    if (key.trimmed() == "")
        return false;
    else
        return true;
}

// Get a key based upon the action
QString ShortcutKeys::getShortcut(QString *action) {
    if (!actionMap->contains(action->toLower()))
        return "";
    QString retval(actionMap->value(action->toLower()));
    return retval;
}

// Get an action based upon the key
QString ShortcutKeys::getAction(QString *shortcut) {
    if (!shortcutMap->contains(shortcut->toLower()))
        return "";
    return shortcutMap->value(shortcut->toLower());
}

