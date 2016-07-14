<?php 
    session_start();
    if (! isset($_SESSION['userID'])) {
        $_SESSION['error'] = "You have to be logged in to view books.";
        header('Location:login.php');
    }
    else {
        $username = $_SESSION['username'];
        $userID = $_SESSION['userID'];
    }

    $dbHostname = "jumbo.db.elephantsql.com";
    $dbUsername = "mwkekjwc";
    $dbPassword = "emObqiQWYsLy9Sr3EiCUbGdVnuVueI47";
    $dbName = "mwkekjwc";
    $conn = pg_connect("host=$dbHostname dbname=$dbName user=$dbUsername password=$dbPassword") or die ("Could not connect to server\n");


?>

<html>
<?php require 'header.php'; ?>
    
<body>
    <nav class="navbar navbar-default">
      <div class="container-fluid">
          <ul class="nav navbar-nav">
            <li><a href="search.php">Search</a></li>
            <li class="active"><a href="view.php">View</a></li>
            <li><a href="logout_submit.php">Logout</a></li>
          </ul>
      </div>
    </nav>

    
    <div class="view">
        <h1>Viewing Books Checked Out By <b><?php echo "$username"; ?></b></h1>
        
        <hr>
        
        <?php 
            if (isset($_SESSION['success'])) {
                echo '<div class="alert alert-success">' . $_SESSION['success'] . '</div>';
                unset($_SESSION['success']);
            }
            if (isset($_SESSION['error'])) {
                echo '<div class="alert alert-danger">' . $_SESSION['error'] . '</div>';
                unset($_SESSION['error']);
            }
        ?>
        
        <table class="table">
            <thead>
                <tr>
                    <th>Title</th>
                    <th>Author</th>
                    <th>Year</th>
                    <th>Language</th>
                </tr>
            </thead>
            <tbody>
                <?php
                    $query = "SELECT b.bookNo, title, author, publishYear, bookLanguage "
                        . "FROM Book b, Checkout c "
                        . "WHERE c.userNo=$userID "
                        . "AND b.bookNo=c.bookNo "
                        . "ORDER BY title;";
                    $results = pg_query($conn, $query) or die('Query failed: ' . pg_last_error());
                    while ($row = pg_fetch_row($results)) {
                        echo "<tr>";
                        echo "<td> $row[1] </td>";
                        echo "<td> $row[2] </td>";
                        echo "<td> $row[3] </td>";
                        echo "<td> $row[4] </td>";
                        echo "<td> <button onclick='returnBook($row[0]);' class='btn btn-primary'>Return</button></td>";
                    }
                    pg_close($conn);
                 ?>
            </tbody>
         </table>
    </div>
    
<script>
function returnBook(bookID) {
    location = 'return_submit.php?bookID=' + bookID;
}    
</script>

</body>
</html>