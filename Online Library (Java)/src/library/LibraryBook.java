package library;

import java.io.Serializable;

public class LibraryBook implements Serializable {
	String title, author, language, yearPublished;
	int ID;
	//private static final long serialVersionUID = 1L;
	
	public LibraryBook(String t, String a, String l, String y, int i) {
		title = t;
		author = a;
		language = l;
		yearPublished = y;
		ID = i;
	}
	
	void print()
	{
		System.out.println(returnPrintableString());
	}
	
	String returnPrintableString() 
	{
		return title + ", by " + author + " - " + yearPublished + "(" + language.substring(0, 3) + ") <ID# " + ID + ">";
	}
	
	String returnFormattedString()
	{
		return ID + "@" + title + "@" + author + "@" + language + "@" + yearPublished;
	}
}
