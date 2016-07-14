package library;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.ObjectInput;
import java.io.ObjectInputStream;
import java.io.PrintWriter;
import java.net.Socket;

public class LibraryClient {
	
	LibraryUser user;
	Socket S;
	BufferedReader stdIn;
	PrintWriter toServer;
	BufferedReader fromServer;
	ObjectInput objectFromServer;
	
	
	public LibraryClient()
	{
		user = null;
	}

	public static void main(String[] args) {
		LibraryClient LC = new LibraryClient();
		String hostname = "localhost";
		int port = 50000; 
		try {
			LC.S = new Socket(hostname, port);
			System.out.println("Connected to " + hostname + " on port " + port);
			
			LC.setStreams();
			LC.talkToServer();
						
		} catch (Exception e) {
			System.out.println("Error: " + e.getMessage());
		}
	}
	
	private void sendMessage(String msg)
	{
		toServer.print(String.format("%04d", msg.length()) + ":" + msg);
		toServer.flush();
	}
	
	private String recvMessage()
	{
		try {
			char[] len = new char[5];
			fromServer.read(len, 0, 5);
			int msgLen = Integer.parseInt(new String(len, 0, 4));
			char[] data = new char[msgLen];
			fromServer.read(data, 0, msgLen);
			return new String(data);
		} catch (IOException e) {
			e.printStackTrace();
			return null;
		}
	}
	
	void setStreams() throws IOException
	{
		stdIn = new BufferedReader(new InputStreamReader(System.in));
		toServer = new PrintWriter(S.getOutputStream(), true);
		fromServer = new BufferedReader(new InputStreamReader(S.getInputStream()));
		objectFromServer = new ObjectInputStream(S.getInputStream());
	}
	
	void talkToServer() 
	{
		try {
			String input;
			while(true) 
			{	System.out.println("\nInput an operation: Login(1), Logout(2), Search(3), Borrow(4), List(5), Return(6)");
				if ((input = stdIn.readLine()) == null)
					break;
				if (! input.matches("\\d+$")) {	// Check if valid int input.
					System.out.println("Not a valid operation choice.");
					continue;
				}

				switch (Integer.parseInt(input)) {
					case 1:
						login();
						break;
					case 2:
						logout();
						break;
					case 3:
						search();
						break;
					case 4:
						borrow();
						break;
					case 5:
						list();
						break;
					case 6:
						returnBook();
						break;
					default:
						System.out.println("Invalid command.");
						break;
				}
			}
			
			toServer.close();
			fromServer.close();
			stdIn.close();
			S.close();
		} catch(Exception e) {
			System.out.println("Error: " + e.getMessage());
		}

	}
	
	void login() throws IOException, ClassNotFoundException 
	{
		System.out.println("LOGIN");
		if (user == null) {
			System.out.println("Enter username: ");
			String U = stdIn.readLine();
			System.out.println("Enter password:");
			String P = stdIn.readLine();
			sendMessage("LOGIN:" + U + ":" + P);
			String result = recvMessage();
			
			if (result.equals("ALREADY LOGGED IN"))
				System.out.println(U + " logged in somewhere else.");
			if (result.equals("SUCCESSFUL LOGIN"))	{
				user = (LibraryUser) objectFromServer.readObject();
				System.out.println(user.username + " now logged in.");
			}
			
			if (result.equals("WRONG PASSWORD"))
				System.out.println("Wrong password for " + U);
			
			if (result.equals("NO USER"))
				System.out.println("User " + U + " not found.");
			
			/*if (result.equals("NEW USER")) {
			System.out.print(U + " is not an existing user, would you like to register? (Y/N) ");
			if (stdIn.readLine().toLowerCase().equals("Y")) {
				sendMessage("NEW USER");
				user = new LibraryUser(U, P);
			}*/
			
			
			
		} else {
			System.out.println("User currently logged in: " + user.username);
		}
	}
	
	void logout()
	{
		System.out.println("LOGOUT");
		if (user == null)
			System.out.println("No user logged in.");
		else {
			sendMessage("LOGOUT:" + user.userID);
			System.out.println("User " + user.username + " logged out.");
			user = null;
		}
	}
	
	void search() throws IOException
	{
		System.out.println("SEARCH");
		System.out.println("Input keyword to search for: ");
		String A = stdIn.readLine();
		sendMessage("SEARCH:" + A);
		
		String result = recvMessage();
		String[] parts = result.split(":");
		if (parts[0].equals("RESULTS")) {
			System.out.println("SEARCH RESULTS");
			int numResults = Integer.parseInt(parts[1]);
			if (numResults == 0)
				System.out.println("No results found.");
			for (int i = 0; i < numResults; i++) {
				result = recvMessage();
				System.out.println(result);
			}
		}
	}
	
	void borrow() throws IOException
	{
		if (user == null) {
			System.out.println("Must be logged in to borrow a book.");
			return;
		}
		System.out.println("BORROW");
		System.out.println("Input ID of book to borrow: ");
		String bookID = stdIn.readLine();
		sendMessage("BORROW:" + user.userID + ":" + bookID);
		
		String[] response = recvMessage().split(":");
		
		if (response[0].equals("BORROW SUCCESSFUL")) {
			LibraryBook currBook;
			try {
				currBook = (LibraryBook) objectFromServer.readObject();
				user.checkoutBook(currBook);
				System.out.println("Checked out " + currBook.returnPrintableString());
			} catch (ClassNotFoundException e) {
				e.printStackTrace();
			}
		} else if (response[0].equals("ERROR")) {
			System.out.println(response[1]);
		}
	}
	
	void list() 
	{
		if (user == null) {
			System.out.println("Must be logged in to list borrowed books.");
			return;
		}
		System.out.println("LIST");
		System.out.println("Books currently borrowed by " + user.username + ":");
		for (LibraryBook B : user.Books) {
			System.out.println(B.returnPrintableString());
		}
	}
	
	void returnBook()
	{
		if (user == null) {
			System.out.println("Must be logged in to return a book.");
			return;
		}
		list();
		System.out.println("RETURN");
		System.out.println("Enter ID of book to return: ");
		int bookID;
		try {
			bookID = Integer.parseInt(stdIn.readLine());
		} catch (NumberFormatException | IOException e) {
			e.printStackTrace();
			return;
		}
		
		sendMessage("RETURNBOOK:" + user.userID + ":" + bookID);
		String result = recvMessage();
		if (result.equals("RETURN ERROR"))
			System.out.println("Cannot return that book, it's not currently checked out.");
		if (result.equals("RETURN SUCCESSFUL")) {
			LibraryBook L = user.getBook(bookID);
			user.returnBook(L);
			System.out.println("Returned " + L.returnPrintableString());
		}
	}
}
