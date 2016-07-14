<?php 
    // Check if user is logged in. If so, unset session variables and return to login. 

    session_start();
    if (isset( $_SESSION['userID'])) {
        session_unset();
        header('Location:login.php');
        $message =  "User " . $_SESSION['username'] . " is already logged in";
    }
    else {
        header('Location:login.php');
        $message = "No user logged in.";
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
