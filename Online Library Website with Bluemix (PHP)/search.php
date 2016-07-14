<?php 
    // Check if user is logged in. If so, allow for book search. Results display is done with AJAX. 

    session_start();
    if (! isset($_SESSION['userID'])) {
        $_SESSION['error'] = "You have to be logged in to search books.";
        header('Location:login.php');
    }
    else {
        $username = $_SESSION['username'];
        $userID = $_SESSION['userID'];
    }
?>

<html>
<?php require 'header.php'; ?>

<body>
    <nav class="navbar navbar-default">
      <div class="container-fluid">
          <ul class="nav navbar-nav">
            <li class="active"><a href="search.php">Search</a></li>
            <li><a href="view.php">View</a></li>
            <li><a href="logout_submit.php">Logout</a></li>
          </ul>
      </div>
    </nav>
    
	<div class="search">
        <h1>Online Library</h1>
        <h3 class="pull-right">Logged in: <a href="view.php">
            <?php echo "$username"; ?>
            </a></h3>
    
        <hr>
    
        <form class="form-inline" action="">
            <div class="form-group">
                <label>Search: </label>
                <input id="keyword" type="text" onkeyup="loadResults(this.value)">
                <!-- <button class="btn btn-default" type="button" onclick="loadDoc();">Search</button> -->
            </div>
         </form>
        
         <table id="searchTable" class="table">
                <thead><tr> 
                    <th>Title</th> 
                    <th>Author</th> 
                    <th>Year</th> 
                    <th>Language</th> 
                </tr></thead>
             <div id="searchResults">
                 
             </div>
         </table>
    </div>
	
</body>

<script>
function loadResults(keyword) {    
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (xhttp.readyState == 4 && xhttp.status == 200) {
            if (keyword.length == 0) 
                document.getElementById('searchTable').innerHTML = "<thead><tr> <th>Title</th> <th>Author</th> <th>Year</th> <th>Language</th> </tr> </thead>";
            else
                document.getElementById('searchTable').innerHTML = xhttp.responseText;
            //console.log('RESPONSE TEXT');
            //console.log(xhttp.responseText);
            //console.log("status text: " + xhttp.statusText + " responseType: " + xhttp.responseType);
            //alert('i');
        }
    };
    xhttp.open("GET", "searchResults.php?q=" + keyword, true);
    xhttp.send();
}
    
function checkout(bookID) {
    location = 'checkout_submit.php?bookID=' + bookID;
}
</script>

</html>