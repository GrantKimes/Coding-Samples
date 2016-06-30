/* Grant Kimes
 * ECE 567 Database Design
 * Project: Musician Queries
 */

import java.io.*;
import java.sql.*;
import oracle.jdbc.*;
import oracle.jdbc.pool.OracleDataSource;

class MusicianQueries
{
    private static final String USERNAME = "grki567";
    private static final String PASSWORD = "c09680";
    private static final String HOST = "localhost"; // Host
    private static final String PORT = "1521"; // Default port
    private static final String SID = System.getenv("ORACLE_SID"); // Oracle SID
    
    private static Connection conn;
    private static BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

    public static void main (String args[]) throws SQLException
    {
        OracleDataSource ods = new OracleDataSource();
        ods.setUser(USERNAME);
        ods.setPassword(PASSWORD);
        ods.setURL("jdbc:oracle:thin:" + "@" + HOST + ":" + PORT + ":" + SID);

        conn = ods.getConnection();

        System.out.print("\n1. Insert a new Musician\n"
                +"2. Delete an existing song\n"
                +"3. Update a musician's address\n"
                +"4. List albums of a musician\n"
                +"5. List musicians who know an instrument\n"
                +"6. List songs in an album\n"
                +"7. List number of albums with copyright dates in a year\n"
                +"8. List musicians who have produced more than average number of albums\n"
                +"9. List musicians who play more than two instruments\n"
                +"10. List number of songs each musician performed\n");

        while (true) {
            try {
                String input;
                System.out.println("\nChoose an operation number to perform: ");
                input = stdIn.readLine();

                switch(input) {
                    case "1":
                        insertMusician();
                        break;
                    case "2":
                        removeSong();
                        break;
                    case "3":
                        updateAddress();
                        break;
                    case "4":
                        listAlbums();
                        break;
                    case "5":
                        listInstrumentPlayers();
                        break;
                    case "6":
                        listAlbumSongs();
                        break;
                    case "7":
                        listAlbumsCopyrightYear();
                        break;
                    case "8":
                        listMoreThanAvgAlbums();
                        break;
                    case "9":
                        listMultipleInstrumentPlayers();
                        break;
                    case "10":
                        listNumSongsPerformed();
                        break;
                    default:
                        System.out.println("Not a valid input number.");
                        break;
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
    
    private static void insertMusician() {
        try {
            System.out.print("Musician SSN: ");
            String ssn2 = stdIn.readLine();
            int ssn = Integer.parseInt(ssn2);
            System.out.print("Musician First Name: ");
            String fName = stdIn.readLine();
            System.out.print("Musician Last Name: ");
            String lName = stdIn.readLine();
            System.out.print("Musician phone number: ");
            String phoneNum2 = stdIn.readLine();
            long phoneNum = Long.parseLong(phoneNum2);

            CallableStatement query;
            ResultSet cursor;
            query = conn.prepareCall("BEGIN OPEN ? FOR SELECT COUNT(*) FROM Musician; end;");
            query.registerOutParameter(1, OracleTypes.CURSOR);
            query.execute();
            cursor = ((OracleCallableStatement)query).getCursor(1);
            cursor.next();
            int newMusicianNo = cursor.getInt(1) + 1;

            Statement stmt = conn.createStatement();
            String sql = "INSERT INTO Musician VALUES ("+newMusicianNo+","+ssn+",'"+fName+"','"+lName+"',"+phoneNum+")";
            stmt.executeUpdate(sql);
            System.out.println("Sucessfully inserted musician " + fName + " " + lName + ".");
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Failed to insert musician.");
        }
    }
    
    private static void removeSong() {
        try {
            System.out.print("Input name of song to remove: ");
            String songName = stdIn.readLine();
            
            CallableStatement query;
            ResultSet cursor;
            query = conn.prepareCall("BEGIN OPEN ? FOR SELECT title FROM Song WHERE title LIKE '%"+songName+"%'; end;");
            query.registerOutParameter(1, OracleTypes.CURSOR);
            query.execute();
            cursor = ((OracleCallableStatement)query).getCursor(1);
            System.out.print("Songs to be removed: ");
            while (cursor.next ()) {
                System.out.printf("%s, ", cursor.getString(1)); 
            }
            System.out.print("\nOK? (Y/N) ");
            String answer = stdIn.readLine();
            
            if (answer.equalsIgnoreCase("Y")) {
                Statement stmt = conn.createStatement();
                String sql = "DELETE FROM Song WHERE title LIKE '%"+songName+"%'";
                stmt.executeUpdate(sql);
                System.out.println("Songs removed.");
            }
            else {
                System.out.println("Not removing songs.");
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Failed to remove song.");
        }
    }
    
    private static void updateAddress() {
        try {
            System.out.print("Last name of musician to update: ");
            String lName = stdIn.readLine();
            System.out.print("New phone number: ");
            long phoneNum = Long.parseLong(stdIn.readLine());
            System.out.print("New street: ");
            String street = stdIn.readLine();
            System.out.print("New city: ");
            String city = stdIn.readLine();
            System.out.print("New state: ");
            String state = stdIn.readLine();
            System.out.print("New zip code: ");
            int zipCode = Integer.parseInt(stdIn.readLine());
            
            Statement stmt = conn.createStatement();
            String sql = "INSERT INTO Place VALUES ("+phoneNum+", '"+street+"', '"+city+"', '"+state+"', "+zipCode+")";
            stmt.executeUpdate(sql);
            sql = "UPDATE Musician SET phoneNum="+phoneNum+" WHERE lName='"+lName+"'";
            stmt.executeUpdate(sql);
            System.out.println("Updated "+lName+"'s address.");

            
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Failed to update address.");
        }
    }
    
    private static void listAlbums() {
        try {
            System.out.print("Input last name of musician to list albums they produced: ");
            String lName = stdIn.readLine();
            CallableStatement query;
            ResultSet cursor;
            query = conn.prepareCall("BEGIN OPEN ? FOR SELECT albumNo, title, copyrightDate, format FROM Album a, Musician m "
                                     +"WHERE lName='"+lName+"' AND producerNo=musicianNo; end;");
            query.registerOutParameter(1, OracleTypes.CURSOR);
            query.execute();
            cursor = ((OracleCallableStatement)query).getCursor(1);
            System.out.println("Albums by "+lName+": ");
            System.out.println("albumNo       title                 date     format");
            System.out.println("-----   --------------------   ------------   --- ");
            while (cursor.next ()) {
                System.out.printf("%5d   %-20s   %12s   %3s\n", cursor.getInt(1), cursor.getString(2), cursor.getDate(3), cursor.getString(4)); 
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Failed to list albums.");
        }
    }
    
    private static void listInstrumentPlayers() {
        try {
            System.out.print("Instrument: ");
            String instrument = stdIn.readLine();
        
            CallableStatement query;
            ResultSet cursor;
            query = conn.prepareCall("BEGIN OPEN ? FOR SELECT DISTINCT m.musicianNo, m.ssn, m.fName, m.lName, m.phoneNum FROM Musician m, Ability a, Instrument i "
                                     +"WHERE a.instrumentNo=i.instrumentNo AND i.name='"+instrument+"' AND m.musicianNo=a.musicianNo "
                                     +"ORDER BY m.lName; end;");
            query.registerOutParameter(1, OracleTypes.CURSOR);
            query.execute();
            cursor = ((OracleCallableStatement)query).getCursor(1);
            System.out.println("Musicians that play " + instrument + ": ");
            System.out.println("musicianNo  ssn       fName          lName          phoneNum");
            System.out.println("-----   --------   ------------   ------------    ------------");
            while (cursor.next ()) {
                System.out.printf("%5d   %8d   %-12s   %-12s   %d12 \n", cursor.getInt(1), cursor.getInt(2), cursor.getString(3), cursor.getString(4), cursor.getLong(5)); 
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Failed to list players of given instrument.");
        }
    }
    
    private static void listAlbumSongs() {
        try {
            System.out.print("Input album name to list songs for: ");
            String albumName = stdIn.readLine();
            CallableStatement query;
            ResultSet cursor;
            query = conn.prepareCall("BEGIN OPEN ? FOR SELECT s.title, s.author, s.songNo FROM Song s, Album a "
                                     +"WHERE s.albumNo=a.albumNo AND a.title='"+albumName+"'; end;");
            query.registerOutParameter(1, OracleTypes.CURSOR);
            query.execute();
            cursor = ((OracleCallableStatement)query).getCursor(1);
            System.out.println("Songs on album " + albumName + ": ");
            System.out.println("       title                author      songNo");
            System.out.println("--------------------   ---------------   ---");
            while (cursor.next ()) {
                System.out.printf("%20s   %15s   %3d \n", cursor.getString(1), cursor.getString(2), cursor.getInt(3) ); 
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Failed to list songs on album.");
        }
    }
    
    private static void listAlbumsCopyrightYear() {
        try {
            System.out.print("Input year to list number of albums from: ");
            int year = Integer.parseInt(stdIn.readLine());
            
            CallableStatement query;
            ResultSet cursor;
            query = conn.prepareCall("BEGIN OPEN ? FOR SELECT COUNT(*) FROM Album "
                                     +"WHERE EXTRACT(YEAR FROM copyrightDate)="+year+"; end;");
            query.registerOutParameter(1, OracleTypes.CURSOR);
            query.execute();
            cursor = ((OracleCallableStatement)query).getCursor(1);
            cursor.next();
            System.out.println(cursor.getInt(1) + " albums in " + year + ".");
            
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Failed to list albums copyrighted in a year.");
        }
    }
    
    private static void listMoreThanAvgAlbums() {
        try {
            CallableStatement query;
            ResultSet cursor;
            query = conn.prepareCall("BEGIN OPEN ? FOR SELECT fName, lName, COUNT(producerNo) AS numAlbums FROM Musician m, Album a "
                                     +"WHERE m.musicianNo=a.producerNo GROUP BY lName, fName "
                                     +"HAVING COUNT(producerNo)>(SELECT CAST(COUNT(albumNo) AS DECIMAL) / COUNT(DISTINCT producerNo) FROM Album); end;");
            query.registerOutParameter(1, OracleTypes.CURSOR);
            query.execute();
            cursor = ((OracleCallableStatement)query).getCursor(1);
            System.out.println("Musicians who produced more than average number of albums: ");
            System.out.println("       fName                  lName          numAlbums");
            System.out.println("--------------------   --------------------   ---");
            while (cursor.next ()) {
                System.out.printf("%20s   %-20s   %3d \n", cursor.getString(1), cursor.getString(2), cursor.getInt(3) ); 
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Failed to list players who produced more than average number of albums.");
        }
    }
    
    private static void listMultipleInstrumentPlayers() {
        try {
            CallableStatement query;
            ResultSet cursor;
            query = conn.prepareCall("BEGIN OPEN ? FOR SELECT fName, lName, COUNT(instrumentNo) AS numInstruments FROM Musician m "
                                     +"LEFT JOIN Ability a ON m.musicianNo=a.musicianNo GROUP BY lName, fName HAVING COUNT(instrumentNo)>2; end;");
            query.registerOutParameter(1, OracleTypes.CURSOR);
            query.execute();
            cursor = ((OracleCallableStatement)query).getCursor(1);
            System.out.println("Musicians who play more than two instruments: ");
            System.out.println("       fName                  lName          numInstruments");
            System.out.println("--------------------   --------------------   ---");
            while (cursor.next ()) {
                System.out.printf("%20s   %-20s   %3d \n", cursor.getString(1), cursor.getString(2), cursor.getInt(3) ); 
            }

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Failed to list multiple instrument players.");
        }
    }
    
    private static void listNumSongsPerformed() {
        try {
            CallableStatement query;
            ResultSet cursor;
            query = conn.prepareCall("BEGIN OPEN ? FOR SELECT fName, lName, COUNT(songNo) AS numSongs FROM Musician m "
                                     +"LEFT JOIN Performance p ON m.musicianNo=p.musicianNo GROUP BY fName, lName; end;");
            query.registerOutParameter(1, OracleTypes.CURSOR);
            query.execute();
            cursor = ((OracleCallableStatement)query).getCursor(1);
            System.out.println("Number of songs each musician performs on: ");
            System.out.println("       fName                  lName           numSongs");
            System.out.println("--------------------   --------------------   ---");
            while (cursor.next ()) {
                System.out.printf("%20s   %-20s   %3d \n", cursor.getString(1), cursor.getString(2), cursor.getInt(3) ); 
            }

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Failed in listing number of songs of each musician.");
        }
    }
    
}
