<?php

$keyword = $_GET['q'];

$hostname = "jumbo.db.elephantsql.com";
$username = "mwkekjwc";
$password = "emObqiQWYsLy9Sr3EiCUbGdVnuVueI47";
$dbname = "mwkekjwc";
$dbConn = pg_connect("host=$hostname dbname=$dbname user=$username password=$password") or die ("Could not connect to server\n");

$query = "SELECT bookNo, title, author, publishYear, bookLanguage FROM Book "
    . "WHERE UPPER(title) LIKE UPPER('%$keyword%') "
    . "OR UPPER(author) LIKE UPPER('%$keyword%') "
    . "ORDER BY title;";
$results = pg_query($dbConn, $query) or die('Query failed: ' . pg_last_error());

echo "<thead><tr> <th>Title</th> <th>Author</th> <th>Year</th> <th>Language</th> </tr> </thead> ";
echo "<tbody>";
while ($row = pg_fetch_row($results)) {
    echo "<tr>";
    echo "<td> $row[1] </td>";
    echo "<td> $row[2] </td>";
    echo "<td> $row[3] </td>";
    echo "<td> $row[4] </td>";
    echo "<td> <button onclick='checkout($row[0]);' class='btn btn-primary'>Checkout</button></td>";
    echo "</tr>";
}
    
echo "</tbody>";
pg_close($dbConn);

?>



<?php
/*
try {
    $dbConn = new PDO("postgres:host=$hostname;dbname=$dbname", $username, $password);
    // set the PDO error mode to exception
    $dbConn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    echo "Connected successfully"; 
    }
catch(PDOException $e)
    {
    echo "Connection failed: " . $e->getMessage();
    }
*/

//$dbConn = new mysqli($servername, $username, $password);
/*if ($dbConn->connect_error) {
    echo "Connection failed";
}
else
    echo "Connection successful";*/








/*
                 <tr>
                     <th>Title</th>
                     <th>Author</th>
                     <th>Year</th>
                     <th>Language</th>
                 </tr>
             </thead><tbody><tr>
                     <td>Book Name</td>
                     <td>Author Guy</td>
                     <td>1999</td>
                     <td>Eng.</td>
                     <td><button class="btn btn-primary">Checkout</button></td>
                 </tr><tr>
                     <td>After PHP Books</td>
                     <td>Author Guy</td>
                     <td>1999</td>
                     <td>Eng.</td>
                     <td><button class="btn btn-primary">Checkout</button></td>
                 </tr>
             </tbody>"
             */
?>