<?php 
    // Recieve book number from GET. Attempt to return that book for current session user. 
    
    session_start();
    if (! isset( $_SESSION['userID'])) {
        $message =  "No user is logged in";
        header('Location:login.php');
    }

    $bookID = $_GET['bookID'];
    $userID = $_SESSION['userID'];

    $dbHostname = "jumbo.db.elephantsql.com";
    $dbUsername = "mwkekjwc";
    $dbPassword = "emObqiQWYsLy9Sr3EiCUbGdVnuVueI47";
    $dbName = "mwkekjwc";
    $dbConn = pg_connect("host=$dbHostname dbname=$dbName user=$dbUsername password=$dbPassword") or die ("Could not connect to server\n");

    $query = "DELETE FROM Checkout "
        . "WHERE bookNo=$bookID "
        . "AND userNo=$userID;";
    pg_query($dbConn, $query) or die('Query failed: ' . pg_last_error());

    $query = "SELECT title FROM Book WHERE bookNo=$bookID";
    $result = pg_query($dbConn, $query) or die('Query failed: ' . pg_last_error());
    $row = pg_fetch_row($result);
    $bookName = $row[0];

    $_SESSION['success'] = "Successfully returned $bookName.";
    header('Location:view.php');
    $message = "Returned book $bookID for user $userID";
    // Redirect to view page, indicate that book was returned. 
    
    pg_close($dbConn);

?>

<html>
<head>
    <title>Return Submit</title>
</head>
<body>
    <p><?php echo $message; ?>
</body>
</html>
