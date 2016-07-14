package library;
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Vector;

public class LibraryServer { 
	// Accepts connections from client, then runs each in a new thread.
	// Shared, synchronized data should be library books, users. 

	protected static Vector<LibraryBook> Books = new Vector<LibraryBook>();
	protected static Vector<LibraryUser> Users = new Vector<LibraryUser>();
	
	public LibraryServer()
	{
	}
	
	public static void main(String[] args)
	{
		try {
			initializeBooks();
			initializeUsers();
	
			int portNum = 50000;
			System.out.println("Server started, listening on port " + portNum + ".");
			ServerSocket servSocket = new ServerSocket(portNum);
			
			while(true) {
				Socket clientSocket = servSocket.accept();
				if (clientSocket == null) break;
				LibraryAppServer libApp = new LibraryAppServer(clientSocket);
				
				//libApp.run();	// Iterative, comment out for concurrent below.
				
				Thread t = new Thread(libApp);
				t.start();
			}
			servSocket.close();
			System.out.println("Error accepting connection"); 
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	private static void initializeBooks() 
	{
		File f = new File("Books.txt");
		try {
			BufferedReader fin = new BufferedReader(new FileReader(f));
			String title;
			while ((title = fin.readLine()) != null) {	// Read 4 lines at a time.
				String author = fin.readLine();
				String language = fin.readLine();
				String year = fin.readLine();
				LibraryBook L = new LibraryBook(title, author, language, year, Books.size());
				Books.addElement(L);
			}
			fin.close();
			
		} catch (IOException e) {
			e.printStackTrace();
		} 
	}
	
	private static void initializeUsers() 
	{
		addUser("0", "0"); // Dummy so that userID is index in Users vector.
		addUser("Grakim", "password");
		addUser("Bob", "password");
		addUser("Greg", "password");
	}
	
	private static void addUser(String username, String password) {
		synchronized(Users) {
			int ID = Users.size();
			Users.addElement(new LibraryUser(ID, username, password));
		}
	}
	
}

	
	/*
	private void sendMessage(String msg)
	{
		toClient.print(String.format("%04d", msg.length()) + ":" + msg);
		toClient.flush();
	}
	
	private String recvMessage()
	{
		try {
			char[] len = new char[5];
			fromClient.read(len, 0, 5);
			int msgLen = Integer.parseInt(new String(len, 0, 4));
			char[] data = new char[msgLen];
			fromClient.read(data, 0, msgLen);
			return new String(data);
		} catch (IOException e) {
			e.printStackTrace();
			return null;
		}
	}

	
	void setStreams(ServerSocket servSocket) throws IOException
	{
		socketToClient = servSocket.accept();
		toClient = new PrintWriter(socketToClient.getOutputStream(), true);
		fromClient = new BufferedReader(new InputStreamReader(socketToClient.getInputStream()));
	}
	
	void talkToClient() throws IOException
	{
		System.out.println("Server connected to client");
		String message;
		//while ((message = fromClient.readLine()) != null) {	// Loop to continually process input from client.
		while ((message = recvMessage()) != null) {	// Loop to continually process input from client.
			System.out.println("Received from client: " + message);
			processInput(message);
		}
		socketToClient.close();
	}
	
	void processInput(String message)
	{
		String[] messageParts = message.split(":");
		switch(messageParts[0]) 
		{
			case "LOGIN":
				break;
			case "LGOUT":
				break;
			case "SEARC":
				search(messageParts);
				break;
			case "BOROW":
				borrow(messageParts);
				break;
			case "LISTB":
				break;
			case "RETRN":
				break;
			default:
				break;
		}
	}
	
	void search(String[] inputs)
	{
		//String searchTerm = inputs[1];
		String searchData = inputs[1].toLowerCase();
		System.out.println("Searching for " + searchData);
		Vector<LibraryBook> searchResults = new Vector<LibraryBook>();
		
		for (LibraryBook book : Books) { 	// Search through book list for author or title match.
			if (book.author.toLowerCase().contains(searchData)
				|| book.title.toLowerCase().contains(searchData)) {
				searchResults.addElement(book);
			}
		}
		
		//toClient.println("SEARR:" + searchResults.size());
		sendMessage("SEARR:" + searchResults.size());
		for (LibraryBook book : searchResults) {
			String line = book.returnPrintableString();
			//toClient.println(line);
			sendMessage(line);
		}
		
	}
	
	void borrow(String[] inputs)
	{
		if (! inputs[2].matches("\\d+$")) {	// Check if valid int input.
			//toClient.println("ERROR:" + "Not a valid book ID to borrow.");
			sendMessage("ERROR:Not a valid book ID to borrow.");
			return;
		}
		int bookID = Integer.parseInt(inputs[2]);
		System.out.println(bookID);
		//toClient.println("BOROR:" + Books.get(bookID).returnFormattedString());
		sendMessage("BOROR:" + Books.get(bookID).returnFormattedString());
	}
	
	void initializeBooks() 
	{
		File f = new File("Books.txt");
		try {
			BufferedReader fin = new BufferedReader(new FileReader(f));
			String title;
			while ((title = fin.readLine()) != null) {	// Read 4 lines at a time.
				String author = fin.readLine();
				String language = fin.readLine();
				String year = fin.readLine();
				LibraryBook L = new LibraryBook(title, author, language, year, Books.size());
				Books.addElement(L);
			}
			fin.close();
			
		} catch (IOException e) {
			e.printStackTrace();
		} 
	}
	*/
