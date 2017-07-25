/*********************************************/
/* This is a sample note-save exit. It will  */
/* do very basic markdown editing when       */
/* saving a note.                            */
/*                                           */
/* These markdown tags are supported:        */
/* "---"  :  horizontal line.                */
/* "___"  :  horizontal line.                */
/* "***"  :  horizontal line.                */
/* "__"   :  italics                         */
/* "--"   :  Bold.                           */
/* "**"   :  Bold.                           */
/* "~~"   :  Strikeout.                      */
/* "[]"   :  insert a todo.                  */
/* "[x]"  :  insert a completed todo.        */
/* "[X]"  :  insert a completed todo.        */
/* "+"    :  Unordered list.                 */
/*********************************************/


/* This function converts tags with end tags (i.e. <b> or <i>) */
function markdown1(content, start, end, tag) {

   current = start;
   while (content.indexOf(tag)>0) {
	content = content.replace(tag,current);
        if (current === start) 
		current = end;
	else
		current = start;
   }
   note.log(content);
   return content;
}




/* This function converts tags with no end (i.e. <hr>) */
function markdown2(content, html, tag) {

   while (content.indexOf(tag)>0) {
	content = content.replace(tag,html);
   }
   note.log(content);
   return content;
}



/* This function converts "+" into an unordered list */
function markdownUl(content, tag) {
   var first = true;
   while (content.indexOf(tag)>0) {
      if (first)
         content = content.replace(tag, "<ul><li>");
      else
         content = content.replace(tag, "<li>");
      first = false;
   }
   return content; 
}






var c = note.getContents();

var todoUnchecked = "<input type=\"CHECKBOX\" onmouseover=\"style.cursor='hand'\" onclick=\"if(!checked) removeAttribute('checked'); else setAttribute('checked', 'checked'); editorWindow.editAlert();\" style=\"cursor: pointer;\">";

var todoChecked = "<input checked=\"checked\" type=\"checkbox\" onclick=\"if(!checked) removeAttribute('checked'); else setAttribute('checked', 'checked'); editorWindow.editAlert();\" style=\"cursor: hand;\">";


c = c.replace("[]", todoUnchecked);
c = c.replace("[x]", todoChecked);
c = c.replace("[X]", todoChecked);

c = markdown2(c, "<hr>", "---");
c = markdown2(c, "<hr>", "***");
c = markdown2(c, "<hr>", "___");


c = markdown1(c, "<b>","</b>", "--");
c = markdown1(c, "<b>","</b>", "**");
c = markdown1(c, "<i>","</i>", "__");
c = markdown1(c, "<del>","</del>", "~~");

c = markdownUl(c, "+");

note.setContents(c);
