/* 
   This is a sample exit. It is called when a note's contents 
   are saved. It will scan the contents of a note for  any
   [], [x], or [X] strings. If found, those strings will be   
   changed into a todo statement.                              	
*/

var c = note.getContents();

var todoUnchecked = "<input type=\"CHECKBOX\" onmouseover=\"style.cursor='hand'\" onclick=\"if(!checked) removeAttribute('checked'); else setAttribute('checked', 'checked'); editorWindow.editAlert();\" style=\"cursor: pointer;\">";

var todoChecked = "<input checked=\"checked\" type=\"checkbox\" onclick=\"if(!checked) removeAttribute('checked'); else setAttribute('checked', 'checked'); editorWindow.editAlert();\" style=\"cursor: hand;\">";


c = c.replace("[]", todoUnchecked);
c = c.replace("[x]", todoChecked);
c = c.replace("[X]", todoChecked);
note.setContents(c);
