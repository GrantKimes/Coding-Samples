package library;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.ObjectOutput;
import java.io.ObjectOutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketException;
import java.util.Vector;

public class LibraryAppServer extends AppServer {
	// Runs the library application interface as a distinct thread.

	private Socket clientSocket;
	private PrintWriter toClient;
	private BufferedReader fromClient;
	private ObjectOutput objectToClient;
	private LibraryUser clientUser;
	
	public LibraryAppServer(Socket cs) 
	{
		try {
			clientSocket = cs;
			toClient = new PrintWriter(clientSocket.getOutputStream(), true);
			fromClient = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
			objectToClient = new ObjectOutputStream(clientSocket.getOutputStream());
			clientUser = null;
		} catch (IOException e) {
			e.printStackTrace();
			System.out.println("Error accepting connection from socket " + clientSocket);
		}
	}
	
	public void run()
	{
		System.out.println("Server connected to client");
		String message;
		try {
			while ((message = recvMessage()) != null) {	// Loop to continually process input from client.
				System.out.println("Received from client: " + message);
				processInput(message);
			}
		} catch (SocketException s) {
			System.out.println("Client terminated connection");
			if (clientUser != null) {
				clientUser.loggedIn = false;
				clientUser = null;
			}
		} catch (IOException e) {
			System.out.println("IOException");
			e.printStackTrace();
		} 
	}
	
	private void sendMessage(String msg)
	{
		toClient.print(String.format("%04d", msg.length()) + ":" + msg);
		toClient.flush();
	}
	
	private String recvMessage() throws IOException
	{
		char[] len = new char[5];
		fromClient.read(len, 0, 5);
		int msgLen = Integer.parseInt(new String(len, 0, 4));
		char[] data = new char[msgLen];
		fromClient.read(data, 0, msgLen);
		return new String(data);
	}
	
	void processInput(String message) 
	{
		String[] messageParts = message.split(":");
		switch(messageParts[0]) 
		{
			case "LOGIN":
				login(messageParts);
				break;
			case "LOGOUT":
				logout(messageParts);
				break;
			case "SEARCH":
				search(messageParts);
				break;
			case "BORROW":
				borrow(messageParts);
				break;
			case "LISTB":
				break;
			case "RETURNBOOK":
				returnBook(messageParts);
				break;
			default:
				System.out.println("Command " + messageParts[0] + " not found.");
				break;
		}
	}
	
	void login(String[] inputs)
	{
		String username = inputs[1];
		String password = inputs[2];
		
		synchronized(LibraryServer.Users) {
			for (LibraryUser U : LibraryServer.Users) 
			{
				System.out.println("Looping through users, now " + U.username);
				if (username.equals(U.username) && password.equals(U.password)) {
					if (U.loggedIn)
						sendMessage("ALREADY LOGGED IN");
					else {
						clientUser = U;
						clientUser.loggedIn = true;
						sendMessage("SUCCESSFUL LOGIN");
						try {
							objectToClient.writeObject(clientUser);
						} catch (IOException e) {
							e.printStackTrace(); } 
					}
					return; 
				}
				if (username.equals(U.username) && ! password.equals(U.password)) {
					sendMessage("WRONG PASSWORD");
					return;
				}
			}
			sendMessage("NO USER");
		}
		
	}
	
	void logout(String[] inputs)
	{
		int ID = Integer.parseInt(inputs[1]);
		synchronized(LibraryServer.Users) {
			if (ID == clientUser.userID) {
				clientUser.loggedIn = false;
				clientUser = null;
			}
		}
	}
	
	void search(String[] inputs)
	{
		// Find matching books on book database.
		String searchData = inputs[1].toLowerCase();
		System.out.println("Searching for " + searchData);
		Vector<LibraryBook> searchResults = new Vector<LibraryBook>();
		
		// Iterate through book list and look for any keyword matches.
		synchronized(LibraryServer.Books) {
			for (LibraryBook book : LibraryServer.Books) { 
				if (book.author.toLowerCase().contains(searchData)
					|| book.title.toLowerCase().contains(searchData)) {
					searchResults.addElement(book);
				}
			}
		}
		
		// Send books back as results of the search.
		sendMessage("RESULTS:" + searchResults.size());
		for (LibraryBook book : searchResults) {
			String line = book.returnPrintableString();
			sendMessage(line);
		}
		
	}
	
	void borrow(String[] inputs)
	{
		if (! inputs[2].matches("\\d+$")) {	// Check if valid int input.
			sendMessage("ERROR:Not a valid book ID to borrow.");
			return;
		}
		int userID = Integer.parseInt(inputs[1]);
		LibraryUser currUser = LibraryServer.Users.get(userID);
		int bookID = Integer.parseInt(inputs[2]);
		
		synchronized(LibraryServer.Books) {
			if (bookID >= LibraryServer.Books.size())
				sendMessage("ERROR:No book with that ID.");
			else if (currUser.bookCheckedOut(bookID))
				sendMessage("ERROR:Book already checked out by user.");
			else if (currUser.Books.size() >= 5)
				sendMessage("ERROR:Five books already checked out by user.");
			else {
				LibraryBook L = LibraryServer.Books.get(bookID);
				sendMessage("BORROW SUCCESSFUL");
				currUser.checkoutBook(L);
				try {
					objectToClient.writeObject(L);
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}
	
	void returnBook(String[] inputs) 
	{
		int userID = Integer.parseInt(inputs[1]);
		int bookID = Integer.parseInt(inputs[2]);
		LibraryUser user = LibraryServer.Users.get(userID);
		if (user.bookCheckedOut(bookID) == false || bookID >= LibraryServer.Books.size()) {
			sendMessage("RETURN ERROR");
			return;
		}
		LibraryBook book;
		synchronized(LibraryServer.Books) {
			book = LibraryServer.Books.get(bookID);
		}
		user.returnBook(book);
		sendMessage("RETURN SUCCESSFUL");
	}
}
