const express = require("express");
const app = express();
const path = require('path');
const port = 3000;
let colors;

app.use(express.json());

app.get("/index", (req, res) => {
    res.send("fucker")
})

app.get("/getcolor", (req, res) => {
    console.log("Test")
    res.send({"red": colors.red, "green": colors.green, "blue": colors.blue});
})

app.post("/test", (req, res) => {
    let body = req.body;
    colors = body;
    console.log(body);
    res.end()   
})

app.get('/color', function(req, res) {
    console.log(path.join(__dirname, '/index.html'))
    res.sendFile(path.join(__dirname, '/index.html'));
  });


app.listen(port);