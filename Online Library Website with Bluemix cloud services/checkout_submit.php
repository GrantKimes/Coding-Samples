<?php 
    // Recieve book number from GET. Attempt to checkout that book for current session user. 
    
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

    $query = "SELECT title FROM Book WHERE bookNo=$bookID";
    $result = pg_query($dbConn, $query) or die('Query failed: ' . pg_last_error());
    $row = pg_fetch_row($result);
    $bookName = $row[0];

    if (alreadyCheckedOut($bookID, $userID, $dbConn)) {
        $_SESSION['error'] = "You have already checked out $bookName.";
        header('Location:view.php');
        $message = "Already checked out book $bookID.";
    }
    else if (tooManyBooksCheckedOut($userID, $dbConn)) {
        $_SESSION['error'] = "You already have five books checked out.";
        header('Location:view.php');
        $message = "You have already checked out 5 books, return one.";
    }
    else {
        $query = "INSERT INTO Checkout VALUES ($userID, $bookID);";
        pg_query($dbConn, $query) or die('Query failed: ' . pg_last_error());
        $_SESSION['success'] = "Successfully checked out $bookName.";
        header('Location:view.php');
        $message = "Checked out book $bookID for user $userID";
    }

    pg_close($dbConn);

function alreadyCheckedOut($bookID, $userID, $dbConn) {
    $query = "SELECT bookNo FROM Checkout "
        . "WHERE userNo=$userID "
        . "AND bookNo=$bookID; ";
    $result = pg_query($dbConn, $query) or die('Query failed: ' . pg_last_error());
    $row = pg_fetch_row($result);
    if ($row) 
        return true;
    else
        return false;
}

function tooManyBooksCheckedOut($userID, $dbConn) {
    $query = "SELECT COUNT(*) FROM Checkout "
        . "WHERE userNo=$userID; ";
    $result = pg_query($dbConn, $query) or die('Query failed: ' . pg_last_error());
    
    $row = pg_fetch_row($result);
    $count = $row[0];
    
    if ($count >= 5) 
        return true;
    else
        return false;
}

?>

<html>
<head>
    <title>Login Submit</title>
</head>
<body>
    <p><?php echo $message; ?>
</body>
</html>
