<?php
    class Book {
        var $title;
        var $author;
        var $year;
        var $language;
        var $id;
        
        function __construct($t, $a, $y, $l) {
            $this->title = $t;
            $this->author = $a;
            $this->year = $y;
            $this->language = $l;
        }
    }
?>