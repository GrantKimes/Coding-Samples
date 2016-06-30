<?php 
    // Recieve username and password from POST. Check database if valid login. If so, set session variables and continue to search page. 

    session_start();
    if (isset( $_SESSION['userID'])) {
        session_unset();
        $message =  "User " . $_SESSION['username'] . " is already logged in";
    }
    if ($_POST['username'] == "" || $_POST['password'] == "" ) {
        $_SESSION['error'] = "Input a username and password.";
        header('Location:login.php');
        $message =  "No username or password inputted";
    }
    else {            
        $dbHostname = "jumbo.db.elephantsql.com";
        $dbUsername = "mwkekjwc";
        $dbPassword = "emObqiQWYsLy9Sr3EiCUbGdVnuVueI47";
        $dbName = "mwkekjwc";
        $dbConn = pg_connect("host=$dbHostname dbname=$dbName user=$dbUsername password=$dbPassword") or die ("Could not connect to server\n");

        $query = "SELECT userNo, username, password FROM LibraryUser "
            . "WHERE username='" . $_POST['username'] . "' "
            . "AND password='" . $_POST['password'] . "'; ";
        $result = pg_query($dbConn, $query) or die('Query failed: ' . pg_last_error());
        $row = pg_fetch_row($result);
        if ($row) {
            $_SESSION['userID'] = $row[0];
            $_SESSION['username'] = $row[1];
            header('Location:search.php');
            $message =  "Successful login for " . $_SESSION['username'] . ", ID=" . $_SESSION['userID'];
        }
        else {
            $_SESSION['error'] = "Incorrect username and password. (Try user: Grakim, pass: 12345)";
            header('Location:login.php');
            $message = "Invalid username and password for " . $_POST['username'];
        }
        
        pg_close($dbConn);
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
