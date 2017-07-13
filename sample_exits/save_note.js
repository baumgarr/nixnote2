/* 
   This is a sample exit. It is called when a note's contents 
   are saved. It just looks for a note that starts 
   with a day of the week, followed by month, day & year. For 
   example, "Monday, January 2, 2017". If this string is 
   found at the beginning of a note it is assigned to the 
   "Journal" notebook. 
*/
var tags = exit.getTags();
var dateCreated = new Date(exit.getCreatedDate("yyyy-MM-dd"));

var contents = exit.getContentsPlainText().split(" ");
var dow = contents[0];
if (dow === "Monday," || dow === "Tuesday," || dow === "Wednesday" || dow === "Thursday," ||
    dow === "Friday," || dow === "Saturday," || dow === "Sunday,") {
    var month = contents[1];
    var day = contents[2];
    var year = contents[3];
    day = day.substring(0,day.length-1);
    var newDate = new Date(month + " " +day + " " + year);
    exit.setNotebook("Journal");
}
