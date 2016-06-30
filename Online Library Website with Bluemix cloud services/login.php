<?php
    session_start();
    if (isset($_SESSION['userID'])) 
        unset($_SESSION['userID']);
?>

<html>
<?php require 'header.php'; ?>

<body>
    <div class="login-box">
        <h1>Online Library Login</h1>
        <form role="form" action="login_submit.php" method="post">
            <div class="form-group">
                <label for="username">Username</label>
                <input name="username" type="text" class="form-control" placeholder="Ex: Grakim">
            </div>
            <div class="form-group">
                <label for="password">Password</label>
                <input name="password" type="password" class="form-control" placeholder="Ex: 12345">
            </div>
            <button type="submit" class="btn btn-primary">Login</button>
        </form>
        
        
        <?php 
            if (isset($_SESSION['error'])) {
                echo '<div class="alert alert-danger">' . $_SESSION['error'] . '</div>';
                unset($_SESSION['error']);
            }
        ?>
    </div>
</body>
    
</html>