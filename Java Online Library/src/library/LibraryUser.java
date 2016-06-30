package library;
import java.io.Serializable;
import java.util.Vector;

public class LibraryUser implements Serializable {
	int userID;
	String username;
	String password;
	boolean loggedIn;
	Vector<LibraryBook> Books;
	
	public LibraryUser(int ID, String u, String p)
	{
		userID = ID;
		username = u;
		password = p;
		loggedIn = false;
		Books = new Vector<LibraryBook>();
	}
	
	synchronized void checkoutBook(LibraryBook L)
	{
		Books.addElement(L);
	}
	
	synchronized void returnBook(LibraryBook L) 
	{
		Books.removeElement(L);
	}
	
	synchronized void returnBook(int bookID)
	{
		for (LibraryBook L : Books) {
			if (bookID == L.ID)
				returnBook(L);
		}
	}
	
	synchronized boolean bookCheckedOut(LibraryBook L)
	{
		for (LibraryBook B : Books)
			if (L.equals(B))
				return true;
		return false;
	}
	
	synchronized boolean bookCheckedOut(int bookID)
	{
		for (LibraryBook B : Books)
			if (bookID == B.ID)
				return true;
		return false;
	}
	
	synchronized LibraryBook getBook(int bookID)
	{
		for (LibraryBook B : Books)
			if (bookID == B.ID)
				return B;
		return null;
	}
}
